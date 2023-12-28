#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
typedef int SOCKET;
#define DELIMETER " "
#define PAYLOAD_SIZE 1024
#define BUFF_SIZE 1024

typedef struct Account1 {
	char id[6];
	char user[25];
	char pass[25];
	int role;
}Account;

typedef struct ThreadArguments {
    SOCKET sock;
    Account* acc;
    char* id;
    char* pass;
    int result;
} ThreadArgs;

typedef struct message1 {
	int opcode;
	char payload[PAYLOAD_SIZE];
} Message;


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
enum FRAME {
	FIRST_FRAME = 1,
	SECOND_FRAME = 2,
	THIRD_FRAME = 3,
	EXIT = 4
};