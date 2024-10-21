#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>

int main() {
    // Connect to Redis
    redisContext *context = redisConnect("127.0.0.1", 6379);
    if (context == NULL || context->err) {
        if (context) {
            printf("Error: %s\n", context->errstr);
            redisFree(context);
        } else {
            printf("Can't allocate redis context\n");
        }
        return 1;
    }

    // Set a key-value pair
    redisReply *setReply = redisCommand(context, "SET yourKey \"Hello\"");

    if (setReply == NULL) {
        printf("Error setting key: %s\n", context->errstr);
        redisFree(context);
        return 1;
    }
    printf("Redis reply -> : %s\n", setReply->str);
    freeReplyObject(setReply); // Free the reply object

    // Get the value of the key
    redisReply *getReply = redisCommand(context, "GET mykey");
    if (getReply == NULL) {
        printf("Error getting key: %s\n", context->errstr);
        redisFree(context);
        return 1;
    }

    // Check if the reply is a string and print it
    if (getReply->type == REDIS_REPLY_STRING) {
        printf("Key: mykey, Value: %s\n", getReply->str);
    } else {
        printf("Key not found or not a string\n");
    }

    redisReply *getReply2 = redisCommand(context, "GET yourKey");
    printf("Key: yourKey : %s\n", getReply2->str);
    freeReplyObject(getReply2);

    // Clean up
    freeReplyObject(getReply);
    redisFree(context);
    return 0;
}
