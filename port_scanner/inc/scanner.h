// Declare scanner functionality

#ifndef __SCANNER_H__
#define __SCANNER_H__


// Include necessary standard libraries
#include <netinet/in.h>


#define PORT_OPEN    0
#define PORT_DROPPED 1
#define PORT_CLOSED  2

#define MAX_FORK 512

// Port range
struct PortRange {
    int lower;
    int upper;
};

// Host information
struct Host {
	struct hostent* entity; // Information about the host
	struct in_addr address; // Actual address in network byte order
    const char* name;
};


// Host utilities
int parseHost_byName(struct Host* host, const char* name);

// Probing utilities
int probeHost_usingRange(const struct Host* host, const struct PortRange* range);
int probeHost_atPort(const struct Host* host, int port);


#endif
