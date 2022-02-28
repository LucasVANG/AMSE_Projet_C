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
#define AREA_NAME1       "TVR"    /* ->nom de la zone partagee                 */
#define AREA_NAME2      "TVL"
#define STOP            "A"      /* ->chaine a saisir pour declencher l'arret */
#define STR_LEN         256         /* ->taille par defaut des chaines           */


double *szInStr1;                  /* ->chaine saisie                          */
double *szInStr2;
float valeurstab[]={1,1,1,1,1,1,1,1,1,1,1,1,1};
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
    
    /* saisie */
    
        *szInStr1 = valeurstab[test];
        printf("Valeur inscrite dans la mémoire=%.2f\n",*szInStr1);
        printf("indice dans le tableau de valeur test=%i\n", test);
        *szInStr2 = valeurstab[test];
        printf("Valeur inscrite dans la mémoire=%.2f\n",*szInStr2);
        printf("indice dans le tableau de valeur test=%i\n", test)+1;
        test++;
        test=test%13;

}


/*#####################*/
/* programme principal */
/*#####################*/
int main( int argc, char *argv[])
{
  void *vAddr1;                    /* ->adresse virtuelle sur la zone          */
  void *vAddr2;
  int  iShmFd1;                    /* ->descripteur associe a la zone partagee */
  int iShmFd2;




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
    szInStr1 = (double *)(vAddr1);

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
    szInStr2 = (double *)(vAddr2);




  struct sigaction      sa,      /* ->configuration de la gestion de l'alarme */
                        sa_old;  /* ->ancienne config de gestion d'alarme     */
  sigset_t              blocked; /* ->liste des signaux bloques               */
  struct itimerval      period;  /* ->periode de l'alarme cyclique            */
  /* verification des arguments */
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
    // if( strcmp(szInStr1,STOP) == 0 )
    //   {
    //       printf("FIN zone 1\n");
    //       shm_unlink(AREA_NAME1);
    //   }
    // if( strcmp(szInStr1,STOP) == 0 )
    //   {
    //       printf("FIN zone 1\n");
    //         shm_unlink(AREA_NAME1);
    //     };
  }
  while( GoOn == 1 );
  /* fini */
  printf("FIN DU DECOMPTE.\n");
  return( 0 );  /* ->on n'arrive pas jusque la en pratique */
}
