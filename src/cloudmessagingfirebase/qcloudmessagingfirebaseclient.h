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

#ifndef QCloudMessagingFirebaseClient_H
#define QCloudMessagingFirebaseClient_H

#include <QtCloudMessaging/QtCloudMessaging>
#include "firebase/app.h"
#include "firebase/messaging.h"
#include "firebase/util.h"
#include <QObject>
#include <QVariantMap>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class QCloudMessagingFirebaseClientPrivate;

class QCloudMessagingFirebaseClient: public QCloudMessagingClient, firebase::messaging::Listener
{

public:

    explicit QCloudMessagingFirebaseClient(QObject *parent = nullptr);

    ~QCloudMessagingFirebaseClient();

    //! Firebase virtual functions

    virtual void OnMessage(const ::firebase::messaging::Message &message) override;

    virtual void OnTokenReceived(const char *token) override;


    //! Qt Cloud messaging client virtual functions

    virtual bool flushMessageQueue() override;

    virtual QString connectClient(const QString &clientId,
                                  const QVariantMap &parameters = QVariantMap()) override;

    virtual void disconnectClient() override;

    virtual bool subscribeToChannel(const QString &channel) override;

    virtual bool unsubscribeFromChannel(const QString &channel) override;

    QString clientToken() override;

    QString clientUuid();

    bool sendMessage(const QByteArray &msg,
                     const QString &clientToken = QString(),
                     const QString &channel = QString()) override;

    void cloudMessageReceived(const QString &client,
                              const QByteArray &message) override;

    void setClientToken(const QString &uuid) override;

private:
    QString parseMessage(firebase::messaging::Message msg_map);

    QScopedPointer<QCloudMessagingFirebaseClientPrivate> d;
};

QT_END_NAMESPACE

#endif // QCloudMessagingFirebaseClient_H
