#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <cjson/cJSON.h>

#include "../Headers/helper.h"
#include "../header_files.h"

FILE *file;

void device_monitor()
{
    int command = 1;
    int start_pos = 0, list_cnt = 10;
    bool is_trim_enable = false;
    pthread_t worker_thread;

    while(command != -1)
    {
        printf("\n-------- Device Monitor --------\n\n===> Enter 1 for run operation\n===> Enter 0 for exit\n\n");
        printCurrentTime();
        //scanf("%d", &command);
        if(command == 0)
        {
            break;
        }
        else if(command == 1)
        {
            int pop_obj_cnt = get_and_process_oid_from_redis(redis_key, start_pos, start_pos + list_cnt -1);
            //printf("Get element  number from redis queue : %d\n", pop_obj_cnt);
            if(pop_obj_cnt == 0)
            {
                sleep(5);
            }
            else
            {
                sleep(1);
            }

            pthread_create(&worker_thread, NULL ,wait_for_response, NULL);
            pthread_join(worker_thread, NULL);

            if(is_trim_enable)
            {
                trim_data_from_redis(redis_key, pop_obj_cnt);
            }
            else
            {
                start_pos = start_pos + pop_obj_cnt;
            }
        }
        else
        {
                printf("Wrong input\n");
                printCurrentTime();
        }
    }
}

void start_log_file()
{

    file = freopen("LogFile.txt", "w", stdout);
    if (file == NULL) {
        printf("Error opening  log file\n");
    }
}

void close_file()
{
    fclose(file);
}
int main()
{
    start_log_file();
    configured_redis_info();
    //update_config_data();
    //update_config_file_database();
    connect_redis();
    device_monitor();
    //wait_for_response(NULL);
    free_redis();
    printf("---Exit---\n");
    close_file();
    return 0;
}
