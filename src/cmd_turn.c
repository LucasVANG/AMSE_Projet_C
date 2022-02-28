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
#include <sys/stat.h>      /* ->INDISPENSABLE pour les types tempo. */
/*....................*/
/* variables globales */
/*....................*/
#define AREA_NAME1       "TVR"    /* ->nom de la zone partagee                 */
#define AREA_NAME2      "TVL"
#define AREA_NAME3       "POSITION"
#define STOP            "A"      /* ->chaine a saisir pour declencher l'arret */
#define STR_LEN         256         /* ->taille par defaut des chaines           */

typedef struct Coordonnees{
    double x;
    double y;
    double theta;
}Coordonnees;

double *tv_R;                  /* ->valeur saisie                          */
double *tv_L;
Coordonnees* coord;

Coordonnees goal;
double angleOrder;
double dt;
int  GoOn = 1;                 /* ->controle d'execution                */

/*...................*/
/* prototypes locaux */
/*...................*/
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
}
double min(double a, double b){
    if (a > b){
        return b;
    }
    return a;
}

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/* gestionnaire de l'alarme cyclique */
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
void cycl_alm_handler( int signal ) //On lit la mémoire partagé a chaque itération
{
    double error = goal.theta-coord->theta;
    double command = min(0.2,error); 
    if (fabs(error) < 0.01){
        command = 0;
        GoOn = 0;
    }
    *tv_R = command;
    *tv_L = -command;
    printf("Current : %.2f,%2f,%.2f -- Goal : %.2f,%2f,%.2f\n",coord->x,coord->y,coord->theta,goal.x,goal.y,goal.theta);
    printf("Command : %.2f -- Error : %.2f\n",command,error);
}

/*#####################*/
/* programme principal */
/*#####################*/
int main( int argc, char *argv[])
{
    // Gestion des arguments
    if(argc != 3){
        printf("Error : incorrect argument number\n");
        return EXIT_FAILURE;
    }
    if( sscanf(argv[1], "%lf", &angleOrder ) == 0){return(0);};
    if( sscanf(argv[2], "%lf", &dt ) == 0){return(0);};

    void *vAddr1;                    /* ->adresse virtuelle sur la zone          */
    void *vAddr2;
    void *vAddr3;
    int iShmFd1;                    /* ->descripteur associe a la zone partagee */
    int iShmFd2;
    int iShmFd3;

    /*..................................*/
    /* tentative de creation de la zone */
    /*..................................*/
    if( (iShmFd1 = shm_open(AREA_NAME1, O_RDWR | O_CREAT, 0600)) < 0)
    {
        /* on essaie de se lier sans creer... */
        printf("echec de creation, lien seul...\n");
        if( (iShmFd1 = shm_open(AREA_NAME1, O_RDWR, STR_LEN)) < 0)
        {  
            fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
    };
    /* on attribue la taille a la zone partagee */
    ftruncate(iShmFd1, STR_LEN);
    /* tentative de mapping de la zone dans l'espace memoire du */
    /* processus                                                */
    if( (vAddr1 = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd1, 0 ))  == NULL)
    {
        fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
        fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
        return( -errno );
    };
    tv_R = (double *)(vAddr1);

    /*..................................*/
    /* tentative de creation de la zone */
    /*..................................*/
    if( (iShmFd2 = shm_open(AREA_NAME2, O_RDWR | O_CREAT, 0600)) < 0)
    {
        /* on essaie de se lier sans creer... */
        printf("echec de creation, lien seul...\n");
        if( (iShmFd2 = shm_open(AREA_NAME2, O_RDWR, STR_LEN)) < 0)
        {  
            fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
            fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
            return( -errno );
        };
    };
    /* on attribue la taille a la zone partagee */
    ftruncate(iShmFd2, STR_LEN);
    /* tentative de mapping de la zone dans l'espace memoire du */
    /* processus                                                */
    if( (vAddr2 = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd2, 0 ))  == NULL)
    {
        fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
        fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
        return( -errno );
    };
    tv_L = (double *)(vAddr2);

    /*..................................*/
    /*......Lecture de l'odométrie......*/
    /*..................................*/
    if( (iShmFd3 = shm_open(AREA_NAME3, O_RDWR, 0600)) < 0)
    {  
        fprintf(stderr,"ERREUR : ---> appel a shm_open()\n");
        fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
        return( -errno );
    };
    ftruncate(iShmFd3, STR_LEN);
    if( (vAddr3 = mmap(NULL, STR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, iShmFd3, 0 ))  == NULL)
    {
        fprintf(stderr,"ERREUR : ---> appel a mmap()\n");
        fprintf(stderr,"         code  = %d (%s)\n", errno, (char *)(strerror(errno)));
        return( -errno );
    };
    coord = (Coordonnees*)(vAddr3);


    // Gestion de signal
    struct sigaction sa;
    sigset_t blocked;
    struct itimerval period;
    sigemptyset( &blocked );
    memset( &sa, 0, sizeof(struct sigaction));
    sa.sa_handler = cycl_alm_handler;
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


    // Initiate Go Order
    goal.theta = coord->theta + angleOrder;
    if(goal.theta> 6.28){
        goal.theta-=6.28;
    }
    if(goal.theta<-6.28){
        goal.theta+=6.28;
    }

    do
    {
    pause();
    }
    while( GoOn == 1 );
    /* fini */
    return( 0 );  /* ->on n'arrive pas jusque la en pratique */
}

