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

typedef struct etat_moteur{
  double i;
  double w;
}etat_moteur;

// Zones partagées
#define AREA_TVL       "TVL"
#define AREA_TVR       "TVR"
#define AREA_U_L       "UL"
#define AREA_U_R       "UR"
#define AREA_STATE_L   "STATEL"
#define AREA_STATE_R   "STATER"
#define STOP           "A"      /* ->chaine a saisir pour declencher l'arret */
#define STR_LEN        256         /* ->taille par defaut des chaines           */
double* sm_tv;
double* sm_u;
etat_moteur* sm_state;

// General Variables
double dt = 0.01;
char side = 'N';
bool etat_regPID = true;

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
    //Read Shared Memory
    tv = *sm_tv;
    state_w = sm_state->w;

    // Compute
    double ek = tv - state_w;
    double dek = (ek-lastError)/dt;
    errorInt += dt*ek;
    u = Kp * (ek + Ki * errorInt + Kd * dek);

    // Write Shared Memory
    *sm_u = u;
    printf("TV=%.2f, State=%.2f, U=%.2f, Side = %c\n",tv,state_w,u,side);
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
            errorInt = 0; //reset de l'erreur intégrale
        }
    }
    if ( signal == SIGALRM ){
        stepPI();
    }
}

// Main Function
int main(int argc, char *argv[]){
    // Gestion des arguments
    if(argc != 6){
        printf("Error : incorrect argument number\n");
        return EXIT_FAILURE;
    }
    if( sscanf(argv[1], "%lf", &Kp ) == 0){return(0);};
    if( sscanf(argv[2], "%lf", &Ki ) == 0){return(0);};
    if( sscanf(argv[3], "%lf", &Kd ) == 0){return(0);};
    if( sscanf(argv[4], "%lf", &dt ) == 0){return(0);};
    if( sscanf(argv[5], "%s", &side ) == 0){return(0);};

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
    sigaction( SIGUSR2, &sa, NULL );
    //
    sigaction(SIGALRM, &sa, NULL );
    /* initialisation de l'alarme  */
    period.it_interval.tv_sec  = 0;  
    period.it_interval.tv_usec = dt*1000000;
    period.it_value.tv_sec     = 0;
    period.it_value.tv_usec    = dt*1000000;
    /* demarrage de l'alarme */
    setitimer( ITIMER_REAL, &period, NULL );

    // Declaration memoire partagée
    if (side == 'L'){
        // TVL
        void *vAddr;
        int  iShmFd;
        if( (iShmFd = shm_open(AREA_TVL, O_RDWR, 0600)) < 0)
        {  
            fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        ftruncate(iShmFd, STR_LEN);
        if( (vAddr = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        sm_tv = (double *)(vAddr);
        // STATEL
        if( (iShmFd = shm_open(AREA_STATE_L, O_RDWR, 0600)) < 0)
        {  
            fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        ftruncate(iShmFd, STR_LEN);
        if( (vAddr = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        sm_state = (etat_moteur *)(vAddr);
        // UL
        if( (iShmFd = shm_open(AREA_U_L, O_RDWR | O_CREAT, 0600)) < 0)
        {
            /* on essaie de se lier sans creer... */
            printf("echec de creation, lien seul...\n");
            if( (iShmFd = shm_open(AREA_U_L, O_RDWR, STR_LEN)) < 0)
            {  
                fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
                fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
                return( -errno );
            };
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
        sm_u = (double *)(vAddr);
    }
    else if (side == 'R'){
        // TVR
        void *vAddr;
        int  iShmFd;
        if( (iShmFd = shm_open(AREA_TVR, O_RDWR, 0600)) < 0)
        {  
            fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        ftruncate(iShmFd, STR_LEN);
        if( (vAddr = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        sm_tv = (double *)(vAddr);
        // STATER
        if( (iShmFd = shm_open(AREA_STATE_R, O_RDWR, 0600)) < 0)
        {  
            fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        ftruncate(iShmFd, STR_LEN);
        if( (vAddr = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd, 0 ))  == NULL)
        {
            fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
        sm_state = (etat_moteur *)(vAddr);
        // UR
        if( (iShmFd = shm_open(AREA_U_R, O_RDWR | O_CREAT, 0600)) < 0)
        {
            /* on essaie de se lier sans creer... */
            printf("echec de creation, lien seul...\n");
            if( (iShmFd = shm_open(AREA_U_R, O_RDWR, STR_LEN)) < 0)
            {  
                fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
                fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
                return( -errno );
            };
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
        sm_u = (double *)(vAddr);
    }
    
    do
    {
        pause();
    } while( 1 );

    return EXIT_SUCCESS;
}