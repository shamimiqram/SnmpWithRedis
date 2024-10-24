#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <cjson/cJSON.h>
#include "json_helper.h"

#define STORE "store_queue"
#define OID "oid_que"
#define REDIS_IP "103.239.252.139"
#define REDIS_PORT 6379
#define HASH_NAME "myhash"
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

    if (reply == NULL) {
        printf("Authentication failed\n");
        free_redis();
        return;
    }

    printf("Redis Connection : Success!\n");

    freeReplyObject(reply);
}

void set_value(const char *key, const char *field, const char *value)
{
    redisReply *reply = (redisReply *)redisCommand(redis, "HSET %s %s %s", key, field, value);

    if (reply == NULL)
    {
        printf("Error: %s\n", redis->errstr);
        redisFree(redis);
        exit(1);
    }

    printf("Test : HSET command result: %lld\n", reply->integer); // Returns 1 if a new field is created, 0 if it was updated
    freeReplyObject(reply);
}

void set_oid_in_redis(char *key, char *val)
{
    const char *command = "SET ";
    size_t len1 = strlen(command);
    size_t len2 = strlen(key);
    size_t len3 = strlen(val);
    char *command_key = (char *)malloc(len1 + len2 + len3 + 5);
    strcpy(command_key, command); // Copy str1 to result
    strcat(command_key, key);
    strcat(command_key, " ");
    strcat(command_key, val);
    // strcat(command_key,"\"");

    printf("%s\n", command_key);
    redisReply *setReply = (redisReply *)redisCommand(redis, command_key);
    if (setReply == NULL)
    {
        printf("Error getting key: %s\n", redis->errstr);
        free_redis();
        return;
    }

    freeReplyObject(setReply);
    // redisFree(context);
}

char *get_oid_from_redis(char *key)
{
    /*const char *command = "GET ";
    size_t len1 = strlen(command);
    size_t len2 = strlen(key);
    char *command_key = (char *)malloc(len1 + len2 + 1);
    strcpy(command_key, command); // Copy str1 to result
    strcat(command_key, key);*/
    char *oid_str;
    printf("%s\n", key);
    FILE *file = fopen("output.txt", "w");

    /*redisReply *existsReply = redisCommand(redis, "KEYS *");
    if (existsReply == NULL) {
        printf("Failed to execute EXISTS command\n");
        redisFree(redis);
        return 1;
    }

    else if(existsReply->str == NULL)
    {

        printf("I am here");
    }
    oid_str = (char *)malloc(strlen(existsReply->str) + 1);
    strcpy(oid_str, existsReply->str);

    printf("Keys : %s\n", oid_str);
*/
    int start = 0, end = 1;
    redisReply *getReply = (redisReply *)redisCommand(redis, "LRANGE %s %d %d",key, start, end);
    if (getReply == NULL)
    {
        printf("Error getting key: %s\n", redis->errstr);
        free_redis();
        return NULL;
    }

    if (getReply->type == REDIS_REPLY_STRING)
    {
         printf("Key: mykey, Value: %s\n", getReply->str);
    }
    else if(getReply->type == REDIS_REPLY_ARRAY)
    {
        printf("%d\n", getReply->elements );
        //oid_str = (char *)malloc((getReply->elements[0]->str) + 1) * getReply->elements + 1);

         for (size_t i = 0; i < getReply->elements; i++) {
            // Each element is of type REDIS_REPLY_STRING
            if (getReply->element[i]->type == REDIS_REPLY_STRING) {

                    cJSON *json = cJSON_Parse(getReply->element[i]->str);
                    printf("Total String : \n\n\n%s\n", cJSON_Print(json));
                    fprintf(file, "%s\n", cJSON_Print(json));
                    /*cJSON *child = json->string;
                    while(json != NULL)
                    {

                        printf("Total String : \n\n\n%s\n", cJSON_Print(child));
                        fprintf(file, "Hello, World!\n");
                        json = json->next;

                    }*/

            } else {
                printf("  Item %zu: Not a string\n", i);
            }
        }
    }
    else
    {
        printf("Key not found or not a string %d\n", getReply->type);
        return key;
    }

    //oid_str = (char *)malloc(strlen(getReply->str) + 1);
   // strcpy(oid_str, getReply->str);
    freeReplyObject(getReply);
    // redisFree(context);
    oid_str = "Free";
    return oid_str;
}

char *get_oid_list(char *key)
{
    return get_oid_from_redis(key);
}

void set_values() {
    for (int i = 1; i <= 10; i++) {  // Example with 1000 fields
        char field[20];
        char value[50];
        snprintf(field, sizeof(field), "field%d", i);
        snprintf(value, sizeof(value), "value%d", i);

        redisCommand(redis, "HSET %s %s %s", HASH_NAME, field, value);
    }
}

void get_values() {
    for (int i = 1; i <= 10; i++) {
        char field[20];
        snprintf(field, sizeof(field), "field%d", i);

        redisReply *reply = redisCommand(redis, "HGET %s %s", HASH_NAME, field);

        if (reply == NULL) {
            printf("Error: %s\n", redis->errstr);
            return;
        }

        if (reply->type == REDIS_REPLY_STRING) {
            printf("%s: %s\n", field, reply->str);
        } else {
            printf("%s: (nil)\n", field);
        }

        freeReplyObject(reply);
    }
}
