#ifndef REDIS_TASK_H
#define REDIS_TASK_H

void connect_redis();
void set_value(const char *key, const char *field, const char *value);
void save_oid_ret_with_hash(char * redis_map_key, char* oid, char* value);
void free_redis();
void get_values();
void set_values();

#endif
