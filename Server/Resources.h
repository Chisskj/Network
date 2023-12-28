#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

typedef int SOCKET;
#define DELIMETER " "

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define BACKLOG 20
#define RECEIVE 0
#define SEND 1
#define ALL 0
#define MISSING 1

#define BUFF_SIZE 2048
#define MAX_USER 100
#define PAYLOAD_SIZE 1024
#define MESSAGE_SIZE 1036
#define FILE_ACCOUNT "account/account.txt"
#define FILE_HISTORY "history/"
typedef struct Account {
	SOCKET sock;
	char id[6];
	char user[25];
	char pass[25];
	int status;
	int islogin;
}Account;
vector<Account> listAccount;
enum ACCOUNT_REQUEST {
	LOGIN = 10,
	REGISTER = 11,
	LOGOUT = 12,
};
enum GAME_REQUEST {
	START = 20,
	PLAY = 21,
	END = 22,
	GET_HISTORY = 23,
};
enum GAME_RESPONSE {
	START_SUCCESS = 200,
	PLAY_SUCCESS = 201,
	END_SUCCESS = 202,
	GET_HISTORY_SUCCESS = 203,
};
enum ACCOUNT_RESPONSE {
	LOGIN_SUCCESS = 100,
	LOGOUT_SUCCESS = 101,

	WRONG_INPUT = 102,
	ID_NOT_FOUND = 103,
	WRONG_PASSWORD = 104,
	ALREADY_LOGIN = 105,
	CANT_LOGIN = 106,
	HAVENT_LOGIN = 107,
	BLOCKED = 108,

	ACCOUNT_EXIST = 109,
	REGISTER_SUCCESS = 110,
	CANT_REGISTER = 111,
};
typedef struct Message {
	int opcode;
	char payload[PAYLOAD_SIZE];
}Message;
