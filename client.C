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
#include <ctime>
#include "SIMPLESOCKET.H"

using namespace std;

class MyClient : public TCPclient{
public:
    void mode1();
    void mode2();
    void mode3();
    void mode4();
    void mode5();
    void init();
    void shoot();
    int play(int modeSelect);
protected:
	string msg = "0";
	int n = 0;
    int x = 0;
    int y = 0;
    int fieldState[11][11];
    int lastHitX = 0;
    int lastHitY = 0;
    int search = 0;
    int firstHitX = 0;
    int firstHitY = 0;
    int end = 0;
};

int main() {
	srand(time(NULL));
	MyClient c;
	string host = "localhost";
	int iterations = 1;
    int modeNmb = 1;
    string command;
    int saveInFile = 0;

    c.init();
    std::ofstream file("DataFile");

	c.conn(host , 2022);    //connect to host

    while(1){


        cout << ("Enter command:");                        //Nutzereingabe
        cin >> command;

        if(command == "ITERATION"){                             //Abfrage, wie oft eine Strategie ausgeführt werden soll
            int temp;
            cout << ("Enter Number of iterations:");
            cin >> temp;

            if(temp >= 0){                                //Überprüfen, ob die Eingabe gültig ist
                iterations = temp;
                cout << "Number of iterations was set to " << iterations << endl;
            }else{
                cout << "ERROR: Value must be higher than 0" << endl;
                iterations = 1;
            }

        }else if(command == "MODE"){
            int temp;
            cout << ("Enter Mode-Number: ");
            cin >> temp;
            cout << endl;

            if(temp > 0 && temp < 6){
                modeNmb = temp;
                cout << "Mode-Number was set to " << modeNmb << endl;
            }else{
                cout << "ERROR: Value must be between 1 and 5" << endl;
                modeNmb = 1;
            }

        }else if(command == "START"){
            cout << "starting" << endl;
            for(int i = 0; i < iterations; i++){
                int n = c.play(modeNmb);

                cout << "Number of shots:" << n << endl;
                if(saveInFile == 1){
                file << n << endl;
                }
            }

        }else if(command == "TOFILE"){
            saveInFile = 1;
            cout << "Data will be saved in file" << endl;
            file << "Mode:" << modeNmb << endl;
            file << "Iterations:" << iterations << endl;

        }else if(command == "CLOSEFILE"){
            saveInFile = 0;
            cout << "Data won't be saved in file" << endl;

        }else if(command == "END"){
            cout << "ending" << endl;
            c.sendData("BYEBYE");
            break;

        }else {
            cout << "UNKNOWN COMMAND" << endl;
        }


    }

    return 0;
}

void MyClient::init(){

    for(int j = 0; j < 11; j++){
        for(int k = 0; k < 11; k++){
            fieldState[j][k] = -1;              //-1 für unbekannt/noch nicht beschossen
        }
    }

    msg = "0";
	n = 0;
    x = 1;
    y = 1;
    lastHitX = 0;
    lastHitY = 0;
    firstHitX = 0;
    firstHitY = 0;
    search = 0;
    end = 0;

    sendData("INIT");
    receive(15);
    return;
}

int MyClient::play(int modeSelect){
    init();

    while(msg == "0" || msg == "1" || msg == "2"){

    switch(modeSelect){
        case 1:
            shoot();
            mode1();
            break;
        case 2:
            mode2();
            shoot();
            break;
        case 3:
            mode3();
            shoot();
            break;
        case 4:
            mode4();
            shoot();

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
            mode5();
            shoot();

            if(fieldState[x][y] == 2){
                firstHitX = 0;
                firstHitY = 0;
                search = 0;
                end = 0;
            }

            if(fieldState[x][y] == 1 && search == 1){

                lastHitX = x;
                lastHitY = y;
                search = 2;
            }

            if(fieldState[x][y] == 1 && search == 0){
                lastHitX = x;
                firstHitX = x;
                lastHitY = y;
                firstHitY = y;
                search = 1;
            }

            if(fieldState[x][y] == 1 && search == 2){
                lastHitX = x;
                lastHitY = y;
            }

            break;
            default:
                cout << "ERROR: Invalid mode-number selected" << endl;
                break;
        }
    }
    return n;
}


void MyClient::shoot(){
    sendData(string("COORD[") + to_string(x) + string(",") + to_string(y) + string("]"));
    msg = receive(5);

    if(msg == "0"){
        fieldState[x][y] = 0;
    }else if(msg == "1"){
        fieldState[x][y] = 1;
    }else if(msg == "2"){
        fieldState[x][y] = 2;
    }else if(msg == "3"){
        fieldState[x][y] = 3;
    }else if(msg == "4"){
        fieldState[x][y] = 4;
    }else{
        std::cout << "ERROR: Can't convert to int" << std::endl;
    }

    n++;
    return;
}

void MyClient::mode1(){
    x++;

    if(x >= 11){
        x = 1;
        y++;
    }

    return;
}

void MyClient::mode2(){
    x = rand()%10 +1;
    y = rand()%10 +1;
    return;
}

void MyClient::mode3(){
    do{
        x = rand()%10 +1;
        y = rand()%10 +1;
    }while(fieldState[x][y] != -1);

    return;
}

void MyClient::mode4(){

    if(search == 1){
        if(lastHitX < 10 && fieldState[lastHitX+1][lastHitY] == -1){
            x = lastHitX +1;
            y = lastHitY;
            return;
        }

        if(lastHitX > 1 && fieldState[lastHitX-1][lastHitY] == -1){
            x = lastHitX -1;
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
    mode3();

    return;
}

void MyClient::mode5(){             //Berechnng der Koordinatenint msgToInt(string msg){

    if(search == 2){
        if(lastHitX == firstHitX){

            if(y >= 1 && y <= 10 && fieldState[lastHitX][lastHitY +1] == -1 && end == 0){
                x = lastHitX;
                y = lastHitY + 1;
                return;
            }else{
                end = 1;
            }

            if(end == 1){
                while(lastHitY >= 1 && lastHitY <= 10 && fieldState[lastHitX][lastHitY] != -1){
                    lastHitY--;
                }
                x = lastHitX;
                y = lastHitY;
                return;
            }

        }

        if(lastHitY == firstHitY){

            if(x >= 1 && x <= 10 && fieldState[lastHitX +1][lastHitY] == -1 && end == 0){
                y = lastHitY;
                x = lastHitX + 1;
                return;
            }else{
                end = 1;
            }

            if(end == 1){
                while(lastHitX >= 1 && lastHitX <= 10 && fieldState[lastHitX][lastHitY] != -1){
                    lastHitX--;
                }
                x = lastHitX;
                y = lastHitY;
                return;
            }

        }
    search = 0;
    }


    mode4();

    return;
}
