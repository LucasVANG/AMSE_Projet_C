#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/time.h>  

#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>      /* ->INDISPENSABLE pour les types tempo. */
/*....................*/
/* variables globales */
/*....................*/
typedef struct etat_moteur{
  double i;
  double w;
}etat_moteur;
#define STATE_BASENAME  "STATE"    /* ->nom de la zone partagee pour STATEL et STATER                */
#define STR_LEN         256         /* ->taille par defaut des chaines           */
#define NB_ARGS         1               /* ->nombre d'arguments a passer en ligne de commande                            */

int  GoOn = 1;                 /* ->controle d'execution                */


// Variable mémoire partagée
etat_moteur* sm_state;


/*--------------*/
/* declarations */
/*--------------*/
void usage( char *);           /* ->aide de ce programme                */


/*&&&&&&&&&&&&&&&&&&&&&&*/
/* aide de ce programme */
/*&&&&&&&&&&&&&&&&&&&&&&*/
void usage( char *pgm_name )
{
  if( pgm_name == NULL )
  {
    exit( -1 );
  };
  printf("%s <drive> = L | R \n", pgm_name );
  printf("Remettre à zéro l'état d'un des moteurs.\n");
  printf("(contenu d'une zone partagée)\n");
  printf("exemple : \n");
  printf("%s ResetState L\n", pgm_name );
}


/*#####################*/
/* programme principal */
/*#####################*/
int main( int argc, char *argv[])
{
    char    cDriveID;                       /* ->caractere pour identifier le moteur            */
    char    szStateAreaName[STR_LEN];       /* ->nom de la zone contenant la commande           */
    int     iFdState;                       /* ->descripteur pour la zone de commande           */
    double  *etat_moteur;                    /* ->pointeur sur la zone partagee contenant l'etat */

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

    if( sscanf(argv[1],"%c",&cDriveID) == 0 )
    {
        fprintf(stderr,"%s.main()  : ERREUR ---> l'argument #1 doit etre un caractere\n", argv[0]);
        usage(argv[0]);
        return( 0 );
    };
    sprintf(szStateAreaName,"%s%c", STATE_BASENAME, cDriveID);

    /*..................................*/
    /* tentative d'acces a la zone */
    /*..................................*/
    /* on essaie de se lier sans creer... */
    /*................................................*/
    /* lien / creation aux zones de memoire partagees */
    /*................................................*/
    sprintf(szStateAreaName,"%s%c", STATE_BASENAME, cDriveID);
    /* zone de commande */
    if(( iFdState = shm_open(szStateAreaName, O_RDWR, 0600)) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a shm_open() \n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    }
    else
    {
        printf("LIEN a la zone %s\n", szStateAreaName);
    };
    if( ftruncate(iFdState, 2 * sizeof(double)) < 0 )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a ftruncate() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    if((etat_moteur = (double *)(mmap(NULL, 2 * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, iFdState, 0))) == MAP_FAILED )
    {
        fprintf(stderr,"%s.main() :  ERREUR ---> appel a mmap() #1\n", argv[0]);
        fprintf(stderr,"             code = %d (%s)\n", errno, (char *)(strerror(errno)));
        exit( -errno );
    };
    /*************************/
    /* fonctionnement normal */
    /*************************/
    memset(etat_moteur,0, sizeof(sm_state));


  return( 0 );
}