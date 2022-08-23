// Define scanner functionality


// Include necessary standard libraries
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// Include custom libraries
#include <protocol.h>
#include <scanner.h>


int parseHost_byName(struct Host* host, const char* name) {
    // get all the host details
    struct Host newHost;
    newHost.name = name;
    
    // Get hostname
    if(!inet_aton(name, &newHost.address)) {
        // Perform DNS lookup
        if(!(newHost.entity = gethostbyname(name))) {
            fprintf(stdout, "ERROR: Invalid hostname '%s'!\n", name);
            return -1;
        }
    }
    
    *host = newHost;

    return 0;
}


int probeHost_atPort(const struct Host* host, int port) {
    int sock_fd;
    int failure;
    char* status;
    struct sockaddr_in sin;

    failure = 0;

    sin.sin_family = AF_INET;

    // Copy address
    memcpy(&sin.sin_addr.s_addr, host->entity->h_addr_list[0], sizeof sin.sin_addr.s_addr);

    // Copy port
    sin.sin_port = htons(port);
    
    // Create socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stdout, "ERROR: socket() failure: %s\n", strerror(errno));

        return -1;
    }

    // Attempt to instantiate connection
    if (connect(sock_fd, ((struct sockaddr *) &sin), sizeof sin) < 0) {
        failure = 1;

        if(errno == ECONNREFUSED || errno == EHOSTUNREACH) {
            PortMap[port] = PORT_CLOSED;
        } else if(errno == ETIMEDOUT) {
            PortMap[port] = PORT_DROPPED;
        } else {
            fprintf(stdout, "ERROR: connect() failure: %s ... %d\n", strerror(errno), errno);
            return -1;
        }
    }
    
    // Connection success
    else {
        PortMap[port] = PORT_OPEN;
    }

    if(!failure && shutdown(sock_fd, SHUT_RDWR) < 0) {
        fprintf(stdout, "ERROR: shutdown() failure: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}


int probeHost_usingRange(const struct Host* host, const struct PortRange* range) {
    int* pid;  // Array of child process IDs
    int port;  // Port iterator
    int pstat; // Probe status
    int wstat; // Wait status
    int eflag; // Exit status flag

    int lower, upper;
    lower = range->lower;
    upper = range->upper;
    eflag = 0;

    // Initialize array of child process IDs (index 0 corresponds to range.lower)
    pid = (int*) malloc((range->upper - range->lower + 1)*(sizeof(int)));

    while(lower < range->upper) {
        // Do some math to set the max port number
        if(range->upper - lower > MAX_FORK) {
            upper = lower + MAX_FORK - 1;
        }
        else {
            upper = range->upper;
        }

        // Iterate over each port of the host and probe it
        for(port = lower; port <= upper; port++) {
            switch(pid[port - range->lower] = fork()) {
                case -1:
                    // Error by fork()
                    fprintf(stderr, "ERROR: fork() failure: %s\n", strerror(errno));
                    exit(-1);
                case 0:
                    // In child
                    if((pstat = probeHost_atPort(host, port)) < 0) {
                        fprintf(stdout, "ERROR: Failure to probe port %d on host '%s'!\n", port, host->name);
                    }
                    exit(pstat);
                default:
                    // In parent
                    continue;
            }
        }

        // Wait for all children to exit
        while(port > lower) {
            wait(&wstat);
            eflag |= wstat;
            port--;
        }

        lower = upper + 1;
    }


    return eflag;
}
