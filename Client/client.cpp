#include "Account.h"
#include "Menu.h"

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
int getHistory(SOCKET sock, Account& acc) {
    pthread_t tid;
    ThreadArgs tArgs = { sock, &acc, NULL, NULL, 0 };
    pthread_create(&tid, NULL, getHistoryThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    return tArgs.result;
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
    server_addr.sin_port = htons(8080);
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
                    break;
                }
                case '2': {
                    break;
                }
                case '3': {
                    break;
                }
                case '4': {
                    int result = getHistory(client_socket, acc);
                    if (result == GET_HISTORY_SUCCESS) {
                        cout<<result<<'\n';
                    }
                    break;
                }
                case '5': {
                    int result = logout(client_socket, acc);
                    if (result == LOGOUT_SUCCESS) {
                        isLoggedIn = false;
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
                    strcpy(acc.pass, pass);
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
                    strcpy(acc.pass, pass);
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
