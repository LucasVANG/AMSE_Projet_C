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
#define AREA_STATE_L   "STATEL"
#define AREA_STATE_R   "STATER"
#define AREA_NAME2      "VELOCITY"
#define STR_LEN         256         /* ->taille par defaut des chaines           */
int test=0;
int  hh,                       /* ->heures                              */
     mm,                       /* ->minutes                             */
     ss;                       /* ->secondes                            */
int  GoOn = 1;                 /* ->controle d'execution                */
double W=0;
double R=0;

/*...................*/
/* prototypes locaux */
/*...................*/
typedef struct etat_moteur{
  double i;
  double w;
}etat_moteur;

typedef struct vitesse{
    double v;
    double w;
}vitesse;

etat_moteur *mot1;
etat_moteur *mot2;
vitesse *szInStr2; 

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
   
    vitesse res;
    res.v=(mot2->w+mot1->w)*0.5*R;
    res.w=(mot2->w-mot1->w)*(1/W);
    *szInStr2=res;
    test++;
    printf("%i\n", test);

    
    /* affichage */
    printf("contenu de la zone L= i=%f\n w=%f\n", mot1->i,mot1->w);
    printf("W=%f,R=%f\n",W,R);
    printf("v=%f,w=%f\n",res.v,res.w);
    
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
  /*verif arguments*/
  if(argc!=4){
      printf("Nombre arguments invalides");
      return (0);
  }
  /* initialisation */
  W=atof(argv[1]);
  R=atof(argv[2]);
  sigemptyset( &blocked );
  memset( &sa, 0, sizeof( sigaction )); /* ->precaution utile... */
  sa.sa_handler = cycl_alm_handler;
  sa.sa_flags   = 0;
  sa.sa_mask    = blocked;
  /* installation du gestionnaire de signal */
  sigaction(SIGALRM, &sa, NULL );
  /* initialisation de l'alarme  */
  
 
  if((int)atof(argv[3])>=100){
    printf("%i",(int)atof(argv[3])/100);
    period.it_interval.tv_sec  = (int)atof(argv[3])/100;  
    period.it_interval.tv_usec = 0;
  }
  else {
    printf("%f",atof(argv[3])*10000);
    period.it_interval.tv_sec  = 0;  
    period.it_interval.tv_usec = (double)atof(argv[3])*10000;
  }
 
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
    if( (iShmFd = shm_open(AREA_STATE_L, O_RDWR, 0600)) < 0)
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
    mot1 = (etat_moteur *)(vAddr);



        if( (iShmFd = shm_open(AREA_STATE_R, O_RDWR, 0600)) < 0)
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
    mot2 = (etat_moteur *)(vAddr);


    // ecriture vitesse
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
    szInStr2 = (vitesse *)(vAddr2);
    
  do
  {
    
    pause();
  }
  while( GoOn == 1 );
  /* fini */
  printf("FIN DU DECOMPTE.\n");
  return( 0 );
}