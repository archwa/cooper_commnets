// Include necessary standard libraries
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// Include custom libraries
#include <protocol.h>
#include <scanner.h>
#include <usage.h>


// Run the actual program
int main(int argc, char** argv) {
    struct PortRange range;
    struct Host host;
    int i;

    // Initialize the protocol and port maps
    initializeProtocolMap();
    initializePortMap();

    // Catch incorrect usage
    if(argc < 4) {
        printUsage();

        return -1;
    }

    // Extract port range
    range.lower = (int) strtoll(argv[POS_PORT_RANGE_START], NULL, 0);
    range.upper = (int) strtoll(argv[POS_PORT_RANGE_END], NULL, 0);
    if(range.lower == LLONG_MIN || range.lower == LLONG_MAX
        || range.upper == LLONG_MIN || range.lower == LLONG_MAX) {
        fprintf(stdout, "ERROR: Failure to convert ports!\n");

        return -1;
    }
    
    // Obtain host info, then probe
    for(i = POS_HOSTS; i < argc; i++) {
        // Parse host name for host information
        if(parseHost_byName(&host, argv[i]) < 0) {
            fprintf(stdout, "ERROR: Host '%s' could not be parsed!\n", argv[i]);
        }

        // Probe host using specified port range
        else if(probeHost_usingRange(&host, &range) < 0) {
            fprintf(stdout, "ERROR: Failure to probe all specified ports on host '%s'!\n", argv[i]);
        }

        // Print the results for the given host
        else {
            printPorts(&host, &range);
        }
    }

    return 0;
}
