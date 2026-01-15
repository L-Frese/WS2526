/*
 * client.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

#include <string>
#include <iostream>
#include <unistd.h> //contains various constants

#include "SIMPLESOCKET.H"

using namespace std;

void init(TCPclient *c, int &x, int &y, int &n, string &msg, int fieldState[10][10]);   //Setzt alle VAriablen auf Startwerte, initialisiert bei Server ein neues Spielfeld
string shoot(TCPclient *c ,int x, int y, int &n);                       //Shießen auf die Koordinaten
void mode1(int &x,int &y);                                      //Berechnng der Koordinaten
void mode2(int &x,int &y);                                      //Berechnng der Koordinaten
void mode3(int &x,int &y, int fieldState[10][10]);              //Berechnng der Koordinaten
void mode4(int &x,int &y, int fieldState[10][10]);              //Berechnng der Koordinaten
int msgToInt(string msg);


int main() {
	srand(time(NULL));
	TCPclient c;
	string host = "localhost";

    string command;             //Speichert die Eingabe des Nutzers
    int iterations = 10;         //Anzahl, wie oft das Spiel wiederholt werden soll
	string msg;                 //Status/Rückgabewert des letzden Feldes
	int n;                      //Anzahl an Schüssen
    int x, y;                   //Koordinaten
    int modeNmb = 1;

    int fieldState[10][10];     //Speichert ggf., ob ein Feld bereits getroffen wurde

    init(&c,x,y,n,msg,fieldState);

	c.conn(host , 2022);    //connect to host

    while(1){

        std::cout << ("Command:");
        std::cin >> command;

        if(command == "ITERATION"){
            int temp;
            std::cout << ("Enter Number of iterrations:");
            std::cin >> temp;

            if(iterations >= 0){
                iterations = temp;
                std::cout << "Number of iterations was set to " << iterations << std::endl;
            }else{
                std::cout << "ERROR: Value must be higher than 0" << std::endl;
            }
        }

        if(command == "MODE"){
            int temp;
            std::cout << ("Enter Mode-Number:");
            std::cin >> temp;
            std::cout << std::endl;

            if(temp > 0 && temp < 5){
                modeNmb = temp;
                std::cout << "Mode-Number was set to " << modeNmb << std::endl;
            }else{
                std::cout << "ERROR: Value must be between 1 and 4" << std::endl;;
            }
        }

        if(command == "START"){
            for(int i = 0; i < iterations; i++){
                init(&c,x,y,n,msg,fieldState);

                while(msg == "0" || msg == "1" || msg == "2"){

                    switch(modeNmb){
                        case 1:
                            msg = shoot(&c,x,y,n);
                            mode1(x,y);
                            break;
                        case 2:
                            mode2(x,y);
                            msg = shoot(&c,x,y,n);
                            break;
                        case 3:
                            mode3(x,y,fieldState);
                            msg = shoot(&c,x,y,n);
                            fieldState[x][y] = msgToInt(msg);
                            break;
                        case 4:
                            msg = shoot(&c,x,y,n);
                            mode4(x,y,fieldState);
                            break;
                        default:
                            std::cout << "ERROR: Invalid mode-number selected" << std::endl;
                            break;
                    }
                }

                std::cout << "Number of shots:" << n << std::endl;
            }
        }

        if(command == "END"){
            c.sendData("BYEBYE");
            break;
        }
    }

    return 0;
}

void init(TCPclient *c, int &x, int &y, int &n, string &msg, int fieldState[10][10]){
    x = 1;
    y = 1;
    n = 0;
    msg = "0";

    for(int j = 1; j <= 10; j++){
        for(int k = 1; k <= 10; k++){
            fieldState[j][k] = 0;
        }
    }

    c->sendData("INIT");
    c->receive(15);
    return;
}

string shoot(TCPclient *c, int x, int y, int &n){
    c->sendData(string("COORD[") + to_string(x) + string(",") + to_string(y) + string("]"));
    string msg = c->receive(5);
    n++;
    return msg;
}

int msgToInt(string msg){
    if(msg == "0"){
        return 0;
    }else if(msg == "1"){
        return 1;
    }else if(msg == "2"){
        return 2;
    }else if(msg == "3"){
        return 3;
    }else if(msg == "4"){
        return 4;
    }else{
        std::cout << "ERROR: Can't convert to int" << std::endl;
        return 0;
    }
}

void mode1(int &x,int &y){
    x++;

    if(x >= 11){
        x = 1;
        y++;
    }

    return;
}

void mode2(int &x,int &y){
    x = rand()%10 +1;
    y = rand()%10 +1;
    return;
}

void mode3(int &x,int &y, int fieldState[10][10]){
    do{
        x = rand()%10 +1;
        y = rand()%10 +1;
    }while(fieldState[x][y] != 0);

    return;
}

void mode4(int &x,int &y, int fieldState[10][10]){


    return;
}
