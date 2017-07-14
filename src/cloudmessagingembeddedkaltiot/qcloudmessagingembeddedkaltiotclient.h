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

#ifndef QCloudMessagingEmbeddedKaltiotClient_H
#define QCloudMessagingEmbeddedKaltiotClient_H

#include <QtCloudMessaging/QtCloudMessaging>
#include <QtCloudMessaging/qtcloudmessagingglobal.h>

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QScopedPointer>

#ifndef ANDROID_OS
#include "ks_gw_client.h"
#else
#include "ks_gw_client_android.h"
#endif

QT_BEGIN_NAMESPACE

class QCloudMessagingEmbeddedKaltiotClientPrivate;

class Q_CLOUDMESSAGING_EXPORT QCloudMessagingEmbeddedKaltiotClient : public  QCloudMessagingClient
{
    Q_OBJECT
public:

    explicit QCloudMessagingEmbeddedKaltiotClient(QObject *parent = nullptr);
    ~QCloudMessagingEmbeddedKaltiotClient();

    // Qt Cloud messaging client virtual functions
    virtual QString connectClient(const QString &clientId,
                                  const QVariantMap &parameters = QVariantMap()) override;

    virtual void disconnectClient() override;

    virtual void cloudMessageReceived(const QString &client, const QByteArray &message) override;

    virtual QString clientToken() override;

    virtual void setClientToken(const QString &token) override;

    virtual bool sendMessage(const QByteArray &msg,
                             const QString &clientToken = QString(),
                             const QString &channel = QString()) override;


    virtual bool flushMessageQueue() override;

    virtual bool subscribeToChannel(const QString &channel) override;

    virtual bool unsubscribeFromChannel(const QString &channel) override;

    void kaltiotMessageReceived(const QString &client, const QString &message);

    ks_gw_client_instance_t *getKaltiotEngineInstance();

private:
    void make_kaltiot_client_registration();
    void runBackgroundThread();

    QScopedPointer<QCloudMessagingEmbeddedKaltiotClientPrivate> d;

};

QT_END_NAMESPACE

#endif // QCloudMessagingEmbeddedKaltiotClient_H
