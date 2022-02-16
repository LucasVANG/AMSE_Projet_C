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
#define AREA_NAME       "TVR"    /* ->nom de la zone partagee                 */
#define STOP            "A"      /* ->chaine a saisir pour declencher l'arret */
#define STR_LEN         256         /* ->taille par defaut des chaines           */
int test=0;
int  hh,                       /* ->heures                              */
     mm,                       /* ->minutes                             */
     ss;                       /* ->secondes                            */
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
  printf("%s <hh> <mm> <ss>\n", pgm_name );
  printf("declenche un compte a rebours qui va decompte de <hh> <mm> <ss>\n");
  printf("jusque 0 en affichant les secondes.\n");
  printf("exemple : \n");
  printf("%s 12 23 57\n", pgm_name );
}
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/* gestionnaire de l'alarme cyclique */
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
void cycl_alm_handler( int signal ) //On lit la mémoire partagé a chaque itération
{
    test++;
    printf("%i\n", test);
    void *vAddr;                    /* ->adresse virtuelle sur la zone          */
    char *szInStr;                  /* ->chaine saisie                          */
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
    szInStr = (char *)(vAddr);
    /* affichage */
    printf("contenu de la zone = %s\n", szInStr);
    //shm_unlink(AREA_NAME);
}


/*#####################*/
/* programme principal */
/*#####################*/
int main( int argc, char *argv[])
{
  struct sigaction      sa,      /* ->configuration de la gestion de l'alarme */
                        sa_old;  /* ->ancienne config de gestion d'alarme     */
  sigset_t              blocked; /* ->liste des signaux bloques               */
  struct itimerval      period;  /* ->periode de l'alarme cyclique            */
  /* verification des arguments */
  if( argc != 4 )
  {
    usage( argv[0] );
    return( 0 );
  };
  /* initialisation */
  sigemptyset( &blocked );
  memset( &sa, 0, sizeof( sigaction )); /* ->precaution utile... */
  sa.sa_handler = cycl_alm_handler;
  sa.sa_flags   = 0;
  sa.sa_mask    = blocked;
  /* installation du gestionnaire de signal */
  sigaction(SIGALRM, &sa, NULL );
  /* initialisation de l'alarme  */
  period.it_interval.tv_sec  = 1;  
  period.it_interval.tv_usec = 0;
  period.it_value.tv_sec     = 1;
  period.it_value.tv_usec    = 0;
  /* demarrage de l'alarme */
  setitimer( ITIMER_REAL, &period, NULL );
  /* on ne fait desormais plus rien d'autre que */
  /* d'attendre les signaux                     */
  do
  {
    pause();
  }
  while( GoOn == 1 );
  /* fini */
  printf("FIN DU DECOMPTE.\n");
  return( 0 );  /* ->on n'arrive pas jusque la en pratique */
}
