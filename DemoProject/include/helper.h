#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <cjson/cJSON.h>

#define MAX_LENGTH 100
#define MAX_LINES 100

#ifndef HELPER_TASK_H
#define HELPER_TASK_H

void printCurrentTime();
void* wait_for_response(void *arg);
int read_config_file(char *filename, char lines[MAX_LINES][MAX_LENGTH]);
void print_oid(oid *oid, size_t oid_len);
void print_variable_list(netsnmp_variable_list *vars);
cJSON* format_oid_result_json(char *result, char *key, char *oid);

#endif
