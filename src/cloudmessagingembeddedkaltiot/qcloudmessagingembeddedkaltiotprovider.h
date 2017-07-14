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

#ifndef KALTIOTPUSHSERVICE_H
#define KALTIOTPUSHSERVICE_H

#include <QtCloudMessaging/QtCloudMessaging>
#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QtCloudMessaging/qcloudmessagingrestapi.h>
#include <QtCloudMessagingEmbeddedKaltiot/qcloudmessagingembeddedkaltiotclient.h>

#include <QObject>
#include <QVariantMap>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

class QCloudMessagingEmbeddedKaltiotProviderPrivate;
class Q_CLOUDMESSAGING_EXPORT QCloudMessagingEmbeddedKaltiotProvider : public
    QCloudMessagingProvider
{
    Q_OBJECT
public:

   explicit QCloudMessagingEmbeddedKaltiotProvider(QObject *parent = nullptr);
    ~QCloudMessagingEmbeddedKaltiotProvider();

    //! Qt Cloud messaging client virtual functions
    virtual bool registerProvider(const QString &providerId,
                                  const QVariantMap &parameters = QVariantMap()) override;

    virtual void deregisterProvider() override;

    virtual QString connectClient(const QString &clientId,
                                  const QVariantMap &parameters = QVariantMap()) override;

    virtual void disconnectClient(const QString &clientId,
                                  const QVariantMap &parameters = QVariantMap()) override;

    virtual bool sendMessage(const QByteArray &msg, const QString &clientId = QString(),
                             const QString &clientToken = QString(),
                             const QString &channel = QString()) override;

    virtual QCloudMessagingProvider::CloudMessagingProviderState setServiceState(
            QCloudMessagingProvider::CloudMessagingProviderState state)  override;

    virtual QMap <QString, QCloudMessagingClient *> *clients() override;

    virtual bool subscribeToChannel(const QString &channel,
                                   const QString &clientId = QString())  override;

    virtual bool unsubscribeFromChannel(const QString &channel,
                                       const QString &clientId = QString()) override;

    virtual bool remoteClients() override;

    /* KALTIOT SPECIFIC FUNCTIONS */
    void cloudMessageReceived(const QString &client, const QByteArray &message);
    QCloudMessagingEmbeddedKaltiotClient *getKaltiotClient(const QString &clientId);

    void setClientToken(const QString &client, const QString &uuid);

private:
    QScopedPointer<QCloudMessagingEmbeddedKaltiotProviderPrivate> d;

};

QT_END_NAMESPACE

#endif // KALTIOTPUSHSERVICE_H
