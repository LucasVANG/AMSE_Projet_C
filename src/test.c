/*=================================================*/
/* programme d'utilisation des "alarmes cycliques" */
/*-------------------------------------------------*/
/* Jacques BOONAERT / cours SEMBA et AMSE          */
/*=================================================*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>          /* ->INDISPENSABLE pour les types tempo. */
/*....................*/
/* variables globales */
/*....................*/
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
void cycl_alm_handler( int signal )
{
    ss--;
    if( ss < 0 )
    {
      ss = 59;
      mm--;
      if( mm < 0 )
      {
	mm = 59;
	hh--;
      };
    };
    printf("%02d:%02d:%02d\n", hh, mm, ss );
    if( (ss == 0)&&
        (mm == 0)&&
        (hh == 0)    )
    {
      printf("TERMINE !!! \n");
      GoOn = 0; /* declenche la sortie de programme */
    };
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
  /* recuperation des arguments */
  if( (sscanf(argv[1],"%d", &hh) == 0)||
      (sscanf(argv[2],"%d", &mm) == 0)||
      (sscanf(argv[3],"%d", &ss) == 0)  )
  {
    printf("ERREUR : probleme de format des arguments\n");
    printf("         passe en ligne de commande.\n");
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
  period.it_interval.tv_sec  = 0;  
  period.it_interval.tv_usec = 100000;
  period.it_value.tv_sec     = 1;
  period.it_value.tv_usec    = 0;
  /* demarrage de l'alarme */
  setitimer( ITIMER_REAL, &period, NULL );
  /* on ne fait desormais plus rien d'autre que */
  /* d'attendre les signaux                     */
  do
  {
    printf("hello");
    pause();
  }
  while( GoOn == 1 );
  /* fini */
  printf("FIN DU DECOMPTE.\n");
  return( 0 );  /* ->on n'arrive pas jusque la en pratique */
}
