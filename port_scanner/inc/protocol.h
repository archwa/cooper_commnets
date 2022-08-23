// Declare scanner functionality

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


// Include custom libraries
#include <scanner.h>


int* PortMap;
const char* ProtocolMap[65536];

int initializePortMap();
int initializeProtocolMap();

void printPorts(struct Host*, struct PortRange*);


#endif
