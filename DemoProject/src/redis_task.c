#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <cjson/cJSON.h>

#include "header_files.h"


#define MAX_OID_CNT 100
#define SNMP_GET_OP 1
#define SNMP_WALK_OP 2

redisContext *redis = NULL;

void free_redis()
{
    redisFree(redis);
}

void connect_redis()
{
    struct timeval timeout;
    timeout.tv_sec = 10;  // Timeout of 1 second
    timeout.tv_usec = 0;

    //redis = redisConnect(redis_ip, redis_port);
    redis = redisConnectWithTimeout(redis_ip, redis_port, timeout);

    if (redis == NULL || redis->err)
    {
        if (redis)
        {
            printf("Error: %s\n", redis->errstr);
            printCurrentTime();
            redisFree(redis);
        }
        else
        {
            printf("Can't allocate redis context\n");
            printCurrentTime();
        }
        exit(1);
    }

    redisReply *reply = redisCommand(redis, "AUTH %s", redis_pass);

    if (reply == NULL)
    {
        printf("Authentication failed\n");
        free_redis();
        return;
    }
    printf("\n    =================================\n");
    printf("    || Redis Connection : Success! ||\n");
    printf("    =================================\n");
    printCurrentTime();

    freeReplyObject(reply);
}

void set_error_value_in_redis(const char *key, char *oid[], int oid_cnt, char *error_msg)
{
    cJSON *json_obj = cJSON_CreateObject();
    cJSON *json_items = cJSON_CreateObject();

    for(int i = 0; i < oid_cnt ; i++)
    {
        cJSON* json_data = oid_info_to_json(oid[i],"", "", error_msg);
        cJSON_AddItemReferenceToObject(json_items, oid[i], json_data);
    }

    cJSON_AddItemReferenceToObject(json_obj, key, json_items);
    char *json_str = malloc(1024 * 100);
    json_str = cJSON_PrintUnformatted(json_obj);

    printf("Error : \n %s\n", json_str);
    redisReply *reply = (redisReply *)redisCommand(redis, "RPUSH %s %s", redis_output_key, json_str);
    if (reply == NULL)
    {
        printf("Error: %s\n", redis->errstr);
        printCurrentTime();
        redisFree(redis);
        exit(1);
    }

    // printf("Test : RPUSH command result: %lld\n", reply->integer); // Returns 1 if a new field is created, 0 if it was updated
    freeReplyObject(reply);
    // printf("RPUSH EYE:SNMP_RESULT %s\n", json_str);
}

void set_value_with_json(const char *key, cJSON *json_data[], char *oid[], int cnt)
{
    cJSON *json_obj = cJSON_CreateObject();
    cJSON *json_items = cJSON_CreateObject();

    for(int i = 0; i < cnt ; i++)
    {
        char *json_data_str = malloc(1024);
        json_data_str = cJSON_PrintUnformatted(json_data[i]);
        //printf("%s\n", json_data_str);
        cJSON_AddItemReferenceToObject(json_items, oid[i], json_data[i]);
    }

    cJSON_AddItemReferenceToObject(json_obj, key, json_items);
    char *json_str = malloc(1024 * 100);
    json_str = cJSON_PrintUnformatted(json_obj);
   // printf("PUSH string : %s\n", json_str);
    redisReply *reply = (redisReply *)redisCommand(redis, "RPUSH %s %s", redis_output_key, json_str);
    if (reply == NULL)
    {
        printf("Error: %s\n", redis->errstr);
        printCurrentTime();
        redisFree(redis);
        exit(1);
    }

    // printf("Test : RPUSH command result: %lld\n", reply->integer); // Returns 1 if a new field is created, 0 if it was updated
    freeReplyObject(reply);
    //printf("RPUSH EYE:SNMP_RESULT %s\n", json_str);
}

void proces_oid_data(char *oid[], int oid_cnt, char *hash_key, int operation_type)
{
    // printf("%s---%s--type : %d\n", oid, hash_key, operation_type);

    if (operation_type == SNMP_GET_OP)
    {
        snmp_get_with_hash_key(oid, oid_cnt, hash_key);
    }
    else if (operation_type == SNMP_WALK_OP)
    {
        snmp_walk_with_hash_key(oid, oid_cnt, hash_key);
    }
    else
    {
        printf("Something went wrong\n");
    }
}

void parse_oid_data_from_json(cJSON *json)
{
    cJSON *device_ip = cJSON_GetObjectItem(json, "device_ip"); // all hard coded string should be replace with constant or define value
    cJSON *snmp_version = cJSON_GetObjectItem(json, "snmp_version");
    cJSON *snmp_community_str = cJSON_GetObjectItem(json, "snmp_community_str");
    cJSON *oid_info = cJSON_GetObjectItem(json, "oid_info");

    //printf("IP : %s\n", device_ip->valuestring);

    if (oid_info == NULL || device_ip == NULL || snmp_version == NULL || snmp_community_str == NULL)
    {
        return;
    }

    cJSON *item;

    init_snmp_server(device_ip->valuestring, snmp_version->valuestring, snmp_community_str->valuestring);
    cJSON_ArrayForEach(item, oid_info)
    {
        if(item == NULL) continue;

        char *str = cJSON_Print(item);

        if(str == NULL) return;
        //printf("ITEM:\n%s\n",str);
        if(strlen(str) < 20)
        {
            printf("ITEM:\n%s\n",str);
            return;
        }


        cJSON *snmpget = cJSON_GetObjectItem(item, "snmpget");
        cJSON *snmpwalk = cJSON_GetObjectItem(item, "snmpwalk");
        cJSON *redis_map_key = cJSON_GetObjectItem(item, "redis_map_key");

        if(redis_map_key == NULL)
        {
            return;
        }
        // Print the redis_map_key
        // printf("Redis Map Key: %s\n", redis_map_key->valuestring);

        if(snmpget != NULL)
        {
            cJSON *snmpget_value;
            char *oid_list[MAX_OID_CNT];
            int oid_cnt = 0;

            cJSON_ArrayForEach(snmpget_value, snmpget)
            {
                oid_list[oid_cnt++] = snmpget_value->valuestring;
                // printf("  SNMP Get OID: %s\n", snmpget_value->valuestring);
                // break;
            }

            if(oid_cnt > 0)  proces_oid_data(oid_list, oid_cnt, redis_map_key->valuestring, SNMP_GET_OP);
        }

        if(snmpwalk != NULL)
        {
            cJSON *snmpwalk_value;
            char *oid_list[MAX_OID_CNT];
            int oid_cnt = 0;
            cJSON_ArrayForEach(snmpwalk_value, snmpwalk)
            {
                oid_list[oid_cnt++] = snmpwalk_value->valuestring;
                // printf("  SNMP Get OID: %s\n", snmpget_value->valuestring);
                // break;
            }

            if(oid_cnt > 0) proces_oid_data(oid_list, oid_cnt, redis_map_key->valuestring, SNMP_WALK_OP);
        }

    }
}

int get_and_process_oid_from_redis(char *key, int start_idx, int last_idx)
{
    int retrive_obj_cnt = 0;
    redisReply *getReply = (redisReply *)redisCommand(redis, "LRANGE %s %d %d", key, start_idx, last_idx);
    if (getReply == NULL)
    {
        printf("Error getting key: %s\n", redis->errstr);
        free_redis();
        return retrive_obj_cnt;
    }

    if (getReply->type == REDIS_REPLY_STRING)
    {
        printf("String Value: %s\n", getReply->str);
    }
    else if (getReply->type == REDIS_REPLY_ARRAY)
    {
        retrive_obj_cnt = getReply->elements;
        for (size_t i = 0; i < getReply->elements; i++)
        {
            if (getReply->element[i]->type == REDIS_REPLY_STRING)
            {
                cJSON *json = cJSON_Parse(getReply->element[i]->str);
                parse_oid_data_from_json(json);
            }
        }
    }
    else
    {
        printf("Key not found or not a string %d\n", getReply->type);
        return retrive_obj_cnt;
    }

    freeReplyObject(getReply);
    return retrive_obj_cnt;
}

void trim_data_from_redis(char *redis_key, int list_cnt)
{
    redisReply *getReply = (redisReply *)redisCommand(redis, "LTRIM %s %d -1", redis_key, list_cnt);
    if (getReply == NULL)
    {
        printf("Error getting key: %s\n", redis->errstr);
        free_redis();
    }
}
