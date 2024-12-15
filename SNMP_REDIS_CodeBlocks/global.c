#include "header_files.h"

#define MAX_LENGTH 100
#define MAX_LINES 100

int active_snmp_req = 0;
char *redis_ip = "103.239.252.139";
int redis_port = 6379;
const char *redis_pass = "Nopass1234";
const char *redis_input_key = "EYE:SNMP_PENDING";
const char *redis_output_key = "EYE:SNMP_PENDING";
int redis_list_cnt = 0;
int redis_trim = 1; // 1 for trim, 0 for no trim

void safe_free(int **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr); // Free the allocated memory
        *ptr = NULL; // Set the pointer to NULL to avoid double free
    }
}

void update_config_data()
{
    char *filename = "config.txt";
    char lines[MAX_LINES][MAX_LENGTH];

    int line_cnt = read_config_file(filename, lines);
    active_snmp_req = 0;

    for (int i = 0; i < line_cnt; i++)
    {
        const char *delemeter = " ";
        char *token = strtok(lines[i], delemeter);
        char *ip_info = "REDIS_IP";
        char *port_info = "REDIS_PORT";
        char *pass_info = "PASS";
        char *input_queue_info = "INPUT_QUEUE";
        char *output_queue_info = "OUTPUT_QUEUE";
        char *redis_trim_info = "TRIM";

        while (token != NULL)
        {
            if (strcmp(token, ip_info) == 0)
            {
                redis_ip = strtok(NULL, delemeter);
                break;
            }
            else if (strcmp(token, port_info) == 0)
            {
                redis_port = atoi(strtok(NULL, delemeter));
                break;
            }
            else if (strcmp(token, pass_info) == 0)
            {
                redis_pass = strtok(NULL, delemeter);
                break;
            }
            else if (strcmp(token, input_queue_info) == 0)
            {
                redis_input_key = strtok(NULL, delemeter);
                break;
            }
            else if (strcmp(token, output_queue_info) == 0)
            {
                redis_output_key = strtok(NULL, delemeter);
                break;
            }
            else if (strcmp(token, redis_trim_info) == 0)
            {
                redis_trim = atoi (strtok(NULL, delemeter));
                break;
            }
            else
            {
                printf("Unsupported info %s\n", lines[i]);
            }

            token = strtok(NULL, delemeter);
        }
    }
    printf("Configuration updated!\nReady for device monitoring!\n\n");
}
