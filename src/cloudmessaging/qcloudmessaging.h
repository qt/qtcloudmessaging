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

#ifndef QTCLOUDMESSAGING_H
#define QTCLOUDMESSAGING_H

#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QtCloudMessaging/qcloudmessagingprovider.h>
#include <QObject>
#include <QVariantMap>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

/*!
 * \class QCloudMessaging
 * \brief The QCloudMessaging class
 *
 * This is the Qt Cloud Messaging Class.
 */
class QCloudMessagingPrivate;


class Q_CLOUDMESSAGING_EXPORT QCloudMessaging : public QObject
{
    Q_OBJECT

public:

    explicit QCloudMessaging(QObject *parent = nullptr);
    ~QCloudMessaging();

    Q_INVOKABLE bool registerProvider(const QString &providerId,
                                      QCloudMessagingProvider *provider,
                                      const QVariantMap &parameters = QVariantMap());

    Q_INVOKABLE void deregisterProvider(const QString &providerId);

    Q_INVOKABLE QString connectClient(const QString &providerId,
                                      const QString &clientId,
                                      const QVariantMap &parameters =
                                                        QVariantMap());

    Q_INVOKABLE void disconnectClient(const QString &providerId,
                                      const QString &clientId,
                                      const QVariantMap &parameters =
                                                        QVariantMap());

    Q_INVOKABLE const QStringList localClients(const QString &providerId);

    Q_INVOKABLE bool requestRemoteClients(const QString &providerId);

    Q_INVOKABLE void removeClient(const QString &providerId,
                                  const QString &clientId);

    Q_INVOKABLE QString clientToken(const QString &providerId,
                                             const QString &clientId);

    Q_INVOKABLE void setClientToken(const QString &providerId,
                                    const QString &clientId,
                                    const QString &token);

    Q_INVOKABLE bool sendMessage(const QByteArray &msg,
                                 const QString &providerId = QString(),
                                 const QString &clientId = QString(),
                                 const QString &clientToken = QString(),
                                 const QString &channel = QString()) ;

    Q_INVOKABLE bool subscribeToChannel(const QString &channel,
                                       const QString &providerId = QString(),
                                       const QString &clientId = QString());

    Q_INVOKABLE bool unsubscribeFromChannel(const QString &channel,
                                           const QString &providerId = QString(),
                                           const QString &clientId = QString());

    Q_INVOKABLE void flushMessageQueue(const QString &providerId);

Q_SIGNALS:
    void clientTokenReceived(const QString &token);

    void messageReceived(const QString &providerId,
                         const QString &clientId,
                         const QByteArray &message);

    void remoteClientsReceived(const QString &clients);

    void serviceStateUpdated(int state);

private:
    QScopedPointer<QCloudMessagingPrivate> d;

};

QT_END_NAMESPACE

#endif // QTCLOUDMESSAGING_H
