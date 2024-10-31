#include <libconfig.h>
#include "../header_files.h"

#define MAX_LENGTH 100
#define MAX_LINES 100

int active_snmp_req = 0;
char *redis_ip = "103.239.252.139";
int redis_port = 6379;
char *redis_pass = "Nopass1234";
char *redis_key = "EYE:SNMP_PENDING";

void process_config(config_t *cfg) {
    const char *host;
    int port;
    const char *user;
    const char *password;

    // Retrieve database settings
    if (config_lookup_string(cfg, "database.host", &host) &&
        config_lookup_int(cfg, "database.port", &port) &&
        config_lookup_string(cfg, "database.user", &user) &&
        config_lookup_string(cfg, "database.password", &password)) {
        printf("Database Configuration:\n");
        printf("  Host: %s\n", host);
        printf("  Port: %d\n", port);
        printf("  User: %s\n", user);
        printf("  Password: %s\n", password);
    } else {
        fprintf(stderr, "Error: No 'database' configuration found.\n");
    }

    // Retrieve log settings
    const char *log_level;
    const char *log_file;

    if (config_lookup_string(cfg, "log.level", &log_level) &&
        config_lookup_string(cfg, "log.file", &log_file)) {
        printf("\nLog Configuration:\n");
        printf("  Level: %s\n", log_level);
        printf("  File: %s\n", log_file);
    } else {
        fprintf(stderr, "Error: No 'log' configuration found.\n");
    }
}

void update_config_file_database()
{

    config_t *cfg;
    config_init(cfg);
    char *filename = "./Othersconfig.cfg";
    config_read_file(cfg, filename);

    process_config(cfg);

    config_destroy(cfg);
}

void update_config_data()
{
    char *filename = "./Others/config.txt";
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
        char *key_info = "KEY";

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
            else if (strcmp(token, key_info) == 0)
            {
                redis_key = strtok(NULL, delemeter);
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
