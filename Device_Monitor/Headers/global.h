#ifndef GLOBALS_H
#define GLOBALS_H

extern int active_snmp_req; // Declaration of the global variable
extern char *redis_key;
extern char *redis_ip;
extern int redis_port;
extern char *redis_pass;

void update_config_data();

#endif
