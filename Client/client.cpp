#include "Menu.h"
#include "Game.h"

using namespace std;

int login(SOCKET sock, Account& acc, char* id, char* pass) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, id, pass, 0 };
    pthread_create(&tid, NULL, loginThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    return tArgs.result;
}

int registerAcc(SOCKET client, char* id, char* pass) {
    pthread_t tid;
    ThreadArgs tArgs = { client, NULL, id, pass, 0 };
    pthread_create(&tid, NULL, registerThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    return tArgs.result;
}
int logout(SOCKET sock, Account& acc) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0 };
    pthread_create(&tid, NULL, logoutThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    return tArgs.result;
}
int getHistory(SOCKET sock, Account& acc, string &payload) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0,payload };
    pthread_create(&tid, NULL, getHistoryThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    payload = tArgs.payload;
    return tArgs.result;
}
int getRank(SOCKET sock, Account& acc, string &payload) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0,payload };
    pthread_create(&tid, NULL, getRankThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    payload = tArgs.payload;
    return tArgs.result;
}
int startGame(SOCKET sock, Account& acc, string &payload) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0 };
    pthread_create(&tid, NULL, startGameThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    payload = tArgs.payload;
    return tArgs.result;
}
int createRoom(SOCKET sock, Account& acc, string &payload) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0,payload };
    pthread_create(&tid, NULL, createRoomThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    payload = tArgs.payload;
    return tArgs.result;
}
int joinRoomClient(SOCKET sock, Account& acc, string &payload) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0,payload };
    pthread_create(&tid, NULL, joinedRoomThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    payload = tArgs.payload;
    return tArgs.result;
}
int checkPass(string passRoom)
{
    if(passRoom.size()<6 || passRoom.size()>10)
    {
        return 2;
    }
    for(int i=0;i<passRoom.size();i++)
    {
        if(!isalpha(passRoom[i]) && !isdigit(passRoom[i]))
        {
            return 3;
        }
    }
    return 1;
}
int main() {
    Account acc;
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        cerr << "Socket creation failed.\n";
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6769);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Địa chỉ IP của máy chủ

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Connection failed.\n";
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    cout << "Connected to server.\n";
    bool isLoggedIn = false;
    char choice;
    do {
        while (isLoggedIn) {
            menu2(choice);
            switch (choice) {
                case '1': {
                    string payload;
                    int result = startGame(client_socket, acc, payload);
                    if (result == START_SUCCESS) {
                        cout << "Game started successfully.\n";
                        int room_id;
                        char opponent[25];
                        sscanf(payload.c_str(), "%d|%s", &room_id, opponent);
                        start_game(acc.user,opponent,room_id,acc,client_socket,false);
                    } else {
                        if (result == WAITTING_FOR_PLAYER) {
                            cout << "Waiting for player to join.\n";
                            cout << "This may take a few seconds.\n";
                            Message msg;
                            recv(client_socket, (char*)&msg, sizeof(msg), 0);
                            cout << msg.opcode << " " << msg.payload << "\n";
                            int room_id;
                            char opponent[25];
                            sscanf(msg.payload, "%d|%s", &room_id, opponent);
                            start_game(acc.user,opponent,room_id,acc,client_socket,true);
                        }
                        else 
                        cout << "Failed to start game.\n";
                    }
                    break;
                }
                case '2': {
                    int choicemenu3=0;
                    do {
                        menu3(choicemenu3);
                        switch (choicemenu3) {
                            case 1: {
                                string passRoom;
                                int check = 0;
                                while(check!=1)
                                {
                                    cout<<"Enter your password: ";
                                    cin>>passRoom;
                                    check = checkPass(passRoom);
                                    if(check == 2)
                                    {
                                        cout<<"Password must be between 6 and 10 characters\n";
                                    }
                                    else if(check == 3)
                                    {
                                        cout<<"Password must contain only letters and numbers\n";
                                    }
                                }
                                cout<<"Password: "<<passRoom<<endl;
                                int result = createRoom(client_socket, acc, passRoom);
                                int room_id;
                                sscanf(passRoom.c_str(), "%d", &room_id);
                                if (result == CREATE_ROOM_SUCCESS) {
                                    cout << "Room created successfully.\n";
                                    cout<<"You are in room "<<room_id<<endl;
                                    cout<< "Please wait for other players to join.\n";
                                    Message msg;
                                    recv(client_socket, (char*)&msg, sizeof(msg), 0);
                                    cout << msg.opcode << " " << msg.payload << "\n";
                                    char opponent[25];;
                                    sscanf(msg.payload, "%d|%s", &room_id, opponent);
                                    start_game(acc.user,opponent,room_id,acc,client_socket,true);
                                }
                                break;
                            }
                            case 2: {
                                int roomID;
                                int check = 0;
                                while(check!=1)
                                {
                                    cout<<"Enter room ID: ";
                                    cin>>roomID;
                                    if(roomID<0)
                                    {
                                        cout<<"Room ID must be greater than 0\n";
                                    }
                                    else
                                    {
                                        check = 1;
                                    }
                                }
                                check = 0;
                                string passRoom;
                                while(check!=1)
                                {
                                    cout<<"Enter your password: ";
                                    cin>>passRoom;
                                    check = checkPass(passRoom);
                                    if(check == 2)
                                    {
                                        cout<<"Password must be between 6 and 10 characters\n";
                                    }
                                    else if(check == 3)
                                    {
                                        cout<<"Password must contain only letters and numbers\n";
                                    }
                                }
                                string payload = "";
                                payload = to_string(roomID) + "|" + passRoom;
                                int result = joinRoomClient(client_socket, acc, payload);
                                if (result == JOIN_ROOM_SUCCESS) {
                                    cout << "Joined room successfully.\n";
                                    int room_id;
                                    char opponent[25];
                                    sscanf(payload.c_str(), "%d|%s", &room_id, opponent);
                                    start_game(acc.user,opponent,room_id,acc,client_socket,false);
                                }
                                else if(result == JOIN_ROOM_FAIL)
                                {
                                    cout << passRoom;
                                    cout << "Press any key to continue...";
                                    getchar();
                                }
                                break;
                            }
                            case 3: {
                                break;
                            }
                            default: {
                                cout << "Invalid choice. Please try again.\n";
                                break;
                            }
                        }
                    } while (choicemenu3 != 1 && choicemenu3 != 2 && choicemenu3 != 3);
                    break;
                }
                case '3': {
                    string payload="";
                    int result = getRank(client_socket, acc, payload);
                    if (result == GET_RANK_SUCCESS) {
                        cout << payload << "\n";
                    }
                    else {
                        cout << "Failed to fetch rank.\n";
                    }
                    break;
                }
                case '4': {
                    string payload="";
                    int result = getHistory(client_socket, acc, payload);
                    if (result == GET_HISTORY_SUCCESS) {
                            cout << payload << "\n";
                        } else {
                            cout << "Failed to fetch history.\n";
                        }
                    break;
                }
                case '5': {
            int result = logout(client_socket, acc);
            if (result == LOGOUT_SUCCESS) {
                isLoggedIn = false;
                cout << "Logged out successfully.\n";
            } else {
                cout << "Logout failed.\n";
            }
            break;
                }
                default: {
                    cout << "Invalid choice. Please try again.\n";
                    break;
                }
            }
        }
        menu1(choice);

        switch (choice) {
            case '1': {
                char id[25], pass[25];
                cout << "Enter username: ";
                cin.getline(id, sizeof(id));
                cout << "Enter password: ";
                cin.getline(pass, sizeof(pass));
                int result = login(client_socket, acc, id, pass);
                if(result == LOGIN_SUCCESS) {
                    strcpy(acc.user, id);
                    acc.user[sizeof(acc.user) - 1] = '\0';
                    strcpy(acc.pass, pass);
                    acc.user[sizeof(acc.user) - 1] = '\0';
                    isLoggedIn = true;
                }
                break;
            }
            case '2': {
                char id[25], pass[25], confirmPass[25];
                cout << "Enter new username: ";
                cin.getline(id, sizeof(id));

                bool passwordsMatch = false;
                do {
                    cout << "Enter new password: ";
                    cin.getline(pass, sizeof(pass));

                    cout << "Confirm new password: ";
                    cin.getline(confirmPass, sizeof(confirmPass));

                    if (strcmp(pass, confirmPass) == 0) {
                        passwordsMatch = true;
                    } else {
                        cout << "Passwords do not match. Please try again.\n";
                    }
                } while (!passwordsMatch);
                // Gọi hàm đăng ký
                int registerResult = registerAcc(client_socket, id, pass);

                if (registerResult == REGISTER_SUCCESS) {
                    strcpy(acc.user, id);
                    acc.user[sizeof(acc.user) - 1] = '\0';
                    strcpy(acc.pass, pass);
                    acc.user[sizeof(acc.user) - 1] = '\0';
                    isLoggedIn = true;
                    // Có thể thực hiện các hành động sau khi đăng ký thành công
                } else {
                    cout << "Registration failed. Error code: " << registerResult << endl;
                }
                break;
            }
            case '3':
                cout << "Quitting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != '3');

    close(client_socket);

    return 0;
}
