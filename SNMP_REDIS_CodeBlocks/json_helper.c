#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>

#include "header_files.h"

#define IP_INFO_KEY "device_ip"
#define VER_INFO_KEY "snmp_version"
#define COMM_STR_KEY "snmp_community_str"
#define PORT_INFO_KEY "snmp_port"
#define OID_INFO_KEY "oid"
#define OP_INFO_KEY "operation_type"
#define REPLY_TYPE_KEY "type"
#define REPLY_VALUE_KEY "value"
#define ERROR_MSG_KEY "error"

cJSON* oid_info_to_json(char *oid, char *type, char *value, char *error_msg)
{
    cJSON *json_object = cJSON_CreateObject();

    cJSON_AddStringToObject(json_object, OID_INFO_KEY, oid);
    cJSON_AddStringToObject(json_object, REPLY_TYPE_KEY, type);
    cJSON_AddStringToObject(json_object, REPLY_VALUE_KEY, value);
    cJSON_AddStringToObject(json_object, ERROR_MSG_KEY, error_msg);

    return json_object;
}

cJSON create_JSON_Object(char ip_address[], char version[], char comm_str[], char port[], char oid[], int operation)
{
    cJSON *json_object = cJSON_CreateObject();

    // Add data to the JSON object

    cJSON_AddStringToObject(json_object, IP_INFO_KEY, ip_address);
    cJSON_AddStringToObject(json_object, VER_INFO_KEY, version);
    cJSON_AddStringToObject(json_object, COMM_STR_KEY, comm_str);
    cJSON_AddStringToObject(json_object, PORT_INFO_KEY, port);
    cJSON_AddStringToObject(json_object, OID_INFO_KEY, oid);
    cJSON_AddNumberToObject(json_object, OP_INFO_KEY, operation);

    return *json_object;
}

char *perse_data_from_json(cJSON *json_object, char key[])
{
    cJSON *object = cJSON_GetObjectItem(json_object, key);

    char *object_data = object->valuestring;
    cJSON_Delete(json_object);
    cJSON_Delete(object);
    return object_data;
}

// cJSON *json = cJSON_Parse(json_string);
// char *string = cJSON_Print(json);
