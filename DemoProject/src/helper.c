#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <unistd.h>

#include "header_files.h"

#define MAX_LENGTH 100
#define MAX_LINES 100

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

void *wait_for_response(void *)
{
    while (active_snmp_req > 0)
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

    printf("\n--No Pending Request--\n");
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

cJSON* format_oid_result_json(char *result, char *key, char *oid)
{
    char *ret_oid = malloc(256);
    char *type_value = malloc (256);
    char *type = malloc(256);
    char *value = malloc(256);
    int token_cnt = 0;


    //printf("OID : %s, KEY : %s. RESULT: %s\n", oid, key, result);

    const char *del_oid = "=";



    const char *delemeter = "=";
    char *token = strtok(result, delemeter);
    char *no = "No";
   // Continue to tokenize the string until strtok returns NULL
    while (token != NULL)
    {
        if (strcmp(token, no) == 0)
        {
            // printf("No data\n");
            type = "";
            value = "";
            break;
        }
        else if (token_cnt == 0)
        {
            strcpy(ret_oid, token);
            token_cnt++;
        }
        else if (token_cnt == 1)
        {
            strcpy(type_value, token);
            token_cnt++;
        }
        // printf("Token: %s \n", token, result);
        token = strtok(NULL, delemeter); // Get the next token
    }

    const char *delemeter_val = ": ";
    token_cnt = 0;
    char *token_value = strtok(type_value, delemeter_val);

    while (token_value != NULL)
    {
        //printf("Token : %s\n", token_value);
        if (token_cnt == 0)
        {
            strcpy(type, token_value);
            token_cnt++;
        }
        else if (token_cnt == 1)
        {
            strcpy(value, token_value);
            token_cnt++;
        }
        token_value = strtok(NULL, delemeter_val); // Get the next token
    }


    cJSON* ret;
    //printf("\nOID: %s, TYPE: %s, VAL : %s\n", ret_oid, type, value);
    ret = oid_info_to_json(ret_oid, type, value, "");

    //cJSON *json_item = cJSON_CreateObject();
    //cJSON_AddItemReferenceToObject(json_item, oid, &ret);
    //printf(" Oid : %s\n", oid);
    //set_value_with_json(key, oid, &ret);
    return ret;
}

int read_config_file(char *filename, char lines[MAX_LINES][MAX_LENGTH])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    int line_count = 0;

    while (line_count < MAX_LINES && fgets(lines[line_count], MAX_LENGTH, file) != NULL)
    {
        // Remove newline character, if present
        lines[line_count][strcspn(lines[line_count], "\n")] = '\0';
        line_count++;
    }

    fclose(file);
    return line_count; // Return the number of lines read
}


