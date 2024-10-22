#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define PUSH_COMMAND "LPUSH"
#define POP_COMMAND "LPOP"
#define SET_COMMAND "SET"
#define HSET_COMMAND "HSET"

int active_req = 0;

void wait_for_response()
{

    while (active_req)
    {
        int fds = 0, block = 1;
        fd_set fdset;
        struct timeval timeout;

        FD_ZERO(&fdset);
        snmp_select_info(&fds, &fdset, &timeout, &block);
        fds = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);
        if (fds < 0)
        {
            perror("select failed");
            exit(1);
        }
        if (fds)
            snmp_read(&fdset);
        else
            snmp_timeout();
    }
    // Clean up

    int x;
    for (int i = 0; i < 1; i++)
    {
        scanf("%d", &x);
        printf("%d --\n", x);
    }
}

void printCurrentTime()
{

    struct timeval tv;
    struct tm *ptm;
    char timeString[40];
    gettimeofday(&tv, NULL);
    ptm = localtime(&tv.tv_sec);
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
    printf("%s.%03ld\n", timeString, tv.tv_usec / 1000);
}

void print_oid(oid *oid, size_t oid_len)
{
    for (size_t i = 0; i < 1; i++)
    {
        if (i > 0)
        {
            printf(".");
        }
        printf("%ld", oid[i]);
    }
    printf("\n");
}

void print_variable_list(netsnmp_variable_list *vars)
{
    while (vars)
    {

        printf("OID: ");
        print_oid(vars->name, vars->name_length);
        printf("Value: ");

        // Handle different variable types
        switch (vars->type)
        {
        case ASN_INTEGER:
            printf("%d\n", *(vars->val.integer));
            break;
        case ASN_OCTET_STR:
            printf("%s\n", vars->val.string);
            break;
        case ASN_OBJECT_ID:
            printf("OID value\n");
            print_oid(vars->val.objid, vars->val_len);
            break;
        case ASN_IPADDRESS:
            printf("IP Address: %d.%d.%d.%d\n",
                   vars->val.integer[0], vars->val.integer[1],
                   vars->val.integer[2], vars->val.integer[3]);
            break;
        // Add more types as needed
        default:
            printf("Unknown type\n");
            break;
        }
        vars = vars->next_variable;
        // vars = vars->next_variable;  // Move to the next variable in the list
    }
}

void async_callback(int operation, struct snmp_session *session, int reqid,
                    netsnmp_pdu *response)
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
    active_req--;
}

char *get_oid_from_redis(redisContext *context, char *key)
{
    const char *command = "GET ";
    size_t len1 = strlen(command);
    size_t len2 = strlen(key);
    char *command_key = malloc(len1 + len2 + 1);
    strcpy(command_key, command); // Copy str1 to result
    strcat(command_key, key);
    char *oid_str;
    printf("%s\n", command_key);
    redisReply *getReply = redisCommand(context, command_key);
    if (getReply == NULL)
    {
        printf("Error getting key: %s\n", context->errstr);
        redisFree(context);
        return 1;
    }

    if (getReply->type == REDIS_REPLY_STRING)
    {
       // printf("Key: mykey, Value: %s\n", getReply->str);
    }
    else
    {
        printf("Key not found or not a string\n");
    }
    oid_str = malloc(strlen(getReply->str) + 1);
    strcpy(oid_str, getReply->str);
    freeReplyObject(getReply);
   // redisFree(context);

    return oid_str;
}

void set_oid_in_redis(redisContext *context, char *key, char *val)
{
    const char *command = "SET ";
    size_t len1 = strlen(command);
    size_t len2 = strlen(key);
    size_t len3 = strlen(val);
    char *command_key = malloc(len1 + len2 + len3 + 5);
    strcpy(command_key, command); // Copy str1 to result
    strcat(command_key, key);
    strcat(command_key," ");
    strcat(command_key, val);
   // strcat(command_key,"\"");
    char *oid_str;
    printf("%s\n", command_key);
    redisReply *setReply = redisCommand(context, command_key);
    if (setReply == NULL)
    {
        printf("Error getting key: %s\n", context->errstr);
        redisFree(context);
        return 1;
    }
    
    freeReplyObject(setReply);
   // redisFree(context);
}

void snmp_get_req(char str[], netsnmp_session session)
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
        return 1;
    }

    session.callback = async_callback;
    session.callback_magic = ss;
    printf("\n\nSend GET request: ");
    printCurrentTime();
    snmp_add_null_var(pdu, oid, oid_len);

    int status = snmp_async_send(ss, pdu, async_callback, NULL);
    active_req++;
    printf("Status : %d --- > ", status);
    printCurrentTime();
    if (status == 0)
    {
        snmp_perror("snmp_send");
        exit(1);
    }
}

void initiate_redis()
{
    redisContext *context = redisConnect("127.0.0.1", 6379);
    if (context == NULL || context->err)
    {
        if (context)
        {
            printf("Error: %s\n", context->errstr);
            redisFree(context);
        }
        else
        {
            printf("Can't allocate redis context\n");
        }
        return;
    }
}

int main(int argc, char **argv)
{
    netsnmp_session session, *ss;
    netsnmp_pdu *pdu;
    netsnmp_variable_list *vars;
    netsnmp_pdu *response;
    int count = 1;
    int status;

    // Initialize the SNMP library
    init_snmp("snmpapp");

    // Set up the SNMP session
    snmp_sess_init(&session);
    session.peername = strdup("103.218.25.9");            // SNMP agent address
    session.version = SNMP_VERSION_2c;                    // SNMP version
    session.community = (u_char *)strdup("audramonitor"); // SNMP community string
    session.community_len = strlen((const char *)session.community);

    redisContext *context = redisConnect("127.0.0.1", 6379);
    if (context == NULL || context->err)
    {
        if (context)
        {
            printf("Error: %s\n", context->errstr);
            redisFree(context);
        }
        else
        {
            printf("Can't allocate redis context\n");
        }
        return 1;
    }

    printf("Database connected !\n");

    for (int i = 0; i < 10; i++)
    {
        char *str;
        char *my_key;
        char *my_val;
        my_key = malloc(sizeof(char) * 15);
        my_val = malloc(sizeof(char) * 40);

        strcpy(my_key, "oid_queue");
        strcpy(my_val, ".1.3.6.1.2.1.2.2.1.6.4");
        set_oid_in_redis(context, my_key, my_val);
        
        str = get_oid_from_redis(context, my_key);
        //printf("Return string : %s\n", str);
        snmp_get_req(str, session);
    }
    // Open the session

    wait_for_response();

    if (response)
        snmp_free_pdu(response);
    snmp_close(ss);
    return 0;
}
