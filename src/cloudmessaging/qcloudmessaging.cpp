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

#include "qcloudmessaging.h"
#include "qcloudmessaging_p.h"
#include <QString>


/*!
    \class QCloudMessaging
    \inmodule QtCloudMessaging
    \since 5.11

    \brief The QtCloudMessaging module implements Qt wrapper API for multiple
    cloud messaging and cloud service providers. The QtCloudMessaging module
    can be easily extended to support private clouds or existing cloud
    providers.

    The QtCloudMessaging module currently has Google
    Firebase Cloud Messaging and Kaltiot IoT push messaging backends to
    get started with IoT or mobile cloud development.

    QCloudMessaging class enables registering of cloud messaging
    provider backends and backend clients.

    Cloud messaging providers and clients implement virtual functions which
    subscribe or unsubscribe to cloud messaging channels and receive or send
    messages to servers and to other clients.

    \list
    \li To create new cloud messaging service provider and client:
        \list
        \li Inherit the QCloudMessagingProvider class and implement the virtual
        functions.
        \li Inherit the QCloudMessagingClient class and implement the virtual
        functions.
        \endlist
    \endlist

*/

QT_BEGIN_NAMESPACE

/*!
 * \brief QCloudMessaging::QCloudMessaging
 *  QCloudMessaging constructor
 *
 * \param parent
 * Parent is QObject
 */
QCloudMessaging::QCloudMessaging(QObject *parent) :
    QObject(parent), d(new QCloudMessagingPrivate)
{
}
/*!
 * \brief QCloudMessaging::~QCloudMessaging
 */
QCloudMessaging::~QCloudMessaging()
{
}

/*!
 * \brief registerProvider
 * Registers the service provider that handles service clients and can be used
 * for server side development.
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API.
 *
 * \param provider
 * Use case specific provider object that can handle the communication as a
 * server and can have multiple internal and external clients.
 *
 * \param parameters
 * Provider specific parameters in a variant map.
 *
 * \return
 * true if register succeeds, false if fails.
 */
bool QCloudMessaging::registerProvider(const QString &providerId,
                                       QCloudMessagingProvider *provider,
                                       const QVariantMap &parameters)
{
    bool return_value = false;
    // If this is duplicate service name
    if (!d->m_cloudProviders.contains(providerId)) {
        d->m_cloudProviders.insert(providerId, provider);

        connect(provider, &QCloudMessagingProvider::messageReceived,
                this, &QCloudMessaging::messageReceived);

        connect(provider, &QCloudMessagingProvider::serviceStateUpdated,
                this, &QCloudMessaging::serviceStateUpdated);

        connect(provider, &QCloudMessagingProvider::remoteClientsReceived,
                this, &QCloudMessaging::remoteClientsReceived);

        connect(provider, &QCloudMessagingProvider::clientTokenReceived,
                this, &QCloudMessaging::clientTokenReceived);

        return_value = d->m_cloudProviders[providerId]->
                registerProvider(providerId,parameters);
    } else {
        return_value = d->m_cloudProviders[providerId]->getServiceState();
    }

    return return_value;
}

/*!
 * \brief connectClient
 * Attaches the client into the provider
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for the
 * provider
 *
 * \param parameters
 * Client specific parameters in a variant map.
 *
 * \return
 * return given ClientId when succeeds, empty string if not.
 *
 */
QString QCloudMessaging::connectClient(const QString &providerId,
                                       const QString &clientId,
                                       const QVariantMap &parameters)
{
    if (d->m_cloudProviders.contains(providerId))
        return d->m_cloudProviders[providerId]->connectClient(clientId,
                                                              parameters);

    return QString();
}

/*!
 * \brief sendMessage
 * Sends a message to one single client or or to a subscribed channel
 *
 * \param msg
 * Service specific message. Usually JSON string.
 *
 * \param providerId
 * Provider identification string that is defined by the user when using
 * the API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for
 * the provider
 *
 * \param clientToken
 * By providing client token, message is targeted straight to client
 *
 * \param channel
 * Channel name if broadcasting the message to channel
 *
 * \return
 * return true when succeeds, false otherwise.
 */
bool QCloudMessaging::sendMessage(const QByteArray &msg,
                                  const QString &providerId,
                                  const QString &clientId,
                                  const QString &clientToken,
                                  const QString &channel)
{

    if (d->m_cloudProviders.contains(providerId))
        return d->m_cloudProviders[providerId]->sendMessage(msg,
                                                            clientId,
                                                            clientToken,
                                                            channel);

    return false;
}


/*!
 * \brief disconnectClient
 * Disconnects the client from the provider
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for the
 * provider
 *
 * \param parameters
 * Client specific parameters in a variant map.
 */
void QCloudMessaging::disconnectClient(const QString &providerId,
                                       const QString &clientId,
                                       const QVariantMap &parameters)
{
    if (d->m_cloudProviders.contains(providerId))
        d->m_cloudProviders[providerId]->disconnectClient(clientId,
                                                          parameters);
}

/*!
 * \brief removeClient
 * Removes client from the provider
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for the
 * provider
 */
void QCloudMessaging::removeClient(const QString &providerId,
                                   const QString &clientId)
{
    if (d->m_cloudProviders.contains(providerId))
        d->m_cloudProviders[providerId]->removeClient(clientId);

}

/*!
 * \brief deRegisterProvider
 * Closes down the provider and disconnects clients.
 *
 * \param providerId
 * Povider identification string that is defined by the user when using the
 * API
 */
void QCloudMessaging::deregisterProvider(const QString &providerId)
{
    if (d->m_cloudProviders.contains(providerId)) {
        disconnect(d->m_cloudProviders[providerId]);

        d->m_cloudProviders[providerId]->deregisterProvider();
        d->m_cloudProviders.remove(providerId);

    }
}

/*!
 * \brief localClients
 * If system has multiple clients connected at the same app process, one can
 * request the
 * list of available clientIds from the provider class.
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \return
 * List of ClientIds as QStringList.
 */
const QStringList QCloudMessaging::localClients(const QString &providerId)
{
    if (d->m_cloudProviders.contains(providerId))
        return d->m_cloudProviders[providerId]->clients()->keys();

    return QStringList();
}

/*!
 *  \brief requestRemoteClients
 * Uses provider rest api interface to request all the remote clients
 * connected to provider service.
 * This can be used e.g. in server implementation or if creating
 * client dashboard etc.
 *
 *  \param providerId
 * Provider identification string that is defined by the user when
 * using the API
 *
 *  \return
 * true if sending request via rest api succeeds, fail if not.
 */
bool QCloudMessaging::requestRemoteClients(const QString &providerId)
{
    if (d->m_cloudProviders.contains(providerId)) {
        return d->m_cloudProviders[providerId]->remoteClients();
    }

    return false;
}

/*!
 * \brief clientToken
 * Gets client token (uuid / rid received from the provider services)
 *
 * \param providerI
 * Provider identification string that is defined by the user when using
 * the API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for
 * the provider
 *
 * \return
 * If found returns client token as QString. Returns empty QString otherwise.
 */
QString QCloudMessaging::clientToken(const QString &providerId,
                                              const QString &clientId)
{
    if (d->m_cloudProviders.contains(providerId))
        return d->m_cloudProviders[providerId]->clientToken(clientId);

    return QString();
}

/*!
 * \brief setClientToken
 * Sets the client token for the client.
 * This can be used in case of client
 * token is received otherwise or defined by
 * API user separately.
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for the
 * provider
 *
 * \param token
 * Token value as string
 */
void QCloudMessaging::setClientToken(const QString &providerId,
                                     const QString &clientId,
                                     const QString &token)
{
    if (d->m_cloudProviders.contains(providerId))
        return d->m_cloudProviders[providerId]->
                client(clientId)->setClientToken(token);
}

/*!
 * \brief QCloudMessaging::subscribeToChannel
 *  Subscribing the client to the channel
 *
 * \param channel
 *  Channel name as string, cannot be empty
 *
 * \param providerId
 *  Provider identification string that is defined by the user when using the API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for
 * the provider
 *
 * \return
 * true if succeeds, false if not.
 */
bool QCloudMessaging::subscribeToChannel(const QString &channel,
                                         const QString &providerId,
                                         const QString &clientId)
{
    if (!d->m_cloudProviders.contains(providerId))
        return false;

    if (!clientId.isEmpty())
    {
        return d->m_cloudProviders[providerId]->client(clientId)->
                                                subscribeToChannel(channel);
    } else
    {
        return d->m_cloudProviders[providerId]->subscribeToChannel(channel,
                                                                  clientId);
    }
}

/*!
 * \brief unsubscribeFromChannel
 * Unsubscribing the client from the channel
 *
 * \param channel
 * Channel name as string, cannot be empty
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 *  API
 *
 * \param clientId
 * Mobile or IoT client identification string (defined by user) added for the
 *  provider
 *
 * \return
 * true if succeeds, false if not.
 */
bool QCloudMessaging::unsubscribeFromChannel(const QString &channel,
                                             const QString &providerId,
                                             const QString &clientId)
{
    if (!d->m_cloudProviders.contains(providerId))
        return false;

    if (!clientId.isEmpty())
    {
        return d->m_cloudProviders[providerId]->client(clientId)->
                                                unsubscribeFromChannel(channel);
    } else
    {
        return d->m_cloudProviders[providerId]->unsubscribeFromChannel(channel,
                                                                      clientId);
    }
}

/*!
 * \brief flushMessageQueue
 * When receiving push messages they can be stored by clients internally.
 * This function gives developers possibility to do flush commnand for them .
 *
 * \param providerId
 * Provider identification string that is defined by the user when using the
 * API
 *
 * \return
 * true if succeeds, false if not
 */
void QCloudMessaging::flushMessageQueue(const QString &providerId)
{
    if (d->m_cloudProviders.contains(providerId))
        d->m_cloudProviders[providerId]->flushMessageQueue();
}

// Signals documentation
/*!
    \fn QCloudMessaging::clientTokenReceived(const QString &token)
    This signal is triggered when connected gets the client
    token from the service provider.

    \param token
    Received token as a QString.
    Token is unique identification value used for straight communication
    and identification with the client.
*/

/*!
    \fn QCloudMessaging::messageReceived(const QString &providerId,
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
    \fn QCloudMessaging::remoteClientsReceived(const QString &clients)
    This signal is triggered when the return value for requestRemoteClients
    function is is received.
    \param response
    Response data is based on the service and can be e.g. a list
    of client tokens in QString format.
*/

/*!
    \fn QCloudMessaging::serviceStateUpdated(int state)
    This signal is triggered when the service provider registered
    state has changed.

    \param state
    State can be one of the enums from the QCloudMessagingProvider

*/

QT_END_NAMESPACE
