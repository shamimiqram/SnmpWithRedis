#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <cjson/cJSON.h>

#include "../Headers/helper.h"
#include "../header_files.h"

FILE *file, *debug_file;
int command = 1;

void* input_thread(void* arg) {
    char buffer[10];

    while (command) {
        fgets(buffer, sizeof(buffer), stdin);

        // Check if the input is "1"
        if (strcmp(buffer, "0\n") == 0) {
            command = 0; // Set running to 0 to break the loop
        }
    }

    return NULL;
}

void device_monitor()
{
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
            printf("Get element  number from redis queue : %d\n", pop_obj_cnt);
            printCurrentTime();
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
    fclose(debug_file);
}

void signal_handler(int signum) {
    if (signum == SIGSEGV) {
        printf("Caught segmentation fault (SIGSEGV)! Log files saving ...\n");
        close_file();
        // Perform any cleanup here
        exit(EXIT_FAILURE); // Exit the program with failure status
    }
}

int main()
{
    signal(SIGSEGV, signal_handler);
    start_log_file();
    pthread_t thread;

    // Create the input thread
    if (pthread_create(&thread, NULL, input_thread, NULL) != 0) {
        perror("Failed to create thread");
        return EXIT_FAILURE;
    }

    debug_file = fopen("DebugFile.txt", "w");
    log_add_fp(debug_file,LOG_LEVEL_DEBUG);
    log_debug("Sample Log Info");

    printCurrentTime();
    configured_redis_info();
    //update_config_data();
    connect_redis();
    device_monitor();
    //wait_for_response(NULL);
    free_redis();
    printf("---Exit---\n");
    close_file();
    return 0;
}
