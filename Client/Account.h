#include "Resources.h"
#include <iostream>

void handleAccountResponse(int msgOpcode) {
	switch (msgOpcode) {
		case LOGIN_SUCCESS:
			cout << " => SYSTEM : Login succesfully" << '\n';
			break;
		case ALREADY_LOGIN:
			cout << " => SYSTEM : You've already login" << '\n';
			break;
		case CANT_LOGIN:
			cout << " => SYSTEM : Can't login cus some one is using" << '\n';
			break;
		case BLOCKED:
			cout << " => SYSTEM : Your account is blocked" << '\n';
			break;
		case ID_NOT_FOUND:
			cout << " => SYSTEM : Account is not found" << '\n';
			break;
		case WRONG_PASSWORD:
			cout << " => SYSTEM : Wrong password" << '\n';
			break;
		case REGISTER_SUCCESS:
			cout << " => SYSTEM : Register successfully. You can login now" << '\n';
			break;
		case ACCOUNT_EXIST:
			cout << " => SYSTEM : Username has already exist" << '\n';
			break;
		case LOGOUT_SUCCESS:
			cout << " => SYSTEM : Logout sucessfully" << '\n';
			break;
	}
}

void sendMessage(SOCKET sock, int opcode, const char* payload) {
    Message msg;
    msg.opcode = opcode;
    strncpy(msg.payload, payload, sizeof(msg.payload));

    send(sock, (char*)&msg, sizeof(msg), 0);
}

// Modify the loginThread function to use the new sendMessage function
void* loginThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET sock = tArgs->sock;
    Account* acc = tArgs->acc;
    char* id = tArgs->id;
    char* pass = tArgs->pass;

    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s", id, pass);
    printf("Message: %s\n", message);

    sendMessage(sock, LOGIN, message); // Assuming LOGIN is the opcode for login

    Message receivedMsg;
    recv(sock, (char*)&receivedMsg, sizeof(receivedMsg), 0);
    tArgs->result = receivedMsg.opcode;
    handleAccountResponse(receivedMsg.opcode);

    pthread_exit(NULL);
}

// Modify the registerThread function similarly
void* registerThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET client = tArgs->sock;
    char* id = tArgs->id;
    char* pass = tArgs->pass;

    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s", id, pass);

    sendMessage(client, REGISTER, message); // Assuming REGISTER is the opcode for registration

    Message receivedMsg;
    recv(client, (char*)&receivedMsg, sizeof(receivedMsg), 0);
    tArgs->result = receivedMsg.opcode;
    handleAccountResponse(receivedMsg.opcode);

    pthread_exit(NULL);
}
void* logoutThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET client = tArgs->sock;
    Account* acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s", acc->user, acc->pass);
    sendMessage(client, LOGOUT, message);

    Message receivedMsg;
    recv(client, (char*)&receivedMsg, sizeof(receivedMsg), 0);
    tArgs->result = receivedMsg.opcode;
    handleAccountResponse(receivedMsg.opcode);

    pthread_exit(NULL);
}
void* getHistoryThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET client = tArgs->sock;
    Account* acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s", acc->user, acc->pass);
    sendMessage(client, GET_HISTORY, message);

    Message receivedMsg;
    recv(client, (char*)&receivedMsg, sizeof(receivedMsg), 0);
    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}

void *getRankThread(void *args) {
    ThreadArgs *tArgs = (ThreadArgs *) args;
    SOCKET client = tArgs->sock;
    Account *acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s", acc->user, acc->pass);
    sendMessage(client, GET_RANK, message);

    Message receivedMsg;
    recv(client, (char *) &receivedMsg, sizeof(receivedMsg), 0);
    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}
void* startGameThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET client = tArgs->sock;
    Account* acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s", acc->user, acc->pass);
    sendMessage(client, START, message);

    Message receivedMsg;
    recv(client, (char*)&receivedMsg, sizeof(receivedMsg), 0);

    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}
void* createRoomThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET client = tArgs->sock;
    Account* acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s|%s", acc->user, acc->pass, tArgs->payload.c_str());
    sendMessage(client, CREATE_ROOM, message);

    Message receivedMsg;
    recv(client, (char*)&receivedMsg, sizeof(receivedMsg), 0);

    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}
void* joinedRoomThread(void* args) {
    ThreadArgs* tArgs = (ThreadArgs*)args;
    SOCKET client = tArgs->sock;
    Account* acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s|%s|%s", acc->user, acc->pass, tArgs->payload.c_str());
    sendMessage(client, JOIN_ROOM, message);

    Message receivedMsg;
    recv(client, (char*)&receivedMsg, sizeof(receivedMsg), 0);

    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}

void* storeHistoryThread(void *args) {
    ThreadArgs *tArgs = (ThreadArgs *) args;
    SOCKET client = tArgs->sock;
    Account *acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s", tArgs->payload.c_str());
    sendMessage(client, STORE_HISTORY, message);

    Message receivedMsg;
    recv(client, (char *) &receivedMsg, sizeof(receivedMsg), 0);

    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}
void* FFThread(void *args) {
    ThreadArgs *tArgs = (ThreadArgs *) args;
    SOCKET client = tArgs->sock;
    Account *acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s", tArgs->payload.c_str());
    sendMessage(client, SURRENDER, message);

    Message receivedMsg;
    recv(client, (char *) &receivedMsg, sizeof(receivedMsg), 0);
    
    pthread_exit(NULL);
}
void* getOnlinePlayersThread(void *args) {
    ThreadArgs *tArgs = (ThreadArgs *) args;
    SOCKET client = tArgs->sock;
    Account *acc = tArgs->acc;
    char message[PAYLOAD_SIZE];
    sprintf(message, "%s", tArgs->payload.c_str());
    sendMessage(client, GET_ONLINE_PLAYERS, message);

    Message receivedMsg;
    recv(client, (char *) &receivedMsg, sizeof(receivedMsg), 0);

    tArgs->result = receivedMsg.opcode;
    tArgs->payload = receivedMsg.payload;

    pthread_exit(NULL);
}