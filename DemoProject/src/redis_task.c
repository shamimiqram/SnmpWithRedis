#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include "redis_task.h"

#define STORE "store_queue"
#define OID "oid_que"
#define REDIS_IP "127.0.0.1"
#define REDIS_PORT 6379

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

    printf("HSET command result: %lld\n", reply->integer); // Returns 1 if a new field is created, 0 if it was updated
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
    const char *command = "GET ";
    size_t len1 = strlen(command);
    size_t len2 = strlen(key);
    char *command_key = (char *)malloc(len1 + len2 + 1);
    strcpy(command_key, command); // Copy str1 to result
    strcat(command_key, key);
    char *oid_str;
    printf("%s\n", command_key);
    redisReply *getReply = (redisReply *)redisCommand(redis, command_key);
    if (getReply == NULL)
    {
        printf("Error getting key: %s\n", redis->errstr);
        free_redis();
        return NULL;
    }

    if (getReply->type == REDIS_REPLY_STRING)
    {
        // printf("Key: mykey, Value: %s\n", getReply->str);
    }
    else
    {
        printf("Key not found or not a string\n");
    }
    oid_str = (char *)malloc(strlen(getReply->str) + 1);
    strcpy(oid_str, getReply->str);
    freeReplyObject(getReply);
    // redisFree(context);

    return oid_str;
}

char *get_oid_list(char *key)
{
    return get_oid_from_redis(key);
}