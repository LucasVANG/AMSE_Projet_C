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
int nbPrint=0;

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
    }
    testCoord.x=oldCoord.x+sin(testCoord.theta)*dt*testVit.v;
    testCoord.y=oldCoord.y+cos(testCoord.theta)*dt*testVit.v;
    *szInStr2=testCoord;

    oldCoord=testCoord;


    
    /* affichage */
    printf("v=%f,w=%f\n",testVit.v,testVit.w);
    printf("x=%f,y=%f,theta=%f",szInStr2->x,szInStr2->y,szInStr2->theta);
      if (nbCoord==100){
        fprintf(fp, "x=%f,y=%f,theta=%f;\n",szInStr2->x,szInStr2->y,szInStr2->theta);
        nbPrint++;
        nbCoord=0;

      }
      nbCoord++;
      if(nbPrint>10){
        fclose(fp);

      }
  

   



}
int main( int argc, char *argv[])
{
  struct sigaction      sa,      /* ->configuration de la gestion de l'alarme */
                        sa_old;  /* ->ancienne config de gestion d'alarme     */
  sigset_t              blocked; /* ->liste des signaux bloques               */
  struct itimerval      period;  /* ->periode de l'alarme cyclique            */
  /*verif arguments*/


  /* initialisation */

  sigemptyset( &blocked );
  memset( &sa, 0, sizeof( sigaction )); /* ->precaution utile... */
  sa.sa_handler = cycl_alm_handler;
  sa.sa_flags   = 0;
  sa.sa_mask    = blocked;
  /* installation du gestionnaire de signal */
  sigaction(SIGALRM, &sa, NULL );
  /* initialisation de l'alarme  */
  
 

    
  period.it_interval.tv_sec  = 0 ;
  period.it_interval.tv_usec = dt*1000000;
  period.it_value.tv_sec     = 1;
  period.it_value.tv_usec    = 0;
  /* demarrage de l'alarme */
  setitimer( ITIMER_REAL, &period, NULL );
  /* on ne fait desormais plus rien d'autre que */
  /* d'attendre les signaux                     */
    void *vAddr;                    /* ->adresse virtuelle sur la zone          */
    void *vAddr2;                
    int  iShmFd;                    /* ->descripteur associe a la zone partagee */
    int  iShmFd2;
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

    // ecriture coord
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
    szInStr2 = (Coordonnees *)(vAddr2);

    
    szInStr = (vitesse *)(vAddr);
    fp = fopen("Output.txt", "w");
  do
  {
    
    pause();
  }
  while( GoOn == 1 );
  /* fini */
  printf("FIN DU DECOMPTE.\n");
  return( 0 );
}