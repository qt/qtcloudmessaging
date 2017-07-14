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
#include <QtCloudMessaging/QtCloudMessaging>
#include "qtcloudmessagingglobal.h"
#include "qcloudmessagingembeddedkaltiotrest.h"

#include <QObject>
#include <QByteArray>

QT_BEGIN_NAMESPACE

/* REST API INTERFACE */
const QString SERVER_ADDRESS = QStringLiteral("https://restapi.torqhub.io");

/*!
 * \brief QCloudMessagingEmbeddedKaltiotRest::getAllDevices
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotRest::getAllDevices()
{
    QString url = SERVER_ADDRESS + "/rids/identities" + "?ApiKey=" + m_auth_key;
    QUrl uri(url);
    QNetworkRequest request(uri);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=ISO-8859-1");

    return sendMessage(GET_MSG, REQ_GET_ALL_DEVICES, request, "", true, "");
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotRest::sendDataToDevice
 * \param rid
 * \param data
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotRest::sendDataToDevice(const QString &rid, const QByteArray &data)
{

    QString url = SERVER_ADDRESS + "/rids/" + rid + "?ApiKey=" + m_auth_key;
    QUrl uri(url);
    QNetworkRequest request(uri);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=ISO-8859-1");

    return sendMessage(POST_MSG, REQ_SEND_DATA_TO_DEVICE, request, data, true, "");
}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotRest::sendBroadcast
 * \param channel
 * \param data
 * \return
 */
bool QCloudMessagingEmbeddedKaltiotRest::sendBroadcast(const QString &channel, const QByteArray &data)
{

    QString url = SERVER_ADDRESS + "/rids/channel/" + channel + "?ApiKey=" + m_auth_key;
    QUrl uri(url);
    QNetworkRequest request(uri);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=ISO-8859-1");

    return sendMessage(POST_MSG, REQ_SEND_BROADCAST_DATA_TO_CHANNEL, request, data, true, "");

}

/*!
 * \brief QCloudMessagingEmbeddedKaltiotRest::xmlHttpRequestReply
 * \param reply
 */
void QCloudMessagingEmbeddedKaltiotRest::xmlHttpRequestReply(QNetworkReply *reply)
{

    getNetworkManager()->disconnect(SIGNAL(finished(QNetworkReply *)));
    QString m_msg_uuid = reply->property("uuid").toString();
    int req_id = reply->property("req_id").toInt();

    if (reply->error()) {
        emit xmlHttpRequestError(reply->errorString());

    }

    // Ok message, lets proceed.

    QByteArray data(reply->readAll());

    switch (req_id) {
    case REQ_GET_DEVICES_BY_CUSTOMER_ID:

        break;
    case REQ_GET_ALL_DEVICES: {
        Q_EMIT remoteClientsReceived(QString::fromUtf8(data));
    }
    break;
    case REQ_SEND_DATA_TO_DEVICE:

        break;
    case REQ_SEND_BROADCAST_DATA_TO_CHANNEL:

        break;
    case REQ_GET_DEVICE_INFO:

        break;
    }

    reply->deleteLater();
    clearMessage(m_msg_uuid);

}

// Signals documentation
/*!
    \fn QCloudMessagingEmbeddedKaltiotRest::remoteClientsReceived(const QString &clients)
    This signal is triggered when the return value for requestRemoteClients
    function is is received
.
    \param response
    Response data is based on the service and can be e.g. a list
    of client tokens in QString format.
*/
QT_END_NAMESPACE
