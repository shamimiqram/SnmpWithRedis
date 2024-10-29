#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "../Headers/helper.h"
#include "../header_files.h"


int main()
{
    connect_redis();
    const char *key = "EYE:SNMP_PENDING";
    char *oid_ret_str;
    oid_ret_str = get_oid_from_redis(key);
    printf("Request done for : %s \n", oid_ret_str);

    wait_for_response();
    free_redis();
    return 0;
}
