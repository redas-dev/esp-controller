#include <cjson/cJSON.h>
#include <stdint.h>
#include <syslog.h>
#include <libserialport.h>
#include <stdio.h>

#include "constants.h"

#ifndef UTILS_H
#define UTILS_H
    char* construct_device_command_json(int pin, const char* action);
    void add_new_device_to_array(char* vid, char* pid, char* port_name, cJSON* arr);
    char* get_device_list_json(cJSON* obj);
    void free_array(cJSON* arr);
    cJSON* create_device_list(struct sp_port **port_list);
    int device_compatible(struct sp_port *port);
    char* parse_response(char* resp);
    char* create_response(cJSON* res, char* type);
#endif