#include "ubus_utils.h"

// Defines the available device

// Send a response json to ubus
void send_ubus_json(struct blob_buf* b, struct ubus_context* ctx, struct ubus_request_data* req, char* json)
{
    blobmsg_add_json_from_string(b, json);
    ubus_send_reply(ctx, req, b->head);
}

// Get the list of connected devices
cJSON* get_device_list(struct ubus_context* ctx, struct ubus_request_data* req, struct blob_buf* b)
{
    struct sp_port **port_list;

    // Get the list of all connected ports
    enum sp_return result = sp_list_ports(&port_list);

    if (result != SP_OK) {
		syslog(LOG_ERR, "Failed to list ports.");

        send_ubus_json(b, ctx, req, "{\"msg\":\"Failed to list ports.\"}");

		return NULL;
	}

    cJSON* temp = create_device_list(port_list);
    sp_free_port_list(port_list);

    return temp;
}

// Turns the provided pin on the port ON
extern int ESPcontroller_on(struct ubus_context* ctx, struct ubus_object* obj, struct ubus_request_data* req, const char* method, struct blob_attr* msg)
{
    struct blob_attr* tb[__MAX_VALUES];
    struct blob_buf b = {};

    blob_buf_init(&b, 0);

    // Get data from msg
    blobmsg_parse(ESPcontroller_policy, __MAX_VALUES, tb, blob_data(msg), blob_len(msg));
    
    if (!tb[PIN] || !tb[PORT]){
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    // Parse PORT and PIN
    const char* port_name = blobmsg_get_string(tb[PORT]);
    const uint32_t pin = blobmsg_get_u32(tb[PIN]);

    struct sp_port *port;

    // Look for the PORT
	enum sp_return result = sp_get_port_by_name(port_name, &port);

	if (result != SP_OK) {
		syslog(LOG_ERR, "Failed to get port %s.", port_name);

        send_ubus_json(&b, ctx, req, "{\"msg\":\"Failed to get port.\"}");
        blob_buf_free(&b);

		return UBUS_STATUS_INVALID_ARGUMENT;
	}

    // Check if device is controllable (understands our commands)
    if (!device_compatible(port)){
        syslog(LOG_ERR, "Device incompatible.");

        send_ubus_json(&b, ctx, req, "{\"msg\":\"Device incompatible.\"}");
        blob_buf_free(&b);
        sp_free_port(port);

        return UBUS_STATUS_NOT_SUPPORTED;
    }

    // Get command json
    char* json = construct_device_command_json(pin, "on");

    char resp[256] = {0};
    // Send data
    result = send_data_serial(port_name, json, resp, 255);

    if (result != 0){
        syslog(LOG_ERR, "Failed to send data to %s.", port_name);

        send_ubus_json(&b, ctx, req, "{\"msg\":\"Failed to send data.\"}");
        blob_buf_free(&b);
    } 
    else {
        char* res = parse_response(resp);
        send_ubus_json(&b, ctx, req, res);
        free(res);
    }

    // Clean up memory
    free(json);
    blob_buf_free(&b);
    sp_free_port(port);

    return UBUS_STATUS_OK;
}

// Turns the provided pin on the port OFF
extern int ESPcontroller_off(struct ubus_context* ctx, struct ubus_object* obj, struct ubus_request_data* req, const char* method, struct blob_attr* msg)
{
    struct blob_attr* tb[__MAX_VALUES];
    struct blob_buf b = {};

    blob_buf_init(&b, 0);

    // Get data from msg
    blobmsg_parse(ESPcontroller_policy, __MAX_VALUES, tb, blob_data(msg), blob_len(msg));

    if (!tb[PIN] || !tb[PORT]){
        return UBUS_STATUS_INVALID_ARGUMENT;
    }

    // Parse PORT and PIN
    const char* port_name = blobmsg_get_string(tb[PORT]);
    const uint32_t pin = blobmsg_get_u32(tb[PIN]);

    struct sp_port *port;

    // Look for the PORT
	enum sp_return result = sp_get_port_by_name(port_name, &port);

	if (result != SP_OK) {
		syslog(LOG_ERR, "Failed to get port %s.", port_name);

        send_ubus_json(&b, ctx, req, "{\"msg\":\"Failed to get port.\"}");
        blob_buf_free(&b);
        
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

    // Check if device is controllable (understands our commands)
    if (!device_compatible(port)){
        syslog(LOG_ERR, "Device incompatible.");

        send_ubus_json(&b, ctx, req, "{\"msg\":\"Device incompatible.\"}");
        blob_buf_free(&b);
        sp_free_port(port);

        return UBUS_STATUS_NOT_SUPPORTED;
    }

    // Get command json
    char* json = construct_device_command_json(pin, "off");

    char resp[256] = {0};

    // Send data and receive response
    result = send_data_serial(port_name, json, resp, sizeof(resp));

    if (result != 0 || resp == NULL){
        syslog(LOG_ERR, "Failed to send data to %s.", port_name);

        send_ubus_json(&b, ctx, req, "{\"msg\":\"Failed to send data.\"}");
        blob_buf_free(&b);
    }
    else {
        char* res = parse_response(resp);
        send_ubus_json(&b, ctx, req, res);
        free(res);
    }

    // Clean up memory
    free(json);
    blob_buf_free(&b);
    sp_free_port(port);

    return UBUS_STATUS_OK;
}

extern int ESPcontroller_devices(struct ubus_context* ctx, struct ubus_object* obj, struct ubus_request_data* req, const char* method, struct blob_attr* msg)
{
    struct blob_buf b = {};
    blob_buf_init(&b, 0);

    // Send device list as response
    cJSON* arr = get_device_list(ctx, req, &b);
    char* json = get_device_list_json(arr);

    if (json == NULL){
        send_ubus_json(&b, ctx, req, "{\"msg\":\"Failed to get device list json.\"}");
        blob_buf_free(&b);

        return UBUS_STATUS_UNKNOWN_ERROR;
    }
    send_ubus_json(&b, ctx, req, json);

    // Clean up memory
    free(json);
    blob_buf_free(&b);
    free_array(arr);

    return UBUS_STATUS_OK;
}