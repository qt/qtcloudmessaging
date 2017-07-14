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

#ifndef QTCLOUDMESSAGINGCLIENT_H
#define QTCLOUDMESSAGINGCLIENT_H

#include <QtCloudMessaging/qtcloudmessagingglobal.h>

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class QCloudMessagingClientPrivate;

class Q_CLOUDMESSAGING_EXPORT QCloudMessagingClient : public QObject
{
    Q_OBJECT

public:

    enum CloudMessagingClientState {
        QtCloudMessgingClientDisconnected = 0,
        QtCloudMessagingClientConnecting,
        QtCloudMessagingClientDisconnecting,
        QtCloudMessagingClientOffline,
        QtCloudMessagingClientOnline
    };
    Q_ENUM(CloudMessagingClientState)

    explicit QCloudMessagingClient(QObject *parent = nullptr);

    virtual ~QCloudMessagingClient();

    virtual QString connectClient(
            const QString &clientId,
            const QVariantMap &parameters = QVariantMap());

    virtual void disconnectClient();

    virtual void cloudMessageReceived(const QString &client,
                                      const QByteArray &message) = 0;

    virtual QString clientToken() = 0;

    virtual void setClientToken(const QString &token) = 0;

    virtual bool sendMessage(
            const QByteArray &msg,
            const QString &clientToken =  QString(),
            const QString &channel = QString()) = 0;

    virtual bool flushMessageQueue() = 0;

    virtual bool subscribeToChannel(const QString &channel) = 0;

    virtual bool unsubscribeFromChannel(const QString &channel) = 0;

    void setClientState(int state);

    int clientState();

    QString clientId();

    QString providerId();

    void setClientId(const QString &clientid);

    void setProviderId(const QString &providerId);

    QVariantMap  clientParameters();

Q_SIGNALS:
    void clientStateChanged(const QString &clientId, int state);

    void messageReceived(const QString &clientId, const QByteArray &msg);

    void clientTokenReceived(const QString &token);

private:

    QScopedPointer<QCloudMessagingClientPrivate> d;

};

QT_END_NAMESPACE

#endif // QTCLOUDMESSAGINGCLIENT_H
