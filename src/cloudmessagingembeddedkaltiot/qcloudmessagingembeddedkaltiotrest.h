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
#ifndef QCLOUDMESSAGINGEMBEDDEDKALTIOTREST_H
#define QCLOUDMESSAGINGEMBEDDEDKALTIOTREST_H

#include <QtCloudMessaging/QtCloudMessaging>
#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QtCloudMessaging/qcloudmessagingrestapi.h>
#include <QtCloudMessagingEmbeddedKaltiot/qcloudmessagingembeddedkaltiotclient.h>
#include <QObject>

QT_BEGIN_NAMESPACE

class QCloudMessagingEmbeddedKaltiotRest : public QCloudMessagingRestApi
{
    Q_OBJECT
public:
    enum KaltiotRESTRequests {
        REQ_NO_REQ,
        REQ_GET_DEVICES_BY_CUSTOMER_ID,
        REQ_GET_ALL_DEVICES,
        REQ_SEND_DATA_TO_DEVICE,
        REQ_SEND_BROADCAST_DATA_TO_CHANNEL,
        REQ_GET_DEVICE_INFO
    };
    Q_ENUM(KaltiotRESTRequests)

    void setAuthKey(QString key)
    {
        m_auth_key = key;
    }

    /* KALTIOT REST API
     *
     * Get Identities
     * GET /rids/identities/customerId/:cust_id - Your device identities by customer id
     * GET /rids/identities/paginStart/:paginStart/paginCount/:paginCount - Your devices identities with pagination
     * GET /rids/identities/timeFrom/:timeFrom/timeTo/:timeTo - Your device identities by creation timestamps
     * GET /rids/identities - All your devices identities
     *
     * Create persistence sessions
     * POST /cn/:cn/data_stream - Create persistent web socket session to all your devices
     * POST /rids/:rid/data_stream - Create persistent web socket session to single device
     *
     * Get data from devices
     * GET /cn/:cn/data_stream/sessionId/:sessionId - Use persistent web socket to all your devices
     * GET /cn/:cn/data_stream - Use non persistent web socket to all your devices
     * GET /rids/:rid/data_stream/sessionId/:sessionId - Use Persistent web socket to single device
     * GET /rids/:rid/data_stream - Use non persistent web socket to single device
     *
     * Send data to devices
     * POST /rids/:rid - Send data to single device
     * POST /rids/channel/:channel - Send data to group of devices
     *
     * Get Device Information
     * GET /rids/:rid/presence - Get your device presence information
     *
    */


    /* getAllDevices implements
     * GET /rids/identities - All your devices identities
    */
    bool getAllDevices();

    /* Implements
     * POST /rids/:rid - Send data to single device
    */
    bool sendDataToDevice(const QString &rid, const QByteArray &data);

    /* Implements
     * POST /rids/channel/:channel - Send data to group of devices
    */
    bool sendBroadcast(const QString &channel, const QByteArray &data);

    /* Error codes for requests */
    /**
     * {"result": "<Error Message>"}
     * 1 - InvalidJSON         Json used in request was not valid
     * 2 - RidNotFound         RID used in request was not connected
     * 3 - HandshakeFailure    WebSocket handshake was unsuccessful
     * 4 - MessageLost         Message received by RestApi was lost, delivery to client cannot be guaranteed
     * 5 - NoRoute             Message received by RestApi was not able to deliver to client
     * 6 - MalformedRequest    Request received by RestApi was not properly formatted
     * 7 - UriNotFound         Request to URI which was made is not existing
     * 8 - MethodNotAllowed    Http method which was used to access URI is not allowed
     * 9 - DeliveryFailure     Message received by RestApi was not able to deliver to client
     * 10 - UnAuthorized        Apikey which was used was not accepted
     */

    void xmlHttpRequestReply(QNetworkReply *reply);

Q_SIGNALS:
    void remoteClientsReceived(const QString &clients);

private:
    QString m_auth_key;
};

QT_END_NAMESPACE

#endif // QCLOUDMESSAGINGEMBEDDEDKALTIOTREST_H
