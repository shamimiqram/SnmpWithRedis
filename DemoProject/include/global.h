#ifndef GLOBALS_H
#define GLOBALS_H

extern int active_snmp_req; // Declaration of the global variable
extern char *redis_input_key;
extern char *redis_output_key;
extern char *redis_ip;
extern int redis_port;
extern char *redis_pass;
extern int redis_list_cnt;
extern int redis_trim;
void update_config_data();
void safe_free(int **ptr);

#endif
