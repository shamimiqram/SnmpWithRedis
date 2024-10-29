#include "all.h"

int active_snmp_req = 0; //

int read_config(const char *filename, char **lines) {
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
