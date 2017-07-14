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

#include "qcloudmessagingclient.h"
#include "qcloudmessagingclient_p.h"

/*!
    \class QCloudMessagingClient
    \inmodule QtCloudMessaging
    \since 5.11

    \brief The QCloudMessagingClient class is a single IoT/embedded or mobile device
    providing way to received and send messages either by broadcasting or
    by direct connect to other client with known token.

    Implementing specific cloud messaging service client needs inherit this
    QCloudMessagingClient class and implement the virtual functions defined in the
    header.

*/

QT_BEGIN_NAMESPACE

/*!
 * \brief QCloudMessagingClient::QCloudMessagingClient
 * QCloudMessagingClient constructor
 *
 * \param parent
 * Parent is QObject
 */
QCloudMessagingClient::QCloudMessagingClient(QObject *parent) :
                                             QObject(parent),
                                             d(new QCloudMessagingClientPrivate)
{
}

/*!
 * \brief QCloudMessagingClient::~QCloudMessagingClient
 * Desctructor
 */
QCloudMessagingClient::~QCloudMessagingClient()
{
    disconnectClient();
}

/*!
 * \brief QCloudMessagingClient::connectClient
 * Attaches the client into the provider. This is virtual function
 * and should be re-implemented in the inheritance for connecting
 * the client to real service.
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for the
 * provider
 *
 * \param parameters
 * Client specific parameters in a variant map.
 *
 * \return
 * return given ClientId when successful, empty string if not.
 *
 */
QString QCloudMessagingClient::connectClient(const QString &clientId,
                                             const QVariantMap &parameters)
{
    d->m_clientId = clientId;
    d->m_clientState = QtCloudMessagingClientConnecting;
    d->m_client_parameters = parameters;

    return QString();
}

/*!
 * \brief QCloudMessagingClient::disconnectClient
 * Virtual function to disconnect the client from the service.
 *
 * \return
 * returns the client specific state as integer
 */
void QCloudMessagingClient::disconnectClient()
{
    d->m_clientState = QtCloudMessagingClientDisconnecting;
}

/*!
 * \brief QCloudMessagingClient::setClientState
 * Sets the client state. This is virtual function and can be
 * re-implemented in the inheritance
 *
 * \param state
 * Setting client specific state as integer.
 */
void QCloudMessagingClient::setClientState(int state)
{
    d->m_clientState = state;
}

/*!
 * \brief QCloudMessagingClient::clientState
 * Gets the client specific state
 *
 * \return
 * Returns clients specific state value as integer.
 */
int QCloudMessagingClient::clientState()
{
    return (CloudMessagingClientState) d->m_clientState;
}

/*!
 * \brief QCloudMessagingClient::clientId
 * Gets the client id.
 *
 * \return
 * Returns client id as QString
 */
QString QCloudMessagingClient::clientId()
{
    return d->m_clientId;
}

/*!
 * \brief QCloudMessagingClient::providerId
 * Gets the client's service provider id.
 *
 * \return
 * Returns the provider id as QString.
 */
QString QCloudMessagingClient::providerId()
{
    return d->m_providerId;
}

/*!
 * \brief QCloudMessagingClient::setClientId
 * Sets the client id
 *
 * \param clientid
 * Client id as QString
 *
 * \return
 * Returns new client id as QString
 */
void QCloudMessagingClient::setClientId(const QString &clientid)
{
    d->m_clientId = clientid;
}

/*!
 * \brief QCloudMessagingClient::setProviderId
 * Gets the client's service provider id.
 *
 * \return
 * Returns new provider id as QString.
 */
void QCloudMessagingClient::setProviderId(const QString &providerId)
{
    d->m_providerId = providerId;
}

/*!
 * \brief QCloudMessagingClient::clientParameters
 * Gets the client parameters
 *
 * \return
 * Returns client parameters as QVariantMap
 */
QVariantMap QCloudMessagingClient::clientParameters()
{
    return d->m_client_parameters;
}

// Pure Virtual functions documentation

/*!
    \fn QCloudMessagingClient::flushMessageQueue()
    When receiving push messages they can be stored by clients internally.
    This function gives developers possibility to do flush commnand for them.

    This is pure virtual function and needs to be implemented in the
    inheritance.

    \return
    return true when successful, false otherwise.
*/
/*!
    \fn QCloudMessagingClient::sendMessage(
        const QByteArray &msg,
        const QString &clientToken = QString(),
        const QString &channel = QString())
    Sends a message to one single client or to subscribed channel.
    This is pure virtual function and needs to be implemented in the inheritance.

    \param msg
    Message as string which is interpreted to the service specific message
    type e.g. json

    \param clientToken
    By providing client token, message is targeted straight to client

    \param channel
    Channel name if broadcasting the message to channel

    \return
    return true when successful, false otherwise.
*/

/*!
    \fn QCloudMessagingClient::setClientToken(const QString &token)
    Sets the client token for the client.
    This can be used in case of client
    token is received otherwise or defined by
    API user separately.


    \param token
    Token value as string
*/

/*!
  \fn QCloudMessagingClient::clientToken()
  Gets client token (uuid / rid received from the provider services)

  \return
  If found returns client token as QString. Returns empty QString otherwise.
*/

/*!
    \fn QCloudMessagingClient::subscribeToChannel(
        const QString &channel)
    Subscribes client to broadcast channel of the provider service.
    This is pure virtual function and needs to be implemented in the
    inheritance.

    \param channnel
    Channel name as QString

    \return
    return true when successful, false otherwise.
*/

/*!
    \fn QCloudMessagingClient::unsubscribeFromChannel(
        const QString &channel)
    Unsubscribes client from the broadcast channel of the provider service.
    This is pure virtual function and needs to be implemented in the
    inheritance.

    \param channnel
    Channel name as QString

    \return
    return true when successful, false otherwise.
*/

// Signals documentation
/*!
    \fn QCloudMessagingClient::clientStateChanged(
                                    const QString &clientId,
                                    int state)
    This signal is triggered when the client state has changed

    \param clientId
    Client idenfitication string

    \param state
    Client State can be either enum from QCloudMessagingClient::CloudMessagingClientState
    or it can be client backend specific state.

*/

/*!
    \fn QCloudMessagingClient::messageReceived(const QString &clientId,
                        const QByteArray &message)
    This signal is triggered when a message is received from the network to client.

    \param clientId
    Receiving clientId string

    \param message
    Received message as QByteArray. Message content is service specific.
*/

/*!
    \fn QCloudMessagingClient::clientTokenReceived(const QString &token)
    This signal is triggered when connected gets the client
    token from the service provider.

    \param token
    Received token as a QString.
    Token is unique identification value used for straight communication
    and identification with the client.
*/

// Enums
/*!
    \enum QCloudMessagingClient::CloudMessagingClientState

    This enum type describes types of QCloudMessagingClient connection states.

    \value QtCloudMessgingClientDisconnected  Client is disconnected.
    \value QtCloudMessagingClientConnecting  Client connection started.
    \value QtCloudMessagingClientDisconnecting  Client disconnection started.
    \value QtCloudMessagingClientOffline  Client is connected but offline.
    \value QtCloudMessagingClientOnline  Client is connected and online.

*/
QT_END_NAMESPACE
