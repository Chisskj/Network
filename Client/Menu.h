#pragma once
#include <bits/stdc++.h>
using namespace std;
void menu1(char& choice)
{
    cout << "Choose an option:\n";
    cout << "1. Login\n";
    cout << "2. Register\n";
    cout << "3. Quit\n";
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore(); // Đọc ký tự thừa '\n'
}
void menu2(char &choice)
{
    cout << "====== Game Menu ======\n";
    cout << "1. Chơi trực tuyến\n";
    cout << "2. Custom Game\n";
    cout << "3. Xem bảng xếp hạng\n";
    cout << "4. Xem lịch sử đấu\n";
    cout<< "5. Online player\n";
    cout << "6. Đăng xuất\n";
    cout << "=======================\n";
    cout << "Nhập lựa chọn của bạn: "; 
    cin >> choice;
    cin.ignore(); // Đọc ký tự thừa '\n
}
void menu3(int &choice)
{
    cout<<"======Option choose======\n";
    cout<<"1. Create Room\n";
    cout<<"2. Join Room\n";
    cout<<"3. Exit\n";
    cout<<"Enter your choice: ";
    cin>>choice;
    cin.ignore();
}