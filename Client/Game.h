#include "Account.h"
using namespace std;
int storeHistory(SOCKET sock, string opponent, int score, int winner, string name) {
    pthread_t tid;
    string payload = "";
    payload += opponent + "|" + to_string(score) + "|" + to_string(winner) + "|" + name;

    ThreadArgs tArgs = { sock, NULL, NULL, NULL, 0, payload }; // Sử dụng string thay vì char*
    pthread_create(&tid, NULL, storeHistoryThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    return tArgs.result;
}
void FF(SOCKET sock, string opponent, int score, int winner, string name,int roomId) {
    pthread_t tid;
    string payload = "";
    payload += opponent + "|" + to_string(score) + "|" + to_string(winner)  + "|" + to_string(roomId) + "|" + name;

    ThreadArgs tArgs = { sock, NULL, NULL, NULL, 0, payload }; // Sử dụng string thay vì char*
    pthread_create(&tid, NULL, FFThread, (void*)&tArgs);
    pthread_join(tid, NULL);
    return;
}

vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}
class player
{
private:
char mat[10][10];
int hits;
//store the total parts of each of the different ship destroyed
int count_ship[5];
//store whether all parts of particular ship is destroyed
bool ship_destroy[5];

public:
string name;
int hitmat[10][10];
player(string Name)
{
    name=Name;
    hits=0;
    for(int i=0;i<5;i++)
    {
        count_ship[i]=0;
        ship_destroy[i]=0;
    }
}
int initialise_player_matrix(SOCKET,int,Account);
int initialise_opponent_matrix(SOCKET,int,string);
int set_ship_position(int,int &col,int &row, char& align);
int set_opponent_ship_position(int,int &col,int &row, char& align);
void display_matrix();
void hit_display_matrix();
void forfeit(int,player,player, SOCKET, bool,int,bool);
int hit(int,int);
void show_winner(int,player,player, SOCKET,bool);
};
void player::display_matrix()
{

    cout<<"    "<<setw(4);
    for(int i=0;i<10;i++)
    {
        cout<<setw(4)<<i;
    }
    cout<<endl;
    for(int i=0;i<10;i++)
    {
        cout<<setw(4)<<i;
        for(int j=0;j<10;j++)
            cout<<setw(4)<<this->mat[i][j];
        cout<<endl;
    }
    return;
}
void player::hit_display_matrix()
{

    cout<<"    "<<setw(4);
    for(int i=0;i<10;i++)
    {
        cout<<setw(4)<<i;
    }
    cout<<endl;
    for(int i=0;i<10;i++)
    {
        cout<<setw(4)<<i;
        for(int j=0;j<10;j++)
        {
            if(this->hitmat[i][j])
            {
               if(this->mat[i][j]=='.')
                    cout<<setw(4)<<"#";
                else
                {
                    if(ship_destroy[mat[i][j]-'a']==1)
                        cout<<setw(4)<<this->mat[i][j];
                    else
                        cout<<setw(4)<<"x";
                }

            }
            else
                cout<<setw(4)<<'.';

        }

        cout<<endl;
    }
    return;
}
int player::set_ship_position(int length,int &col,int &row, char& align)
{
    this->display_matrix();
    bool f=1;
    string input;
    cin>>input;
    if(input == "ff")return 4;
    if(input.length()>=2) return 2;
    row = input[0]-'0';
    cin>>input;
    if(input == "ff")return 4;
    if(input.length()>=2) return 2;
    col = input[0]-'0';
    cin>>input;
    if(input == "ff")return 4;
    if(input.length()>=2) return 2;
    align = input[0];
    align=toupper(align);
    if(row>=10||row<0||col>=10||col<0)
        return 2;
    if(align!='H'&&align!='V')
        return 2;
    if(align=='H')
        if(col+length>10)
            return 2;
    if(align=='V')
        if(row+length>10)
            return 2;
    if(align=='H')
        {
            for(int i=col;i<col+length;i++)
                {
                    if(this->mat[row][i]!='.')
                    {
                        f=0;
                        return 3;
                    }
                }
            if(f)
                {
                    for(int i=col;i<col+length;i++)
                        this->mat[row][i]='f'-length;
                    return 1;
                }
        }
    if(align=='V')
        {
            for(int i=row;i<row+length;i++)
                {
                    if(this->mat[i][col]!='.')
                    {
                        f=0;
                        return 3;
                    }
                }
            if(f)
                {
                    for(int i=row;i<row+length;i++)
                        this->mat[i][col]='f'-length;
                    return 1;
                }
        }
 return 0;
}

int player::set_opponent_ship_position(int length,int &col,int &row, char& align)
{
    bool f=1;
    if(align=='H')
        {
            for(int i=col;i<col+length;i++)
                {
                    if(this->mat[row][i]!='.')
                    {
                        f=0;
                        return 3;
                    }
                }
            if(f)
                {
                    for(int i=col;i<col+length;i++)
                        this->mat[row][i]='f'-length;
                    return 1;
                }
        }
    if(align=='V')
        {
            for(int i=row;i<row+length;i++)
                {
                    if(this->mat[i][col]!='.')
                    {
                        f=0;
                        return 3;
                    }
                }
            if(f)
                {
                    for(int i=row;i<row+length;i++)
                        this->mat[i][col]='f'-length;
                    return 1;
                }
        }
 return 0;
}
int player::initialise_player_matrix(SOCKET sock,int roomID,Account acc)
{
    //Initialize all values of matrix to '.' at start '.' denotes water
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
        {
            mat[i][j]='.';
            hitmat[i][j]=0;
        }

    int ship_length=5;

    int flag=1;
    //calling set_ship_position function to set ship's position for different length of ship
    for(int i=0;i<5;)
    {
        if(flag==1)
        {
        
            cout<<this->name<<"'s turn to set its ship position"<<endl;
            cout<<"\n\n";
            cout<<"Enter the starting position of ship in the form row index column ,index and alignment in one line"<<endl;
            cout<<"Enter row index in range '0-9' , column index in range '0-9' and H for horizontal alignment and V for vertical allignment"<<endl;
            cout<<"Example: '3 5 H' for starting position in 3rd row,5th column and horizontal alignment"<<endl;
            cout<<"----------------------------------------------------------------------------------------------------"<<endl;
            cout<<"Enter the starting position of ship of length "<<ship_length<<endl;
            int col,row;
            char align;
            flag=this->set_ship_position(ship_length,col,row,align);
            if(flag == 4)
            return 0;
            if(flag==1)
            {
                i++;
                ship_length--;
                char message[PAYLOAD_SIZE];
                sprintf(message, "%s|%d|%d|%d|%c", acc.user, roomID, col, row, align);
                sendMessage(sock, SET_SHIP, message);
            }
            else
            {
                if(flag==2)
                cout<<"The position is out of bound! please enter the start position again"<<endl;
                if(flag==3)
                cout<<"The position is already occupied by other ship! Please enter the start position again"<<endl;
                flag=1;

            }
        }

    }
    system("clear");
    cout<<this->name<<"'s matrix"<<endl;
    this->display_matrix();
    getchar();
    getchar();
    return 1;
}
int player::initialise_opponent_matrix(SOCKET sock,int roomID,string name)
{
    cout<<"Waiting for "<<name<<" to set its ships"<<endl;
    //Initialize all values of matrix to '.' at start '.' denotes water
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
        {
            mat[i][j]='.';
            hitmat[i][j]=0;
        }

    int ship_length=5;

    int flag=1;
    Message receivedMsg;
    recv(sock, (char*)&receivedMsg, sizeof(receivedMsg), 0);
    if(receivedMsg.opcode == SURRENDER_SUCCESS)
    {
        cout<<"Opponent has Surrendered"<<endl;
        return 0;
    }
    if(receivedMsg.opcode == SET_SHIP_SUCCESS) cout<<"Ship set successfully"<<endl;
    string payload = receivedMsg.payload;
    payload = payload.substr(1);
    vector<string> data = split(payload, '|');
    for(int i=0;i<5;)
    {
            int col = stoi(data[i*3]),row = stoi(data[i*3+1]);
            char align = data[i*3+2][0];
            flag=this->set_opponent_ship_position(ship_length,col,row,align);
            i++;
            ship_length--;
    }
    return 1;
}
int player::hit(int x,int y)
{
    if(mat[x][y]!='.')
        {
            hits++;
            count_ship[mat[x][y]-'a']++;
            if(count_ship[mat[x][y]-'a']>=('e'-mat[x][y]+1))
                ship_destroy[mat[x][y]-'a']=1;
        }
    hitmat[x][y]=1;
    if(hits==15)
        return 1;
    else if(mat[x][y]!='.')
        return 2;
    else
        return 0;
}
void player::show_winner(int w,player p1,player p2, SOCKET sock,bool isPrivate)
{
      if(w==1)
      {   
          cout<<"Congrats!! "<<p1.name<<" won by "<<abs(p1.hits-p2.hits)<<" points"<<endl;

      }
      if(w==2)
      {
          cout<<"Congrats!! "<<p2.name<<" won by "<<abs(p2.hits-p1.hits)<<" points"<<endl;

      }
      int score = p2.hits - p1.hits;
      if(isPrivate) score =0;
        storeHistory(sock,p2.name,score, 2-w,p1.name);
      getchar();
      getchar();
}
void player::forfeit(int w,player p1,player p2, SOCKET sock, bool isPrivate,int roomID, bool isFF)
{
    if(w==1)
    {
        cout<<p2.name<<" surrendered"<<endl<<p1.name<<" won by "<<abs(p1.hits-p2.hits)<<" points"<<endl;
    }
    if(w==2)
    {
        cout<<p1.name<< " surrendered"<<endl<<p2.name<<" won by "<<abs(p2.hits-p1.hits)<<" points"<<endl;
    }
    int score = p2.hits - p1.hits;
    if(isPrivate) score =0;
    if(isFF) FF(sock,p2.name,score, 2-w,p1.name,roomID);
    cout<<"Press any key to exit"<<endl;
    getchar();
    getchar();
}
void start_game(string name1,string name2, int roomID, Account acc, SOCKET sock,bool isFirst, bool isPrivate)
{
    player p1(name1),p2(name2);
    bool isFF = false;
    //Initialize position of ships of both players
    int checkff = p1.initialise_player_matrix(sock,roomID,acc);
    if(checkff==0)
    {
        p1.forfeit(2,p1,p2,sock,isPrivate,roomID,true);
        isFF = true;
        return;
    }
    checkff = p2.initialise_opponent_matrix(sock,roomID,name2);
    if(checkff==0)
    {
        p1.forfeit(1,p1,p2,sock,isPrivate,roomID,false);
        isFF = true;
        return;
    }
    //Game play
    int gameover=0,player_chance=1,winner;
    if(isFirst)
        player_chance=1;
    else
        player_chance=2;
    cout<<"Game started"<<'\n';
    //loop for to control player chances
    while(!gameover)
    {
        //loop to control player 1 game play
        while(player_chance==1&&!gameover)
        {  
            cout<<"It's "<<p1.name<<"'s Chance to attack"<<endl;
            cout<<"\n\n";
            cout<<p2.name<<"'s current affected areas ('x' denotes correct hit ,'#' denotes miss hit)   "<<endl;
            p2.hit_display_matrix();
            cout<<"Enter the coordinates you want to attack : "<<endl;
            int x,y;
            string input;
            cin>>input;
            if(input == "ff"){p1.forfeit(2,p1,p2,sock,isPrivate,roomID,true);isFF = true;return;}
            if(input.length()>=2) x=-1;
            x= input[0]-'0';
            cin>>input;
            if(input == "ff"){p1.forfeit(2,p1,p2,sock,isPrivate,roomID,true);isFF = true;return;}
            if(input.length()>=2) y=-1;
            y= input[0]-'0';
            if(x<0||x>=10||y<0||y>=10)
                cout<<"Please enter the coordinates which are present in the matrix! "<<endl;
            else if(p2.hitmat[x][y])
                cout<<"This position is already targeted please enter some another position"<<endl;
            else
            {
                Message msg;
                int tt =1;
                if(isFirst) tt=0;
                sprintf(msg.payload, "%d|%d|%d|%d",roomID, x, y,tt);
                sendMessage(sock, HIT, msg.payload);
                player_chance=2;
               int win=p2.hit(x,y);
            
               p2.hit_display_matrix();
               if(win==2)
               {
                   cout<<"It's a hit!! "<<p1.name<<"will get another chance!!"<<endl;
                   player_chance=1;
               }
               if(win==1)
               {
                   gameover=1;
                   winner=1;
                   player_chance=1;
               }
               if(!win)
               {
                   cout<<"It's a miss hit"<<endl;
               }
            }
        }

        //loop to control player 2 game play
        while(player_chance==2&&!gameover)
        {
        
            cout<<"It's "<<p2.name<<"'s Chance to attack ('x' denotes correct hit ,'#' denotes miss hit)"<<endl;
            cout<<"\n\n";
            int x,y;
            Message receiveMsg;
            recv(sock, (char*)&receiveMsg, sizeof(receiveMsg), 0);
            if(receiveMsg.opcode == SURRENDER_SUCCESS)
            {
            p1.forfeit(1,p1,p2,sock,isPrivate,roomID,false);
            isFF = true;
            return;
            }
            x = receiveMsg.payload[0] - '0';
            y = receiveMsg.payload[2] - '0';
            cout<<x<<" "<<y<<endl;
            if(x<0||x>=10||y<0||y>=10)
                cout<<"Please enter the coordinates which are present in the matrix! "<<endl;
            else if(p1.hitmat[x][y])
                cout<<"This position is already targeted please enter some another position"<<endl;
            else
            {

               player_chance=1;
               int win=p1.hit(x,y);
            
               p1.hit_display_matrix();
               if(win==2)
               {
                   cout<<"It's a hit!!"<<p2.name<<" will get another chance!!"<<endl;
                   player_chance=2;
               }
               if(win==1)
               {
                   gameover=1;
                   winner=2;

               }
               if(!win)
               {
                    cout<<"It's a miss hit"<<endl;
               }
            }
        }
    }
    if(!isFF)
      p1.show_winner(winner,p1,p2,sock,isPrivate);
}