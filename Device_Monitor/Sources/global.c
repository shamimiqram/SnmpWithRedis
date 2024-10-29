#include "../header_files.h"
#define MAX_LENGTH 100
#define MAX_LINES 100

int active_snmp_req = 0; //
char *redis_key = "EYE:SNMP_PENDING";
char *redis_ip = "103.239.252.139";
int redis_port =  6379;
char *redis_pass = "Nopass1234";

int read_config_file(char *filename, char **lines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 on failure
    }

    int line_count = 0;

    while (line_count < MAX_LINES && fgets(lines[line_count], MAX_LENGTH, file) != NULL) {
        // Remove newline character, if present
        lines[line_count][strcspn(lines[line_count], "\n")] = '\0';
        line_count++;
    }

    fclose(file);
    return line_count; // Return the number of lines read
}
