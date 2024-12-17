#include <cjson/cJSON.h>
#ifndef REDIS_TASK_H
#define REDIS_TASK_H

void connect_redis();
void set_value_with_json(const char *key, cJSON *json_obj[], char *oid[], int cnt);
void set_error_value_in_redis(const char *key, char *oid[], int oid_cnt, char *error_msg);
void free_redis();
int get_and_process_oid_from_redis(char *key, int start_idx, int last_idx);
void trim_data_from_redis(char *redis_key, int list_cnt);

#endif
