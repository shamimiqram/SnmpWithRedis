#include <cjson/cJSON.h>
#ifndef REDIS_TASK_H
#define REDIS_TASK_H

void connect_redis();
void set_value_with_json(const char *key, char* oid, cJSON* json_obj);
void free_redis();
char *get_oid_from_redis(char *key);

#endif
