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

#ifndef QTCLOUDMESSAGINGPROVIDER_H
#define QTCLOUDMESSAGINGPROVIDER_H

#include <QObject>
#include <QVariantMap>
#include <QMap>
#include <QString>
#include <QScopedPointer>
#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QtCloudMessaging/qcloudmessagingclient.h>

QT_BEGIN_NAMESPACE

/*!
 * \brief The QtCloudMessagingProvider class
 */
class QCloudMessagingProviderPrivate;

class Q_CLOUDMESSAGING_EXPORT QCloudMessagingProvider : public QObject
{
    Q_OBJECT

public:

    enum CloudMessagingProviderState {
        QtCloudMessagingProviderNotRegistered,
        QtCloudMessagingProviderRegistering,
        QtCloudMessagingProviderRegistered
    };
    Q_ENUM(CloudMessagingProviderState)

    explicit QCloudMessagingProvider(QObject *parent = nullptr);
    ~QCloudMessagingProvider();

    virtual bool registerProvider(
            const QString &providerId,
            const QVariantMap &parameters = QVariantMap());

    virtual void deregisterProvider();

    virtual QString connectClient(
            const QString &clientId,
            const QVariantMap &parameters = QVariantMap()) = 0;

    virtual void disconnectClient(
            const QString &clientId,
            const QVariantMap &parameters = QVariantMap());

    virtual bool sendMessage(
            const QByteArray &msg,
            const QString &clientId = QString(),
            const QString &clientToken = QString(),
            const QString &channel = QString()) = 0;

    virtual CloudMessagingProviderState setServiceState(QCloudMessagingProvider::CloudMessagingProviderState state);

    virtual QMap <QString, QCloudMessagingClient *>  *clients();

    QString clientToken(const QString &clientId);

    virtual bool remoteClients() = 0;

    QCloudMessagingClient *client(const QString &clientId);

    QString providerId();

    void setProviderId(const QString &providerId);

    QCloudMessagingProvider::CloudMessagingProviderState getServiceState();

    bool removeClient(const QString &clientId);

    virtual bool subscribeToChannel(
            const QString &channel,
            const QString &clientId = QString()) = 0;

    virtual bool unsubscribeFromChannel(
            const QString &channel,
            const QString &clientId = QString()) = 0;


    bool flushMessageQueue();

    QString connectClientToProvider(
            const QString &clientId,
            const QVariantMap &parameters = QVariantMap(),
            QCloudMessagingClient *serviceClient = nullptr);

private Q_SLOTS:
    void messageReceivedSlot(const QString &clientId,
                             const QByteArray &message);

Q_SIGNALS:
    void clientTokenReceived(const QString &token);

    void messageReceived(const QString &providerId,
                         const QString &clientId,
                         const QByteArray &message);

    void remoteClientsReceived(const QString &clients);

    void serviceStateUpdated(int state);

    void clientStateChanged(const QString &clientId,
                            int status);


private:
    QScopedPointer<QCloudMessagingProviderPrivate> d;
};

QT_END_NAMESPACE

#endif // QTCLOUDMESSAGINGPROVIDER_H
