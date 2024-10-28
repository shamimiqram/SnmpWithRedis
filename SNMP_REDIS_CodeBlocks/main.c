#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "all.h"


void json_task()
{
    cJSON *json_object = cJSON_CreateObject();

    // Add data to the JSON object


    cJSON_AddStringToObject(json_object, "device_ip", "127.0.0.1");
    cJSON_AddStringToObject(json_object, "oid", ".1.1.1.3.4.5.6.7.8");
    cJSON_GetObjectItem(json_object, "name");

    // Convert the JSON object to a string
    char *json_string = cJSON_Print(json_object);
    if (json_string == NULL) {
        fprintf(stderr, "Error printing JSON\n");
        cJSON_Delete(json_object);
        return;
    }

    // Print the JSON string
    printf("%s\n", json_string);
    cJSON *oid= cJSON_GetObjectItem(json_object, "oid");
    printf("Direct : %s\n", oid->valuestring);
    char *object_id =  oid->valuestring;
    printf("Type Cast : %s\n", object_id);

    // Clean up
    free(json_string);
    cJSON_Delete(json_object);
}

int main()
{
    // Connect to Redis
    connect_redis();
    const char *key = "EYE:SNMP_PENDING";
    //set_values();
    //get_values();

    char *oid_ret_str;
    oid_ret_str = get_oid_from_redis(key);
    //printf("\n\n Total reply \n\n %s\n", oid_ret_str);
    //json_task();
    // Example data for HSET
    //const char *key = "first";
   // const char *field = "name";
   // const char *value = "VS_Code";

    // Set the value in Redis
    //set_value(key, field, value);

    //init_snmp_task();

    // Example OID and value for SNMP
    //const char *oid = ".1.3.6.1.2.1.1.5.0.2"; // Change this to your desired OID
    //const char *snmp_value = "MyHostName";

    // Set the SNMP value
    // Clean up Redis connection
    wait_for_response();
    free_redis();
    return 0;
}
