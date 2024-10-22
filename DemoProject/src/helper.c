#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "global.h"

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
        printf("%d --\n", x);
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
