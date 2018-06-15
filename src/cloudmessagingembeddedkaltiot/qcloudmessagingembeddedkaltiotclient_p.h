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

#include <QtCloudMessaging/qtcloudmessagingglobal.h>
#include <QStringList>
#include <QSettings>
#include <QThread>
#include <QTimer>

#ifndef ANDROID_OS
#include "ks_gw_client.h"
#else
#include "ks_gw_client_android.h"
#endif

QT_BEGIN_NAMESPACE

class QCloudMessaging;
class QCloudMessagingEmbeddedKaltiotClient;

class QCloudMessagingEmbeddedKaltiotClientPrivate
{
public:
    QCloudMessagingEmbeddedKaltiotClientPrivate()
    {
        m_running = false;
    }

    ~QCloudMessagingEmbeddedKaltiotClientPrivate() = default;

    bool m_running;
    QString m_uuid;
    QString m_address;
    QString m_version;
    QString m_customer_id;
    QStringList m_channels;
    QString m_rid;
    QSettings m_client_settings;
    QThread m_clientThread;
    QTimer m_threadTimer;
    ks_gw_client_instance_t m_kaltiot_client_instance;
    QString daemonIpcPath;
};

QT_END_NAMESPACE

#endif // QCLOUDMESSAGINGCLIENT_P_H
