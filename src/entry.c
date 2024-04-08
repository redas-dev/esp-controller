#include "ubus_utils.h"

int entry(int argc, char** argv)
{
    struct ubus_context *ctx;
    uloop_init();

    // Connect to ubus
    ctx = ubus_connect(NULL);

    if (!ctx){
        syslog(LOG_ERR, "Failed to connect to ubus.");
        return -1;
    }

    // Listen for commands
    ubus_add_uloop(ctx);
    ubus_add_object(ctx, &ESPcontroller_object);
    uloop_run();

    // Disconnect from ubus
    ubus_free(ctx);
    uloop_done();

    return 0;
}