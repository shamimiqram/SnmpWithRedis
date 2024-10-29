#include <cjson/cJSON.h>
#ifndef HELPER_TASK_H
#define HELPER_TASK_H

cJSON create_JSON_Object(char ip_address[], char version[], char comm_str[], char port[], char oid[], int operation);
cJSON oid_info_to_json(char* oid, char* type, char* value);
char* perse_data_from_json(cJSON *json_object, char key[]);

#endif
