#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "../Headers/helper.h"
#include "../header_files.h"

void device_monitor()
{
    int command = 0;
    int start_pos = 0, list_cnt = 10;
    bool is_trim_enable = true;
    while(command != -1)
    {
        printf("Device Monitor\n Enter 1 for run operation\n Enter 0 for exit\n");
        scanf("%d", &command);
        if(command == 0)
        {
            break;
        }
        else
        {
            get_oid_from_redis(redis_key, start_pos, start_pos + list_cnt -1);

            if(is_trim_enable == true)
            {
                trim_data_from_redis(redis_key, list_cnt);
            }
            else
            {
                start_pos = start_pos + list_cnt;
            }
        }
    }
}

int main()
{
    connect_redis();
    update_config_data();
    device_monitor();
    wait_for_response();
    free_redis();
    return 0;
}
