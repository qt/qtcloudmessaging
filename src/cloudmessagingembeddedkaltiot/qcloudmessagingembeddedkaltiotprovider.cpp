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

#include "qcloudmessagingembeddedkaltiotprovider.h"
#include "qcloudmessagingembeddedkaltiotprovider_p.h"

#ifdef ANDROID_OS
#include <QtAndroid>
#include "jni.h"
#endif

QT_BEGIN_NAMESPACE

/**
 * @brief QCloudMessagingProvider
 * @abstract Make the smart iot class accessible outside of the class for callback msgs.
 */

static QCloudMessagingEmbeddedKaltiotProvider *m_KaltiotServiceProvider;

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::QCloudMessagingEmbeddedKaltiotProvider
 */
QCloudMessagingEmbeddedKaltiotProvider::QCloudMessagingEmbeddedKaltiotProvider(QObject *parent) :
    QCloudMessagingProvider(parent),
    d(new QCloudMessagingEmbeddedKaltiotProviderPrivate)
{
    m_KaltiotServiceProvider = this;
    connect(&d->m_restInterface, &QCloudMessagingEmbeddedKaltiotRest::remoteClientsReceived,
            this, &QCloudMessagingEmbeddedKaltiotProvider::remoteClientsReceived);
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::~QCloudMessagingEmbeddedKaltiotProvider
 */
QCloudMessagingEmbeddedKaltiotProvider::~QCloudMessagingEmbeddedKaltiotProvider()
{
    deregisterProvider();
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::registerProvider
 * \param serviceID
 * \param parameters
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotProvider::registerProvider(const QString  &serviceID,
                                                              const QVariantMap &parameters)
{
    d->m_serviceID = serviceID;

    QCloudMessagingProvider::registerProvider(serviceID, parameters);

    setServiceState(QtCloudMessagingProviderRegistered);

    // Get the API key for HTTP communication
    d->m_key = parameters.value(QStringLiteral("API_KEY")).toString();
    d->m_restInterface.setAuthKey(d->m_key);

    return QtCloudMessagingProviderRegistered;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::deregisterProvider
 * \return
 */
void QCloudMessagingEmbeddedKaltiotProvider::deregisterProvider()
{
    QCloudMessagingProvider::deregisterProvider();
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::setServiceState
 * \param service_mode
 * \return
 */
QCloudMessagingProvider::CloudMessagingProviderState QCloudMessagingEmbeddedKaltiotProvider::setServiceState(QCloudMessagingProvider::CloudMessagingProviderState service_mode)
{
    if (getServiceState() != QtCloudMessagingProviderNotRegistered) {
        return QCloudMessagingProvider::setServiceState(service_mode);
    }
    return QtCloudMessagingProviderNotRegistered;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::connectClient
 * \param clientId
 * \param parameters
 * \return
 */
QString QCloudMessagingEmbeddedKaltiotProvider::connectClient(const QString  &clientId,
                                                              const QVariantMap &parameters)
{
    if (!providerId().isEmpty()) {
        QCloudMessagingEmbeddedKaltiotClient *serviceClient = new QCloudMessagingEmbeddedKaltiotClient();

        QString retval = connectClientToProvider(clientId, parameters, serviceClient);

#ifdef ANDROID_OS
        QtAndroid::androidActivity().callMethod<void>("init_kaltiot_wrapper", "()V");
#endif
        return retval;
    }
    return QString();
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::sendMessage
 * \param msg
 * \param clientId
 * \param clientToken
 * \param channel
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotProvider::sendMessage(const QByteArray &msg,
                                                         const QString &clientId,
                                                         const QString &clientToken,
                                                         const QString &channel)
{
    // Is this local client?
    if (!clientId.isEmpty()) {

        if (getKaltiotClient(clientId) && clientToken.isEmpty()) {
            getKaltiotClient(clientId)->messageReceived(clientId, msg);
            return true;
        }

        // No, not local. Its somewhere out there
    } else {

        if (!clientToken.isEmpty() && channel.isEmpty()) {

            // Publish message to server via kaltiot SDK
            QCloudMessagingEmbeddedKaltiotClient *tempClient =
                    (QCloudMessagingEmbeddedKaltiotClient *) clients()->first();

            if (tempClient)
                return tempClient->sendMessage(msg, clientToken, QString());
        }

        // Send to known device by using rest api - by giving not empty string to channel
        if (!clientToken.isEmpty() && !channel.isEmpty())
            return d->m_restInterface.sendDataToDevice(clientToken, msg);

        // Broadcast to subscribed channel!
        if (clientToken.isEmpty() && !channel.isEmpty())
            return d->m_restInterface.sendBroadcast(channel, msg);
    }

    return false;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::disconnectClient
 * \param clientId
 * \param parameters
 * \return
 */
void QCloudMessagingEmbeddedKaltiotProvider::disconnectClient(
        const QString  &clientId,
        const QVariantMap &parameters)
{
    QCloudMessagingProvider::disconnectClient(clientId, parameters);
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::clients
 * \return
 */
QMap <QString, QCloudMessagingClient*> *QCloudMessagingEmbeddedKaltiotProvider::clients()
{
    return QCloudMessagingProvider::clients();
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::remoteClients
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotProvider::remoteClients()
{
    // Requesting remote clients via REST APi interface.
    return d->m_restInterface.getAllDevices();
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::setClientToken
 * \param client
 * \param uuid
 */
void QCloudMessagingEmbeddedKaltiotProvider::setClientToken(const QString  &client,
                                                            const QString  &uuid)
{
    getKaltiotClient(client)->setClientToken(uuid);
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::cloudMessageReceived
 * \param client
 * \param message
 */
void QCloudMessagingEmbeddedKaltiotProvider::cloudMessageReceived(const QString  &client,
                                                                  const QByteArray  &message)
{
    getKaltiotClient(client)->cloudMessageReceived(client, message);

}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::subscribeToChannel
 * \param channel
 * \param clientId
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotProvider::subscribeToChannel(const QString  &channel,
                                                               const QString  &clientId)
{
    if (getKaltiotClient(clientId)) {
        return getKaltiotClient(clientId)->subscribeToChannel(channel);
    }
    return false;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::unsubscribeFromChannel
 * \param channel
 * \param clientId
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotProvider::unsubscribeFromChannel(
        const QString  &channel,
        const QString  &clientId)
{
    if (getKaltiotClient(clientId)) {
        return getKaltiotClient(clientId)->unsubscribeFromChannel(channel);
    }
    return false;
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotProvider::getKaltiotClient
 * \param clientId
 * \return
 */
QCloudMessagingEmbeddedKaltiotClient *QCloudMessagingEmbeddedKaltiotProvider::
                                        getKaltiotClient(const QString &clientId)
{
    return (QCloudMessagingEmbeddedKaltiotClient *)client(clientId);
}

/*** CALLBACK IMPLEMENTATIONS AGAINST KALTIOT SMART IOT LIBRARY */
/*!
 * \brief ks_gw_client_notification_cb
 * Handles the callback function from the backend daemon.
 * Informs the incoming notification / message from the server.
 * \param address
 * \param payload
 * \param payload_length
 * \param payload_type
 * \param msg_id
 * \param msg_id_length
 */
void ks_gw_client_notification_cb(const char *address, const char *payload,
                                  const uint16_t payload_length,
                                  const payload_type_t payload_type,
                                  const char *msg_id,
                                  const uint16_t msg_id_length)
{
    Q_UNUSED(payload_type);
    Q_UNUSED(msg_id);
    Q_UNUSED(msg_id_length);

    QString client = address != nullptr ? QString::fromLatin1(address) : QString();
    QByteArray b_payload = payload != nullptr ? QByteArray(payload,
                                                        payload_length) : QString().toLatin1();
    //QString msg =   QString::fromLatin1(b_payload);

    if (!client.isEmpty())
        m_KaltiotServiceProvider->cloudMessageReceived(client, b_payload);
}

/*!
 * \brief ks_gw_client_state_changed_cb
 * Handles the callback function from the backend daemon.
 * Gets the state and forwards it to right client.
 * \param state
 * \param error
 */
void ks_gw_client_state_changed_cb(KaltiotSmartState state, KaltiotSmartError error)
{
    Q_UNUSED(error);

    // State related to client - assuming first client
    QCloudMessagingEmbeddedKaltiotClient *firstClient = (QCloudMessagingEmbeddedKaltiotClient *)m_KaltiotServiceProvider->clients()->first();
    emit firstClient->clientStateChanged(firstClient->clientId(), state);

}

/*!
 * \brief ks_gw_client_rid_cb
 * Handles the callback function from the backend daemon.
 * Gets the token and forwards it to right client.
 * \param address
 * \param rid
 * \param secret
 */
void ks_gw_client_rid_cb(const char *address, const char *rid,
                         const char *secret)
{
    if (!address) address = "nullptr";
    if (!rid) rid = "nullptr";
    if (!secret) secret = "nullptr";

    m_KaltiotServiceProvider->setClientToken(QString::fromLatin1(address), QString::fromLatin1(rid));

}

#ifdef ANDROID_OS

/*!
 * \brief fromJavaOnStateChanged
 * Handles the Android Kaltiot daemon message through JNI interface.
 * Informs the client state change.
 * \param env
 * \param thiz
 * \param address
 * \param state
 */
static void fromJavaOnStateChanged(JNIEnv *env, jobject thiz, jstring address, jstring state)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    QAndroidJniObject _address = address;
    QAndroidJniObject _state = state;

    ks_gw_client_state_changed_cb((KaltiotSmartState)_state.toString().toInt(),
                                   (KaltiotSmartError) 0);
}

/*!
 * \brief fromJavaOnRidChanged
 * Handles the Android Kaltiot daemon message through JNI interface.
 * Informs the client token change.
 * \param env
 * \param thiz
 * \param address
 * \param rid
 */
static void fromJavaOnRidChanged(JNIEnv *env, jobject thiz, jstring address, jstring rid)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)


    QAndroidJniObject _address = address;
    QAndroidJniObject _rid = rid;

    ks_gw_client_rid_cb(_address.toString().toLatin1(), _rid.toString().toLatin1(), nullptr);

}

/*!
 * \brief fromJavaOnAppIdChanged
 * Handles the Android Kaltiot daemon message through JNI interface.
 * Informs the application id string change.
 * \param env
 * \param thiz
 * \param address
 * \param appid
 */
static void fromJavaOnAppIdChanged(JNIEnv *env, jobject thiz, jstring address, jstring appid)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    QAndroidJniObject _address = address;
    QAndroidJniObject _appid = appid;

}

/*!
 * \brief fromJavaOnNotification
 * Handles the Android Kaltiot daemon message through JNI interface.
 * Informs the incoming notification / message from the server.
 * \param env
 * \param thiz
 * \param address
 * \param payload
 * \param msg
 * \param payload_length
 * \param payload_type
 */
static void fromJavaOnNotification(JNIEnv *env, jobject thiz, jstring address, jstring payload,
                                   jstring msg, jint payload_length, jint payload_type)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    const char *_payload = env->GetStringUTFChars(payload, JNI_FALSE);

    const char *_address = env->GetStringUTFChars(address, JNI_FALSE);
    const char *_msg = env->GetStringUTFChars(msg, JNI_FALSE);
    ks_gw_client_notification_cb(_address,
                                 _payload,
                                 payload_length,
                                 (payload_type_t)payload_type,
                                 _msg,
                                 sizeof(_msg));

}

static JNINativeMethod methods[] {
    //{"native_notification_callback", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;II;)V", reinterpret_cast<void *>(fromJavaOnNotification)},
    {"native_state_callback", "(Ljava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void *>(fromJavaOnStateChanged)},
    {"native_rid_callback", "(Ljava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void *>(fromJavaOnRidChanged)},
    {"native_appid_callback", "(Ljava/lang/String;Ljava/lang/String;)V", reinterpret_cast<void *>(fromJavaOnAppIdChanged)},

};

extern "C"
{

    JNIEXPORT void JNICALL
    Java_com_snowgrains_radarsensor_QtApp_native_1notification_1callback__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2II(
        JNIEnv *env, jobject thiz, jstring address, jstring payload, jstring msg, jint payload_length,
        jint payload_type)
    {
        env = env;
        const char *_payload = payload != nullptr ? env->GetStringUTFChars(payload, JNI_FALSE) : nullptr;
        const char *_address = address != nullptr ? env->GetStringUTFChars(address, JNI_FALSE) : nullptr;
        const char *_msg = msg != nullptr ? env->GetStringUTFChars(msg, JNI_FALSE) : nullptr;
        ks_gw_client_notification_cb(_address,
                                     _payload,
                                     payload_length,
                                     (payload_type_t)payload_type,
                                     _msg,
                                     sizeof(_msg));
    }

    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *)
    {

        JNIEnv *env;
        if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
            return JNI_FALSE;
        }
        jclass clazz = env->FindClass("com/snowgrains/radarsensor/QtApp");
        if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
            return JNI_FALSE;
        }
        return JNI_VERSION_1_4;
    }
}
#endif

/** CALLBACK IMPLEMENTATIONS ENDED ***/

// Provide link to main - which will be in the app using this service.
extern int main(int argc, char *argv[]);

QT_END_NAMESPACE
