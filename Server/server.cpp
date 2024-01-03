#include "room.h"
using namespace std;
mutex listAccountMutex;
mutex fileAccountMutex;
bool compareAccounts(const Account &a, const Account &b) {
    return a.point > b.point; // So sánh theo điểm số giảm dần
}
void loadAccountTxt(vector<Account>& list) {
    vector<Account> listLoad;
    ifstream file_(FILE_ACCOUNT);
    int count = 0;

    if (file_.is_open()) { // Kiểm tra xem file có mở thành công không
        while (!file_.eof()) {
            Account acc = {}; // Khởi tạo một Account mới với giá trị mặc định
            file_ >> acc.id >> acc.user >> acc.pass >> acc.status >> acc.islogin >> acc.point;

            // Kiểm tra xem việc đọc từ file có thành công không
            if (file_.fail()) {
                break; // Đã đọc hết hoặc có lỗi xảy ra khi đọc, thoát khỏi vòng lặp
            }

            listLoad.push_back(acc);
            count++;
        }
        cout << " => LOADED "<< count << " account(s)" << '\n';
    } else {
        cout << " => ERROR : Cannot open file " << FILE_ACCOUNT << '\n';
    }
    list = listLoad;
    sort(list.begin(), list.end(), compareAccounts);
    file_.close();
}
void addAccountTxt(const string& id, const string& user, const string& pass, int status = 1, int islogin=0, int point = 0) {
    ofstream file_(FILE_ACCOUNT, ios::app); // Mở file để ghi tiếp vào cuối file

    if (file_.is_open()) {
        file_ << '\n'<<id << " " << user << " " << pass << " " << status<<" "<<islogin <<" "<<point;
        cout << " => Added new account: " << user << endl;
    } else {
        cout << " => ERROR : Cannot open file " << FILE_ACCOUNT << " for writing\n";
    }

    file_.close();
}

void printListAcc(vector<Account> list) {
	for (int i = 0; i < list.size(); i++) {
		cout << list[i].id << " " << list[i].user << " " << list[i].pass << " " << list[i].status<<" "<<list[i].islogin << " " << list[i].point << endl;
	}
}
void add_history_file(const string& user) {
    ofstream file_(FILE_HISTORY+user+".txt", ios::app);
    file_.close();
}
void storeHistoryToDB(string user, int score, int winner, string name) {
    cout<<"storeHistoryToDB"<< " "<< user << " " << score << " " << winner << " " << name <<endl;
    // Lưu lại dữ liệu hiện có
    std::ifstream infile(FILE_HISTORY + name + ".txt");
    std::string restOfFile((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
    infile.close();

    // Mở tệp tin để ghi
    std::ofstream file_(FILE_HISTORY + name + ".txt");

    if (file_.is_open()) {
        // Đẩy dữ liệu mới vào đầu tệp tin
        file_ << time(0) << " " << user << " " << score << " " << winner << std::endl << restOfFile;

        file_.close();
    } else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}


void updatePoints(string user, int point) {
    listAccountMutex.lock();
    for (int i = 0; i < listAccount.size(); i++) {
        if (listAccount[i].user == user) {
            listAccount[i].point += point;
            break;
        }
    }
    listAccountMutex.unlock();

    // Đồng bộ hóa việc ghi vào tệp FILE_ACCOUNT
    fileAccountMutex.lock();
    ifstream file_(FILE_ACCOUNT);
    if (file_.is_open()) {
        string line;
        stringstream updatedData;

        while (getline(file_, line)) {
            istringstream iss(line);
            Account acc;
            if (iss >> acc.id >> acc.user >> acc.pass >> acc.status >> acc.islogin >> acc.point) {
                if (user == acc.user) {
                    acc.point += point; // Sửa điểm
                }
                updatedData << acc.id << " " << acc.user << " " << acc.pass << " " << acc.status << " " << acc.islogin << " " << acc.point << "\n";
            }
        }

        file_.close();

        // Mở lại tệp để ghi lại dữ liệu đã được cập nhật
        ofstream outfile(FILE_ACCOUNT);
        outfile << updatedData.str();
        outfile.close();
    } else {
        cerr << "Unable to open file!" << endl;
    }
    fileAccountMutex.unlock();
}
string timestampToDateTime(time_t timestamp) {
    struct tm *timeinfo;
    char buffer[30];

    timeinfo = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%Y:%m:%d:%H:%M:%S", timeinfo);
    return string(buffer);
}


string get_history(const string& user) {
    string history = "";
    cout << "History of " << FILE_HISTORY + user + ".txt" << ":\n";
    ifstream file_(FILE_HISTORY + user + ".txt");
    string line;
    int sl=0;
    while (getline(file_, line)) {
        sl++;
        int stt, status, point;
        char name[25]; // Định nghĩa một mảng ký tự để lưu trữ tên
        sscanf(line.c_str(), "%d %24s %d %d", &stt, name, &status, &point);
        string time = timestampToDateTime(stt);
        string name_str(name); // Chuyển đổi tên từ mảng ký tự sang string
       history += time + " " + name_str + " " + to_string(status) + " " + to_string(point) + "\n";
        if(sl==5) break;
    }
    file_.close();
    return history;
}
string getRank() {
    string rank = "";
    sort(listAccount.begin(), listAccount.end(), compareAccounts);
    int sl = min(5, (int)listAccount.size());
    for (int i = 0; i < sl; i++) {
        rank += (string)listAccount[i].user + " " + to_string(listAccount[i].point) + "\n";
    }
    return rank;
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
            for (int i = 0; i < listAccount.size(); ++i) {
                if (listAccount[i].sock == client_socket) {
                    listAccount[i].islogin = 0;
                    break;
                }
            }
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
                            listAccount[i].islogin = 1;
                            listAccount[i].sock = client_socket;
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
                        acc.sock = client_socket;
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
                    string history = get_history(get_history_user);
                    Message responseMsg;
                    responseMsg.opcode = GET_HISTORY_SUCCESS;
                    strncpy(responseMsg.payload, history.c_str(), sizeof(responseMsg.payload));
                    send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                }
                break;
            }
            case START: {
                cout << "Received START request with payload: " << receivedMsg.payload << "\n";
                char start_user[25];
                char start_pass[25];
                if (sscanf(receivedMsg.payload, "%24[^|]|%24s", start_user, start_pass) == 2) {
                    // Lọc user và pass của payload
                    Account accUser;
                    bool found = false;
                    for (int i = 0; i < listAccount.size(); i++) {
                        if (strcmp(start_user, listAccount[i].user) == 0 && strcmp(start_pass, listAccount[i].pass) == 0) {
                            accUser = listAccount[i];
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        Message responseMsg;
                        responseMsg.opcode = ID_NOT_FOUND;
                        strncpy(responseMsg.payload, "Username or password is incorrect!", sizeof(responseMsg.payload));
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                    } else {
                        bool joinedRoom = false;
                        for (int j = 0; j < rooms.size(); j++) {
                            if (rooms[j].ispublic == 0  && rooms[j].players.size() < 2) {
                                joinRoom(rooms[j].roomID, accUser , "");
                                joinedRoom = true;
                                cout<< "Joined room " << rooms[j].roomID << endl;
                                cout<< "Players: " << rooms[j].players.size() << endl;
                                Message responseMsg;
                                responseMsg.opcode = START_SUCCESS;
                                char mess[PAYLOAD_SIZE];
                                sprintf(mess, "%d|%s", rooms[j].roomID, rooms[j].players[1].user);
                                strncpy(responseMsg.payload, mess, sizeof(responseMsg.payload));
                                send(rooms[j].players[0].sock, (char *)&responseMsg, sizeof(responseMsg), 0);
                                sprintf(mess, "%d|%s", rooms[j].roomID, rooms[j].players[0].user);
                                strncpy(responseMsg.payload, mess, sizeof(responseMsg.payload));
                                send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                                break;
                            }

                        }
                        if (!joinedRoom) {
                            createRoom(rooms.size(), 0, "");
                            joinRoom(rooms.size() - 1, accUser, "");
                            Message responseMsg;
                            responseMsg.opcode = WAITTING_FOR_PLAYER;
                            strncpy(responseMsg.payload, "Waiting for other player...", sizeof(responseMsg.payload));
                            send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                        }
                    }
                } else {
                    cerr << "Failed to extract username and password.\n";
                }
                break;
            }
            case SET_SHIP: {
                cout << "Received SET_SHIP request with payload: " << receivedMsg.payload << "\n";
                int roomID;
                int col;
                int row;
                char userName[25];
                char align; // Đổi align thành char
                if (sscanf(receivedMsg.payload, "%24[^|]|%d|%d|%d|%c", userName, &roomID, &col, &row, &align) == 5) {
                    rooms[roomID].playersReady[userName] += "|" + to_string(col) + "|" + to_string(row) + "|" + align;
                    if(rooms[roomID].playersReady[userName].size() == 30){
                        rooms[roomID].playerReadyCount++;
                        if(rooms[roomID].playerReadyCount == rooms[roomID].players.size()){
                            Message responseMsg;
                            responseMsg.opcode = SET_SHIP_SUCCESS;
                            for(int i = 0; i < rooms[roomID].players.size(); i++){
                                strncpy(responseMsg.payload, rooms[roomID].playersReady[rooms[roomID].players[1-i].user].c_str(), sizeof(responseMsg.payload));
                                send(rooms[roomID].players[i].sock, (char *)&responseMsg, sizeof(responseMsg), 0);
                            }
                        }
                    }
                }
                break;
            }
            case HIT: {
                cout << "Received HIT request with payload: " << receivedMsg.payload << "\n";
                int roomID;
                int col;
                int row;
                int tt;
                if (sscanf(receivedMsg.payload, "%d|%d|%d|%d", &roomID, &col, &row, &tt) == 4) {
                    Message responseMsg;
                    responseMsg.opcode = HIT_SUCCESS;
                    sprintf(responseMsg.payload, "%d|%d", col, row);
                    cout<<roomID<<" "<<col<<" "<<row<<" "<<tt<<endl;
                    send(rooms[roomID].players[1-tt].sock, (char *)&responseMsg, sizeof(responseMsg), 0);
                }
                break;
            }
            case CREATE_ROOM: {
                cout << "Received CREATE_ROOM request with payload: " << receivedMsg.payload << "\n";
                char userCreateName[25];
                char passCreateName[25];
                char passRoom[11];
                if (sscanf(receivedMsg.payload, "%24[^|]|%24[^|]|%10s", userCreateName, passCreateName, passRoom) == 3) {
                    createRoom(rooms.size(), false, passRoom);
                    Account accUser;
                    strcpy(accUser.user, userCreateName);
                    strcpy(accUser.pass, passCreateName);
                    accUser.sock = client_socket;
                    accUser.islogin = 1;
                    joinRoom(rooms.size()-1, accUser, passRoom);
                    Message responseMsg;
                    responseMsg.opcode = CREATE_ROOM_SUCCESS;
                    sprintf(responseMsg.payload,"%d", (int)(rooms.size() - 1));
                    send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                }
                break;
            }
            case JOIN_ROOM: {
                cout << "Received JOIN_ROOM request with payload: " << receivedMsg.payload << "\n";
                char userJoinName[25];
                char passJoinName[25];
                char passRoom[11];
                int roomID;
                if (sscanf(receivedMsg.payload, "%24[^|]|%24[^|]|%d|%10s", userJoinName, passJoinName, &roomID, passRoom) == 4) {
                    if(roomID<0 || roomID >= rooms.size()){
                        Message responseMsg;
                        responseMsg.opcode = JOIN_ROOM_FAIL;
                        sprintf(responseMsg.payload,"Room %d does not exist", roomID);
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                        break;
                    }
                    if(rooms[roomID].isClosed){
                        Message responseMsg;
                        responseMsg.opcode = JOIN_ROOM_FAIL;
                        sprintf(responseMsg.payload,"Room %d is closed", roomID);
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                        break;
                    }
                    Account accUser;
                   strcpy(accUser.user, userJoinName);
                    strcpy(accUser.pass, passJoinName);
                    accUser.sock = client_socket;
                    accUser.islogin = 1;
                     bool joinedRoom = joinRoom(roomID, accUser, passRoom);
                    if (!joinedRoom) {
                        Message responseMsg;
                        responseMsg.opcode = JOIN_ROOM_FAIL;
                        sprintf(responseMsg.payload,"Room %d is full", roomID);
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                    }
                    else
                    {
                        Message responseMsg;
                        responseMsg.opcode = JOIN_ROOM_SUCCESS;
                        sprintf(responseMsg.payload,"%d|%s", roomID, rooms[roomID].players[0].user);
                        send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                        sprintf(responseMsg.payload,"%d|%s", roomID, rooms[roomID].players[1].user);
                        send(rooms[roomID].players[0].sock, (char *)&responseMsg, sizeof(responseMsg), 0);
                        cout<<client_socket<<" "<<rooms[roomID].players[0].sock<<" "<<rooms[roomID].players.size()<<endl;
                    }
                }
                break;
            }
            case GET_RANK: {
                cout << "Received GET_RANK request with payload: " << receivedMsg.payload << "\n";
                string payload = getRank();
                Message responseMsg;
                responseMsg.opcode = GET_RANK_SUCCESS;
                strncpy(responseMsg.payload, payload.c_str(), sizeof(responseMsg.payload));
                send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
                break;
            }
            case STORE_HISTORY: {
            cout << "Received STORE_HISTORY request with payload: " << receivedMsg.payload << "\n";
            char opponent[25];
            int score;
            int winner;
            char name[25];
            if (sscanf(receivedMsg.payload, "%24[^|]|%d|%d|%24s", opponent, &score, &winner, name) == 4) {
                string opponentStr(opponent);
                string nameStr(name);
                storeHistoryToDB(opponentStr, score, winner, nameStr);
                updatePoints(nameStr, score);
                Message responseMsg;
                responseMsg.opcode = STORE_HISTORY_SUCCESS;
                send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
            } else {
                Message responseMsg;
                responseMsg.opcode = STORE_HISTORY_FAIL;
                send(client_socket, (char *)&responseMsg, sizeof(responseMsg), 0);
            }
            break;
        }

            // Các trường hợp xử lý khác
            // ...
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
    setlocale(LC_ALL, "en_US.UTF-8"); 
    struct sockaddr_in server_addr;
    socklen_t client_addr_size;

    // Conditional variable
    //pthread_cond_init(&player_to_join, NULL);
    //pthread_mutex_init(&general_mutex, NULL);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Socket creation failed.\n";
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6769);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bind failed.\n";
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 20);
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
