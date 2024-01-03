#include "Resources.h"

struct Room {
    int roomID;
    vector<Account> players; // Danh sách người chơi trong phòng
    // Các thông tin khác cần thiết cho phòng chơi
    bool ispublic;
    string code = "";
    bool isClosed = false;
    map <string, string> playersReady = {};
    int playerReadyCount = 0;
};
vector<Room> rooms;

void createRoom(const int& roomID, const bool& ispublic, const string& code) {
    Room newRoom;
    newRoom.roomID = roomID;
    newRoom.ispublic = ispublic;
    newRoom.code = code;
    // Khởi tạo các thông tin khác của phòng chơi
    rooms.push_back(newRoom);
    // Gửi thông báo cho người chơi hoặc thực hiện các hành động khác cần thiết
}

bool joinRoom(const int& roomID, const Account& user, const string& enteredCode) {
    for (Room& room : rooms) {
        if (room.roomID == roomID) {
            if (room.ispublic || room.code == enteredCode) {
                if (room.players.size() < 2) { // Kiểm tra số lượng người chơi trong phòng
                    bool playerExists = false;
                    for (const Account& player : room.players) {
                        if (player.user == user.user) {
                            cout << "Player with username " << user.user << " already exists in the room." << '\n';
                            playerExists = true;
                            break;
                        }
                    }
                    if (!playerExists) {
                        room.players.push_back(user);
                        return true; // Nếu tham gia thành công
                    } else {
                        cout << "Player with username " << user.user << " already exists in the room." << '\n';
                        return false;
                    }
                } else {
                    cout << "Room is full." << '\n';
                    return false;
                }
            } else {
                cout << "Wrong room code." << '\n';
                // Mã code không khớp (cho phòng không công khai), không thể tham gia
                return false;
            }
        }
    }
    cout << "Room not found." << '\n';
    return false; // Nếu không tìm thấy phòng
}

bool leaveRoom(const int& roomID, const Account& user) {
    for (Room& room : rooms) {
        if (room.roomID == roomID) {
            for (auto it = room.players.begin(); it != room.players.end(); ++it) {
                if (it->user == user.user) {
                    // Xóa người chơi ra khỏi phòng
                    room.players.erase(it);
                    return true; // Nếu rời phòng thành công
                }
            }
        }
    }
    return false; // Nếu không tìm thấy phòng hoặc người chơi
}