// Define usage functionality


// Include necessary standard libraries
#include <stdio.h>

// Include custom libraries
#include <usage.h>

int printUsage() {
    printf("Usage:\t\t{binary} PORT_RANGE_START PORT_RANGE_END LIST_OF_HOSTS\n");
    printf("Example:\t{binary} 1 100 192.168.1.1 192.168.1.2 192.168.1.3\n");

    return 0;
}
