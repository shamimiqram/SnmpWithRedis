#ifndef REDIS_TASK_H
#define REDIS_TASK_H

void connect_redis();
void set_value(const char *key, const char *field, const char *value);
void free_redis();

#endif
