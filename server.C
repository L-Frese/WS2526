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
    MyServer(int port, int bufferSize) : TCPserver(port, bufferSize){};
protected:
    string myResponse(string input);
};


int main(){
	srand(time(nullptr));
	MyServer srv(2022,25);
	srv.run();
}

string MyServer::myResponse(string input){
    int x,y;
    int e;
    e = sscanf(input.c_str(),"COORD[%d,%d]",&x,&y);
    if(e != 2){
        return string("ERROR");
    }else{
        return(to_string(x+y));
    }


/*
    if(input.compare(0,4,"INIT") == 0){
        //initialize a new game

        return string("OKAY");
    }

    if(input.compare(0,6,"COORD[") == 0){
        //verarbeite Koordinaten

        TASK3::ShootResult res;
        res = TASK3::WATER;

        string msg = to_string(res);
        return msg;
    }

*/
    return(string("UNKNOWN COMMAND"));
}
