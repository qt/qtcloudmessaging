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

#ifndef QCLOUDMESSAGINGFIREBASECLIENT_P_H
#define QCLOUDMESSAGINGFIREBASECLIENT_P_H

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

#include <qtcloudmessagingglobal.h>
#include "qcloudmessagingfirebaseclient.h"
#include "qcloudmessagingfirebaserest.h"
#include "firebase/app.h"
#include "firebase/messaging.h"
#include "firebase/util.h"


QT_BEGIN_NAMESPACE

class QCloudMessaging;

class QCloudMessagingFirebaseClientPrivate
{
public:
    QCloudMessagingFirebaseClientPrivate()
    {
    }

    ~QCloudMessagingFirebaseClientPrivate() = default;

    QString m_uuid;
    QString m_token;

    ::firebase::App *m_firebaseApp;
    ::firebase::ModuleInitializer  m_firebase_initializer;
    ::firebase::messaging::Message m_last_firebase_message;

};

QT_END_NAMESPACE

#endif // QCLOUDMESSAGINGFIREBASECLIENT_P_H
