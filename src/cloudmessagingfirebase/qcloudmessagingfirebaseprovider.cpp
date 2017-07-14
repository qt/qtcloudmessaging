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

#include "qcloudmessagingfirebaseprovider.h"
#include "qcloudmessagingfirebaseprovider_p.h"

#include "firebase/app.h"
#include "firebase/messaging.h"
#include "firebase/util.h"

QT_BEGIN_NAMESPACE

static QCloudMessagingFirebaseProvider *m_FirebaseServiceProvider;

/*!
 * \brief LogMessage
 * \param format
 */
void LogMessage(const char *format, ...)
{
    static const int kLineBufferSize = 100;
    char buffer[kLineBufferSize + 2];

    va_list list;
    va_start(list, format);
    int string_len = vsnprintf(buffer, kLineBufferSize, format, list);
    string_len = string_len < kLineBufferSize ? string_len : kLineBufferSize;
    // append a linebreak to the buffer:
    buffer[string_len] = '\n';
    buffer[string_len + 1] = '\0';
    va_end(list);
}

/*!
 * \brief QCloudMessagingFirebaseProvider::QCloudMessagingFirebaseProvider
 */
QCloudMessagingFirebaseProvider::QCloudMessagingFirebaseProvider(QObject *parent) :
    QCloudMessagingProvider(parent),
    d(new QCloudMessagingFirebaseProviderPrivate)
{
    m_FirebaseServiceProvider  = this;
}

/*!
 * \brief QCloudMessagingFirebaseProvider::~QCloudMessagingFirebaseProvider
 */
QCloudMessagingFirebaseProvider::~QCloudMessagingFirebaseProvider()
{
    deregisterProvider();
}

/*!
 * \brief QCloudMessagingFirebaseProvider::registerProvider
 * \param providerId
 * \param parameters
 * \return
 */
bool QCloudMessagingFirebaseProvider::registerProvider(const QString &providerId,
                                                       const QVariantMap &parameters)
{

    QCloudMessagingProvider::registerProvider(providerId, parameters);

    setServiceState(QtCloudMessagingProviderRegistered);

    // Get the API key for HTTP communication
    d->m_key = parameters.value(QStringLiteral("SERVER_API_KEY")).toString();
    d->m_restInterface.setAuthKey(d->m_key);

    return true;
}

/*!
 * \brief QCloudMessagingFirebaseProvider::deregisterProvider
 * \return
 */
void QCloudMessagingFirebaseProvider::deregisterProvider()
{
    ::firebase::messaging::Terminate();
    QCloudMessagingProvider::deregisterProvider();
}

/*!
 * \brief QCloudMessagingFirebaseProvider::setServiceState
 * \param service_mode
 * \return
 */
QCloudMessagingProvider::CloudMessagingProviderState QCloudMessagingFirebaseProvider::setServiceState(QCloudMessagingProvider::CloudMessagingProviderState service_mode)
{
    if (getServiceState() != QtCloudMessagingProviderNotRegistered) {
        return QCloudMessagingProvider::setServiceState(service_mode);
    }

    return QCloudMessagingProvider::QtCloudMessagingProviderNotRegistered;
}

/*!
 * \brief QCloudMessagingFirebaseProvider::connectClient
 * \param clientId
 * \param parameters
 * \return
 */
QString QCloudMessagingFirebaseProvider::connectClient(const QString &clientId,
                                                       const QVariantMap &parameters)
{
    if (!providerId().isEmpty()) {
        QCloudMessagingFirebaseClient *serviceClient = new QCloudMessagingFirebaseClient();
        QString retval = connectClientToProvider(clientId, parameters, serviceClient);
        return retval;
    }
    return QString();
}


/*!
 * \brief QCloudMessagingFirebaseProvider::sendMessage
 * \param msg
 * \param clientId
 * \param send_to_device
 * \param send_to_channel
 * \return
 */
bool QCloudMessagingFirebaseProvider::sendMessage(const QByteArray &msg, const QString &clientId,
                                                  const QString &clientToken, const QString &channel)
{
    //! Sending to internal client
    if (!clientId.isEmpty() && clientToken.isEmpty() && channel.isEmpty()) {

        if (client(clientId)) {
            client(clientId)->messageReceived(clientId, msg);
            return true;
        }
    } else
        //! Sending to device or channel out there, by using the client (e.g. in mobile)
        if (!clientId.isEmpty() && (!clientToken.isEmpty() && !channel.isEmpty())) {

            if (client(clientId)) {
                return client(clientId)->sendMessage(msg, clientToken, channel);
            }

        } else {
            //! Sending via rest api interface (SERVER side)
            if (!channel.isEmpty())
                return d->m_restInterface.sendBroadcast(channel, msg);

            if (!clientToken.isEmpty())
                return d->m_restInterface.sendToDevice(clientToken, msg);
        }
    return false;
}

/*!
 * \brief QCloudMessagingFirebaseProvider::disconnectClient
 * \param clientId
 * \param parameters
 * \return
 */
void QCloudMessagingFirebaseProvider::disconnectClient(const QString &clientId,
                                                       const QVariantMap &parameters)
{
    return QCloudMessagingProvider::disconnectClient(clientId, parameters);
}

/*!
 * \brief QCloudMessagingFirebaseProvider::clients
 * \return
 */
QMap <QString, QCloudMessagingClient *>   *QCloudMessagingFirebaseProvider::clients()
{
    return QCloudMessagingProvider::clients();
}

/*!
 * \brief QCloudMessagingFirebaseProvider::setClientToken
 * \param client
 * \param uuid
 */
void QCloudMessagingFirebaseProvider::setClientToken(const QString &clientId, const QString &uuid)
{
    client(clientId)->setClientToken(uuid);
}

/*!
 * \brief QCloudMessagingFirebaseProvider::cloudMessageReceived
 * \param client
 * \param message
 */
void QCloudMessagingFirebaseProvider::cloudMessageReceived(const QString &clientId,
                                                           const QByteArray &message)
{
    client(clientId)->messageReceived(clientId, message);
}

/*!
 * \brief QCloudMessagingFirebaseProvider::subscribeToChannel
 * \param channel
 * \param clientId
 * \return
 */
bool QCloudMessagingFirebaseProvider::subscribeToChannel(const QString &channel,
                                                        const QString &clientId)
{
    if (!clientId.isEmpty())
        return client(clientId)->subscribeToChannel(channel);

    return true;
}

/*!
 * \brief QCloudMessagingFirebaseProvider::unsubscribeFromChannel
 * \param channel
 * \param clientId
 * \return
 */
bool QCloudMessagingFirebaseProvider::unsubscribeFromChannel(const QString &channel,
                                                            const QString &clientId)
{
    if (!clientId.isEmpty())
        return client(clientId)->unsubscribeFromChannel(channel);

    return true;
}

/*!
 * \brief QCloudMessagingFirebaseProvider::clientToken
 * \param clientId
 * \return
 */
QString QCloudMessagingFirebaseProvider::clientToken(const QString &clientId)
{
    return client(clientId)->clientToken();
}

/*!
 * \brief QCloudMessagingFirebaseProvider::remoteClients
 * \return
 */
bool QCloudMessagingFirebaseProvider::remoteClients()
{
    return false;
}

QT_END_NAMESPACE



