#include <stdio.h>
#include <stdlib.h>

// General Variables
double dt = 0.01;

// PI Constants
double Kp = 1.0;
double Ki = 1.0;
// PI Variables
double lastError = 0;
double lastErrorInt = 0;

// Shared Memory Variables
double tv_r = 0.0;
double state_r_w = 0.0;
double u_r = 0.0;

// PI main funcion
void stepPI(){
    lastErrorInt = lastErrorInt + Ki * dt * lastError;
    u_r = Kp * (tv_r - state_r_w) + (lastErrorInt);
}

// Main Function
int main(int argc, char *argv[]){
    
    // Read TVR
    // tv_r = 

    // Read SRW
    // state_r_w = 

    stepPI();

    // Write UR
    // u_r 

    return EXIT_SUCCESS;
}