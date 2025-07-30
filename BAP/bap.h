#ifndef bt_h
#define bt_h


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Defining states with #define
#define STATE_SEND_D0 0
#define STATE_WAIT_ACK0 1
#define STATE_SEND_D1 2
#define STATE_WAIT_ACK1 3
#define STATE_RECEIVE_D0 4
#define STATE_RECEIVE_D1 5
#define STATE_SEND_ACK0 6
#define STATE_SEND_ACK1 7
#define STATE_WAIT_D0  8
#define STATE_WAIT_D1  9


// Defining events with #define
#define EVENT_SEND_D0 0
#define EVENT_SEND_D1 1
#define EVENT_RECEIVE_D0 2
#define EVENT_RECEIVE_D1 3
#define EVENT_ACK0_RECEIVED 4
#define EVENT_ACK1_RECEIVED 5
#define EVENT_TIMEOUT 6
#define EVENT_ERROR 7
#define EVENT_WAIT_D0  8
#define EVENT_WAIT_D1  9

// Structure representing the ABP state machine
typedef struct {
    int currentState;
    int currentEvent;
} ABP;

// Function declarations
int receiveEvent(SOCKET, char *, int, struct sockaddr_in *, socklen_t);
int sendData(SOCKET, char *, int, struct sockaddr_in *, socklen_t );
int sendAck(SOCKET, char *, int, struct sockaddr_in *, socklen_t);
#endif
