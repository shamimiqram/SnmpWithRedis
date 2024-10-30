#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <cjson/cJSON.h>

#include "../header_files.h"

#define SNMP_GET_OP 1
#define SNMP_WALK_OP 2
#define REDIS_IP "103.239.252.139"
#define REDIS_PORT 6379
#define PASS "Nopass1234"

redisContext *redis = NULL;

void free_redis()
{
    redisFree(redis);
}

void connect_redis()
{
    redis = redisConnect(REDIS_IP, REDIS_PORT);

    if (redis == NULL || redis->err)
    {
        if (redis)
        {
            printf("Error: %s\n", redis->errstr);
            redisFree(redis);
        }
        else
        {
            printf("Can't allocate redis context\n");
        }
        exit(1);
    }

    redisReply *reply = redisCommand(redis, "AUTH %s", PASS);

    if (reply == NULL)
    {
        printf("Authentication failed\n");
        free_redis();
        return;
    }

    printf("Redis Connection : Success!\n");

    freeReplyObject(reply);
}

void set_null_value_in_redis(const char *key, char *oid)
{
    cJSON json_data;
    json_data = oid_info_to_json(oid, "", "");
    cJSON *json_obj = cJSON_CreateObject();
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddItemReferenceToObject(json_item, oid, &json_data);
    cJSON_AddItemReferenceToObject(json_obj, key, json_item);
    char *json_str = malloc(1024);
    json_str = cJSON_PrintUnformatted(json_obj);
    /*redisReply *reply = (redisReply *)redisCommand(redis, "RPUSH EYE:SNMP_RESULT %s", json_str);
    if (reply == NULL)
    {
        printf("Error: %s\n", redis->errstr);
        redisFree(redis);
        exit(1);
    }*/

    // printf("Test : RPUSH command result: %lld\n", reply->integer); // Returns 1 if a new field is created, 0 if it was updated
    // freeReplyObject(reply);
    printf("RPUSH EYE:SNMP_RESULT %s\n", json_str);
}

void set_value_with_json(const char *key, char *oid, cJSON *json_data)
{
    cJSON *json_obj = cJSON_CreateObject();
    cJSON *json_item = cJSON_CreateObject();
    cJSON_AddItemReferenceToObject(json_item, oid, json_data);
    cJSON_AddItemReferenceToObject(json_obj, key, json_item);
    char *json_str = malloc(1024);
    json_str = cJSON_PrintUnformatted(json_obj);
    /*redisReply *reply = (redisReply *)redisCommand(redis, "RPUSH EYE:SNMP_RESULT %s", json_str);
    if (reply == NULL)
    {
        printf("Error: %s\n", redis->errstr);
        redisFree(redis);
        exit(1);
    }*/

    // printf("Test : RPUSH command result: %lld\n", reply->integer); // Returns 1 if a new field is created, 0 if it was updated
    // freeReplyObject(reply);
    printf("RPUSH EYE:SNMP_RESULT %s\n", json_str);
}

void proces_oid_data(char *oid, char *hash_key, int operation_type)
{
    printf("%s---%s--type : %d\n", oid, hash_key, operation_type);
    if (operation_type == SNMP_GET_OP)
    {
        snmp_get_with_hash_key(oid, hash_key);
    }
    else if (operation_type == SNMP_WALK_OP)
    {
        snmp_walk_with_hash_key(oid, hash_key);
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

    // printf("IP : %s\n", device_ip->valuestring);

    cJSON *item;

    init_snmp_server(device_ip->valuestring, snmp_version->valuestring, snmp_community_str->valuestring);
    cJSON_ArrayForEach(item, oid_info)
    {
        cJSON *snmpget = cJSON_GetObjectItem(item, "snmpget");
        cJSON *snmpwalk = cJSON_GetObjectItem(item, "snmpwalk");
        cJSON *redis_map_key = cJSON_GetObjectItem(item, "redis_map_key");

        // Print the redis_map_key
        printf("Redis Map Key: %s\n", redis_map_key->valuestring);

        cJSON *snmpget_value;
        cJSON_ArrayForEach(snmpget_value, snmpget)
        {
            proces_oid_data(snmpget_value->valuestring, redis_map_key->valuestring, SNMP_GET_OP);
            // printf("  SNMP Get OID: %s\n", snmpget_value->valuestring);
            // break;
        }

        cJSON *snmpwalk_value;
        cJSON_ArrayForEach(snmpwalk_value, snmpwalk)
        {
            proces_oid_data(snmpwalk_value->valuestring, redis_map_key->valuestring, SNMP_WALK_OP);
            // printf("  SNMP Get OID: %s\n", snmpget_value->valuestring);
            break;
        }
    }
}

char *get_oid_from_redis(char *key, int start_idx, int last_idx)
{
    redisReply *getReply = (redisReply *)redisCommand(redis, "LRANGE %s %d %d", key, start_idx, last_idx);
    if (getReply == NULL)
    {
        printf("Error getting key: %s\n", redis->errstr);
        free_redis();
        return NULL;
    }

    if (getReply->type == REDIS_REPLY_STRING)
    {
        printf("String Value: %s\n", getReply->str);
    }
    else if (getReply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < getReply->elements; i++)
        {
            if (getReply->element[i]->type == REDIS_REPLY_STRING)
            {
                cJSON *json = cJSON_Parse(getReply->element[i]->str);
                parse_oid_data_from_json(json);
            }
            // break;
        }
    }
    else
    {
        printf("Key not found or not a string %d\n", getReply->type);
        return key;
    }

    freeReplyObject(getReply);
    char *oid_str = "Free";
    return oid_str;
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