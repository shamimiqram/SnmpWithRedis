#ifndef SNMP_TASK_H
#define SNMP_TASK_H

void init_snmp_server(char *ip, char *ver, char *comm_str);
void snmp_get_with_hash_key(char *str, char *hash_key);

#endif
