#include <stdio.h>
#include <stdlib.h>

// General Variables
double dt = 0.01;
char side = 'N';

// PI Constants
double Kp = 0.0;
double Ki = 0.0;
double Kd = 0.0;
// PI Variables
double lastError = 0;
double errorInt = 0;

// Shared Memory Variables
double tv = 0.0;
double state_w = 0.0;
double u = 0.0;

// PI main funcion
void stepPI(){
    double ek = tv - state_w;
    double dek = (ek-lastError)/dt;
    errorInt += dt*ek;
    u = Kp * (ek + Ki * errorInt + Kd * dek);
}

// Main Function
int main(int argc, char *argv[]){
    if(argc != 6){
        printf("Error : incorrect argument number\n");
        return EXIT_FAILURE;
    }
    if( sscanf(argv[1], "%lf", &Kp ) == 0){return(0);};
    if( sscanf(argv[2], "%lf", &Ki ) == 0){return(0);};
    if( sscanf(argv[3], "%lf", &Kd ) == 0){return(0);};
    if( sscanf(argv[4], "%lf", &dt ) == 0){return(0);};
    if( sscanf(argv[5], "%s", &side ) == 0){return(0);};
    
    return EXIT_SUCCESS;
}