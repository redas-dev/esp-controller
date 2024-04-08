#include "utils.h"

// Create a new command to send by serial
char* construct_device_command_json(int pin, const char* action)
{
    cJSON* data = cJSON_CreateObject();

    if (data == NULL) return NULL;
    
    // Add data to object
    cJSON_AddStringToObject(data, "action", action);
    cJSON_AddNumberToObject(data, "pin", pin);

    // Get JSON in string format
    char* command = cJSON_PrintUnformatted(data);

    // Free data from object
    cJSON_DeleteItemFromObject(data, "action");
    cJSON_DeleteItemFromObject(data, "pin");
    cJSON_Delete(data);

    return command;
}

cJSON* create_device_list(struct sp_port **port_list)
{
    // Create new object to store ports array
    cJSON* jsonObj = cJSON_CreateObject();
    if (jsonObj == NULL){
        syslog(LOG_ERR, "Failed to create JSON object.");

        return NULL;
    }

    // Add an array to store ports
    cJSON* arr = cJSON_AddArrayToObject(jsonObj, "devices");
    if (arr == NULL){
        syslog(LOG_ERR, "Failed to add array to object.");

        cJSON_Delete(jsonObj);

        return NULL;
    }

	for (int i = 0; port_list[i] != NULL; i++) {
		struct sp_port *port = port_list[i];

        // Get port name
		char *port_name = sp_get_port_name(port);

        // Get VID and PID from port
        int usb_vid, usb_pid;
		sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);

        if (!device_compatible(port))
            continue;

        // Change format from int to hex
        char vid[6] = {0}; char pid[6] = {0};
        snprintf(vid, 5, "%x", usb_vid);
        snprintf(pid, 5, "%x", usb_pid);
        vid[5] = '\0';
        pid[5] = '\0';

        add_new_device_to_array(vid, pid, port_name, arr);
	}

    return jsonObj;
}

void add_new_device_to_array(char* vid, char* pid, char* port_name, cJSON* arr)
{
    // Create new device object and add it to the array
    cJSON* temp = cJSON_CreateObject();

    // Add data to object
    cJSON_AddStringToObject(temp, "VendorID", vid);
    cJSON_AddStringToObject(temp, "ProductID", pid);
    cJSON_AddStringToObject(temp, "Port", port_name);

    cJSON_AddItemToArray(arr, temp);
}

char* get_device_list_json(cJSON* obj)
{
    if (obj == NULL) return NULL;
    return cJSON_Print(obj);
}

// Free cJSON object array. Format: {"devices": [{"Port": string, "VendorID": string, "ProductID": string}, {...}]}
void free_array(cJSON* obj)
{
    // Get array from object
    cJSON* arr = cJSON_DetachItemFromObject(obj, "devices");

    uint32_t arrSize = cJSON_GetArraySize(arr);

    for(int i = 0; i < arrSize; i++){
        // Get i'th element from array
        cJSON* temp = cJSON_DetachItemFromArray(arr, i);

        // Delete data from object
        cJSON_DeleteItemFromObject(temp, "Port");
        cJSON_DeleteItemFromObject(temp, "VendorID");
        cJSON_DeleteItemFromObject(temp, "ProductID");
        cJSON_Delete(temp);
    }

    cJSON_Delete(arr);
    cJSON_Delete(obj);
}

// Checks if the device that's connected to port is compatible with the commands
int device_compatible(struct sp_port *port)
{
    int usb_vid = 0, usb_pid = 0;
    sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid);

    return usb_pid == PRODUCTID && usb_vid == VENDORID;
}

// Parse response gotten from the device
char* parse_response(char* resp)
{
    cJSON* res = cJSON_Parse(resp);
    if (res == NULL) return NULL;

    char* parsedData = NULL;

    parsedData = create_response(res, "msg");

    cJSON_DeleteItemFromObject(res, "msg");
    cJSON_DeleteItemFromObject(res, "response");
    cJSON_Delete(res);

    return parsedData;
}

// Form a new response object and return it in a string format
char* create_response(cJSON* res, char* type)
{
    cJSON* temp = cJSON_CreateObject();
    
    cJSON_AddStringToObject(temp, type, cJSON_GetObjectItem(res, "msg")->valuestring);
    char* parsedData = cJSON_Print(temp);

    cJSON_DeleteItemFromObject(temp, "success");
    cJSON_Delete(temp);

    return parsedData;
}