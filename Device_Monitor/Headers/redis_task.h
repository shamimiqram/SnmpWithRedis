#include <cjson/cJSON.h>
#ifndef REDIS_TASK_H
#define REDIS_TASK_H

void connect_redis();
void set_value_with_json(const char *key, char *oid, cJSON *json_obj);
void set_null_value_in_redis(const char *key, char *oid);
void free_redis();
char *get_oid_from_redis(char *key, int start_idx, int last_idx);
void trim_data_from_redis(char *redis_key, int list_cnt);

#endif
