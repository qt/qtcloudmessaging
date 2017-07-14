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

#ifndef QCLOUDMESSAGINGCLIENT_P_H
#define QCLOUDMESSAGINGCLIENT_P_H
//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QMap>
#include <QVariantMap>
#include <QtCloudMessaging/qtcloudmessagingglobal.h>

QT_BEGIN_NAMESPACE

class QCloudMessagingClient;
class QCloudMessagingProvider;

class QCloudMessagingProviderPrivate
{
public:
    QCloudMessagingProviderPrivate()
        : m_serviceState(0)
    {
    }

    ~QCloudMessagingProviderPrivate() = default;

    QString m_providerId;
    int m_serviceState;

    QVariantMap m_provider_parameters;
    QMap <QString, QCloudMessagingClient *> m_QtCloudMessagingClients;

};

QT_END_NAMESPACE

#endif // QCLOUDMESSAGINGCLIENT_P_H
