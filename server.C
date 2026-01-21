/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */
#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses

#include <string>

#include "TASK3.H"
#include "SIMPLESOCKET.H"

class MyServer : public  TCPserver{
public:
    MyServer(int port, int bufferSize) : TCPserver(port, bufferSize){w = new TASK3::World();};
protected:
    string myResponse(string input);
private:
    TASK3::World* w = nullptr;
};


int main(){
	srand(time(nullptr));
	MyServer srv(2022,25);
	srv.run();
}

string MyServer::myResponse(string input){

    if(input.compare(0,4,"INIT") == 0){     //neues Spiel initialisieren
        delete w;
        w = new TASK3::World();             //Neues Objekt der klasse World erstellen
        return string("INITIALIZED\n");
    }

    if(input.compare(0,6,"COORD[") == 0){   //Koordinaten eines Schusses erhalten und verarbeiten

        int x,y;            //Variablen für Koordinaten
        int e;              //Variable für Antwort
        e = sscanf(input.c_str(),"COORD[%i,%i]",&x,&y); //einlesen der Eingabe

        TASK3::ShootResult res = w->shoot(x,y);         //Schießen auf die Koordinaten, speichern des Ergebnisses

        string msg = to_string(res) + "\n";                    //Ergebnis in string speichern
        return msg;
    }

    if(input.compare(0,5,"PRINT") == 0){                //Ausgabe des Spielfeldes

        w->printBoard();
        return "PRINTED\n";
    }


    return(string("UNKNOWN COMMAND\n"));                  //Bei unbekannter eingabe
}



