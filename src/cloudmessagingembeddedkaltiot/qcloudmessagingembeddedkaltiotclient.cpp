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

#include "qcloudmessagingembeddedkaltiotclient.h"
#include <qcloudmessagingembeddedkaltiotclient_p.h>

#include <QStringList>

#ifdef ANDROID_OS
#include <QtAndroid>
#include "jni.h"
#endif

QT_BEGIN_NAMESPACE

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::QCloudMessagingEmbeddedKaltiotClient
 * Kaltiot client constructor
 */
QCloudMessagingEmbeddedKaltiotClient::QCloudMessagingEmbeddedKaltiotClient(QObject *parent) :
    QCloudMessagingClient(parent),
    d(new QCloudMessagingEmbeddedKaltiotClientPrivate)
{
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::~QCloudMessagingEmbeddedKaltiotClient
 * Desctructor
 */
QCloudMessagingEmbeddedKaltiotClient::~QCloudMessagingEmbeddedKaltiotClient()
{
    // if thread is running, ask for exit it first and then terminate thread
    if (d->m_running && d->m_clientThread.isRunning()) {
        d->m_running = false;
        d->m_clientThread.quit();
        d->m_clientThread.terminate();
    }
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::connectClient
 * \param clientId
 * \param parameters
 * \return
 */
QString QCloudMessagingEmbeddedKaltiotClient::connectClient(const QString &clientId,
                                                             const QVariantMap &parameters)
{
    // Call parent function to setup service ids and states.
    QCloudMessagingClient::connectClient(clientId, parameters);

    QStringList channels;
    QVariantMap params = parameters;
    d->m_address = params.value(QStringLiteral("address")).toString();
    d->m_version = params.value(QStringLiteral("version")).toString();
    d->m_customer_id = params.value(QStringLiteral("customer_id")).toString();

    channels = params.value(QStringLiteral("channels")).toStringList();

    for (int i = 0; i < channels.count(); i++) {
        bool new_channel = true;
        for (int j = 0; j < d->m_channels.count(); j++) {
            if (channels[i] == d->m_channels[j])
                new_channel = false;
        }
        if (new_channel) {
            d->m_channels.append(channels[i]);
        }
    }

    setClientToken(clientId);

    if (!make_kaltiot_client_registration())
        return QString();

    runBackgroundThread();
    return d->m_address;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::runBackgroundThread
 */
void QCloudMessagingEmbeddedKaltiotClient::runBackgroundThread()
{
    this->moveToThread(&d->m_clientThread);
    d->m_clientThread.start();

    // Run service task in the thread.
    connect(&(d->m_threadTimer), &QTimer::timeout, [ = ] {

        if (!d->m_running) d->m_running = true;
#ifdef EMBEDDED_AND_DESKTOP_OS
        ks_gw_client_task(&d->m_kaltiot_client_instance);
#endif
        d->m_threadTimer.start();
    });

    d->m_threadTimer.setInterval(1);
    d->m_threadTimer.start();
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::make_kaltiot_client_registration
 */
bool QCloudMessagingEmbeddedKaltiotClient::make_kaltiot_client_registration()
{
#ifdef EMBEDDED_AND_DESKTOP_OS

    char const *channels[d->m_channels.count()];
    QList<QByteArray> constChannels;

    for (int i = 0; i < d->m_channels.count(); i++) {
        constChannels.append(d->m_channels[i].toLatin1());
        channels[i] = constChannels[constChannels.count()-1].constData();
    }

    uint16_t num_channels = d->m_channels.count();

    ks_gw_client_init(&d->m_kaltiot_client_instance);

    if (!ks_gw_client_connect(&d->m_kaltiot_client_instance, NULL))
        return false; // Failed to connect!;

    if (d->m_rid.isEmpty()) {
        d->m_rid = d->m_client_settings.value("clients/" + d->m_address + "/rid").toString();
    }

    if (!d->m_rid.isEmpty())
        setClientToken(d->m_rid);

    ks_gw_client_set_engine_enabled(&d->m_kaltiot_client_instance, true);

    ks_gw_client_set_network_available(&d->m_kaltiot_client_instance,
                                       NETWORK_STATE_MOBILE_2G, "123","45");

    ks_gw_client_register_iot(&d->m_kaltiot_client_instance,
                              d->m_address.toLatin1().constData(),
                              d->m_version.toLatin1().constData(),
                              d->m_customer_id.toLatin1().constData(),
                              channels, num_channels);

    ks_gw_client_request_rid(&d->m_kaltiot_client_instance);

    constChannels.clear();

#endif

#ifdef ANDROID_OS

    QAndroidJniObject j_rid  = QtAndroid::androidActivity().callObjectMethod<jstring>("get_rid");

    d->m_rid = j_rid.toString();
    if (d->m_rid.isEmpty()) {
        d->m_rid = d->m_client_settings.value("clients/" + d->m_address + "/rid").toString();

    }
    if (!d->m_rid.isEmpty())
        setClientToken(d->m_rid);

#endif
    return true;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::sendMessage
 * \param msg
 * \param send_to_device
 * \param send_to_channel
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotClient::sendMessage(const QByteArray &msg,
                                                       const QString &clientToken,
                                                       const QString &channel)
{
    // In Kaltiot client send message goes via client daemon and channel or
    // clientToken attributes are not used.
    Q_UNUSED(channel);
    Q_UNUSED(clientToken);


#ifdef EMBEDDED_AND_DESKTOP_OS
    // TAG NOT USED ATM.
    ks_gw_client_publish_message(&d->m_kaltiot_client_instance,
                                 (const uint8_t *)msg.constData(),
                                 msg.size(),
                                 KS_GW_CLIENT_PAYLOAD_STRING,
                                 nullptr);
#endif

    return true;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::disconnectClient
 * \return
 */
void QCloudMessagingEmbeddedKaltiotClient::disconnectClient()
{
    QCloudMessagingClient::disconnectClient();

    d->m_running = false;
#ifdef EMBEDDED_AND_DESKTOP_OS
    ks_gw_client_unregister_iot(&d->m_kaltiot_client_instance, d->m_address.toLatin1(),
                                d->m_version.toLatin1(), clientToken().toLatin1());
    ks_gw_client_disconnect(&d->m_kaltiot_client_instance);
    ks_gw_client_set_engine_enabled(&d->m_kaltiot_client_instance, false);
#endif
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::cloudMessageReceived
 * \param client
 * \param message
 */
void  QCloudMessagingEmbeddedKaltiotClient::cloudMessageReceived(const QString &client,
                                                                 const QByteArray &message)
{
    emit messageReceived(client, message);
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::setClientToken
 * \param token
 */
void QCloudMessagingEmbeddedKaltiotClient::setClientToken(const QString &token)
{
    d->m_rid = token;
    if (!d->m_rid.isEmpty())
        d->m_client_settings.setValue("clients/" + d->m_address + "/rid", d->m_rid);

    emit clientTokenReceived(d->m_rid);
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::clientToken
 * \return
 */
QString QCloudMessagingEmbeddedKaltiotClient::clientToken()
{
    return d->m_rid;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::getKaltiotEngineInstance
 * \return
 */
ks_gw_client_instance_t *QCloudMessagingEmbeddedKaltiotClient::getKaltiotEngineInstance()
{
    return &d->m_kaltiot_client_instance;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::subscribeToChannel
 *  * Unsubscribes client from the channel
 *
 * \param channel
 * * Channel name QString
 *
 * \return
 * false if channel not found, true if found and updated
 */
bool QCloudMessagingEmbeddedKaltiotClient::subscribeToChannel(const QString &channel)
{

    for (int i = 0; i < d->m_channels.count(); i++) {
        if (channel == d->m_channels[i])
            return false; // Already subscribed
    }

    // Not found, lets add new channel and restart client.
    d->m_channels.append(channel);

    disconnectClient();
    connectClient(clientToken(), clientParameters());

    return true;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::unsubscribeFromChannel
 * Unsubscribes client from the channel
 *
 * \param channel
 * Channel name QString
 *
 * \return
 * false if channel not found, true if found and updated
 */
bool QCloudMessagingEmbeddedKaltiotClient::unsubscribeFromChannel(const QString &channel)
{

    for (int i = 0; i < d->m_channels.count(); i++) {
        if (channel == d->m_channels[i]) {
            d->m_channels.removeAt(i);

            disconnectClient();
            connectClient(clientToken(), clientParameters());

            return true;
        }
    }
    return false; // Not found


}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotClient::flushMessageQueue
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotClient::flushMessageQueue()
{
    return true;
}

// Provide link to main - which will be in the app using this service.
extern int main(int argc, char *argv[]);

QT_END_NAMESPACE
