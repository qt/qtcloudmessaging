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
#ifndef QCLOUDMESSAGINGFIREBASEREST_H
#define QCLOUDMESSAGINGFIREBASEREST_H

#include <QtCloudMessaging/QtCloudMessaging>
#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QtCloudMessaging/qcloudmessagingrestapi.h>
#include <QtCloudMessagingFirebase/qcloudmessagingfirebaseclient.h>

QT_BEGIN_NAMESPACE

class FirebaseRestServer : public QCloudMessagingRestApi
{
    Q_OBJECT
public:
    enum FirebaseRESTRequests {
        REQ_NO_REQ,
        REQ_GET_DEVICES_BY_CUSTOMER_ID,
        REQ_GET_ALL_DEVICES,
        REQ_SEND_DATA_TO_DEVICE,
        REQ_SEND_BROADCAST_DATA_TO_CHANNEL,
        REQ_GET_DEVICE_INFO
    };
    Q_ENUM(FirebaseRESTRequests)

    void setAuthKey(const QString &key)
    {
        m_auth_key = key;
    }

    // Response function
    void  xmlHttpRequestReply(QNetworkReply *reply);

    bool sendToDevice(const QString &token, const QByteArray &data);
    bool sendBroadcast(const QString &channel, const QByteArray &data);

Q_SIGNALS:
    void xmlHttpRequestReplyData(const QByteArray &data);

private:
    QString m_auth_key;
};

QT_END_NAMESPACE

#endif // QCLOUDMESSAGINGFIREBASEREST_H
