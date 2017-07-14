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

#ifndef QTCLOUDMESSAGINGRESTAPI_H
#define QTCLOUDMESSAGINGRESTAPI_H

#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QtCloudMessaging/qcloudmessagingprovider.h>

#include <QObject>
#include <QNetworkRequest>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;
class QAuthenticator;
class QNetworkReply;


class QCloudMessagingNetworkMessage
{
public:
    int type;
    int req_id;
    QString uuid;
    QNetworkRequest request;
    QByteArray data;
    QString related_uuid;
    QString info;
    int retry_count;

};

class QCloudMessagingRestApiPrivate;

class Q_CLOUDMESSAGING_EXPORT QCloudMessagingRestApi : public QObject
{
    Q_OBJECT

public:

    enum MessageType{
        NO_TYPE = 0,
        POST_MSG,
        GET_MSG,
        PUT_MSG,
        DELETE_MSG
    };
    Q_ENUM(MessageType)

    explicit QCloudMessagingRestApi(QObject *parent = nullptr);

    ~QCloudMessagingRestApi();


    bool sendMessage(MessageType type, int req_id, QNetworkRequest request,
                     QByteArray data, int immediate,
                     const QString &related_uuid);

    void sendNetworkMessage(const QCloudMessagingNetworkMessage &msg,
                        int immediate);

    void clearMessageBuffer();

    int getNetworkRequestCount();

    bool getOnlineState();

    QNetworkAccessManager *getNetworkManager();

    void setServerTimers(int messageTimer,
                         int messageRetryCount,
                         int messageNoResponseTimer);

    void clearMessage(const QString &msg_uuid);

    QNetworkReply *xmlHttpPostRequest(QNetworkRequest request,
                                      QByteArray data,
                                      int req_id,
                                      const QString &uuid,
                                      const QString &info);

    QNetworkReply *xmlHttpGetRequest(QNetworkRequest request,
                                     int req_id,
                                     const QString &uuid,
                                     const QString &info);

    QNetworkReply *xmlHttpPutRequest(QNetworkRequest request,
                                     QByteArray data,
                                     int req_id,
                                     const QString &uuid,
                                     const QString &info);

    QNetworkReply *xmlHttpDeleteRequest(QNetworkRequest request,
                                        int req_id,
                                        const QString &uuid,
                                        const QString &info);
Q_SIGNALS:
    void xmlHttpRequestError(const QString &errorString);

public Q_SLOTS:
    virtual void xmlHttpRequestReply(QNetworkReply *reply) = 0;
    virtual void provideAuthentication(QNetworkReply *reply, QAuthenticator *authenticator);

private Q_SLOTS:
    void networkMsgTimerTriggered();
    void onlineStateChanged(bool online);

private:
    void append_network_request(int req_id, const QString &param, QVariant data);

    QScopedPointer<QCloudMessagingRestApiPrivate> d;

};

QT_END_NAMESPACE

#endif // QTCLOUDMESSAGINGRESTAPI_H
