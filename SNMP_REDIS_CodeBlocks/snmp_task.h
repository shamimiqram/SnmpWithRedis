#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifndef SNMP_TASK_H
#define SNMP_TASK_H

void init_snmp_task();
void init_snmp_with_ip(char *ip, char * ver, char* comm_str);
void snmp_get_req(char str[], netsnmp_session session);
void snmp_get_with_hash_key(char* str, char * hash_key);
int async_callback(int operation, struct snmp_session *session, int reqid,netsnmp_pdu *response, void *magic);
int async_callback_with_hash_key(int operation, struct snmp_session *session, int reqid, netsnmp_pdu *response, void *magic);

#endif