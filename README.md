 # Projet C
 
 Ce depot git contient l'ensemble du travail effectué par ce groupe composé de Fabien Plouvier, Quentin Delezenne et Lucas Vangaeveren.
 
  ## Composition
  
  Ce depot et composé de 5 fichiers.
  
  - Ce README.md
  - Le fichier CMakeList.text et compilerProjet.sh qui permet de compiler l'ensemble des .c nécessaires au bon fonctionnement du projet (voir ci-dessous)
  - Du répertoire src contenant l'ensemble des .c créé
  - Du répertoire bin contenant les versions compilés des fichiers contenus dans src
    
 ### Composition de src
 
  Ici se trouve l'ensemble des fichiers .c nécessaire:
  - DCmoteur qui permet de simulé une entrée et sortie moteur, et qui se remet à 0 lorsqu'il reçoit le signal SIGUSR2
  - Kinematics qui lit les données de sorties du moteur droit et gauche afin d'en sortir la vitesse linéaire et angulaire du robot
  - Odometry qui lit la sortie de Kinematics afin de mettre à jour l'emplacement du robot d'un repère 2D et son orientation dans celui-ci. Il inscrit à intervalle
  régulier les coordonées dans un fichier Output.csv. Il arrete ou reprend l'écriture dans ce fichier lorsqu'il reçoit le signal SIGUSR2
  - ResetState qui permet de remettre à zero les zones TV et U
  - SetTv qui permet d'imposer une valeur d'entrée aux regulateurs(./setTV L(ou R) valeur)
  - SetU qui permet d'imposer une valeur d'entrée aux regulateurs (./setU L(ou R) valeur)
  - regPID le régulateur du moteur afin que la vitesse voulue soit bien obtenue qui se remet à 0 quand il reçoit le signal SIGUSR2
  - Les fichiers concernant les ordres:
    - cmd_go qui permet d'avancer en ligne droite d'une distance donnée (./cmd_go distance dt)
    - cmd_turn qui permet de faire tourner le robot sur place (./cmd_turn angle(radians) dt)
    - cmd_goTo qui regroupe les deux précédents permettant de voyager jusqu'à un point du repère global (./cmdgoTo x y dt)

  
  ### compilerProjet
  Le fichier compilerProjet permet de compiler simplement et rapidement tout le projet.   
  *Veillez à avoir CMake installé sur votre machine.*   
  Pour l'utiliser, faites la commande :
  ```bash
  ./compilerProjet
  ```



  ### StartRobot
  StartRobot est un script bach présent dans bin avec l'ensemble des fichiers compilé qui permet de lancer l'ensemble des processus dans l'ordre voulu pour un bon fonctionnement
  
  Un fois lancé, il reste juste à utilisé cmd_go,cmd_turn, cmd_goTo pour diriger le robot simulé.
