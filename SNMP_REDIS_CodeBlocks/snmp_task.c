#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "helper.h"
#include "redis_task.h"

#define PUSH_COMMAND "LPUSH"
#define POP_COMMAND "LPOP"
#define SET_COMMAND "SET"
#define HSET_COMMAND "HSET"
#define SNMP_VERSION SNMP_VERSION_2c
#define COMMUNITY_STRING "audramonitor"
#define HOST_IP "103.218.25.9"

netsnmp_session session, *ss;
netsnmp_pdu *pdu;
netsnmp_variable_list *vars;
netsnmp_pdu *response;

int async_callback(int operation, struct snmp_session *session, int reqid, netsnmp_pdu *response, void *magic)
{

    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {

        for (netsnmp_variable_list *vars = response->variables; vars; vars = vars->next_variable)
        {
            print_variable(vars->name, vars->name_length, vars);

            printf("GET Response : ");
            printCurrentTime();
        }
    }

    else
    {
        // Handle error
        fprintf(stderr, "Error receiving SNMP response\n");
    }
    active_snmp_req--;
    return 1;
}


char* format_variable(netsnmp_variable_list *var) {
    char buffer[1024];
    size_t buffer_length = sizeof(buffer);
    char *result = NULL;

    // Convert the OID to a string
    if (1 < 6) {
        // Prepare the output string based on the variable type
        switch (var->type) {
            case ASN_INTEGER:
                result = malloc(256);
                snprintf(result, 256, "OID: %s, Value: %d", buffer, *var->val.integer);
                break;
            case ASN_OCTET_STR:
                result = malloc(256);
                snprintf(result, 256, "OID: %s, Value: %s", buffer, (char *)var->val.string);
                break;
            case ASN_IPADDRESS: {
                // Format the IP address from val.string
                unsigned char *ip = var->val.string;
                result = malloc(256);
                snprintf(result, 256, "OID: %s, Value: %d.%d.%d.%d",
                         buffer, ip[0], ip[1], ip[2], ip[3]);
                break;
            }
            default:
                result = malloc(256);
                snprintf(result, 256, "OID: %s, Unknown type: %d", buffer, var->type);
        }
    } else {
        result = strdup("Failed to convert OID to string");
    }

    return result;
}

int async_callback_with_hash_key(int operation, struct snmp_session *session, int reqid, netsnmp_pdu *response, void *magic)
{
    char *hash_key = (char *)magic;
    printf("GET Response : key : %s", hash_key);
    printCurrentTime();

    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {

        for (netsnmp_variable_list *vars = response->variables; vars; vars = vars->next_variable)
        {
            print_variable(vars->name, vars->name_length, vars);
        }
         char *result = format_variable(vars);
         printf("Result : %s\n", vars);
    }

    else
    {
        // Handle error
        fprintf(stderr, "Error receiving SNMP response\n");
    }
    active_snmp_req--;
    return 1;
}

void init_snmp_task()
{
    init_snmp("snmpapp");
    active_snmp_req = 0;

    // Set up the SNMP session
    snmp_sess_init(&session);
    session.peername = strdup(HOST_IP);
    session.version = SNMP_VERSION;
    session.community = (u_char *)strdup(COMMUNITY_STRING);
    session.community_len = strlen((const char *)session.community);
}

void init_snmp_with_ip(char *ip, char * ver, char* comm_str)
{
    init_snmp(ip);
    snmp_sess_init(&session);
    session.peername = strdup(ip);
    session.version = SNMP_VERSION;
    session.community = (u_char *)strdup(comm_str);
    session.community_len = strlen((const char *)session.community);
}

void snmp_get_with_hash_key(char* str, char * hash_key)
{
    //printf("From here :\n\n%s \n\n %s\n\n", str, hash_key);
    netsnmp_session *ss;
    netsnmp_pdu *pdu;

    ss = snmp_open(&session);
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    oid oid[MAX_OID_LEN];
    size_t oid_len = MAX_OID_LEN;
    if (!snmp_parse_oid(str, oid, &oid_len))
    {
        fprintf(stderr, "Failed to parse OID\n");
        return;
    }

    session.callback = async_callback_with_hash_key;
    session.callback_magic = ss;
    //printf("\n\nSend GET request: ");
    printCurrentTime();
    snmp_add_null_var(pdu, oid, oid_len);

    int status = snmp_async_send(ss, pdu, async_callback_with_hash_key, hash_key);
    active_snmp_req++;
    printf("Status : %d --- > ", status);
    printCurrentTime();
    if (status == 0)
    {
        snmp_perror("snmp_send");
        exit(1);
    }
}


void snmp_get_req(char str[])
{
    netsnmp_session *ss;
    netsnmp_pdu *pdu;

    ss = snmp_open(&session);
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    oid oid[MAX_OID_LEN];
    size_t oid_len = MAX_OID_LEN;

    if (!snmp_parse_oid(str, oid, &oid_len))
    {
        fprintf(stderr, "Failed to parse OID\n");
        return;
    }

    session.callback = async_callback;
    session.callback_magic = ss;
    printf("\n\nSend GET request: ");
    printCurrentTime();
    snmp_add_null_var(pdu, oid, oid_len);

    int status = snmp_async_send(ss, pdu, async_callback, NULL);
    active_snmp_req++;
    printf("Status : %d --- > ", status);
    printCurrentTime();
    if (status == 0)
    {
        snmp_perror("snmp_send");
        exit(1);
    }
}
