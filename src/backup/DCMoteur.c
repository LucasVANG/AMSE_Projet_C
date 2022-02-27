#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>  
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>


// Variables générales
#define AREA_NAME_UL       "UL"    /* ->nom de la zone partagee pour ul                 */
#define AREA_NAME_UR       "UR"    /* ->nom de la zone partagee pour ur                */
#define AREA_NAME_STATEL       "STATEL"    /* ->nom de la zone partagee pour statel                */
#define AREA_NAME_STATER       "STATER"    /* ->nom de la zone partagee pour stater               */
#define STOP            "A"      /* ->chaine a saisir pour declencher l'arret */
#define STR_LEN         256         /* ->taille par defaut des chaines           */

//Constantes
double R = 0.0;         //résistance d'induit
double L = 0.0;         //l'inductance
double Ke = 0.0;        //la constante électrique
double Km = 0.0;        //la constante moteur
double f = 0.0;         //le coefficient de frottement
double J = 0.0;         //l'inertie totale ramenée sur le rotor
double Te = 0.0;        //période d'échantillonnage alarme cyclique (sec)
char side = 'N';        //le côté du moteur choisi (L ou R)
bool etat_regPID = true;

/*...................*/
/* prototypes locaux */
/*...................*/
typedef struct etat_moteur_actuel{
  double i;
  double w;
}etat_moteur_actuel;

//Variables
double ikL = 0.0;
double ikLplus1 = 0.0;
double wkL = 0.0;
double wkLplus1 = 0.0;
double ikR = 0.0;
double ikRplus1 = 0.0;
double wkR = 0.0;
double wkRplus1 = 0.0;
etat_moteur_actuel *sm_state_actual;

// Variable mémoire partagée
double *uL;
double *uR;

//fonction reset
void reset(){
    wkL = 0.0;
    wkLplus1 = 0.0;
    wkR = 0.0;
    wkRplus1 = 0.0;
    ikL = 0.0;
    ikLplus1 = 0.0;
    ikR = 0.0;
    ikRplus1 = 0.0;
}

// fonction calcul paramètres
double CalculZ0(double Te, double R, double L){
    double z0 = 0.0;
    double x = (Te*R)/L;
    z0 = exp(-x);
    return z0;
}

double CalculB0(double R, double z0){
    double b0 = 0.0;
    b0 = (1/R)*(1-z0);
    return b0;
}

double CalculZ1(double Te, double f, double J){
    double z1 = 0.0;
    double x = (Te*f)/J;
    z1 = exp(-x);
    return z1;
}

double CalculB1(double Km, double f, double z1){
    double b1 = 0.0;
    b1 = (Km/f)*(1-z1);
    return b1;
}

double CalculIkplus1(double z0, double ik, double Ke, double b0, double wk, double uk){
    double ikplus1 = 0.0;
    double nombre1 = 0.0;
    double nombre2 = 0.0;
    double nombre3 = 0.0;
    nombre1 = z0 * ik;
    nombre2 = Ke*b0*wk;
    nombre3 = b0*uk;

    ikplus1 = nombre1 - nombre2 + nombre3;
    return ikplus1;    
}

double CalculWkplus1(double z1, double wk, double b1, double ik){
    double wkplus1 = 0.0;
    double nombre1 = z1*wk;
    double nombre2 = b1*ik;

    wkplus1 = nombre1 + nombre2;
    return wkplus1;
}

char RecupererValeurMemoirePartagee(char *AREA_NAME){
    void *vAddr;                    /* ->adresse virtuelle sur la zone          */                  /* ->chaine saisie                          */
    int  iShmFd;                    /* ->descripteur associe a la zone partagee */
    /*..................................*/
    /* tentative d'acces a la zone */
    /*..................................*/
    /* on essaie de se lier sans creer... */
    if( (iShmFd = shm_open(AREA_NAME, O_RDWR, 0600)) < 0)
    {  
        fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
        fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
        return( -errno );
    };
    /* on attribue la taille a la zone partagee */
    ftruncate(iShmFd, STR_LEN);
    /* tentative de mapping de la zone dans l'espace memoire du */
    /* processus                                                */
    if( (vAddr = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd, 0 ))  == NULL)
    {
        fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
        fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
        return( -errno );
    };
    return vAddr;
}

void RecupererTensionCommandeUL(){
    uL = (char *)(RecupererValeurMemoirePartagee(AREA_NAME_UL));
}

void RecupererTensionCommandeUR(){
    uR = (char *)(RecupererValeurMemoirePartagee(AREA_NAME_UR));
}

void MettreAJourStateL(double z1, double z0, double b1, double b0){
    wkL = wkLplus1;
    ikL = ikLplus1;
    RecupererTensionCommandeUL();
    wkLplus1 = CalculWkplus1(z1, wkL, b1, ikL);
    ikLplus1 = CalculIkplus1(z0, ikL, Ke, b0, wkL, *uL);
    sm_state_actual->w = wkLplus1;
    sm_state_actual->i = ikLplus1;
}

void MettreAJourStateR(double z1, double z0, double b1, double b0){
    wkR = wkRplus1;
    ikR = ikRplus1;
    RecupererTensionCommandeUR();
    wkRplus1 = CalculWkplus1(z1, wkR, b1, ikR);
    ikRplus1 = CalculIkplus1(z0, ikR, Ke, b0, wkR, *uR);
    sm_state_actual->w = wkRplus1;
    sm_state_actual->i = ikRplus1;
}

// Routines de signaux
void signal_handler( int signal )
{
    // SIGUSR2 -> PID ON/OFF
    if ( signal == SIGUSR2 ){
        etat_regPID = !etat_regPID;
        if (etat_regPID) {printf("ON\n");}
        else {printf("OFF\n");}
        if (etat_regPID){
            reset();
        }
    }
    if ( signal == SIGALRM ){
        printf("Update\n");
    }
}

// Fonction principale
int main(int argc, char *argv[]){
    if(argc != 8){
        printf("ERREUR : Nombre d'arguments incorrect (8 requis)\n");
        return EXIT_FAILURE;
    }
    if( sscanf(argv[1], "%lf", &R ) == 0){return(0);};
    if( sscanf(argv[2], "%lf", &L ) == 0){return(0);};
    if( sscanf(argv[3], "%lf", &Ke ) == 0){return(0);};
    if( sscanf(argv[4], "%lf", &Km ) == 0){return(0);};
    if( sscanf(argv[5], "%lf", &f ) == 0){return(0);};
    if( sscanf(argv[6], "%lf", &J ) == 0){return(0);};
    if( sscanf(argv[7], "%lf", &Te ) == 0){return(0);};
    if( sscanf(argv[8], "%s", &side ) == 0){return(0);};
    
    // Gestion de signal
    struct sigaction sa;
    sigset_t blocked;
    struct itimerval period;
    sigemptyset( &blocked );
    memset( &sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sa.sa_mask = blocked;
    //
    sigaction(SIGUSR2, &sa, NULL );
    //
    sigaction(SIGALRM, &sa, NULL );
    /* initialisation de l'alarme  */
    period.it_interval.tv_sec  = 0;  
    period.it_interval.tv_usec = Te*1000000;
    period.it_value.tv_sec     = 0;
    period.it_value.tv_usec    = Te*1000000;
    /* demarrage de l'alarme */
    setitimer( ITIMER_REAL, &period, NULL );
    double z0 = CalculZ0(Te, R, L);
    double z1 = CalculZ1(Te, f, J);
    double b0 = CalculB0(R, z0);
    double b1 = CalculB1(Km, f, z1);

    if (side == 'L'){
        MettreAJourStateL(z1, z0, b1, b0);
    }else if (side == 'R'){
        MettreAJourStateR(z1, z0, b1, b0);
    }
    return EXIT_SUCCESS;
}