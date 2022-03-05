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
double lastDistance;
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
double dist(Coordonnees* a, Coordonnees* b){
    return sqrt(pow((b->x - a->x),2)+pow((b->y - a->y),2));
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
    double distance = dist(coord,&goal);
    double lin_cmd = 0.0;
    double rot_cmd = 0.0;
    double heading = 0.0;

    // Calcule du cap à suivre
    if ((goal.x >= coord->x) && (goal.y >= coord->y)){
        heading = atan((goal.x-coord->x)/(goal.y-coord->y));
    }
    else if ((goal.x >= coord->x) && (goal.y <= coord->y)){
        heading = atan((goal.x-coord->x)/(goal.y-coord->y)) + M_PI;
    }
    else if ((goal.x <= coord->x) && (goal.y <= coord->y)){
        heading = atan((goal.x-coord->x)/(goal.y-coord->y)) + M_PI;
    }
    else {
        heading = atan((goal.x-coord->x)/(goal.y-coord->y)) + 2*M_PI;
    }
    // Suivre le cap
    double error = heading-coord->theta;
    rot_cmd = min(0.05,error); 
    if (fabs(error) < 0.001){
        rot_cmd = 0;
    }

    // Avancer
    lin_cmd = min(0.5,distance); 

    // Stopper
    if ((distance <= 0.05) || (lastDistance<distance)){
        lin_cmd = 0;
        GoOn = 0;
    }

    // Gestion de l'arrêt
    if ((distance <= 0.05) || (lastDistance<distance)){
        lin_cmd = 0;
        rot_cmd = 0;
        GoOn = 0;
    }

    *tv_R = lin_cmd - rot_cmd;
    *tv_L = lin_cmd + rot_cmd;
    printf("Current : %.2f,%2f,%.2f -- Goal : %.2f,%2f,%.2f\n",coord->x,coord->y,coord->theta,goal.x,goal.y,goal.theta);
    printf("Commands : %.2f:%.2f -- Dist : %.2f\n",lin_cmd,rot_cmd,dist(coord,&goal));
}

/*#####################*/
/* programme principal */
/*#####################*/
int main( int argc, char *argv[])
{
    // Gestion des arguments
    if(argc != 4){
        printf("Error : incorrect argument number\n");
        return EXIT_FAILURE;
    }
    if( sscanf(argv[1], "%lf", &(goal.x) ) == 0){return(0);};
    if( sscanf(argv[1], "%lf", &(goal.y) ) == 0){return(0);};
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
    lastDistance = dist(coord,&goal)+1.0;

    do
    {
    pause();
    }
    while( GoOn == 1 );
    /* fini */
    return( 0 );  /* ->on n'arrive pas jusque la en pratique */
}

