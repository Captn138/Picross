#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h> 
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

int L, C; //L is for the line and C is for the column

void ncurses_initialiser(){  //Initialisation of ncurses (given)
  initscr();	        //Starts the ncurses mode
  cbreak();	            /* Pour les saisies clavier (desac. mise en buffer) */
  noecho();             /* Désactive l'affichage des caractères saisis */
  keypad(stdscr, TRUE);	/* Active les touches spécifiques */
  refresh();            /* Met a jour l'affichage */
  curs_set(FALSE);      /* Masque le curseur */
}

void ncurses_couleurs(){  //Initialisation of the colours (given)
  /* Vérification du support de la couleur */
  if(has_colors() == FALSE) {
    endwin();
    fprintf(stderr, "Le terminal ne supporte pas les couleurs.\n");
    exit(EXIT_FAILURE);
  }

  /* Activation des couleurs */
  start_color();
  init_color(COLOR_WHITE, 1000, 1000, 1000);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_BLACK);

}

void ncurses_souris(){  //Initialisation of the mouse (given)
  if(!mousemask(ALL_MOUSE_EVENTS, NULL)) {
    endwin();
    fprintf(stderr, "Erreur lors de l'initialisation de la souris.\n");
    exit(EXIT_FAILURE);
  }

  if(has_mouse() != TRUE) {
    endwin();
    fprintf(stderr, "Aucune souris n'est détectée.\n");
    exit(EXIT_FAILURE);
  }
}

int click_souris(){  //Get the coordinates (x,y) of the clic (given)
  MEVENT event ;
  int ch;

  while((ch = getch()) != KEY_F(1)){
    switch(ch){
		case KEY_F(2): /*Pour quitter la boucle*/
			return 1;
		case KEY_MOUSE:
			if(getmouse(&event) == OK){
				C = event.x;
				L = event.y;
				if(event.bstate & BUTTON1_CLICKED){ 
					return 0;
				}
			}
	}
  }
  return 0;
}

void print_picross(){ //displays the menu
	char c;
	FILE *f;
	f=fopen("img", "r");
	while((c=fgetc(f)) != EOF){
		printf("%c", c);
	}
	fclose(f);
}

void loadArray(int nblin, int nbcol, int board[nblin][nbcol]){ //Fills the array with 0
    int k, l;
    for(k=0; k<nblin; k++){
        for(l=0; l<nbcol; l++){
            board[k][l]=0;
        }
    }
}

void randomArray(int nblin, int nbcol, int board[nblin][nbcol]){ //Fills the array with random numbers between 0 and 1
    for(int k=0; k<nblin; k++){
        for(int l=0; l<nbcol; l++){
            board[k][l]=rand()%2;
        }
    }
}

void printArray(int nblin, int nbcol, int up, int left, int uptab[up][nbcol], int lefttab[nblin][left], int user[nblin][nbcol], int board[nblin][nbcol]){ //displays the board
	int k, l, m;
	printw(" ");
	for(l=0; l<=left-1; l++){ 
		printw("  ");
	} 
	addch(ACS_ULCORNER);
	for(l=0; l<nbcol-1; l++){
		addch(ACS_HLINE);
		addch(ACS_HLINE);
		addch(ACS_TTEE);
	}
	addch(ACS_HLINE);
	addch(ACS_HLINE);
	addch(ACS_URCORNER); //from addch(ACS_ULCORNER) to addch(ACS_URCORNER) : we create the first line of the board with the upper-left corner
	printw("\n");
	for(k=0; k<up; k++){
		printw(" ");
		for(l=0; l<=left-1; l++){
			printw("  ");
		} // from the first "for" to here : creation of the the empty part of the board (the upper-left one)
		for(l=0; l<nbcol; l++){
			addch(ACS_VLINE); //we trace the columns (the vertical lines)
			m=uptab[k][l]; //this is the part of the board that gives us the number of good cases per column
			if(m==0){
				printw("  "); //if this is a 0, it won't be displayed
			}else{
				printw("%2d", m); //else, we print it, using 2 spaces.
			}
		}
		addch(ACS_VLINE); //we trace the last vertical line
		printw("\n");
	}
	addch(ACS_ULCORNER); //we trace the upper-left corner
	for(k=0; k<left; k++){
		addch(ACS_HLINE);
		addch(ACS_HLINE);
	} //we trace the horizontal lines of the the left part of the array
	addch(ACS_PLUS);
	for(k=0; k<nbcol-1; k++){
		addch(ACS_HLINE);
		addch(ACS_HLINE);
		addch(ACS_PLUS);
	} 
	addch(ACS_HLINE);
	addch(ACS_HLINE);
	addch(ACS_RTEE);
	printw("\n");
	for(k=0; k<nblin-1; k++){
		addch(ACS_VLINE);
		for(l=0; l<left; l++){
			m=lefttab[k][l]; //this is the part of the board that gives us the number of good cases per lines
			if(m==0){
				printw("  "); //if this is a 0, it won't be displayed
			}else{
				printw("%2d", m); //else, we'll display this number with 2 spaces
			}
		}
		addch(ACS_VLINE); //we display the vertical lines of the board
		for(l=0; l<nbcol; l++){
			m=user[k][l]; //this is the "playable" part of the board
			if(m==0){
				printw("  "); //if the case is equal to 0, the case is empty
			}else if(m==1){
				addch(ACS_CKBOARD);
				addch(ACS_CKBOARD); //if the case is equal to 1, a small stripple will appear
			}else if(m==2){
				printw("><"); //if the case is equal to 2, a cross will appear
			}
			addch(ACS_VLINE);
		}
		printw("\n");
		addch(ACS_LTEE); //we add a tee pointing right
		for(l=0; l<left; l++){
			addch(ACS_HLINE);
			addch(ACS_HLINE); //we display the lines of the left array
		}
		addch(ACS_PLUS);
		for(l=0; l<nbcol-1; l++){
			addch(ACS_HLINE);
			addch(ACS_HLINE); //we display the lines of the user's array
			addch(ACS_PLUS); //at each line, we add a little + to separate the cases
		}
		addch(ACS_HLINE);
		addch(ACS_HLINE); //we display all the horizontal lines
		addch(ACS_RTEE); //we add a a tee pointing left
		printw("\n");
	}
	addch(ACS_VLINE);
	for(k=0; k<left; k++){
		m=lefttab[nblin-1][k];
		if(m==0){
			printw("  ");
		}else{
			printw("%2d", m);
		} //this is the last row of the left-tab (the one that shows the number of cases per lines )
	}
	addch(ACS_VLINE);
	for(k=0; k<nbcol; k++){
		m=user[nblin-1][k];
		if(m==0){
			printw("  ");
		}else if(m==1){
			addch(ACS_CKBOARD);
			addch(ACS_CKBOARD);
		}else if(m==2){
			printw("><");
		}
		addch(ACS_VLINE);
	} //this is the last row of the user's board
	printw("\n");
	addch(ACS_LLCORNER); //we add the low-left corner
	for(k=0; k<left; k++){
		addch(ACS_HLINE);
		addch(ACS_HLINE);
	}
	addch(ACS_BTEE);
	for(k=0; k<nbcol-1; k++){
		addch(ACS_HLINE);
		addch(ACS_HLINE); //we add the last horizontal lines
		addch(ACS_BTEE); //and also a tee pointing up
	}
	addch(ACS_HLINE);
	addch(ACS_HLINE);
	addch(ACS_LRCORNER); //we add the low-right corner
	printw("\n");
	
	
	//CHEATS
	printw("\n");
	for(k=0; k<nblin; k++){
		for(l=0; l<nbcol; l++){
			printw("%d", board[k][l]);
		}
		printw("\n");
	}
}

int numOfVal(int dim1, int dim2, int board[dim1][dim2]){ //this function gives the number of lines (or columns) of the left array (or the up array)
	int res=0, count=0, k, l, prev=0;
	for(k=0; k<dim1; k++){
		for(l=dim2-1; l>=0; l--){
			if(board[k][l]==1 && prev==0){
				count++;
			}
			prev=board[k][l];
		}
		if(count>res){
			res=count;
		}
		prev=0;
		count=0;
	}
	return res;
}

void loadBorders(int nblin, int nbcol, int up, int left, int uptab[up][nbcol], int lefttab[nblin][left], int board[nblin][nbcol]){ //this function calculates the number of "good" cases and displays it in the ararys (up and left)
	int k, l, x=0, prev=0;
	for(k=0; k<nbcol; k++){  //This part fills the upper-array
		for(l=nblin-1; l>=0; l--){
			if(board[l][k]==1){
				uptab[up-x-1][k]++;
			}else if(board[l][k]==0 && prev==1){
				x++;
			}
			prev=board[l][k];
		}
		x=0;
		prev=0;
	}
	for(k=0; k<nblin; k++){  //This part fills the left-array
		for(l=nbcol-1; l>=0; l--){
			if(board[k][l]==1){
				lefttab[k][left-x-1]++;
			}else if(board[k][l]==0 && prev==1){
				x++;
			}
			prev=board[k][l];
		}
		x=0;
		prev=0;
	}
}

/*void checkBorders(int nblin, int nbcol, int up, int left, int uptab[up][nbcol], int lefttab[nblin][left], int user[nblin][nbcol], int lig, int col){
//This function was set to change the numbers in the up-array or in the left-array in negative if the number of good cases was checked.
//Then, it would be printed in green in the printArray function.
//Unfortunately, this function never worked
	int k, m=1, count;
	for(k=left-1; k>=0; k--){
		if(lefttab[lig][k]!=0){
			lefttab[lig][k]=abs(lefttab[lig][k]);
			count=0;
			while(user[lig][nbcol-m]==0 || user[lig][nbcol-m]==2){
				m++;
			}
			while(user[lig][nbcol-m]==1){
				m++;
				count++;
			}
			if(count==abs(lefttab[lig][k])){
				lefttab[lig][k]=-1*abs(lefttab[lig][k]);
			}
		}
	}
	m=1;
	for(k=up-1; k>=0; k--){
		if(uptab[k][col]!=0){
			uptab[k][col]=abs(uptab[k][col]);
			count=0;
			while(user[nblin-m][col]==0 || user[nblin-m][col]==2){
				m++;
			}
			while(user[nblin-m][col]==1){
				m++;
				count++;
			}
			if(count==abs(uptab[k][col])){
				uptab[k][col]=-1*abs(uptab[k][col]);
			}
		}
	}
}*/

int checkWin(int nblin, int nbcol, int board[nblin][nbcol], int user[nblin][nbcol]){ //this function checks if the user won or not : if win=1, the player won.
	int win=1, m;
	for(int k=0; k<nblin; k++){  //To check if the user won, we compare each case of the user-array with the soluce-array
		for(int l=0; l<nbcol; l++){
			m=user[k][l];
			if(m==2){
				m=0;
			}
			if(m!=board[k][l]){
				win=0;
				return win;
			}
		}
	}
	return win;
}

void endOfGame(int nblin, int nbcol, int user[nblin][nbcol]){  //This function is used when the user wins, it prints a big "WINNER" and the shape of the game played
	int a;
	/*a=rand()%5; //Here we wanted to play a random music when the user won, but the commands that we would use prints many error messages that we couldn't erase
	if(a==0){
		system("play -q 'vic1.ogg' &");
	}else if(a==1){
		system("play -q 'vic2.ogg' &");
	}else if(a==2){
		system("play -q 'vic3.ogg' &");
	}else if(a==3){
		system("play -q 'vic4.ogg' &");
	}else if(a==4){
		system("play -q 'vic5.ogg' &");
	}*/ 			
	clear();
	printw("\n\n\n\n\t");
	printw("WWWWWWWW                           WWWWWWWW IIIIIIIIII NNNNNNNN        NNNNNNNN NNNNNNNN        NNNNNNNN EEEEEEEEEEEEEEEEEEEEEE RRRRRRRRRRRRRRRRR\n\t");
	printw("W::::::W                           W::::::W I::::::::I N:::::::N       N::::::N N:::::::N       N::::::N E::::::::::::::::::::E R::::::::::::::::R\n\t");
	printw("W::::::W                           W::::::W I::::::::I N::::::::N      N::::::N N::::::::N      N::::::N E::::::::::::::::::::E R::::::RRRRRR:::::R\n\t");
	printw("W::::::W                           W::::::W II::::::II N:::::::::N     N::::::N N:::::::::N     N::::::N EE::::::EEEEEEEEE::::E RR:::::R     R:::::R\n\t");
	printw(" W:::::W           WWWWW           W:::::W    I::::I   N::::::::::N    N::::::N N::::::::::N    N::::::N   E:::::E       EEEEEE   R::::R     R:::::R\n\t");
	printw("  W:::::W         W:::::W         W:::::W     I::::I   N:::::::::::N   N::::::N N:::::::::::N   N::::::N   E:::::E                R::::R     R:::::R\n\t");
	printw("   W:::::W       W:::::::W       W:::::W      I::::I   N:::::::N::::N  N::::::N N:::::::N::::N  N::::::N   E::::::EEEEEEEEEE      R::::RRRRRR:::::R\n\t");
	printw("    W:::::W     W:::::::::W     W:::::W       I::::I   N::::::N N::::N N::::::N N::::::N N::::N N::::::N   E:::::::::::::::E      R:::::::::::::RR\n\t");
	printw("     W:::::W   W:::::W:::::W   W:::::W        I::::I   N::::::N  N::::N:::::::N N::::::N  N::::N:::::::N   E:::::::::::::::E      R::::RRRRRR:::::R\n\t");
	printw("      W:::::W W:::::W W:::::W W:::::W         I::::I   N::::::N   N:::::::::::N N::::::N   N:::::::::::N   E::::::EEEEEEEEEE      R::::R     R:::::R\n\t");
	printw("       W:::::W:::::W   W:::::W:::::W          I::::I   N::::::N    N::::::::::N N::::::N    N::::::::::N   E:::::E                R::::R     R:::::R\n\t");
	printw("        W:::::::::W     W:::::::::W           I::::I   N::::::N     N:::::::::N N::::::N     N:::::::::N   E:::::E       EEEEEE   R::::R     R:::::R\n\t");
	printw("         W:::::::W       W:::::::W          II::::::II N::::::N      N::::::::N N::::::N      N::::::::N EE::::::EEEEEEEE:::::E RR:::::R     R:::::R\n\t");
	printw("          W:::::W         W:::::W           I::::::::I N::::::N       N:::::::N N::::::N       N:::::::N E::::::::::::::::::::E R::::::R     R:::::R\n\t");
	printw("           W:::W           W:::W            I::::::::I N::::::N        N::::::N N::::::N        N::::::N E::::::::::::::::::::E R::::::R     R:::::R\n\t");
	printw("            WWW             WWW             IIIIIIIIII NNNNNNNN         NNNNNNN NNNNNNNN         NNNNNNN EEEEEEEEEEEEEEEEEEEEEE RRRRRRRR     RRRRRRR\n\n\n");
	printw("\t\t"); //we display a great WINNER word
	addch(ACS_ULCORNER);
	for(int k=0; k<nbcol; k++){
		addch(ACS_HLINE);
		addch(ACS_HLINE);
	}
	addch(ACS_URCORNER);
	printw("\n");
	for(int k=0; k<nblin; k++){
		printw("\t\t");
		addch(ACS_VLINE);
		for(int l=0; l<nbcol; l++){
			if(user[k][l]==1){
				addch(ACS_CKBOARD);
				addch(ACS_CKBOARD);
			}else{
				printw("  ");
			}
		}
		addch(ACS_VLINE);
		printw("\n");
	}
	printw("\t\t");
	addch(ACS_LLCORNER);
	for(int k=0; k<nbcol; k++){
		addch(ACS_HLINE);
		addch(ACS_HLINE);
	}
	addch(ACS_LRCORNER); //from addch(ACS_ULCORNER) to here : we display a small version of the board that the user made
	printw("\n\n");
	printw("\t\tPress enter\n\n\n\n\n\n");
	scanw("%d", &a);  //The user can type whatever he wants, he will need to press enter to go through the scanw
	endwin();  //We end the ncurses window
}

void convert(int nblin, int nbcol, int up, int left, int user[nblin][nbcol], int *plig, int *pcol){ //This function converts the click of the mouse in a position in the array
	if(L-1-up<0 || L>2*nblin+1+up || C-1-2*left<0 || C>3*nbcol+1+2*left){ //we check that the user clicked in the board
		*plig=-1;  //If he hasn't clicked in the board, the value for the line will be -1
	}else{
		*plig=(L-1-up)/2;
		*pcol=(C-1-2*left)/3; //and then we convert the (x,y) coordinates of the screen into coordinates (x,y) of the board
	}
} 

int selectGame(int *pnbl, int *pnbc){ //for the pre-set mode : this functions asks the user to chose a drawing
	int choice=0, scan;
	printf("\n\n\t\t\tChoose your game\n\n\n");
	printf("\tMario Star (1):\n\t\t15 rows, 15 columns\n\n");
	printf("\tSpace Invader (2):\n\t\t8 rows, 11 columns\n\n");
	printf("\tChampignon (3):\n\t\t16 rows, 16 columns\n\n");
	printf("\tBatman (4):\n\t\t10 rows, 15 columns\n\n");
	do{
		scan=scanf("%d", &choice);
	}while(scan!=1 || choice<1 || choice>4);
	if(choice==1){
		*pnbl=15;
		*pnbc=15;
	}else if(choice==2){
		*pnbl=8;
		*pnbc=11;
	}else if(choice==3){
		*pnbl=16;
		*pnbc=16;
	}else if(choice==4){
		*pnbl=10;
		*pnbc=15;
	}
	return choice;
}

void LANCEMENT_JEU(int mode); //Defined here in order to be called in the next function

void game(int nblin, int nbcol, int up, int left, int uptab[up][nbcol], int lefttab[nblin][left], int board[nblin][nbcol], int user[nblin][nbcol]){ //this function runs the game
	ncurses_initialiser(); //intialization of ncurses
	ncurses_souris(); //initialization of the mouse
	ncurses_couleurs(); //initialization of the colours 
	attron(COLOR_PAIR(1)); //initialization of the colours
	clear();
	printArray(nblin, nbcol, up, left, uptab, lefttab, user, board); //the board is printed
	int win=checkWin(nblin, nbcol, board, user), lig, col=-1, *plig=&lig, *pcol=&col;
	while(win==0 && click_souris()==0){  //While the game is not won and each time the user clicks somewhere
		convert(nblin, nbcol, up, left, user, plig, pcol);  //We convert the click
		if(lig!=-1){
			user[lig][col]=(user[lig][col]+1)%3;  //We increase the value of the user-board with a modulo 3
		}
		//checkBorders(nblin, nbcol, up, left, uptab, lefttab, user, lig, col);
		clear();
		printArray(nblin, nbcol, up, left, uptab, lefttab, user, board);  //We update the screen
		win=checkWin(nblin, nbcol, board, user);  //And we check for the win
	}
	endOfGame(nblin, nbcol, user); //at the end, the "WINNER" message apperars
	system("clear");
	print_picross();//the menu dispays and the user choses his gamemode
	int mode;
	while(scanf("%d", &mode)!=1 || mode<1 || mode>3){
		printf("Invalid entry.\n");
	}
	system("clear");
	LANCEMENT_JEU(mode); //We call the function LANCEMENT_JEU again
}

void LANCEMENT_JEU(int mode){  //This function will prepare the game, whatever the game mode is
	int left, up, nblin, nbcol;
	if(mode==3){ //if the user types 3, the game will be stopped
		system("clear");
		system("setterm -cursor on");
		exit(-1);
	}else if(mode==2){ //if the user types 2, he will be able to chose one of the preset modes
		system("clear");
		FILE *f;
		int choice, *pnbl=&nblin, *pnbc=&nbcol;
		choice=selectGame(pnbl, pnbc);
		int user[nblin][nbcol], board[nblin][nbcol], var;
		loadArray(nblin, nbcol, user);
		if(choice==1){  //To fill the preset array, we use external files
			f=fopen("game1.txt", "r");
		}else if(choice==2){
			f=fopen("game2.txt", "r");
		}else if(choice==3){
			f=fopen("game3.txt", "r");
		}else{
			f=fopen("game4.txt", "r");
		}
		for(int k=0; k<nblin; k++){  //We get the values from the file
			for(int l=0; l<nbcol; l++){
				fscanf(f, "%d", &var);
				board[k][l]=var;
			}
		}
		fclose(f);
		left=numOfVal(nblin, nbcol, board);  //We prepare the others board using the functions defined first
		up=numOfVal(nbcol, nblin, board);
		int uptab[up][nbcol], lefttab[nblin][left];
		loadArray(up, nbcol, uptab);
		loadArray(nblin, left, lefttab);
		loadBorders(nblin, nbcol, up, left, uptab, lefttab, board);
		game(nblin, nbcol, up, left, uptab, lefttab, board, user);
	}else{
		printf("Select the size of your game: \"rows columns\"\n"); //if the user types 1, he will have a random mode, with the dimensions bewteen 1x1 and 15x15
	    int flag=1;
		while(flag==1){  //This loop checks if the size of the array typed by the user is not too big, too small or if he has typed something else than integers
	       		if(scanf("%d %d", &nblin, &nbcol)==2){
				if(nblin>15){
					printf("\nToo many lines.\n");
				}else if(nblin<1){
					printf("\nNot enough lines.\n");
				}else if(nbcol>15){
					printf("\nToo many columns.\n");
				}else if(nbcol<1){
					printf("\nNot enough columns.\n");
				}else{
					flag=0;
				}
			}else{
				printf("\nThe values typed are not integers.\n");
			}
		}
		int board[nblin][nbcol], user[nblin][nbcol];  //As for the preset mode, we prepare the others arrays
		randomArray(nblin, nbcol, board);
		loadArray(nblin, nbcol, user);
		left=numOfVal(nblin, nbcol, board);
		up=numOfVal(nbcol, nblin, board);
		int uptab[up][nbcol], lefttab[nblin][left];
		loadArray(up, nbcol, uptab);
		loadArray(nblin, left, lefttab);
		loadBorders(nblin, nbcol, up, left, uptab, lefttab, board);
		game(nblin, nbcol, up, left, uptab, lefttab, board, user); //then he launches the game.
	}
}

int main(){
	int mode=0, scan;
	srand(time(NULL));
	system("setterm -cursor off"); //We remove the cursor (the same command with "-cursor on" brings it back)
	system("clear"); //removing of the text in the terminal
	print_picross(); //displays the menu
	do{
		scan=scanf("%d", &mode);
	}while(scan!=1 || mode<1 || mode>3);
	system("clear");
	LANCEMENT_JEU(mode); //Lauches the game
	system("clear");
	system("setterm -cursor on");
	return 0;
}

//Projetc of ALMEIDA Mickael & DRAY Gabriel
//1A International ESIEA
//
//To compile:
//gcc -Wall projetc.c -o picross -lncurses
//
//Be sure to be in the same folder as ALL the files
