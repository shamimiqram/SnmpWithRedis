#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <stdlib.h>

#include "../Headers/helper.h"
#include "../header_files.h"

netsnmp_session session, *ss;
netsnmp_pdu *pdu;
netsnmp_variable_list *vars;
netsnmp_pdu *response;

char *parse_oid_info(oid *oid, size_t oid_len)
{
    char *buffer;
    buffer = malloc(1024);

    for (size_t i = 0; i < oid_len; i++)
    {
        if (i > 0)
        {
            strcat(buffer, ".");
        }
        char str[20];
        sprintf(str, "%ld", oid[i]);
        if (i == 0)
        {
            strcpy(buffer, str);
        }
        else
        {
            strcat(buffer, str);
        }
    }
    // printf("OID BUFFER : %s\n",buffer);
    return buffer;
    // printf("printed \n");
}

int async_callback_with_hash_key(int operation, struct snmp_session *session, int reqid, netsnmp_pdu *response, void *magic)
{
    char *hash_key = (char *)magic;
    //printf("GET Response : key : %s", hash_key);
    //printCurrentTime();

    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {

        for (netsnmp_variable_list *vars = response->variables; vars; vars = vars->next_variable)
        {
            char buffer[1024];
            size_t buffer_length = sizeof(buffer);
            snprint_variable(buffer, buffer_length, vars->name, vars->name_length, vars);
            //printf("%s\n", buffer);

            char *oid = malloc(1024);
            oid = parse_oid_info(vars->name, vars->name_length);
            // printf("%s\n", oid);
            format_oid_result_json(buffer, hash_key, oid);
        }
    }

    else
    {
        // Handle error
        char *oid = malloc(1024);
        oid = parse_oid_info(response->variables->name, response->variables->name_length);
        //fprintf(stderr, "Error receiving SNMP response : %s \n", oid);
        set_error_value_in_redis(hash_key, oid, "Error receiving SNMP response");
        
    }

    // snmp_free_pdu(response);
    // free(magic);
    active_snmp_req--;
    return 1;
}

long version_convert(char *ver)
{
    long ver_info;

    if (strcmp(ver, "SNMPv1") == 0)
    {
        ver_info = SNMP_VERSION_1;
    }
    else if (strcmp(ver, "SNMPv2c") == 0)
    {
        ver_info = SNMP_VERSION_2c;
    }
    else if (strcmp(ver, "SNMPv3") == 0)
    {
        ver_info = SNMP_VERSION_3;
    }
    else
        ver_info = SNMP_VERSION_2c;

    return ver_info;
}

void init_snmp_server(char *ip, char *ver, char *comm_str)
{
    init_snmp(ip);
    snmp_sess_init(&session);
    session.peername = strdup(ip);
    session.version = version_convert(ver);
    session.community = (u_char *)strdup(comm_str);
    session.community_len = strlen((const char *)session.community);
}

void snmp_get_with_hash_key(char *str, char *hash_key)
{
    // printf("From here :\n\n%s \n\n %s\n\n", str, hash_key);
    netsnmp_session *ss;
    netsnmp_pdu *pdu;

    ss = snmp_open(&session);
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    oid oid[MAX_OID_LEN];
    size_t oid_len = MAX_OID_LEN;
    if (!snmp_parse_oid(str, oid, &oid_len))
    {
        //fprintf(stderr, "Failed to parse OID : %s\n", str);
        set_error_value_in_redis(hash_key, str, "Failed to parse OID");
        return;
    }

    session.callback = async_callback_with_hash_key;
    session.callback_magic = ss;
    // printf("\n\nSend GET request: ");
    //printCurrentTime();
    snmp_add_null_var(pdu, oid, oid_len);

    int status = snmp_async_send(ss, pdu, async_callback_with_hash_key, hash_key);
    active_snmp_req++;
    // printf("Status : %d --- > ", status);
    //  printCurrentTime();
    if (status == 0)
    {
        snmp_perror("snmp_send");
        exit(1);
    }
}

void snmp_walk_with_hash_key(char *str, char *hash_key)
{
    // printf("From here :\n\n%s \n\n %s\n\n", str, hash_key);
    netsnmp_session *ss;
    netsnmp_pdu *pdu;

    ss = snmp_open(&session);
    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
    oid oid[MAX_OID_LEN];
    size_t oid_len = MAX_OID_LEN;
    if (!snmp_parse_oid(str, oid, &oid_len))
    {
        fprintf(stderr, "Failed to parse OID : %s\n", str);
        set_error_value_in_redis(hash_key, str, "Failed to parse OID");
        return;
    }

    session.callback = async_callback_with_hash_key;
    session.callback_magic = ss;
    //printf("\n\nSend WALK request: ");
    //printCurrentTime();
    snmp_add_null_var(pdu, oid, oid_len);

    int status = snmp_async_send(ss, pdu, async_callback_with_hash_key, hash_key);
    active_snmp_req++;
    //printf("Status : %d --- > ", status);
    //printCurrentTime();
    if (status == 0)
    {
        snmp_perror("snmp_send");
        exit(1);
    }
}
