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

#include "qcloudmessagingprovider.h"
#include "qcloudmessagingprovider_p.h"
#include <QMapIterator>

/*!
    \class QCloudMessagingProvider
    \inmodule QtCloudMessaging
    \since 5.11

    \brief The QCloudMessagingProvider class provides the functions to register
    multiple cloud messaging providers and clients. CloudMessagingProvider class
    manages locally registered cloud messaging clients and provides the
    status of the services.

    To implement specific cloud messaging provider functionality, the inherit of this
    QCloudMessagingProvider class is needed. Inherited class also needs to implement the
    QCloudMessagingProvider virtual functions.

*/

QT_BEGIN_NAMESPACE

/*!
 * \brief QCloudMessagingProvider::QCloudMessagingProvider
 * Class constructor
 *
 * \param parent
 * QObject parent
 */
QCloudMessagingProvider::QCloudMessagingProvider(QObject *parent) :
                                                 QObject(parent),
                                                 d(new QCloudMessagingProviderPrivate)
{
}

/*!
 * \brief QCloudMessagingProvider::~QCloudMessagingProvider
 * Class descructor
 */
QCloudMessagingProvider::~QCloudMessagingProvider()
{
}

/*!
 * \brief QCloudMessagingProvider::registerProvider
 * Registers the service provider that handles service clients and can be used
 * for server side development.
 * This is pure virtual function which needs to be implemented by the provider
 * class inheritance.
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \param parameters
 * Provider specific parameters in the variant map.
 *
 * \return
 */
bool QCloudMessagingProvider::registerProvider(const QString &providerId,
                                               const QVariantMap &parameters)
{
    Q_UNUSED(parameters);

    d->m_providerId = providerId;

    d->m_serviceState = CloudMessagingProviderState::QtCloudMessagingProviderRegistering;
    emit serviceStateUpdated(d->m_serviceState);
    return true;
}

/*!
 * \brief QCloudMessagingProvider::connectClientToProvider
 * Function to create necessary signal connections between the new service client
 * and calls serviceClients connectClient virtual function.
 * This function should be called from the inherited class connectClient function.
 *
 * Example code:
 * \code
 * QCloudMessagingEmbeddedKaltiotClient *serviceClient = new QCloudMessagingEmbeddedKaltiotClient();
 * QString retval = connectClientToProvider(clientId, parameters, serviceClient);
 * \endcode
 *
 * \param clientId
 * Client id as QString
 *
 * \param parameters
 * Client specific parameters in a variant map.
 *
 * \param serviceClient
 * Service client instance of inheritance of QCloudMessagingClient class
 *
 * \return
 * Returns clientId as QString if succesfull, empty QString otherwise.
 */
QString QCloudMessagingProvider::connectClientToProvider(const QString &clientId,
                                                         const QVariantMap &parameters,
                                                         QCloudMessagingClient *serviceClient)
{
    if (!d->m_providerId.isEmpty()) {
        d->m_provider_parameters = parameters;
        if (!clientId.isEmpty()) {

            connect(serviceClient, &QCloudMessagingClient::messageReceived,
                    this, &QCloudMessagingProvider::messageReceivedSlot);

            connect(serviceClient, &QCloudMessagingClient::clientTokenReceived,
                    this, &QCloudMessagingProvider::clientTokenReceived);

            connect(serviceClient, &QCloudMessagingClient::clientStateChanged,
                    this, &QCloudMessagingProvider::clientStateChanged);

            serviceClient->connectClient(clientId, parameters);

            d->m_QtCloudMessagingClients.insert(clientId, serviceClient);

            return clientId;
        }
    }

    return QString();
}

/*!
 * \brief QCloudMessagingProvider::messageReceivedSlot
 * This slot is executed when new message is received from the service provider.
 *
 * \param clientId
 * Client id receiving the message
 *
 * \param message
 * Message content as QByteArray
 *
 */
void QCloudMessagingProvider::messageReceivedSlot(const QString &clientId, const QByteArray &message)
{
    emit messageReceived(providerId(), clientId, message);
}

/*!
 * \brief QCloudMessagingProvider::disconnectClient
 * \param clientId
 * \param parameters
 * \return
 */
void QCloudMessagingProvider::disconnectClient(const QString &clientId,
                                               const QVariantMap &parameters)
{
    Q_UNUSED(parameters);

    if (!d->m_providerId.isEmpty() && d->m_QtCloudMessagingClients[clientId]) {
        QCloudMessagingClient *client = d->m_QtCloudMessagingClients[clientId];

        // Disconnect connections for the client
        disconnect(client);

        d->m_QtCloudMessagingClients[clientId]->disconnectClient();
    }

}

/*!
 * \brief QCloudMessagingProvider::removeClient
 * \param clientId
 * \return
 */
bool QCloudMessagingProvider::removeClient(const QString &clientId)
{
    if (!d->m_providerId.isEmpty() && d->m_QtCloudMessagingClients[clientId]) {

        disconnectClient(clientId);
        d->m_QtCloudMessagingClients.remove(clientId);

        return true;
    }

    return false;
}

/*!
 * \brief QCloudMessagingProvider::deregisterProvider
 * Function to deregistering the service provider. This
 * function automatically removes clients registered to the provider.
 * This is virtual function and can be implemented by the provider
 * class inheritance.
 *
 * \return
 */
void QCloudMessagingProvider::deregisterProvider()
{
    if (d->m_QtCloudMessagingClients.count()) {

        QMapIterator<QString, QCloudMessagingClient *> i(d->m_QtCloudMessagingClients);

        while (i.hasNext()) {
            i.next();
            removeClient(i.key());
        }

        d->m_serviceState = CloudMessagingProviderState::QtCloudMessagingProviderNotRegistered;
        emit serviceStateUpdated(d->m_serviceState);
    }
}

/*!
 * \brief QCloudMessagingProvider::flushMessageQueue
 * This function calls the service provider to clear clients message buffers.
 * Client flushMessageQueue is virtualized for client specific usages.
 *
 * \return
 * false if no clients found, true if also clients removed.
 */
bool QCloudMessagingProvider::flushMessageQueue()
{
    if (d->m_QtCloudMessagingClients.count()) {
        QMapIterator<QString, QCloudMessagingClient *> i(d->m_QtCloudMessagingClients);
        while (i.hasNext()) {
            i.next();
            d->m_QtCloudMessagingClients[i.key()]->flushMessageQueue();
        }
        return true;
    }
    return false;
}

/*!
 * \brief QCloudMessagingProvider::clientToken
 * Get the clientToken from the client
 *
 * \param clientId
 * ClientId as string.
 *
 * \return
 * Return clientId token as string if found. Empty string if client not found.
 */
QString QCloudMessagingProvider::clientToken(const QString &clientId)
{
    if (!d->m_providerId.isEmpty() && d->m_QtCloudMessagingClients[clientId]) {
        return d->m_QtCloudMessagingClients[clientId]->clientToken();

    }
    return QString();
}

/*!
 * \brief QCloudMessagingProvider::clients
 * Get client instances as key value pairs, where clientId is the key.
 *
 * \return
 * Client instances as key value pairs
 */
QMap <QString, QCloudMessagingClient *>  *QCloudMessagingProvider::clients()
{
    return &d->m_QtCloudMessagingClients;
}

/*!
 * \brief QCloudMessagingProvider::client
 * Finds the client instance based on the clientId
 *
 * \param clientId
 * ClientId as QString
 *
 * \return
 * Client instance if found, nullptr if not found.
 */
QCloudMessagingClient *QCloudMessagingProvider::client(const QString &clientId)
{
    if (d->m_QtCloudMessagingClients.find(clientId) == d->m_QtCloudMessagingClients.end())
        return nullptr;

    return d->m_QtCloudMessagingClients[clientId];
}

/*!
 * \brief QCloudMessagingProvider::setServiceState
 * Sets the provider service state
 *
 * \param state
 * State value from the enum CloudMessagingProviderState.
 *
 * \return
 * Returns the given state back.
 */
QCloudMessagingProvider::CloudMessagingProviderState QCloudMessagingProvider::
    setServiceState(QCloudMessagingProvider::CloudMessagingProviderState state)
{
    d->m_serviceState = state;
    return (QCloudMessagingProvider::CloudMessagingProviderState)d->m_serviceState;
}

/*!
 * \brief QCloudMessagingProvider::getServiceState
 * Gets the service provider state
 *
 * \return
 *  Return the current state value as CloudMessagingProviderState.
 */
QCloudMessagingProvider::CloudMessagingProviderState QCloudMessagingProvider::getServiceState()
{
    return (QCloudMessagingProvider::CloudMessagingProviderState) d->m_serviceState;
}

/*!
 * \brief QCloudMessagingProvider::providerId
 * Gets the service provider Id
 *
 * \return
 * return service provider id as a QString
 */
QString QCloudMessagingProvider::providerId()
{
    return d->m_providerId;
}

/*!
 * \brief QCloudMessagingProvider::setProviderId
 * Sets the service provider Id
 *
 * \param providerId
 * Service provider id as QString
 */
void QCloudMessagingProvider::setProviderId(const QString &providerId)
{
    d->m_providerId = providerId;
}

// Pure Virtual functions documentation
/*!
    \fn QCloudMessagingProvider::connectClient(
    const QString &clientId,
    const QVariantMap &parameters = QVariantMap())
    Connects the client to provider. This is pure virtual function
    and needs to be implemented in the inheritance.

    \param clientId
    Mobile or IoT client identification string (defined by user) added for the
    provider

    \param parameter
    Client specific parameters in a variant map.

    \return
    return given ClientID when successful, empty string otherwise.
*/
/*!
    \fn QCloudMessagingProvider::disconnectClient(
    const QString &clientId,
    const QVariantMap &parameters = QVariantMap())
    Disconnects the client from the provider. This is pure virtual function
    and needs to be implemented in the inheritance.

    \param clientId
    Mobile or IoT client identification string (defined by user) added for the
    provider

    \param parameter
    Client specific parameters in a variant map.

    \return
    return true when successful, false otherwise.
*/

/*!
    \fn QCloudMessagingProvider::sendMessage(
        const QByteArray &msg,
        const QString &clientId = QString(),
        const QString &clientToken = QString(),
        const QString &channel = QString())
    Sends a message to one single client or to subscribed channel.
    This is pure virtual function and needs to be implemented in the inheritance.

    \param msg
    Message as string which is interpreted to the service specific message
    type e.g. json

    \param clientId
    Mobile or IoT client identification string (defined by user) added for the
    provider

    \param clientToken
    By providing client token, message is targeted straight to client

    \param channel
    Channel name if broadcasting the message to channel

    \return
    return true when successful, false otherwise.
*/

/*!
    \fn QCloudMessagingProvider::subscribeToChannel(
        const QString &channel,
        const QString &clientId = QString())
    Subscribes client to broadcast channel of the provider service.
    This is pure virtual function and needs to be implemented in the
    inheritance.

    \param channnel
    Channel name as QString

    \param clientId
    Client id as QString

    \return
    return true when successful, false otherwise.
*/

/*!
    \fn QCloudMessagingProvider::unsubscribeFromChannel(
        const QString &channel,
        const QString &clientId = QString())
    Unsubscribes client from the broadcast channel of the provider service.
    This is pure virtual function and needs to be implemented in the
    inheritance.

    \param channnel
    Channel name as QString

    \param clientId
    Client id as QString

    \return
    return true when successful, false otherwise.
*/

/*!
    \fn QCloudMessagingProvider::remoteClients()
    Requests remote clients from the provider services.
    This is pure virtual function and needs to be implemented in the
    inheritance.

    \return
    return true if request successed, false otherwise
*/


// Signals documentation
/*!
    \fn QCloudMessagingProvider::clientTokenReceived(const QString &token)
    This signal is triggered when connected gets the client
    token from the service provider.

    \param token
    Received token as a QString.
    Token is unique identification value used for straight communication
    and identification with the client.
*/

/*!
    \fn QCloudMessagingProvider::messageReceived(const QString &providerId,
                        const QString &clientId,
                        const QByteArray &message)
    This signal is triggered when a message is received from the network to client.

    \param providerId
    Receiving Provider identification string

    \param clientId
    Receiving clientId string

    \param message
    Received message as QByteArray. Message is service specific.
*/

/*!
    \fn QCloudMessagingProvider::remoteClientsReceived(const QString &clients)
    This signal is triggered when the return value for requestRemoteClients
    function is is received.

    \param response
    Response data is based on the service and can be e.g. a list
    of client tokens in QString format.
*/

/*!
    \fn QCloudMessagingProvider::serviceStateUpdated(int state)
    This signal is triggered when the service provider register
    state has changed.

    \param state
    State can be one of the enums from the QCloudMessagingProvider

*/

/*!
    \fn QCloudMessagingProvider::clientStateChanged(
                                    const QString &clientId,
                                    int state)
    This signal is triggered when the client state has changed

    \param clientId
    Client idenfitication string

    \param state
    Client State can be either enum from QCloudMessagingClient::CloudMessagingClientState
    or it can be client backend specific state.

*/

// Public slots documentation


// Enums
/*!
    \enum QCloudMessagingProvider::CloudMessagingProviderState

    This enum type describes types of QCloudMessagingProvider connection states.

    \value QtCloudMessagingProviderNotRegistered  Messaging Provider is not registered.
    \value QtCloudMessagingProviderRegistering  Messaging Provider register started.
    \value QtCloudMessagingProviderRegistered  Messaging Provider registered.
*/

QT_END_NAMESPACE
