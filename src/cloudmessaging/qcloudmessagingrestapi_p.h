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

#ifndef QTCLOUDMESSAGINGRESTAPI_P_H
#define QTCLOUDMESSAGINGRESTAPI_P_H
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
#include <QList>
#include <QNetworkReply>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>

QT_BEGIN_NAMESPACE

class QCloudMessagingNetworkMessage;

class QCloudMessagingRestApiPrivate
{
public:
    QCloudMessagingRestApiPrivate()
    {
        m_online_state = false;
        m_server_message_timer = 800;
        m_server_wait_for_response_counter = 10;
        m_server_message_retry_count = 1;
    }

    ~QCloudMessagingRestApiPrivate() = default;

    void setServerTimers(int messageTimer, int waitForResponseCounter, int messageRetryCount)
    {
        m_server_message_timer = messageTimer;
        m_server_wait_for_response_counter = waitForResponseCounter;
        m_server_message_retry_count = messageRetryCount;
    }

    QNetworkAccessManager m_manager;
    bool m_wait_for_last_request_response;
    QTimer m_msgTimer;
    bool m_online_state;
    QList <QCloudMessagingNetworkMessage> m_network_requests;
    QNetworkConfigurationManager m_network_info;
    int m_waiting_counter;
    int m_server_message_timer;
    int m_server_wait_for_response_counter;
    int m_server_message_retry_count;

};

QT_END_NAMESPACE

#endif // QTCLOUDMESSAGINGRESTAPI_P_H
