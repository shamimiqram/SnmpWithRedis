#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

void printCurrentTime()
{
    
    struct timeval tv;
    struct tm* ptm;
    char timeString[40];

    // Get the current time
    gettimeofday(&tv, NULL);
    
    // Convert to local time format
    ptm = localtime(&tv.tv_sec);

    // Format the date and time into a string
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
    
    // Print the time with milliseconds
    printf("%s.%03ld\n", timeString, tv.tv_usec / 1000);
}

void print_oid(oid *oid, size_t oid_len) {
    for (size_t i = 0; i < 1; i ++){
        if (i > 0) {
            printf(".");
        }
        printf("%ld", oid[i]);
    }
    printf("\n");
}

void print_variable_list(netsnmp_variable_list *vars) {
    while (vars) {
        
       printf("OID: ");
        print_oid(vars->name, vars->name_length);
        printf("Value: ");
        
        // Handle different variable types
        switch (vars->type) {
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
        //vars = vars->next_variable;  // Move to the next variable in the list
    }
}
int main(int argc, char **argv) {
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
    session.peername = strdup("103.218.25.9");  // SNMP agent address
    session.version = SNMP_VERSION_2c;       // SNMP version
    session.community = (u_char *)strdup("audramonitor");  // SNMP community string
    session.community_len = strlen((const char *)session.community);

    for(int i = 0; i < 10; i++)
    {
         ss = snmp_open(&session);
        if (!ss) {
            snmp_perror("snmp_open");
            exit(1);
        }

        // Create a PDU for the GET request
        pdu = snmp_pdu_create(SNMP_MSG_GET);
        oid oid[MAX_OID_LEN];
        size_t oid_len = MAX_OID_LEN;

        // OID for sysDescr (change as needed)
        if (!snmp_parse_oid(".1.3.6.1.2.1.25.2.3.1.0", oid, &oid_len)) {
            fprintf(stderr, "Failed to parse OID\n");
            return 1;
        }

        snmp_add_null_var(pdu, oid, oid_len);
        time_t t;   // not a primitive datatype
        time(&t);
        printf("\n\nSend GET request: ");
        printCurrentTime();

        // Send the request
        status = snmp_synch_response(ss, pdu, &response);
        //printf("Pass : 44  -- %d \n", status);

    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        /*
        * SUCCESS: Print the result variables
        */

        for(vars = response->variables; vars; vars = vars->next_variable)
            print_variable(vars->name, vars->name_length, vars);
            printf("GET Response : ");
            printCurrentTime();

        /* manipuate the information ourselves */
        for(vars = response->variables; vars; vars = vars->next_variable) {
            if (vars->type == ASN_OCTET_STR) {
        char *sp = (char *)malloc(1 + vars->val_len);
        memcpy(sp, vars->val.string, vars->val_len);
        sp[vars->val_len] = '\0';
            //printf("value #%d is a string: %s\n", count++, sp);
        free(sp);
        }
            //else
            //printf("value #%d is NOT a string! Ack!\n", count++);
        }
        } else {
        /*
        * FAILURE: print what went wrong!
        */

        if (status == STAT_SUCCESS)
            fprintf(stderr, "Error in packet\nReason: %s\n",
                    snmp_errstring(response->errstat));
        else if (status == STAT_TIMEOUT)
            fprintf(stderr, "Timeout: No response from %s.\n",
                    session.peername);
        else
            snmp_sess_perror("snmpdemoapp", ss);

        }

    }
    // Open the session
   

    printf("Pass : 55");
    // Clean up
     if (response)
      snmp_free_pdu(response);
    snmp_close(ss);
    return 0;
}
