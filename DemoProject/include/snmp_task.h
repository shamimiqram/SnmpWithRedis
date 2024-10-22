#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifndef SNMP_TASK_H
#define SNMP_TASK_H

void init_snmp_task();
void snmp_get_req(char str[], netsnmp_session session);
int async_callback(int operation, struct snmp_session *session, int reqid,netsnmp_pdu *response, void *magic);

#endif