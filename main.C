//============================================================================
// Name        : INF3_Prak.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <string>
#include <iostream>

#include <cstdio>      /* printf, NULL */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

#include <unistd.h>

#include "SHA256.H"
#include "TASK1.H"
#include "TASK2.H"
#include "TASK3.H"
#include "TASK4.H"
#include "TASK5.H"
#include "TASK6.H"

using namespace std;


int main(){
//
	/*srand(time(nullptr));

	TASK3::demoTask3();
*/

    float x, y;
    int erg;

    x = 1;
    y = 6;

    string input;
    input = string("COORD[") + to_string(x) + string(",") + to_string(y)+ string("]");
    std::cout << "INPUT-String= \"" << input << "\"\n";




    erg = sscanf(input.c_str(),"COORD[%f,%f",&x,&y);



        if(erg == 2){
            printf("Eingelesen: x = %.3f und y = %.3f\n",x,y);
        }else{
            printf("Fehler\n");
        }



   	 return 0;
}





