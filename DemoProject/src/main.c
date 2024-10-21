#include <stdio.h>
#include <stdlib.h>
#include "redis_task.h"
#include "snmp_task.h"
#include "helper.h"

int main() {
    // Connect to Redis
    connect_redis();

    // Example data for HSET
    const char *key = "user:1000";
    const char *field = "name";
    const char *value = "Alice";

    // Set the value in Redis
    set_value(key, field, value);

    // Clean up Redis connection
    free_redis();
    return 0;
}
