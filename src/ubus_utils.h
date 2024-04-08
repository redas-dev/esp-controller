#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <fcntl.h>
#include <syslog.h>
#include <libserialport.h>

#include "utils.h"
#include "serial_utils.h"

#ifndef _UBUS_UTILS_H
#define _UBUS_UTILS_H
    void send_ubus_json(struct blob_buf* b, struct ubus_context* ctx, struct ubus_request_data* req, char* json);
    cJSON* get_device_list(struct ubus_context* ctx, struct ubus_request_data* req, struct blob_buf* b);
    
    enum {
        PORT,
        PIN,
        __MAX_VALUES
    };

    static const struct blobmsg_policy ESPcontroller_policy[] = {
        [PORT]       = { .name = "port"     , .type = BLOBMSG_TYPE_STRING },
        [PIN]        = { .name = "pin"      , .type = BLOBMSG_TYPE_INT32 }
    };
    
    extern int ESPcontroller_on(struct ubus_context* ctx, struct ubus_object* obj, struct ubus_request_data* req, const char* method, struct blob_attr* msg);
    extern int ESPcontroller_off(struct ubus_context* ctx, struct ubus_object* obj, struct ubus_request_data* req, const char* method, struct blob_attr* msg);
    extern int ESPcontroller_devices(struct ubus_context* ctx, struct ubus_object* obj, struct ubus_request_data* req, const char* method, struct blob_attr* msg);
    
    static const struct ubus_method ESPcontroller_methods[] = {
        UBUS_METHOD("on"           , ESPcontroller_on     , ESPcontroller_policy),
        UBUS_METHOD("off"          , ESPcontroller_off    , ESPcontroller_policy),
        UBUS_METHOD_NOARG("devices", ESPcontroller_devices)
    };
    static struct ubus_object_type ESPcontroller_object_type = {
        .name = "controller",
        .id = 0,
        .methods = ESPcontroller_methods,
        .n_methods = ARRAY_SIZE(ESPcontroller_methods)
    };

    static struct ubus_object ESPcontroller_object = {
        .name      = "espcontroller",
        .type      = &ESPcontroller_object_type,
        .methods   = ESPcontroller_methods,
        .n_methods = ARRAY_SIZE(ESPcontroller_methods),
    };
#endif