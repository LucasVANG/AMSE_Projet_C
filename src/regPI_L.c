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
double tv_l = 0.0;
double state_l_w = 0.0;
double u_l = 0.0;

// PI main funcion
void stepPI(){
    lastErrorInt = lastErrorInt + Ki * dt * lastError;
    u_l = Kp * (tv_l - state_l_w) + (lastErrorInt);
}

// Main Function
int main(int argc, char *argv[]){
}