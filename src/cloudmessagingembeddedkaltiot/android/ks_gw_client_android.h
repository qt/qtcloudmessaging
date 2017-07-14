/****************************************************************************
**
** Copyright (c) by Kaltiot Oy 2017
** Copyright (C) 2015 The Qt Company Ltd.
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

#ifndef __STATIC_LIBRARY_KS_GW_CLIENT_H__
#define __STATIC_LIBRARY_KS_GW_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  KS_STATE_OFFLINE,
  KS_STATE_DISCONNECTING,
  KS_STATE_CONNECTING,
  KS_STATE_ONLINE
} KaltiotSmartState;

typedef enum {
  KS_ERROR_NO_ERROR = 0,
  KS_ERROR_CONNECT_ERROR,
  KS_ERROR_GATEWAY_DISCONNECT,
  KS_ERROR_UNKNOWN,
  KS_ERROR_CLIENT_DISCONNECT
} KaltiotSmartError;

typedef enum {
  KS_GW_CLIENT_PAYLOAD_BINARY = 1,
  KS_GW_CLIENT_PAYLOAD_INT,
  KS_GW_CLIENT_PAYLOAD_STRING,
  KS_GW_CLIENT_PAYLOAD_PING,
  KS_GW_CLIENT_PAYLOAD_PONG
} payload_type_t;

typedef enum {
  NETWORK_STATE_DISABLED,
  NETWORK_STATE_MOBILE_2G,
  NETWORK_STATE_MOBILE_3G,
  NETWORK_STATE_MOBILE_4G,
  NETWORK_STATE_MOBILE_5G,
  NETWORK_STATE_WIFI,
  NETWORK_STATE_COUNT
} network_state_e;

typedef struct {
  void* userdata;
  bool connected;
  int32_t socket_fd;
  void* old_info;
} ks_gw_client_instance_t;


/**
 * Callback function that is called when a notification is received. You
 * should implement this.
 *
 * If payload_type is KS_CLIENT_PAYLOAD_STRING, KS_CLIENT_PAYLOAD_PING or
 * KS_CLIENT_PAYLOAD_PONG, payload and length will include the null
 * terminator of the string.
 *
 * @param address        Null-terminated string.
 * @param payload        Byte data.
 * @param payload_length Length of the payload.
 * @param payload_type   Type of the payload.
 * @param msg_id         Message id
 * @param msg_id_length  Message id length

 */
void ks_gw_client_notification_cb(const char *address, const char *payload,
                                const uint16_t payload_length,
                                const payload_type_t payload_type,
                                const char *msg_id,
                                const uint16_t msg_id_length);

/**
 * Callback function that is called when a rid is received. You should
 * implement this.
 *
 * @param address Null-terminated string.
 * @param rid     Null-terminated string.
                  Unique resource identifier for every iot application
                  registered to the service. Using this identifier you are
                  able to send notifications to the application.
 * @param secret  Null-terminated string.
 */
void ks_gw_client_rid_cb(const char *address, const char *rid,
                       const char *secret);

/**
 * Callback function that is called when the state of the daemon changes. You
 * should implement this.
 *
 * @param state Status code that tells the new state.
 * @param error Possible error code associated with the state.
 */
void ks_gw_client_state_changed_cb(KaltiotSmartState state, KaltiotSmartError error);


/**
 * Initialize the client instance. Must be the first function to call for new client.
 */
void ks_gw_client_init(ks_gw_client_instance_t *inst);

/**
 * Connects to the daemon. Call this before registering. Returns true if the
 * connection has been successfully established.
 *
 * @param path The path or address to connect to. It is treated as a unix
 *             domain socket path unless it has a colon in it, in which case
 *             the IPC will use local TCP on ip:port. You can also leave it
 *             NULL, then the default path (/tmp/ks_gw_socket) will be
 *             used.
 */
extern bool ks_gw_client_connect(ks_gw_client_instance_t *inst, const char *path);

/**
 * Disconnects from the daemon.
 */
extern void ks_gw_client_disconnect(ks_gw_client_instance_t *inst);

/**
 * Registers with the daemon. App will start receiving notifications and other
 * events.
 *
 * NOTE: If you need to update the channels, you have to first unregister
 * and then register again, with the different channels.
 *
 * @param address      Alphanumeric, null-terminated string.
 * @param version      Alphanumeric, null-terminated string.
 * @param customer_id  Alphanumeric, null-terminated string.
                       Identifier given by customer to the IoT application
                       registering to Kaltiot Smart Iot. Use this field to identify your
                       iot application on gateway. It has to be unique for a
                       given gateway. For example "sensor1".
 * @param channels     Array of alphanumeric, null-terminated strings that
 *                     represent the channels such as "temperature" or
 *                     "motion".
 * @param num_channels The number of channels in the array.
 */
extern void ks_gw_client_register_iot(ks_gw_client_instance_t *inst,
                                    const char *address,
                                    const char *version,
                                    const char *customer_id,
                                    const char **channels,
                                    uint16_t num_channels);

/**
 * Unregisters from the daemon.
 *
 * @param address     Alphanumeric, null-terminated string.
 * @param version     Alphanumeric, null-terminated string.
 * @param customer_id Alphanumeric, null-terminated string.
 */
extern void ks_gw_client_unregister_iot(ks_gw_client_instance_t *inst,
                                      const char *address,
                                      const char *version,
                                      const char *customer_id);

/**
 * Publishes a message to the server.
 *
 * If payload_type is KS_CLIENT_PAYLOAD_STRING, KS_CLIENT_PAYLOAD_PING or
 * KS_CLIENT_PAYLOAD_PONG, payload and length should include the null
 * terminator of the string.
 *
 * @param payload      Byte data.
 * @param payload_len  Length of the payload.
 * @param payload_type Type of the payload.
 * @param tag          Tag of the payload. maximum size could be MAX_TAG_LENGTH
 * @return bool        true if the parameters are valid else false
 */
extern bool ks_gw_client_publish_message(ks_gw_client_instance_t *inst,
                                         const uint8_t *payload,
                                         const uint16_t payload_len,
                                         const payload_type_t payload_type,
                                         const char* tag);

/**
 * Should be called periodically. It checks whether there is any data waiting
 * to be read in the buffer, and based on said data calls the above callbacks.
 */
extern void ks_gw_client_task(ks_gw_client_instance_t *inst);

/**
 * Forces state_changed_cb to be called with the current state.
 */
extern void ks_gw_client_request_state(ks_gw_client_instance_t *inst);

/**
 * Forces rid_cb to be called with the current rid info.
 */
extern void ks_gw_client_request_rid(ks_gw_client_instance_t *inst);

typedef void (*ks_app_id_callback) (const char *app_id, void *arg);

/**
 * Requests the application id from the daemon. The callback is called with
 * the application id and an optional argument that the user can specify.
 *
 * @param callback Callback function.
 * @param arg      Optional argument passed to the callback.
 */
extern void ks_gw_client_request_app_id(ks_gw_client_instance_t *inst,
                                      ks_app_id_callback callback,
                                      void *arg);

/**
 * Enables or disables the entire engine. Equivalent to closing the daemon.
 *
 * @param enabled
 */
extern void ks_gw_client_set_engine_enabled(ks_gw_client_instance_t *inst, bool enabled);

/**
 * Sets the network status. MCC and MNC can be included with a mobile network
 * state.
 *
 * @param state
 * @param mcc   Can be an empty null-terminated string, not NULL.
 * @param mnc   Can be an empty null-terminated string, not NULL.
 */
extern void ks_gw_client_set_network_available(ks_gw_client_instance_t *inst,
                                             network_state_e state,
                                             const char *mcc, const char *mnc);

#ifdef __cplusplus
}
#endif

#endif
