#include "header_files.h"

typedef struct {
    char* redis_ip;
    int redis_port;
    char* redis_pass;
    char* redis_key;
    int redis_pop_count;
    int redis_trim;
    char* log_level;
    char* log_file;
} config_t;

// Callback function to handle the parsed data
int handler(void* user, const char* section, const char* name, const char* value) {
    config_t* config = (config_t*)user;

    if (strcmp(section, "REDIS") == 0) {
            //printf("%s : %s\n", name, value);

        if (strcmp(name, "IP") == 0)
            {
                redis_ip = malloc(1024);
                strcpy(redis_ip, value);
        }
        else if (strcmp(name, "PORT") == 0) redis_port= atoi(value);
        else if (strcmp(name, "PASS") == 0)
        {
            redis_pass = malloc(1024);
            strcpy(redis_pass, value);
        }
        else if (strcmp(name, "KEY") == 0)
        {
            redis_key = malloc(1024);
            strcpy(redis_key, value);
        }
        else if (strcmp(name, "POP_COUNT") == 0) redis_list_cnt= atoi(value);
        else if (strcmp(name, "TRIM") == 0) redis_trim = atoi(value);
    } else if (strcmp(section, "log") == 0) {
        //if (strcmp(name, "level") == 0) strcpy(config->log_level, value);
        //else if (strcmp(name, "file") == 0) strcpy(config->log_file, value);
    }
    return 1; // 1 to continue parsing
}

void configured_redis_info()
{
    config_t config;
    config.redis_ip = NULL;
    config.redis_port = -1;
    config.redis_pass = NULL;
    config.redis_key = NULL;
    config.redis_pop_count = -1;
    config.redis_trim = -1;
    config.log_level = NULL;
    config.log_file = NULL;
    //printf("  IP: %s\n", redis_ip);
    // Parse the config file
    char *filename = "config.ini";
    if (ini_parse(filename, handler, &config) < 0) {
        fprintf(stderr, "Can't load 'config.ini'\n");
        return ;
    }

    // Output the parsed configuration
    printf("Redis Configuration:\n");
    printf("  IP: %s\n", redis_ip);
    printf("  PORT: %d\n", redis_port);
    printf("  PASS: %s\n", redis_pass);
    printf("  KEY: %s\n", redis_key);
    printf("  POP Count: %d\n", redis_list_cnt);
    printf("  TRIM: %d\n", redis_trim);
}
