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

#ifndef FIREBASESERVICE_H
#define FIREBASESERVICE_H

#include <QtCloudMessaging/QtCloudMessaging>
#include <QtCloudMessaging/qcloudmessagingrestapi.h>
#include <QtCloudMessagingFirebase/qcloudmessagingfirebaseclient.h>
#include "firebase/app.h"
#include "firebase/messaging.h"
#include "firebase/util.h"
#include <QObject>
#include <QVariantMap>
#include <QByteArray>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class QCloudMessagingFirebaseProviderPrivate;

class QCloudMessagingFirebaseProvider : public QCloudMessagingProvider
{
    Q_OBJECT
public:

    explicit QCloudMessagingFirebaseProvider(QObject *parent = nullptr);

    ~QCloudMessagingFirebaseProvider();

    virtual bool registerProvider(const QString &providerId, const QVariantMap &parameters = QVariantMap()) override;

    virtual void deregisterProvider() override;

    virtual QCloudMessagingProvider::CloudMessagingProviderState setServiceState(
            QCloudMessagingProvider::CloudMessagingProviderState state) override;

    virtual QString connectClient(const QString &clientId, const QVariantMap &parameters = QVariantMap()) override;

    virtual void disconnectClient(const QString &clientId, const QVariantMap &parameters = QVariantMap()) override;

    virtual bool sendMessage(const QByteArray &msg, const QString &clientId = QString(),
                     const QString &clientToken = QString(),
                     const QString &channel = QString()) override;


    virtual bool subscribeToChannel(const QString &channel, const QString &clientId = QString()) override;

    virtual bool unsubscribeFromChannel(const QString &channel, const QString &clientId = QString()) override;

    QMap <QString, QCloudMessagingClient *>  *clients() override;

    // Firefox server API tbd.
    bool remoteClients() override;

    QString clientToken(const QString &clientId);

    void setClientToken(const QString &clientId, const QString &uuid);


private Q_SLOTS:
    void cloudMessageReceived(const QString &clientId, const QByteArray &message);

private:
   QScopedPointer<QCloudMessagingFirebaseProviderPrivate> d;
};

QT_END_NAMESPACE

#endif // FIREBASESERVICE_H
