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

#include "qcloudmessagingrestapi.h"
#include "qcloudmessagingrestapi_p.h"
#include <QUuid>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QTimer>

/*!
    \class QCloudMessagingRestApi
    \inmodule QtCloudMessaging
    \since 5.11

    \brief Cloud messaging services usually needs two way communication
    from the server or the client. The QCloudMessagingRestApi class
    provides the functions to send XML HTTP(s) requests to server or
    other clients.

    The QCloudMessagingRestApi implements typical get, post, put and delete
    functionalites.

    To use the class in own implementation, first inherit the
    QCloudMessagingRestApi
    class and then implement the virtual function to receive responses.

*/

QT_BEGIN_NAMESPACE

/*!
 * \brief QCloudMessagingRestApi::QCloudMessagingRestApi
 * QCloudMessagingRestApi constructor
 *
 * \param parent
 * Parent as QObject
 */
QCloudMessagingRestApi::QCloudMessagingRestApi(QObject *parent) :
    QObject(parent),
    d(new QCloudMessagingRestApiPrivate)
{

    d->m_online_state = d->m_network_info.isOnline();
    d->m_wait_for_last_request_response = false;
    d->m_waiting_counter = 0;

    connect(&(d->m_network_info), &QNetworkConfigurationManager::onlineStateChanged,
            this, &QCloudMessagingRestApi::onlineStateChanged);

    connect(&(d->m_manager), &QNetworkAccessManager::authenticationRequired,
            this, &QCloudMessagingRestApi::provideAuthentication);

    connect(&(d->m_msgTimer), &QTimer::timeout,
            this, &QCloudMessagingRestApi::networkMsgTimerTriggered);

}

/*!
 * \brief QCloudMessagingRestApi::~QCloudMessagingRestApi
 * Destructor
 */
QCloudMessagingRestApi::~QCloudMessagingRestApi()
{
}

/*!
 * \brief QCloudMessagingRestApi::setServerTimers
 * Sets rest server message retry and timeouts.
 *
 * \param messageTimer
 * Timeout to wait before sending the next message.
 * Same timeout is used for waiting response from the server.
 * Useful when sending lot of message in fast sequency.
 * Default is 800 milliseconds.
 *
 * \param waitForResponseCounter
 * How long we wait for response to last send message
 * Time is waitForResponseCounter x messageTimer milliseconds.
 * Useful to tune for slower network conditions.
 * Default is 3 times = 3 x 800 ms = 2400 ms
 *
 * \param messageRetryCount
 * Describes how many times the message sending is retried if
 * no response from the server.
 */
void QCloudMessagingRestApi::setServerTimers(int messageTimer,
                                             int waitForResponseCounter,
                                             int messageRetryCount)
{
    d->setServerTimers(messageTimer, waitForResponseCounter, messageRetryCount);
}

/*!
 * \brief QCloudMessagingRestApi::provideAuthentication
 * Virtual function to provide authentication for the network connection.
 * By default this function is omitted and service is connected without
 * separate authentication.
 *
 * \param reply
 * Sent QNetworkReply data
 * \param authenticator
 * Authenticator instance where to set username and password.
 */
void QCloudMessagingRestApi::provideAuthentication(
        QNetworkReply *reply,
        QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    Q_UNUSED(authenticator);
}

/*!
 * \brief QCloudMessagingRestApi::xmlHttpPostRequest
 * Private function to send Post specific message to server.
 * Connects to virtualized xmlHttpRequestReply function.
 *
 * \param request
 * QNetworkRequest instance
 *
 * \param data
 * Data to be sent as QByteArray

 * \param req_id
 * Requirement id to identify the received message.
 *
 * \param uuid
 * Unique uuid for internal message array manipulation.
 *
 * \param info
 * Additional info to provide via QNetworkReply instance
 *
 * \return
 * Returns updated QNetworkReply instance after sending the post message.
 */
QNetworkReply *
QCloudMessagingRestApi::xmlHttpPostRequest(
        QNetworkRequest request,
        QByteArray data,
        int req_id,
        const QString &uuid,
        const QString &info)
{

    connect(&d->m_manager, &QNetworkAccessManager::finished,
            this, &QCloudMessagingRestApi::xmlHttpRequestReply);

    QNetworkReply *reply = d->m_manager.post(request, data);

    reply->setProperty("req_id", req_id);
    reply->setProperty("uuid", uuid);
    reply->setProperty("info", info);

    return reply;
}

/*!
 * \brief QCloudMessagingRestApi::xmlHttpPutRequest
 * Private function to send Put specific message to server.
 * Connects to virtualized xmlHttpRequestReply function.
 *
 * \param request
 * QNetworkRequest instance
 *
 * \param data
 * Data to be sent as QByteArray

 * \param req_id
 * Requirement id to identify the received message.
 *
 * \param uuid
 * Unique uuid for internal message array manipulation.
 *
 * \param info
 * Additional info to provide via QNetworkReply instance
 *
 * \return
 * Returns updated QNetworkReply instance after sending the post message.
 */
QNetworkReply *
QCloudMessagingRestApi::xmlHttpPutRequest(
        QNetworkRequest request,
        QByteArray data,
        int req_id,
        const QString &uuid,
        const QString &info)
{

    connect(&d->m_manager, &QNetworkAccessManager::finished,
            this, &QCloudMessagingRestApi::xmlHttpRequestReply);


    QNetworkReply *reply = d->m_manager.put(request, data);

    reply->setProperty("req_id", req_id);
    reply->setProperty("uuid", uuid);
    reply->setProperty("info", info);

    return reply;
}

/*!
 * \brief QCloudMessagingRestApi::xmlHttpDeleteRequest
 * Private function to send Delete specific message to server.
 * Connects to virtualized xmlHttpRequestReply function.
 *
 * \param request
 * QNetworkRequest instance
 *
 * \param data
 * Data to be sent as QByteArray

 * \param req_id
 * Requirement id to identify the received message.
 *
 * \param uuid
 * Unique uuid for internal message array manipulation.
 *
 * \param info
 * Additional info to provide via QNetworkReply instance
 *
 * \return
 * Returns updated QNetworkReply instance after sending the post message.
 */
QNetworkReply *
QCloudMessagingRestApi::xmlHttpDeleteRequest(
        QNetworkRequest request,
        int req_id,
        const QString &uuid,
        const QString &info)
{

    connect(&d->m_manager, &QNetworkAccessManager::finished,
            this, &QCloudMessagingRestApi::xmlHttpRequestReply);


    QNetworkReply *reply = d->m_manager.deleteResource(request);

    reply->setProperty("req_id", req_id);
    reply->setProperty("uuid", uuid);
    reply->setProperty("info", info);

    return reply;
}

/*!
 * \brief QCloudMessagingRestApi::xmlHttpGetRequest
 * Private function to send Get specific message to server.
 * Connects to virtualized xmlHttpRequestReply function.
 *
 * \param request
 * QNetworkRequest instance
 *
 * \param data
 * Data to be sent as QByteArray

 * \param req_id
 * Requirement id to identify the received message.
 *
 * \param uuid
 * Unique uuid for internal message array manipulation.
 *
 * \param info
 * Additional info to provide via QNetworkReply instance
 *
 * \return
 * Returns updated QNetworkReply instance after sending the post message.
 */
QNetworkReply *QCloudMessagingRestApi::xmlHttpGetRequest(
        QNetworkRequest request,
        int req_id,
        const QString &uuid,
        const QString &info)
{

    connect(&d->m_manager, &QNetworkAccessManager::finished,
            this, &QCloudMessagingRestApi::xmlHttpRequestReply);


    QNetworkReply *reply = d->m_manager.get(request);

    reply->setProperty("req_id", req_id);
    reply->setProperty("uuid", uuid);
    reply->setProperty("info", info);

    return reply;
}

/*!
 * \brief QCloudMessagingRestApi::sendNetworkMessage
 * Sends message with info from the QCloudMessagingNetworkMessage class parameter.
 *
 * \param msg
 * QCloudMessagingNetworkMessage class as parameter
 *
 * \param immediate
 * If true, defines that message is sent immediately and no message timers are involved
 * if false, message is set to message queue and message will be sent after message timer
 * timeouts.
 */
void QCloudMessagingRestApi::sendNetworkMessage(
        const QCloudMessagingNetworkMessage &msg,
        int immediate)
{
    sendMessage((QCloudMessagingRestApi::MessageType)msg.type,
                 msg.req_id,
                 msg.request,
                 msg.data,
                 immediate,
                 QString());
}

/*!
 * \brief QCloudMessagingRestApi::sendMessage
 * Sends type specific network message to server
 *
 * \param type
 * Type as QCloudMessagingRestApi::MessageType
 *
 * \param req_id
 * Requirement id to identify the received message.
 *
 * \param request
 * QNetworkRequest instance
 *
 * \param data
 * Data to be sent as QByteArray
 *
 * \param immediate
 * If true, defines that message is sent immediately and no message timers are involved
 * if false, message is set to message queue and message will be sent after message timer
 * timeouts.
 *
 * \param info
 * Additional info to provide via QNetworkReply instance
 *
 * \return
 * Return true if message was sent immediately. False if it went to the queue.
 */
bool QCloudMessagingRestApi::sendMessage(QCloudMessagingRestApi::MessageType type,
                                         int req_id,
                                         QNetworkRequest request,
                                          QByteArray data,
                                         int immediate,
                                         const QString &info)
{
    QCloudMessagingNetworkMessage msg;
    bool sent = false;

    // Remember the message if not online
    if (!immediate || !d->m_online_state) {
        QUuid quid = QUuid::createUuid();;

        QString pure_quid = quid.toString();
        pure_quid = pure_quid.mid(1, pure_quid.length() - 2);

        msg.req_id = req_id;
        msg.type = type;
        msg.uuid = pure_quid;
        msg.request = request;
        msg.data = data;
        msg.retry_count = 0;
        msg.info = info;

        d->m_network_requests.append(msg);

        if (!d->m_msgTimer.isActive()) d->m_msgTimer.start(d->m_server_message_timer);
    } else {
        if (type == POST_MSG) {
            xmlHttpPostRequest(request, data, req_id, msg.uuid, info);
        }

        if (type == GET_MSG) {
            xmlHttpGetRequest(request, req_id, msg.uuid, info);
        }
        if (type == PUT_MSG) {
            xmlHttpPutRequest(request, data, req_id, msg.uuid, info);
        }
        if (type == DELETE_MSG) {
            xmlHttpDeleteRequest(request, req_id, msg.uuid, info);
        }
        sent = true;
    }
    return sent;
}

/*!
 * \brief QCloudMessagingRestApi::clearMessage
 * Clears the specific messages from the message queue
 * Should be used in the implementatio of xmlHttpRequestReply
 *
 * \param msg_uuid
 * Unique uuid of the message
 */
void QCloudMessagingRestApi::clearMessage(const QString &msg_uuid)
{
    for (int i = 0; i < d->m_network_requests.length(); i++) {
        if (d->m_network_requests[i].uuid == msg_uuid) {
            d->m_network_requests.removeAt(i);
            return;
        }
    }
}


/*!
 * \brief QCloudMessagingRestApi::clearMessageBuffer
 * Clears the message queue.
 */
void QCloudMessagingRestApi::clearMessageBuffer()
{
    d->m_network_requests.clear();
}

/*!
 * \brief QCloudMessagingRestApi::networkMsgTimerTriggered
 * Private slot for handling message timeouts and resending of the
 * messages.
 */
void QCloudMessagingRestApi::networkMsgTimerTriggered()
{
    // Do not send messages if we are not online or we are waiting
    // for message to arrive
    if (!d->m_online_state) {
        d->m_msgTimer.start(d->m_server_message_timer);
        return;
    }

    if (d->m_wait_for_last_request_response) {
        d->m_waiting_counter++;

        if (d->m_waiting_counter > d->m_server_wait_for_response_counter) {
            d->m_wait_for_last_request_response = false;
            d->m_waiting_counter = 0;
        }
        d->m_msgTimer.start(d->m_server_message_timer);
        return;
    }

    d->m_waiting_counter = 0;

    // Send latest message.
    if (d->m_network_requests.length() > 0) {

        if (d->m_network_requests[0].type == POST_MSG) {

            xmlHttpPostRequest(d->m_network_requests[0].request,
                               d->m_network_requests[0].data,
                               d->m_network_requests[0].req_id,
                               d->m_network_requests[0].uuid,
                               d->m_network_requests[0].info);

            d->m_network_requests[0].retry_count++;
        }

        if (d->m_network_requests[0].type == GET_MSG &&
            d->m_network_requests[0].retry_count < d->m_server_message_retry_count) {

            xmlHttpGetRequest(d->m_network_requests[0].request,
                              d->m_network_requests[0].req_id,
                              d->m_network_requests[0].uuid,
                              d->m_network_requests[0].info);

            d->m_network_requests[0].retry_count++;
        }

        if (d->m_network_requests[0].type == PUT_MSG &&
            d->m_network_requests[0].retry_count < d->m_server_message_retry_count) {

            xmlHttpPutRequest(d->m_network_requests[0].request,
                              d->m_network_requests[0].data,
                              d->m_network_requests[0].req_id,
                              d->m_network_requests[0].uuid,
                              d->m_network_requests[0].info);

            d->m_network_requests[0].retry_count++;
        }

        if (d->m_network_requests[0].type == DELETE_MSG &&
            d->m_network_requests[0].retry_count < d->m_server_message_retry_count) {

            xmlHttpDeleteRequest(d->m_network_requests[0].request,
                                 d->m_network_requests[0].req_id,
                                 d->m_network_requests[0].uuid,
                                 d->m_network_requests[0].info);

            d->m_network_requests[0].retry_count++;
        }

        if (d->m_network_requests[0].retry_count < d->m_server_message_retry_count) {
            d->m_network_requests.move(0, d->m_network_requests.length() - 1);
        } else {
            d->m_network_requests.removeAt(0);
        }

        if (d->m_network_requests.length() > 0)
            d->m_msgTimer.start(d->m_server_message_timer);
    }
}

/*!
 * \brief QCloudMessagingRestApi::onlineStateChanged
 * Private slot for saving the online state based on the network class
 * info
 *
 * \param online
 * True if online, false if offline
 */
void QCloudMessagingRestApi::onlineStateChanged(bool online)
{
    d->m_online_state = online;
}

/*!
 * \brief QCloudMessagingRestApi::getNetworkRequestCount
 * Returns the count of network messages in the queue waiting
 * to be sent.
 *
 * \return
 * Returns amount of network messages in queue.
 */
int QCloudMessagingRestApi::getNetworkRequestCount()
{
    return d->m_network_requests.count();
}

/*!
 * \brief QCloudMessagingRestApi::getOnlineState
 * Get the current online state info from the class.
 *
 * \return
 * Returns true if online. False if offline.
 */
bool QCloudMessagingRestApi::getOnlineState()
{
    return d->m_online_state;
}

/*!
 * \brief QCloudMessagingRestApi::getNetworkManager
 * Gets the QNetworkAccessManager for the current connection.
 * Used in xmlHttpRequestReply implementation.
 *
 * \return
 * Return the used QNetworkAccessManager instance.
 */
QNetworkAccessManager *QCloudMessagingRestApi::getNetworkManager()
{
    return &(d->m_manager);
}

// Signals documentation
/*!
  \fn QCloudMessagingRestApi::xmlHttpRequestError(const QString &errorString)
  This signal is emitted when there is an error in the network transport.

  \param errorString
  Error message as QString
*/

// Public slots documentation
/*!
  \fn virtual void QCloudMessagingRestApi:: xmlHttpRequestReply(QNetworkReply *reply)

  This slot is executed when \c QNetworkReply is received for the client request.
  This is virtual function which needs to be implemented the by inheriting class.

  Example implementation:
  \code
    void QCloudMessagingEmbeddedKaltiotRest::xmlHttpRequestReply(QNetworkReply *reply)
    {
        getNetworkManager()->disconnect(SIGNAL(finished(QNetworkReply *)));

        QString m_msg_uuid = reply->property("uuid").toString();
        int req_id = reply->property("req_id").toInt();

        if (reply->error()) {
            Q_EMIT xmlHttpRequestError(reply->errorString());
        }

        // Ok message, lets proceed and read all
        QByteArray data(reply->readAll());

        // Make decisions about req_id
        switch (req_id) {
            case REQ_GET_ALL_DEVICES: {
                emit remoteClientsReceived(QString::fromUtf8(data));
            }
        }

        reply->deleteLater();

        clearMessage(m_msg_uuid);

    }
  \endcode
  \param reply
  provides QNetworkReply for reading data from the network response.


*/

/*!
  \fn QCloudMessagingRestApi::provideAuthentication(QNetworkReply *reply, QAuthenticator *authenticator)
  This slot is executed when network request requires authentication.
  This is virtual function which needs to be implemented by inheriting class if
  created service is know to request specific authentications. Otherwise
  provideAuthentication request is omitted.

  \param reply
  provides QNetworkReply for reading data from the network response.

  \param authentication
  QAuthenticator pointer to provide username and password back.
*/

// Enums
/*!
    \enum QCloudMessagingRestApi::MessageType

    This enum type describes types of QCloudMessagingRestApi xmlHttpRequest
    messaging types.

    \value NO_TYPE  No type.
    \value POST_MSG  Post message type for uploading data to server.
    \value GET_MSG  Get message type for requesting data from the server.
    \value PUT_MSG  Put message type for updating data in the server.
    \value DELETE_MSG  Delete message type for deleting data from the server.
*/

QT_END_NAMESPACE
