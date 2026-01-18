/*
 * client.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

#include <string>
#include <iostream>
#include <unistd.h> //contains various constants
#include <fstream>

#include "SIMPLESOCKET.H"

using namespace std;

void init(TCPclient *c, int fieldState[11][11]);   //Setzt alle VAriablen auf Startwerte, initialisiert bei Server ein neues Spielfeld
string shoot(TCPclient *c ,int x, int y, int &n);               //Schießen auf die Koordinaten
void mode1(int &x,int &y);                                      //Berechnng der Koordinaten
void mode2(int &x,int &y);                                      //Berechnng der Koordinaten
void mode3(int &x,int &y, int fieldState[11][11]);              //Berechnng der Koordinaten
void mode4(int &x,int &y, int &lastHitX, int &lastHitY, int &search, int fieldState[11][11]);              //Berechnng der Koordinaten
void mode5(int &x,int &y, int &lastHitX, int &lastHitY,
            int &search, int &orientation, int &dircection,
            int fieldState[11][11]);              //Berechnng der Koordinaten
int msgToInt(string msg);


int main() {
	srand(time(NULL));
	TCPclient c;
	string host = "localhost";

    string command;             //Speichert die Eingabe des Nutzers
    int iterations = 100;         //Anzahl, wie oft das Spiel wiederholt werden soll
	string msg;                 //Status/Rückgabewert des letzden Feldes
	int n;                      //Anzahl an Schüssen
    int x, y;                   //Koordinaten
    int modeNmb = 5;
    int saveInFile = 1;
    int fieldState[11][11];     //Speichert ggf., ob ein Feld bereits getroffen wurde
    int lastHitX = 0;
    int lastHitY = 0;
    int search = 0;             //Suchmodus 0:Zufall 1:Erster Treffer, orientierung unbekannt, 2:Orientierung bekannt
    int orientation = 0;
    int direction = 1;

    init(&c,fieldState);
    std::ofstream file("DataFile");

	c.conn(host , 2022);    //connect to host

    while(1){


        std::cout << ("Enter command:");                        //Nutzereingabe
        std::cin >> command;

        if(command == "ITERATION"){                             //Abfrage, wie oft eine Strategie ausgeführt werden soll
            int temp;
            std::cout << ("Enter Number of iterations:");
            std::cin >> temp;

            if(iterations >= 0){                                //Überprüfen, ob die Eingabe gültig ist
                iterations = temp;
                std::cout << "Number of iterations was set to " << iterations << std::endl;
            }else{
                std::cout << "ERROR: Value must be higher than 0" << std::endl;
            }

        }else if(command == "MODE"){
            int temp;
            std::cout << ("Enter Mode-Number: ");
            std::cin >> temp;
            std::cout << std::endl;

            if(temp > 0 && temp < 6){
                modeNmb = temp;
                std::cout << "Mode-Number was set to " << modeNmb << std::endl;
            }else{
                std::cout << "ERROR: Value must be between 1 and 5" << std::endl;
            }

        }else if(command == "START"){
            for(int i = 0; i < iterations; i++){
                init(&c,fieldState);
                x = 1;
                y = 1;
                n = 0;
                msg = "0";
                lastHitX = 0;
                lastHitY = 0;
                search = 0;
                orientation = 0;
                direction = 1;

                if(saveInFile == 1){
                    file << "Mode:" << modeNmb << std::endl;
                }

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
                            mode4(x,y,lastHitX,lastHitY,search,fieldState);
                            msg = shoot(&c,x,y,n);
                            fieldState[x][y] = msgToInt(msg);

                            if(fieldState[x][y] == 1){
                                lastHitX = x;
                                lastHitY = y;
                                search = 1;
                            }

                            if(fieldState[x][y] == 2){
                                search = 0;
                            }

                            break;
                        case 5:
                            mode5(x,y,lastHitX,lastHitY,search,orientation,direction,fieldState);
                            msg = shoot(&c,x,y,n);
                            fieldState[x][y] = msgToInt(msg);

                            if(fieldState[x][y] == 1){
                                lastHitX = x;
                                lastHitY = y;

                                if(search == 0){            //erster Treffer, suche starten
                                    search = 1;
                                }else if(search == 1){      //wenn suche bereits gestartet, orientierung setzen
                                    if(x == lastHitX){
                                        orientation = 1;
                                    }
                                    if(y == lastHitY){
                                        orientation = -1;
                                    }
                                }
                            }

                            if(fieldState[x][y] == 2){      //Wenn schiff versenkt
                                search = 0;
                                orientation = 0;
                                direction = 1;
                            }

                            break;
                        default:
                            std::cout << "ERROR: Invalid mode-number selected" << std::endl;
                            break;
                    }
                }

                std::cout << "Number of shots:" << n << std::endl;
                if(saveInFile == 1){
                    file << n << std::endl;
                }
            }

        }else if(command == "TOFILE"){
            saveInFile = 1;
            std::cout << "Data will be saved in file" << std::endl;

        }else if(command == "CLOSEFILE"){
            saveInFile = 0;
            std::cout << "Data won't be saved in file" << std::endl;

        }else if(command == "END"){
            c.sendData("BYEBYE");
            break;

        }else {
            std::cout << "UNKNOWN COMMAND" << std::endl;
        }


    }

    return 0;
}

void init(TCPclient *c, int fieldState[11][11]){

    for(int j = 0; j < 11; j++){
        for(int k = 0; k < 11; k++){
            fieldState[j][k] = -1;              //-1 für unbekannt/noch nicht beschossen
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

void mode3(int &x,int &y, int fieldState[11][11]){
    do{
        x = rand()%10 +1;
        y = rand()%10 +1;
    }while(fieldState[x][y] != -1);

    return;
}

void mode4(int &x,int &y, int &lastHitX, int &lastHitY, int &search, int fieldState[11][11]){

    if(search == 1){
        if(lastHitX > 1 && fieldState[lastHitX-1][lastHitY] == -1){
            x = lastHitX -1;
            y = lastHitY;
            return;
        }
        if(lastHitX < 10 && fieldState[lastHitX+1][lastHitY] == -1){
            x = lastHitX +1;
            y = lastHitY;
            return;
        }
        if(lastHitY < 10 && fieldState[lastHitX][lastHitY+1] == -1){
            x = lastHitX;
            y = lastHitY +1;
            return;
        }
        if(lastHitY > 1 && fieldState[lastHitX][lastHitY-1] == -1){
            x = lastHitX;
            y = lastHitY -1;
            return;
        }

        search = 0;
    }
    mode3(x,y,fieldState);

    return;
}

void mode5(int &x,int &y, int &lastHitX, int &lastHitY, int &search, int &orientation, int &direction, int fieldState[11][11]){

    if(search == 2){
        if(orientation == 1){
            int nextX = lastHitX + direction;

            if(nextX >= 1 && nextX <= 10 && fieldState[nextX][lastHitY] == -1){
                x = nextX;
                y = lastHitY;
                return;
            }

            direction = direction * -1;
            do{
                nextX = lastHitX + direction;
            }while(nextX >= 1 && nextX <= 10 && fieldState[nextX][lastHitY] != -1);

            if(nextX >= 1 && nextX <= 10 && fieldState[nextX][lastHitY] == -1){
                x = nextX;
                y = lastHitY;
                return;
            }

        }

        if(orientation == -1){
            int nextY = lastHitY + direction;

            if(nextY >= 1 && nextY <= 10 && fieldState[lastHitX][nextY] == -1){
                x = lastHitX;
                y = nextY;
                return;
            }

            direction = direction * -1;

            do{
                nextY = lastHitY + direction;
            }while(nextY >= 1 && nextY <= 10 && fieldState[lastHitX][nextY] != -1);


            if(nextY >= 1 && nextY <= 10 && fieldState[lastHitX][nextY] == -1){
                x = lastHitX;
                y = nextY;
                return;
            }
        }

        search = 0;
        orientation = 0;
        direction = 1;
    }

    mode4(x,y,lastHitX,lastHitY,search,fieldState);

    return;
}
