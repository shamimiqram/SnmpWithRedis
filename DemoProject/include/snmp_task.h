#ifndef SNMP_TASK_H
#define SNMP_TASK_H

void init_snmp_task();
void set_snmp_value(const char *oid, const char *value);
void async_callback(int operation, struct snmp_session *session, int reqid,netsnmp_pdu *response);

#endif