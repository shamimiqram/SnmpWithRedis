#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "../header_files.h"

void printCurrentTime()
{
    return;
    struct timeval tv;
    struct tm *ptm;
    char timeString[40];
    gettimeofday(&tv, NULL);
    ptm = localtime(&tv.tv_sec);
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
    printf("%s.%03ld\n", timeString, tv.tv_usec / 1000);
}

void wait_for_response()
{
    while (active_snmp_req)
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
        printf("--Exit--\n");
    }
}

void print_oid(oid *oid, size_t oid_len)
{
    for (size_t i = 0; i < oid_len; i++)
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
            printf("%ld\n", *(vars->val.integer));
            break;
        case ASN_OCTET_STR:
            printf("%s\n", vars->val.string);
            break;
        case ASN_OBJECT_ID:
            printf("OID value\n");
            print_oid(vars->val.objid, vars->val_len);
            break;
        case ASN_IPADDRESS:
            printf("IP Address: %ld.%ld.%ld.%ld\n",
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

char* format_oid_result_json(char *result, char* key, char* oid)
{
    char *ret_oid = malloc(256);
    char *type = malloc (256);
    char *value = malloc(256);
    int token_cnt = 0;

    const char *delemeter = " :\\";
    char *token = strtok(result, delemeter);
    char *no = "No";
    char * equal = "=";

    // Continue to tokenize the string until strtok returns NULL
    while (token != NULL)
    {

        if (strcmp(token, no) == 0)
        {
            //printf("No data\n");
            type = "";
            value = "";
            break;
        }
        else if(strcmp(token, equal) == 0)
        {
           // printf("equal sign \n");
        }
        else if(token_cnt == 0)
        {
            strcpy(ret_oid, token);
            token_cnt++;
        }
        else if(token_cnt == 1)
        {
            strcpy(type, token);
            token_cnt++;
        }
        else if(token_cnt == 2)
        {
            strcpy(value, token);
            token_cnt++;
        }
        else strcat(value, token);

       printf("Token: %s \n", token, result);
        token = strtok(NULL, delemeter); // Get the next token
    }

    cJSON ret;
    ret =  oid_info_to_json(ret_oid, type, value);
    printf(" Oid : %s\n", oid);
    set_value_with_json(key, oid, &ret);
    return ret_oid;
}
