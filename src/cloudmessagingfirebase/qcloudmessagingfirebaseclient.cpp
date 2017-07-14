/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCloudMessaging module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3-COMM$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcloudmessagingfirebaseclient.h"
#include "qcloudmessagingfirebaseclient_p.h"

#if defined(Q_OS_ANDROID)
#include <QtAndroid>
#include <QtAndroidExtras>
#include <QAndroidJniObject>
#elif defined(Q_OS_DARWIN)
extern "C" {
#include <objc/objc.h>
}  // extern "C"
#endif  // __ANDROID__

QT_BEGIN_NAMESPACE

extern void LogMessage(const char *format, ...);
static QCloudMessagingFirebaseClient *m_client_pointer;

/*!
 * \brief QCloudMessagingFirebaseClient::QCloudMessagingFirebaseClient
 */
QCloudMessagingFirebaseClient::QCloudMessagingFirebaseClient(QObject *parent) :
    QCloudMessagingClient(parent),
    d(new QCloudMessagingFirebaseClientPrivate)
{
}

/*!
 * \brief QCloudMessagingFirebaseClient::~QCloudMessagingFirebaseClient
 */
QCloudMessagingFirebaseClient::~QCloudMessagingFirebaseClient()
{
}

/*!
 * \brief QCloudMessagingFirebaseClient::connectClient
 * \param clientId
 * \param parameters
 * \return
 */
QString QCloudMessagingFirebaseClient::connectClient(const QString &clientId,
                                                     const QVariantMap &parameters)
{
    QCloudMessagingClient::connectClient(clientId, parameters);
    // Call parent function to setup service ids and states.

    // Setup Firebase app for the client.
#if defined(__ANDROID__)
    d->m_firebaseApp = ::firebase::App::Create(::firebase::AppOptions(), QAndroidJniEnvironment(),
                                                QtAndroid::androidActivity().object());

#endif
#if defined(__APPLE__)
    d->m_firebaseApp = ::firebase::App::Create(::firebase::AppOptions());

#endif  // defined(__ANDROID__)
    m_client_pointer = this;

    d->m_firebase_initializer.Initialize(d->m_firebaseApp,
                                         nullptr, [](::firebase::App * fapp, void *) {
        LogMessage("Try to initialize Firebase Messaging");
        return ::firebase::messaging::Initialize(
                    *fapp,
                    (::firebase::messaging::Listener *)m_client_pointer);
    });

    while (d->m_firebase_initializer.InitializeLastResult().status() !=
            firebase::kFutureStatusComplete) {

        LogMessage("Firebase: InitializeLastResult wait...");
    }

    setClientId(clientId);
    return clientId;
}

/*!
 * \brief QCloudMessagingFirebaseClient::sendMessage
 * \param msg
 * \param send_to_device
 * \param send_to_channel
 * \return
 */
bool QCloudMessagingFirebaseClient::sendMessage(const QByteArray &msg,  const QString &clientToken,
                                                const QString &channel)
{
    Q_UNUSED(channel)

    ::firebase::messaging::Message message;

    QString message_to = clientToken + "@gcm.googleapis.com";

    message.to = message_to.toStdString();

    //TODO: USE Uuid Qt generator
    message.message_id = QString("uniqueId" + QString::number(rand() * 10000)).toStdString();

    //TODO: QString to std map conversion from the input message
    Q_UNUSED(msg);
    // message.data

    // TODO: Use specific client parameters for time to live
    message.time_to_live = 1000000 ;//TIME_TO_LIVE;
    ::firebase::messaging::Send(message);

    return true;
}

/*!
 * \brief QCloudMessagingFirebaseClient::disconnectClient
 * \return
 */
void QCloudMessagingFirebaseClient::disconnectClient()
{
    QCloudMessagingClient::disconnectClient();
}

/*!
 * \brief QCloudMessagingFirebaseClient::cloudMessageReceived
 * \param client
 * \param message
 */
void QCloudMessagingFirebaseClient::cloudMessageReceived(const QString  &client,
                                                         const QByteArray  &message)
{
    emit messageReceived(client, message);
}

/*!
 * \brief QCloudMessagingFirebaseClient::OnTokenReceived
 * \param token
 */
void QCloudMessagingFirebaseClient::OnTokenReceived(const char *token)
{
    d->m_token = QString::fromLatin1(token);
}

/*!
 * \brief QCloudMessagingFirebaseClient::OnMessage
 * \param message
 */
void QCloudMessagingFirebaseClient::OnMessage(const::firebase::messaging::Message &message)
{
    d->m_last_firebase_message = message;
    emit messageReceived(clientId(), parseMessage(d->m_last_firebase_message).toUtf8());
}

/*!
 * \brief QCloudMessagingFirebaseClient::setClientToken
 * \param uuid
 */
void QCloudMessagingFirebaseClient::setClientToken(const QString  &uuid)
{
    d->m_token = uuid;
    emit clientTokenReceived(d->m_token);
}

/*!
 * \brief QCloudMessagingFirebaseClient::parseMessage
 * \param msg_map
 * \return
 */
QString QCloudMessagingFirebaseClient::parseMessage(::firebase::messaging::Message  msg_map)
{
    QString msg;
    msg = QString::fromLatin1("{");
    bool dotSign = false;

    if (!msg_map.from.empty()) {
        if (dotSign) msg += QString::fromLatin1(",");
        msg += QString::fromLatin1("\"from\":\"") + QString::fromLatin1(msg_map.from.c_str()) + "\"";
        dotSign = true;
    }
    if (!msg_map.error.empty()) {
        if (dotSign) msg += QString::fromLatin1(",");
        msg += QString::fromLatin1("\"error\":\"") + QString::fromLatin1(msg_map.error.c_str()) + "\"";
        dotSign = true;
    }
    if (!msg_map.message_id.empty()) {
        if (dotSign) msg += QString::fromLatin1(",");
        msg += QString::fromLatin1("\"message_id\":\"") + QString::fromLatin1(
                   msg_map.message_id.c_str()) + "\"";
        dotSign = true;
    }

    if (msg_map.notification) {

        if (dotSign) msg += QString::fromLatin1(",");

        msg += QString::fromLatin1("\"notification\":{");

        if (msg_map.notification_opened)
            msg += QString::fromLatin1("\"notification_opened\":true");
        else
            msg += QString::fromLatin1("\"notification_opened\":false");

        if (!msg_map.notification->title.empty())
            msg += QString::fromLatin1(",\"title\":\"") + QString::fromLatin1(
                   msg_map.notification->title.c_str()) + QString::fromLatin1("\"");

        if (!msg_map.notification->body.empty())
            msg += QString::fromLatin1(",\"body\":\"") + QString::fromLatin1(msg_map.notification->body.c_str())
                   + QString::fromLatin1("\"");

        if (!msg_map.notification->icon.empty())
            msg += QString::fromLatin1(",\"icon\":\"") + QString::fromLatin1(msg_map.notification->icon.c_str())
                   + QString::fromLatin1("\"");

        if (!msg_map.notification->tag.empty())
            msg += QString::fromLatin1(",\"tag\":\"") + QString::fromLatin1(msg_map.notification->tag.c_str()) +
                   QString::fromLatin1("\"");

        if (!msg_map.notification->color.empty())
            msg += QString::fromLatin1(",\"color\":\"") + QString::fromLatin1(
                   msg_map.notification->color.c_str()) + QString::fromLatin1("\"");

        if (!msg_map.notification->sound.empty())
            msg += QString::fromLatin1(",\"sound\":\"") + QString::fromLatin1(
                   msg_map.notification->sound.c_str()) + QString::fromLatin1("\"");

        if (!msg_map.notification->click_action.empty())
            msg += QString::fromLatin1(",\"click_action\":\"") + QString::fromLatin1(
                   msg_map.notification->click_action.c_str()) + QString::fromLatin1("\"");

        msg += QString::fromLatin1("}");

        dotSign = true;
    }
    if (msg_map.data.size() > 0) {
        if (dotSign) msg += QString::fromLatin1(",");

        dotSign = false;

        msg += QString::fromLatin1("\"data\":{");

        for (const auto &field : msg_map.data) {

            if (!field.first.empty() && !field.second.empty()) {
                if (dotSign) msg += QString::fromLatin1(",");

                msg += QString::fromLatin1("\"") + QString::fromStdString(field.first) + QString::fromLatin1("\":")
                       + QString::fromStdString(field.second);

                dotSign = true;
            }
        }
        msg += QString::fromLatin1("}");
    }
    msg += QString::fromLatin1("}");
    return msg;
}

/*!
 * \brief QCloudMessagingFirebaseClient::flushMessageQueue
 * \return
 */
bool QCloudMessagingFirebaseClient::flushMessageQueue()
{
    if (!d->m_last_firebase_message.message_id.empty()) {
        emit messageReceived(clientId(), parseMessage(d->m_last_firebase_message).toUtf8());
    }
    return true;
}

/*!
 * \brief QCloudMessagingFirebaseClient::subscribeToChannel
 * \param channel
 * \return
 */
bool QCloudMessagingFirebaseClient::subscribeToChannel(const QString  &channel)
{
    ::firebase::messaging::Subscribe(channel.toLatin1());
    return true;
}

/*!
 * \brief QCloudMessagingFirebaseClient::unsubscribeFromChannel
 * \param channel
 * \return
 */
bool QCloudMessagingFirebaseClient::unsubscribeFromChannel(const QString  &channel)
{
    ::firebase::messaging::Unsubscribe(channel.toLatin1());
    return true;
}

/*!
 * \brief QCloudMessagingFirebaseClient::clientToken
 * \return
 */
QString QCloudMessagingFirebaseClient::clientToken()
{
    return d->m_token;
}

/*!
 * \brief QCloudMessagingFirebaseClient::clientUuid
 * \return
 */
QString QCloudMessagingFirebaseClient::clientUuid()
{
    return d->m_token;
}

// Provide link to main - which will be in the app using this service.
extern int main(int argc, char *argv[]);

QT_END_NAMESPACE
