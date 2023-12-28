#include <bits/stdc++.h>
using namespace std;
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
void initialise_player_matrix();
int set_ship_position(int);
void display_matrix();
void hit_display_matrix();
int hit(int,int);
int show_winner(int,player,player);
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