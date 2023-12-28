#include "Resources.h"
using namespace std;
void loadAccountTxt(vector<Account>& list) {
    ifstream file_(FILE_ACCOUNT);
    int count = 0;

    if (file_.is_open()) { // Kiểm tra xem file có mở thành công không
        while (!file_.eof()) {
            Account acc = {}; // Khởi tạo một Account mới với giá trị mặc định
            file_ >> acc.id >> acc.user >> acc.pass >> acc.status >> acc.islogin;

            // Kiểm tra xem việc đọc từ file có thành công không
            if (file_.fail()) {
                break; // Đã đọc hết hoặc có lỗi xảy ra khi đọc, thoát khỏi vòng lặp
            }

            list.push_back(acc);
            count++;
        }
        cout << " => LOADED "<< count << " account(s)" << '\n';
    } else {
        cout << " => ERROR : Cannot open file " << FILE_ACCOUNT << '\n';
    }

    file_.close();
}
void addAccountTxt(const string& id, const string& user, const string& pass, int status = 1, int islogin=0) {
    ofstream file_(FILE_ACCOUNT, ios::app); // Mở file để ghi tiếp vào cuối file

    if (file_.is_open()) {
        file_ << '\n'<<id << " " << user << " " << pass << " " << status<<" "<<islogin;
        cout << " => Added new account: " << user << endl;
    } else {
        cout << " => ERROR : Cannot open file " << FILE_ACCOUNT << " for writing\n";
    }

    file_.close();
}

void printListAcc(vector<Account> list) {
	for (int i = 0; i < list.size(); i++) {
		cout << list[i].id << " " << list[i].user << " " << list[i].pass << " " << list[i].status<<" "<<list[i].islogin << endl;
	}
}
void add_history_file(const string& user) {
    ofstream file_(FILE_HISTORY+user+".txt", ios::app);
    file_.close();
}
void get_history(const string& user) {
    ifstream file_(FILE_HISTORY+user+".txt");
    string line;
    while (getline(file_, line)) {
        if (line.find(user) != string::npos) {
            cout << line << endl;
        }
    }
    file_.close();
}
void *handleClient(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFF_SIZE];
    int bytes_received;
    loadAccountTxt(listAccount);
    printListAcc(listAccount);

    while (true) {
        Message receivedMsg;
        bytes_received = recv(client_socket, (char *)&receivedMsg, sizeof(receivedMsg), 0);

        if (bytes_received <= 0) {
            cerr << "Connection closed by client.\n";
            break;
        }

        cout << "Received opcode: " << receivedMsg.opcode << " from client " << client_socket << "\n";

        switch (receivedMsg.opcode) {
            case LOGIN:
            {
                cout << "Received LOGIN request with payload: " << receivedMsg.payload << "\n";
                char login_user[25];
                char login_pass[25];
                if (sscanf(receivedMsg.payload, "%24[^|]|%24s", login_user, login_pass) == 2) {
                    // Lấy được user và pass thành công từ payload
                    cout << "Username: " << login_user << ", Password: " << login_pass << endl;
                    bool found = false;
                    for (int i = 0; i < listAccount.size(); i++) {
                        if (strcmp(login_user, listAccount[i].user) == 0 && strcmp(login_pass, listAccount[i].pass) == 0) {
                            found = true;
                            if (listAccount[i].status == 0) {
                                Message responseMsg;
                                responseMsg.opcode = CANT_LOGIN;
                                strncpy(responseMsg.payload, "Your account is locked!", sizeof(responseMsg.payload));
                                send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                                break;
                            }
                            if(listAccount[i].islogin == 1){
                                Message responseMsg;
                                responseMsg.opcode = ALREADY_LOGIN;
                                strncpy(responseMsg.payload, "You are already logged in!", sizeof(responseMsg.payload));
                                send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                                break;
                            }
                            Message responseMsg;
                            responseMsg.opcode = LOGIN_SUCCESS;
                            strncpy(responseMsg.payload, "Login successful!", sizeof(responseMsg.payload));
                            send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                            break;
                        }
                    }
                    if (!found) {
                        // Username và password không khớp hoặc không tồn tại
                        Message responseMsg;
                        responseMsg.opcode = ID_NOT_FOUND;
                        strncpy(responseMsg.payload, "Username or password is incorrect!", sizeof(responseMsg.payload));
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                    }
                } else {
                    // Không thể lấy được user và pass từ payload
                    cerr << "Failed to extract username and password.\n";
                }
                break;
            }

            case REGISTER:
            {
                cout << "Received REGISTER request with payload: " << receivedMsg.payload << "\n";
                char reg_user[25];
                char reg_pass[25];
                if (sscanf(receivedMsg.payload, "%24[^|]|%24s", reg_user, reg_pass) == 2) {
                    // Lọc user và pass từ payload
                    cout << "Username: " << reg_user << ", Password: " << reg_pass << endl;
                    bool usernameExists = false;
                    for (int i = 0; i < listAccount.size(); i++) {
                        if (strcmp(reg_user, listAccount[i].user) == 0) {
                            usernameExists = true;
                            Message responseMsg;
                            responseMsg.opcode = ACCOUNT_EXIST;
                            strncpy(responseMsg.payload, "Username already exists!", sizeof(responseMsg.payload));
                            send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                            break;
                        }
                    }
                    if (!usernameExists) {
                        int idmax = 0;
                        for (int i = 0; i < listAccount.size(); i++) {
                            if (stoi(listAccount[i].id) > idmax) {
                                idmax = stoi(listAccount[i].id);
                            }
                        }
                        idmax++;
                        //thêm acc vừa tạo vào listAccount
                        Account acc;
                        strncpy(acc.id, to_string(idmax).c_str(), sizeof(acc.id) - 1);
                        acc.id[sizeof(acc.id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi

                        strncpy(acc.user, reg_user, sizeof(acc.user) - 1);
                        acc.user[sizeof(acc.user) - 1] = '\0'; // Đảm bảo kết thúc chuỗi

                        strncpy(acc.pass, reg_pass, sizeof(acc.pass) - 1);
                        acc.pass[sizeof(acc.pass) - 1] = '\0'; // Đảm bảo kết thúc chuỗi

                        acc.status = 1;
                        acc.islogin = 1;
                        listAccount.push_back(acc);
                        add_history_file(reg_user);
                        addAccountTxt(to_string(idmax), reg_user, reg_pass, 1);
                        Message responseMsg;
                        responseMsg.opcode = REGISTER_SUCCESS;
                        strncpy(responseMsg.payload, "Register successful!", sizeof(responseMsg.payload));
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                    }
                } else {
                    // Không thể lọc user và pass từ payload
                    cerr << "Failed to extract username and password.\n";
                }
                break;
            }
            case LOGOUT:
            {
                cout << "Received LOGOUT request with payload: " << receivedMsg.payload << "\n";
                
                char logout_user[25];
                char logout_pass[25];
                if (sscanf(receivedMsg.payload, "%24[^|]|%24s", logout_user, logout_pass) == 2) {
                    // Lọc user và pass từ payload
                    cout << "Username: " << logout_user << ", Password: " << logout_pass << endl;
                    bool found = false;
                    for (int i = 0; i < listAccount.size(); i++) {
                        if (strcmp(logout_user, listAccount[i].user) == 0 && strcmp(logout_pass, listAccount[i].pass) == 0) {
                            found = true;
                            listAccount[i].islogin = 0;
                            Message responseMsg;
                            responseMsg.opcode = LOGOUT_SUCCESS;
                            strncpy(responseMsg.payload, "Logout successful!", sizeof(responseMsg.payload));
                            send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                            break;
                        }
                    }
                    if (!found) {
                        // Username và password không không hop le
                        Message responseMsg;
                        responseMsg.opcode = ID_NOT_FOUND;
                        strncpy(responseMsg.payload, "Username or password is incorrect!", sizeof(responseMsg.payload));
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                    }

                }
                break;
            }
            case GET_HISTORY:
            {
                cout << "Received GET_HISTORY request with payload: " << receivedMsg.payload << "\n";
                char get_history_user[25];
                char get_history_pass[25];
                if (sscanf(receivedMsg.payload, "%24[^|]|%24s", get_history_user, get_history_pass) == 2) {
                    // Lọc user và pass từ payload
                    cout << "Username: " << get_history_user << ", Password: " << get_history_pass << endl;
                    get_history(get_history_user);
                    Message responseMsg;
                    responseMsg.opcode = GET_HISTORY_SUCCESS;
                    strncpy(responseMsg.payload, "Get history successful!", sizeof(responseMsg.payload));
                    send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                }
                break;
            }
            default:
                cerr << "Unknown opcode received.\n";
                break;
            }
    }
    close(client_socket);
    pthread_exit(NULL);
}


int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    socklen_t client_addr_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Socket creation failed.\n";
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bind failed.\n";
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 5);
    cout << "Server is running...\n";

    while (true) {
        int client_socket;
        struct sockaddr_in client_addr;

        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket == -1) {
            cerr << "Accept connection failed.\n";
            continue;
        }

        cout << "Connect from: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << "\n";

        // Tạo một luồng mới để xử lý kết nối từ client
        pthread_t tid;
        int *client_sock = new int;
        *client_sock = client_socket;

        if (pthread_create(&tid, NULL, handleClient, (void *)client_sock) != 0) {
            cerr << "Failed to create thread.\n";
            delete client_sock;
            continue;
        }

        // Note: Điều này sẽ cho phép server tiếp tục chấp nhận các kết nối mới
        // trong khi mỗi kết nối đang chạy trong một luồng riêng biệt.
    }

    close(server_socket);

    return 0;
}
