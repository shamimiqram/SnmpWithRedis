#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include <json-c/json.h>

void push_json_to_redis(redisContext *context, const char *key, json_object *jobj) {
    const char *json_string = json_object_to_json_string(jobj);
    redisCommand(context, "LPUSH %s %s", key, json_string);
}

json_object *pop_json_from_redis(redisContext *context, const char *key) {
    redisReply *reply = redisCommand(context, "RPOP %s", key);
    if (reply == NULL) {
        return NULL;  // Handle error
    }
    
    if (reply->type == REDIS_REPLY_STRING) {
        json_object *jobj = json_tokener_parse(reply->str);
        freeReplyObject(reply);
        return jobj;
    }
    
    freeReplyObject(reply);
    return NULL;  // No valid JSON object returned
}

int main() {
    // Connect to Redis
    redisContext *context = redisConnect("107.0.0.1", 6379);
    if (context == NULL || context->err) {
        if (context) {
            fprintf(stderr, "Error: %s\n", context->errstr);
            redisFree(context);
        } else {
            fprintf(stderr, "Can't allocate redis context\n");
        }
        return 1;
    }

    printf("Connection success\n");

    // Create a JSON object
    json_object *jobj = json_object_new_object();
    json_object_object_add(jobj, "name", json_object_new_string("Alice"));
    json_object_object_add(jobj, "age", json_object_new_int(30));

    // Push JSON object to Redis
    push_json_to_redis(context, "mylist", jobj);
    json_object_put(jobj);  // Free JSON object

    // Pop JSON object from Redis
    json_object *popped_jobj = pop_json_from_redis(context, "mylist");
    if (popped_jobj) {
        printf("Popped JSON: %s\n", json_object_to_json_string(popped_jobj));
        json_object_put(popped_jobj);  // Free the popped JSON object
    } else {
        printf("No JSON object found in the list.\n");
    }

    // Clean up and close the connection
    redisFree(context);
    return 0;
}

