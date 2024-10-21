#include <stdio.h>
#include <stdlib.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_client.h>
#include "snmp_task.h"

void init_snmp_task() {
    // Initialize the SNMP library
    init_snmp("snmpapp");
}

void set_snmp_value(const char *oid, const char *value) {
    snmp_session session, *ss;
    snmp_pdu *pdu;
    snmp_pdu *response;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;

    // Initialize session
    snmp_sess_init(&session);
    session.version = SNMP_VERSION_2c;
    session.peername = "localhost"; // SNMP agent address
    session.community = (u_char *)"public"; // SNMP community string
    session.community_len = strlen((const char *)session.community);

    // Open session
    ss = snmp_open(&session);
    if (!ss) {
        snmp_perror("snmp_open");
        exit(1);
    }

    // Convert OID from string to an OID object
    anOID_len = MAX_OID_LEN;
    if (!snmp_parse_oid(oid, anOID, &anOID_len)) {
        snmp_perror(oid);
        exit(1);
    }

    // Create PDU for SET request
    pdu = snmp_pdu_create(SNMP_SET);
    snmp_add_var(pdu, anOID, anOID_len, 's', value);

    // Send the request
    int status = snmp_send(ss, pdu);
    if (status == 0) {
        snmp_perror("snmp_send");
    }

    // Clean up
    snmp_close(ss);
}
