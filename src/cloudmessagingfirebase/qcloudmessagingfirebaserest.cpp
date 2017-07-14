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
#include "qcloudmessagingfirebaserest.h"

#include <QByteArray>

/* REST API INTERFACE */
const QString SERVER_ADDRESS = QStringLiteral("https://fcm.googleapis.com/fcm/send");

/*!
 * \brief FirebaseRestServer::sendToDevice
 * \param token
 * \param data
 * \return
 */
bool FirebaseRestServer::sendToDevice(const QString &token, const QByteArray &data)
{
    QString data_to_send = "{\"to\":\"" + token + "\",\"data\":" + QString::fromUtf8(data) + "}";
    QString url = SERVER_ADDRESS;
    QUrl uri(url);
    m_auth_key = "key=" + m_auth_key;
    QNetworkRequest request(uri);
    QString authHeader = QString::fromLatin1("Authorization");
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/json"));
    request.setRawHeader(authHeader.toLocal8Bit(), m_auth_key.toLocal8Bit());

    return sendMessage(POST_MSG,
                       REQ_SEND_BROADCAST_DATA_TO_CHANNEL,
                       request,
                       data_to_send.toUtf8(),
                       true,
                       QString());
}

/*!
 * \brief FirebaseRestServer::sendBroadcast
 * \param channel
 * \param data
 * \return
 */
bool FirebaseRestServer::sendBroadcast(const QString &channel, const QByteArray &data)
{
    QString mod_data = QString::fromUtf8(data);
    if (mod_data[0] == '{')
        mod_data.remove(0, 1);
    if (mod_data[mod_data.length() - 1] == '}')
        mod_data.remove(mod_data.length() - 1, 1);

    QString data_to_send = "{\"to\":\"/topics/" + channel + "\"," + mod_data + "}";

    QString url = SERVER_ADDRESS;
    QUrl uri(url);
    QString auth = "key=" + m_auth_key;
    QNetworkRequest request(uri);
    QString authHeader = QString::fromLatin1("Authorization");
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/json"));
    request.setRawHeader(authHeader.toLocal8Bit(), auth.toLocal8Bit());

    return sendMessage(POST_MSG,
                       REQ_SEND_BROADCAST_DATA_TO_CHANNEL,
                       request,
                       data_to_send.toUtf8(),
                       true,
                       QString());

}

/*!
 * \brief FirebaseRestServer::xmlHttpRequestReply
 * \param reply
 */
void FirebaseRestServer::xmlHttpRequestReply(QNetworkReply *reply)
{
    getNetworkManager()->disconnect(SIGNAL(finished(QNetworkReply *)));
    QString m_msg_uuid = reply->property("uuid").toString();
    int req_id = reply->property("req_id").toInt();

    if (reply->error()) {
        emit xmlHttpRequestError(reply->errorString());

    }

    // Ok message, lets proceed.

    QByteArray data(reply->readAll());

    emit xmlHttpRequestReplyData(data);

    reply->deleteLater();
    clearMessage(m_msg_uuid);
}
