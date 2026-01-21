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
    void mode1();               //Methoden für die Strategien
    void mode2();
    void mode3();
    void mode4();
    void mode5();
    void init();                //Initialisieren eines neuen Durchganges
    void shoot();               //Schießen auf die Koordinaten x und y
    int play(int modeSelect);   //Spielablauf
protected:
	string msg = "0";           //Antwort des Servers nach einem Schuss
	int n = 0;                  //Aktuelle Anzahl der Züge
    int x = 0;                  //x Koordinate
    int y = 0;                  //y Koordinate
    int fieldState[11][11];     //Feld zum speichern der erhaltenen Rückgabewerte
    int lastHitX = 0;           //x Koordinate des letzten Treffers
    int lastHitY = 0;           //y Koordinate des letzten Treffers
    int search = 0;             //Suchmodus, 0- Zufallszüge, 1- Nachtreffer die Umliegenden Felder, 0- Lage des Schiffes ermittelt und in Reihe weiter schießen
    int firstHitX = 0;          //x Koordinate des ersten Treffers auf ein Schiff
    int firstHitY = 0;          //y Koordinate des ersten Treffers auf ein Schiff
    int end = 0;                //Variable, ob ein Ende eines Schiffes erreicht wurde
};

int main() {
	srand(time(NULL));
	MyClient c;
	string host = "localhost";

	int iterations = 1;             //Anzahl der Wiederholungen
    int modeNmb = 1;                //Mode nummer
    string command;                 //Variable zur Eingabe des Nutzers
    int saveInFile = 0;             //Variable, ob die Daten gespeichert werden sollen

    c.init();                       //Initialisieren eines neues Durchganges
    std::ofstream file("DataFile"); //Erstellen einer Datei zur Datenspeicehrung

	c.conn(host , 2022);            //connect to host

    while(1){


        cout << ("Enter command:");                             //Nutzereingabe
        cin >> command;

        if(command == "ITERATION"){                             //Abfrage, wie oft eine Strategie ausgeführt werden soll
            int temp;
            cout << ("Enter Number of iterations:");
            cin >> temp;

            if(temp >= 0){                                      //Überprüfen, ob die Eingabe gültig ist
                iterations = temp;
                cout << "Number of iterations was set to " << iterations << endl;
            }else{
                cout << "ERROR: Value must be higher than 0" << endl;
                iterations = 1;
            }

        }else if(command == "MODE"){                            //Wahl eines Spielmodes
            int temp;
            cout << ("Enter Mode-Number: ");                    //Nutzereingabe der entsprechenden Nummer
            cin >> temp;
            cout << endl;

            if(temp > 0 && temp < 6){                           //Überprüfen, ob die Eingabe gültig ist
                modeNmb = temp;
                cout << "Mode-Number was set to " << modeNmb << endl;
            }else{
                cout << "ERROR: Value must be between 1 and 5" << endl;
                modeNmb = 1;
            }

        }else if(command == "START"){                           //Start der Spieldurchgänge
            cout << "starting" << endl;
            for(int i = 0; i < iterations; i++){                //Wiederholen der Durchgänge, wie eingestellt
                int n = c.play(modeNmb);                        //Methodenaufruf auf den Spielablauf

                cout << "Number of shots:" << n << endl;        //Ausgabe der Anzahl der Züge
                if(saveInFile == 1){                            //ggf. Speichern der Züge in der Datei
                file << n << endl;
                }
            }

        }else if(command == "TOFILE"){                          //Eingabe, dass die Daten in der Datei gespeichert werden sollen
            saveInFile = 1;
            cout << "Data will be saved in file" << endl;
            file << "Mode:" << modeNmb << endl;                 //Modus und iterationen werden zu beginn in die Datei geschrieben
            file << "Iterations:" << iterations << endl;

        }else if(command == "CLOSEFILE"){                       //Daten werden nicht mehr in die Datei geschrieben
            saveInFile = 0;
            cout << "Data won't be saved in file" << endl;

        }else if(command == "END"){                             //Ende der Verbindung von Server und Client
            cout << "ending" << endl;
            c.sendData("BYEBYE");
            break;

        }else {
            cout << "UNKNOWN COMMAND" << endl;                  //Fehlerausgabe, bei unbekannter Nutzereingabe
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

    msg = "0";                                  //Zurücksetzen der Variablen auf Startwerte
	n = 0;
    x = 1;
    y = 1;
    lastHitX = 0;
    lastHitY = 0;
    firstHitX = 0;
    firstHitY = 0;
    search = 0;
    end = 0;

    sendData("INIT");                                   //Sende den Befehl zum Initialisieren an den Server
    receive(15);
    return;
}

int MyClient::play(int modeSelect){
    init();                                             //Initialisieren

    while(msg == "0" || msg == "1" || msg == "2"){      //Wiederholen, solange, wie Wasser oder schiffe getroffen werden, wenn das Spiel beendet wird oder bei ungültigen Werten

    switch(modeSelect){                                 //Auswahl eingegebenen Modus
        case 1:
            shoot();                                    //Schießen
            mode1();                                    //Berechnung der nächsten Koordinaten (Reihenweise durchgehen)
            break;
        case 2:
            mode2();                                    //Bestimmung der Koordinaten (Zufallszüge)
            shoot();                                    //schießen
            break;
        case 3:
            mode3();                                    //Bestimmung der Koordinaten (Zufallszüge ohne doppelte Züge)
            shoot();                                    //Schießen
            break;
        case 4:
            mode4();                                    //Bestimmen der Koordinaten (Zufallszüge, bei Treffer die Nachbarfelder auswählen)
            shoot();                                    //Schießen

            if(fieldState[x][y] == 1){                  //Wenn der letze Zug ein Treffer war
                lastHitX = x;                           //Setzen der Variablen für die letzten Treffer
                lastHitY = y;
                search = 1;                             //gehe über in den zweiten Suchmodus (Nachbarfelder absuchen)
            }

            if(fieldState[x][y] == 2){                  //Wenn ein Schiff versenkt wurde
                search = 0;                             //Gehe über in den ersten suchmodus (zufallszüge)
            }

            break;
        case 5:
            mode5();                                    //Bestimmen der Koordinaten (Zufallszüge, bei Treffer die Nachbarfelder auswählen, dann die Reihe abgehen)
            shoot();                                    //Schießen

            if(fieldState[x][y] == 2){                  //Wenn das Schiff versenkt wurde
                firstHitX = 0;                          //Zurücksetzen der Variablen
                firstHitY = 0;
                search = 0;
                end = 0;
            }

            if(fieldState[x][y] == 1 && search == 1){   //Weiterer Treffer

                lastHitX = x;                           //Aktualisieren der Variablen für den letzten Treffer
                lastHitY = y;
                search = 2;                             //Übergehen in den dritten Suchmodus
            }

            if(fieldState[x][y] == 1 && search == 0){   //Erster Treffer
                lastHitX = x;                           //Setzen der ersten Variablen
                firstHitX = x;
                lastHitY = y;
                firstHitY = y;
                search = 1;                             //übergehen in den ersten Suchmodus
            }

            if(fieldState[x][y] == 1 && search == 2){   //Weiterer Treffer im zweiten Suchmodus
                lastHitX = x;                           //Aktualisieren der Variablen für den letzten Treffer
                lastHitY = y;
            }

            break;
            default:
                cout << "ERROR: Invalid mode-number selected" << endl;  //fehler bei ungültiger eingabe
                break;
        }
    }
    return n;                                           //Anzahl der Züge zurückgeben
}


void MyClient::shoot(){
    sendData(string("COORD[") + to_string(x) + string(",") + to_string(y) + string("]"));   //Erstellen und senden eines strings zum schießen an die koordinaten x und y
    msg = receive(5);           //erhalten und speichern der Antwort

    if(msg == "0"){             //Speichern der Antwort im Datenfeld
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

    n++;        //erhöher der Variable zur Zuganzahl
    return;
}

void MyClient::mode1(){
    x++;                    //x wert erhöhen

    if(x >= 11){            //Wenn der x wert außerhalb des feldes ist, x wieder an den Anfang, y erhöhen
        x = 1;
        y++;
    }

    return;
}

void MyClient::mode2(){
    x = rand()%10 +1;                   //Bestimmen zufälliger Werte zwischen 1 und 10
    y = rand()%10 +1;
    return;
}

void MyClient::mode3(){
    do{
        x = rand()%10 +1;               //Bestimmen zufälliger Werte zwischen 1 und 10
        y = rand()%10 +1;
    }while(fieldState[x][y] != -1);     //Wiederholen, wenn das Feld schon ausgewählt wurde

    return;
}

void MyClient::mode4(){

    if(search == 1){                                                    //Beim ersten suchmodus
        if(lastHitX < 10 && fieldState[lastHitX+1][lastHitY] == -1){    //Überprüfen, ob die Nachbarfelder
            x = lastHitX +1;                                            //Setzen der Variablen für den Zuf
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

        search = 0;         //Falls kein feld frei ist zurück in den modus 1
    }
    mode3();                //Falls search = 0 ist werden zufallszüge gemacht
    return;
}

void MyClient::mode5(){             //Berechnng der Koordinatenint msgToInt(string msg){

    if(search == 2){                    //Wenn der Suchmodus 3 ist (min zwei treffer)
        if(lastHitX == firstHitX){      //Überprüfen, ob das Schiff wagerecht ist

            if(y >= 1 && y <= 10 && fieldState[lastHitX][lastHitY +1] == -1 && end == 0){   //überprüfen, ob das rechtr feld ein gültiger zug ist
                x = lastHitX;
                y = lastHitY + 1;
                return;
            }else{
                end = 1;        //Wenn kein gültiger zug möglich ist, ist das ende erreicht
            }

            if(end == 1){           //Benn das ende erreicht wurde
                while(lastHitY >= 1 && lastHitY <= 10 && fieldState[lastHitX][lastHitY] != -1){     //Überprüfen, ob ein gültiger zug möglich ist
                    lastHitY--;     //gehe in die andere richtung, bis ein gültiger zug möglich ist
                }
                x = lastHitX;
                y = lastHitY;
                return;
            }

        }

        if(lastHitY == firstHitY){          //Gleiches vorgehen wie oben, aber für senkrechtes vorgehen

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
    search = 0;     //Falls kein Zug möglich zurück zu zufallszügen
    }


    mode4();      //Wenn kein zweiter Treffer erfolgt, vorgehen wie bei Modus 4

    return;
}
