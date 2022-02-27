/*
Auteur : Fabien Plouvier
Date : 27/02/2022
Sujet : AMSE - Projet C
Commentaire : Compiler le programme avec cette commande : gcc DCMoteur.c -o ../bin/DCmoteur -lm -lrt
*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdbool.h>


// Variables générales
#define CMD_BASENAME    "U"         /* ->nom de la zone partagee pour UL et UR                */
#define STATE_BASENAME  "STATE"    /* ->nom de la zone partagee pour STATEL et STATER                */
#define NB_ARGS         9               /* ->nombre d'arguments a passer en ligne de commande                            */
#define REFRESH_RATE    50              /* ->nombre d'iterations a realiser pour 1 affichage de l'etat et de la commande */
#define OFFSET_W        0               /* ->offset sur la zone d'etat pour acceder a la vitesse angulaire               */
#define OFFSET_I        1               /* ->offset sur la zone d'etat pour acceder a l'intensite                        */
#define STR_LEN         256         /* ->taille par defaut des chaines           */

/*----------*/
/* globales */
/*----------*/
int     GoOn = 1;       /* ->controle d'execution du processus */
double  a11,            /* ->coeff du modele d'etat du moteur  */
        a12,            /* ->coeff du modele d'etat du moteur  */
        a21,            /* ->coeff du modele d'etat du moteur  */
        a22,            /* ->coeff du modele d'etat du moteur  */
        b11,            /* ->coeff du modele d'etat du moteur  */
        z1,             /*->coeff. pour la recurrence */
        b1;             /*->coeff. pour la recurrence */
bool etat_regPID = true;

// Variable mémoire partagée
double *share_u;
double *share_w;
double *share_i;


/*--------------*/
/* declarations */
/*--------------*/
void usage( char *);                                                         // ->aide de ce programme  
void initModel( double, double, double, double, double, double, double);     // ->initialisation des coeffs. du modele d'etat.                    */
void updateState(void);                                                      // ->mise a jour de l'etat du moteur
void SignalHandler(int);                                                     // ->gestionnaire de signale 
double CalculZ0(double, double, double);                                     // ->calcul valeur z0
double CalculB0(double, double);                                             // ->calcul valeur b0
double CalculZ1(double, double, double);                                     // ->calcul valeur z1
double CalculB1(double, double, double);                                     // ->calcul valeur b1
double CalculIkplus1(double, double, double);                                // ->calcul valeur ik+1
double CalculWkplus1(double, double, double, double);                         // ->calcul valeur wk+1

/*&&&&&&&&&&&&&&&&&&&&&&*/
/* aide de ce programme */
/*&&&&&&&&&&&&&&&&&&&&&&*/
void usage( char *szPgmName)
{
    if( szPgmName == NULL)
    {
        exit( -1 );
    };
    printf("%s <Resistance> <Inductance> <Cste electrique> <Cste moteur> <Coeff de frottement> <Inertie totale> <Periode d'ech.> <drive>\n", szPgmName);
    printf("   avec <drive> = L | R \n");
}



/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/* initialisation des coefficients */
/* du modele d'etat                */
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
void initModel( double r,           /* ->resistance                 */
                double l,           /* ->inductance                 */
                double Ke,          /* ->constante electrique       */
                double Km,          /* ->constante moteur           */
                double f,           /* ->coefficient de frottements */
                double j,           /* ->inertie totale             */
                double Te   )       /* ->periode d'echantilonnage   */
{
    double z0;  /* ->coeff. pour la recurrence */
    double b0;  /* ->coeff. pour la recurrence */
    z0 = CalculZ0(Te, r, l);
    z1 = CalculZ1(Te, f, j);
    b0 = CalculB0(r, z0);
    b1 = CalculB1(Km, f, z1);
    
    a11 = z0;
    a12 = -Ke*b0;
    a21 = b1;
    a22 = z1;
    b11 = b0;
}

//fonction reset
void resetModel(){
    a11 = 0.0;
    a12 = 0.0;
    a21 = 0.0;
    a22 = 0.0;
    b11 = 0.0;
    b1 = 0.0;
    z1 = 0.0;
    
}
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/* mise a jour de l'etat du moteur */
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
void updateState( void )
{
    double u;       /* ->valeur courante de la commande          */
    double w;       /* ->valeur courante de la vitesse angulaire */
    double i;       /* ->valeur courante du courant              */
    double w_new;   /* ->nouvelle valeur de la vitesse angulaire */
    double i_new;   /* ->nouvelle valeur du courant              */
    
    /* photo.. */
    u = *share_u;
    i = *share_i;
    w = *share_w;

    /* calcul */
    i_new = CalculIkplus1(i, w, u);
    w_new = CalculWkplus1(z1, w, b1, i);
    /* mise a jour */
    *share_i = i_new;
    *share_w = w_new;
}


// fonction calcul paramètres
double CalculZ0(double Te, double R, double L){
    double z0;
    double x = (Te*R)/L;
    z0 = exp(-x);
    return z0;
}

double CalculB0(double R, double z0){
    double b0;
    b0 = (1/R)*(1-z0);
    return b0;
}

double CalculZ1(double Te, double f, double J){
    double z1;
    double x = (Te*f)/J;
    z1 = exp(-x);
    return z1;
}

double CalculB1(double Km, double f, double z1){
    double b1;
    b1 = (Km/f)*(1-z1);
    return b1;
}


double CalculIkplus1(double ik, double wk, double uk){
    double ikplus1;
    double nombre1;
    double nombre2;
    double nombre3;
    nombre1 = a11 * ik;
    nombre2 = a12 * wk;
    nombre3 = b11 * uk;

    ikplus1 = nombre1 - nombre2 + nombre3;
    return ikplus1;    
}

double CalculWkplus1(double z1, double wk, double b1, double ik){
    double wkplus1;
    double nombre1 = z1*wk;
    double nombre2 = b1*ik;

    wkplus1 = nombre1 + nombre2;
    return wkplus1;
}

/*&&&&&&&&&&&&&&&&&&&&&&&&*/
/* gestionnaire de signal */
/*&&&&&&&&&&&&&&&&&&&&&&&&*/
void SignalHandler( int signal )
{
    // SIGUSR2 -> PID ON/OFF
    if ( signal == SIGUSR2 ){
        etat_regPID = !etat_regPID;
        if (etat_regPID) {
            printf("ON\n");
            GoOn = 1;
            }
        else {
            printf("OFF\n");
            GoOn = 0;
            }
        if (etat_regPID){
            printf("reset\n");
            resetModel();
        }
    }
    if( signal == SIGALRM)
    {
        //printf("Update\n");
        updateState();
    };
}

// Fonction principale
int main(int argc, char *argv[]){
    char    cDriveID;                       /* ->caractere pour identifier le moteur            */
    char    szCmdAreaName[STR_LEN];         /* ->nom de la zone contenant la commande           */
    char    szStateAreaName[STR_LEN];       /* ->nom de la zone contenant l'etat du moteur      */
    int     iFdCmd;                         /* ->descripteur pour la zone de commande           */
    int     iFdState;                       /* ->descripteur pour la zone d'etat                */
    int     iLoops = 0;                     /* ->compte le nombre d'iterations effectuees       */
    double  *lpdb_state;                    /* ->pointeur sur la zone partagee contenant l'etat */
    double  r;                              /* ->resistance de l'induit                         */
    double  l;                              /* ->inductance                                     */
    double  Ke;                             /* ->constante electrique                           */
    double  Km;                             /* ->constante moteur                               */
    double  f;                              /* ->coefficient de frottement                      */
    double  j;                              /* ->inertie totale au rotor                        */
    double  Te;                             /* ->periode d'echantillonnage                      */
    struct sigaction    sa;                 /* ->gestion du signal handler                      */
    struct sigaction    sa_old;             /* ->gestion du signal handler                      */
    sigset_t            mask;               /* ->liste des signaux a masquer                    */
    struct itimerval    sTime;              /* ->periode du timer                               */

    /*.......*/
    /* check */
    /*.......*/
    if( argc != NB_ARGS)
    {
        usage(argv[0]);
        return( 0 );
    };
    /*............................*/
    /* recuperation des arguments */
    /*............................*/
    if( sscanf(argv[1],"%lf",&r) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #1 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[2],"%lf",&l) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #2 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[3],"%lf",&Ke) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #3 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[4],"%lf",&Km) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #4 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[5],"%lf",&f) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #5 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[6],"%lf",&j) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #6 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[7],"%lf",&Te) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #7 doit etre reel\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    if( sscanf(argv[8],"%c",&cDriveID) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #8 doit etre un caractere\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    
    /*.......................*/
    /* mise a jour du modele */
    /*.......................*/
    initModel(r,l,Ke,Km,f,j,Te);
    /*................................................*/
    /* lien / creation aux zones de memoire partagees */
    /*................................................*/
    sprintf(szCmdAreaName,"%s%c", CMD_BASENAME, cDriveID);
    sprintf(szStateAreaName,"%s%c", STATE_BASENAME, cDriveID);
    /* zone de commande */
    if(( iFdCmd = shm_open(szCmdAreaName, O_RDWR, 0600)) < 0 )
    {
        if(( iFdCmd = shm_open(szCmdAreaName, O_RDWR | O_CREAT, 0600)) < 0)
        {
            fprintf(stderr,"%s.main() :  ERREUR ---> appel a shm_open() #1\n", argv[0]);
            fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
            exit( -errno );
        }
        else
        {
            printf("CREATION de la zone %s\n", szCmdAreaName);
        };
    }
    else
    {
        printf("LIEN a la zone %s\n", szCmdAreaName);
    };
    if( ftruncate(iFdCmd, sizeof(double)) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a ftruncate() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    if((share_u = (double *)(mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, iFdCmd, 0))) == MAP_FAILED )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a mmap() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    /* zone d'etat */
    if(( iFdState = shm_open(szStateAreaName, O_RDWR, 0600)) < 0 )
    {
        if(( iFdState = shm_open(szStateAreaName, O_RDWR | O_CREAT, 0600)) < 0)
        {
            fprintf(stderr,"%s.main() :  ERREUR ---> appel a shm_open() #2\n", argv[0]);
            fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
            exit( -errno );
        }
        else
        {
            printf("CREATION de la zone %s\n", szStateAreaName);
        };
    }
    else
    {
        printf("LIEN a la zone %s\n", szStateAreaName);
    };
    if( ftruncate(iFdState, 2 * sizeof(double)) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a ftruncate() #2\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    if((lpdb_state = (double *)(mmap(NULL, 2 * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, iFdState, 0))) == MAP_FAILED )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a mmap() #2\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    share_w = &lpdb_state[OFFSET_W];
    share_i = &lpdb_state[OFFSET_I];
    /*............................................*/
    /* installation de la routine d'interception  */
    /*............................................*/
    memset(&sa,0,sizeof(struct sigaction));
    sigemptyset( &mask );
    sa.sa_mask = mask;
    sa.sa_handler = SignalHandler;
    sa.sa_flags = 0;
    if( sigaction(SIGALRM, &sa, &sa_old) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a sigaction() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    if( sigaction(SIGUSR2, &sa, &sa_old) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a sigaction() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    /*........................*/
    /* configuration du timer */
    /*........................*/
    sTime.it_interval.tv_sec = (int)(Te);
    sTime.it_interval.tv_usec = (int)((Te - (int)(Te))*1e6);
    
    sTime.it_value.tv_sec = (int)(Te);
    sTime.it_value.tv_usec = (int)((Te - (int)(Te))*1e6);
    
    if( setitimer( ITIMER_REAL, &sTime, NULL) < 0 )
     {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a setitimer() \n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    /*************************/
    /* fonctionnement normal */
    /*************************/
    while( GoOn)
    {
        pause();
        if( (iLoops % (int)(REFRESH_RATE)) == 0)
        {
            //printf("u = %lf w = %lf i = %lf side = %c\n", *share_u, *share_w, *share_i, cDriveID);
        };
        iLoops++;
    }
    return( 0 );
}