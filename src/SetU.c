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
#define AREA_NAME       "UR"    /* ->nom de la zone partagee                 */
#define AREA_NAME2      "UL"
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


double *szInStr;


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
    
    
}


/*#####################*/
/* programme principal */
/*#####################*/
int main( int argc, char *argv[])
{

  if(argc!=3){
      printf("Nombre arguments invalides");
      return (0);
  }

  /* initialisation de l'alarme  */
  

  /* on ne fait desormais plus rien d'autre que */
  /* d'attendre les signaux                     */
    void *vAddr;                    /* ->adresse virtuelle sur la zone          */          
    int  iShmFd;                    /* ->descripteur associe a la zone partagee */
    /*..................................*/
    /* tentative d'acces a la zone */
    /*..................................*/
    /* on essaie de se lier sans creer... */
    if(strcmp("L",argv[2])==0){
        if( (iShmFd = shm_open(AREA_NAME2, O_RDWR | O_CREAT, 0600)) < 0)
        {
            /* on essaie de se lier sans creer... */
            printf("echec de creation, lien seul...\n");
            if( (iShmFd = shm_open(AREA_NAME2, O_RDWR, STR_LEN)) < 0)
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

    }
    else {
        if( (iShmFd = shm_open(AREA_NAME, O_RDWR | O_CREAT, 0600)) < 0)
        {
            /* on essaie de se lier sans creer... */
            printf("echec de creation, lien seul...\n");
            if( (iShmFd = shm_open(AREA_NAME, O_RDWR, STR_LEN)) < 0)
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

    }

  
    szInStr = (double *)(vAddr);
    *szInStr=atof(argv[1]);
    printf("%f",*szInStr);




  return( 0 );
}