#include <stdio.h>
#include <stdlib.h>
#include "redis_task.h"
#include "snmp_task.h"
#include "helper.h"

static int active_req;

int main() {
    // Connect to Redis
    connect_redis();

    // Example data for HSET
    const char *key = "user:1000";
    const char *field = "name";
    const char *value = "Alice";

    // Set the value in Redis
    set_value(key, field, value);

    init_snmp_task();

    // Example OID and value for SNMP
    const char *oid = ".1.3.6.1.2.1.1.5.0"; // Change this to your desired OID
    const char *snmp_value = "MyHostName";

    // Set the SNMP value
    set_snmp_value(oid, snmp_value);

    // Clean up Redis connection
    free_redis();
    return 0;
}
