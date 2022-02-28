#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>  
#include <math.h>

#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>   


#define AREA_NAME      "VELOCITY"
#define AREA_NAME2       "POSITION"
#define STOP            "A"      /* ->chaine a saisir pour declencher l'arret */
#define STR_LEN         256                  /* ->controle d'execution                */

FILE *fp;
int nbCoord=0;
int allowPrint=0;

typedef struct Coordonnees{
    double x;
    double y;
    double theta;
}Coordonnees;

typedef struct vitesse{
    double v;
    double w;
}vitesse;


vitesse *szInStr; 
int  GoOn = 1; 
double dt=0.01;

Coordonnees oldCoord;

Coordonnees * szInStr2;

void usage( char *);           /* ->aide de ce programme                */
void cycl_alm_handler( int );  /* ->gestionnaire pour l'alarme cyclique */
/*&&&&&&&&&&&&&&&&&&&&&&*/
/* aide de ce programme */
/*&&&&&&&&&&&&&&&&&&&&&&*/
void usage( char *pgm_name )
{
  if( pgm_name == NULL )
  {
    exit( -1 );
  };
  printf("%s <hh> <mm> <ss>\n", pgm_name );
  printf("declenche un compte a rebours qui va decompte de <hh> <mm> <ss>\n");
  printf("jusque 0 en affichant les secondes.\n");
  printf("exemple : \n");
  printf("%s 12 23 57\n", pgm_name );
}
void cycl_alm_handler( int signal ) //On lit la mémoire partagé a chaque itération
{
  if ( signal == SIGALRM ){
    vitesse testVit;
    testVit.v=szInStr->v;
    testVit.w=szInStr->w;
    Coordonnees testCoord;
    testCoord.theta=(oldCoord.theta+dt*testVit.w);
    if(testCoord.theta> 6.28){
        testCoord.theta-=6.28;
    }
    if(testCoord.theta<-6.28){
        testCoord.theta+=6.28;
