// Define protocol functionality


// Include necessary standard libraries
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// Include custom libraries
#include <protocol.h>
#include <scanner.h>


int initializePortMap() {
    // Initialize for all possible ports
    if((PortMap = (int*) mmap(NULL, 65536*sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED) {
        fprintf(stdout, "oh no...\n");
    }
}

int initializeProtocolMap() {
    int i;

    // Initialize map with ???
    for(i = 0; i < 65536; i++) {
        ProtocolMap[i] = "???";
    }

    ProtocolMap[21] = "FTP";
    ProtocolMap[22] = "SSH";
    ProtocolMap[23] = "TELNET";
    ProtocolMap[25] = "SMTP";
    ProtocolMap[53] = "DNS";
    ProtocolMap[69] = "TFTP";
    ProtocolMap[80] = "HTTP";
    ProtocolMap[110] = "POP3";
    ProtocolMap[123] = "NTP";
    ProtocolMap[143] = "IMAP";
    ProtocolMap[389] = "LDAP";
    ProtocolMap[443] = "HTTPS";
    ProtocolMap[547] = "DHCP";
    
    return 0;
}

void printPorts(struct Host* host, struct PortRange* range) {
    int port;
    const char* status[3] = {
        "OPEN",
        "DROPPED",
        "CLOSED"
    };

    for(port = range->lower; port <= range->upper; port++) {
        fprintf(stdout, "%s,%d,%s,%s\n", host->name, port, status[PortMap[port]], ProtocolMap[port]);
    }
}
