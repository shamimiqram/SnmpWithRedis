#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#ifndef HELPER_TASK_H
#define HELPER_TASK_H

void printCurrentTime();
void wait_for_response();
void print_oid(oid *oid, size_t oid_len);
void print_variable_list(netsnmp_variable_list *vars);

#endif