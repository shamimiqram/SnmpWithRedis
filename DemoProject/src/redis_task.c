#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include "redis_task.h"

redisContext *redis = NULL;

void free_redis()
{
    redisFree(redis);
}

void connect_redis() {
    redis = redisConnect("127.0.0.1", 6379);
    if (redis == NULL || redis->err) {
        if (redis) {
            printf("Error: %s\n", redis->errstr);
            redisFree(redis);
        } else {
            printf("Can't allocate redis context\n");
        }
        exit(1);
    }
}

void set_value(const char *key, const char *field, const char *value) {
    redisReply *reply = redisCommand(redis, "HSET %s %s %s", key, field, value);
    if (reply == NULL) {
        printf("Error: %s\n", redis->errstr);
        redisFree(redis);
        exit(1);
    }

    printf("HSET command result: %lld\n", reply->integer);  // Returns 1 if a new field is created, 0 if it was updated
    freeReplyObject(reply);
}
