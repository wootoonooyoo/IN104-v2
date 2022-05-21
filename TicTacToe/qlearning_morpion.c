#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "int_list.c" //A changer
#include "morpion.c" 

#define NB_OF_GRID 19683 //Number of different grid = 3^9

#define PLAYER 0
#define COMPUTER 1
#define RANDOM 2


void free_q_matrix(double** q_matrix) {
	for (int i=0;i<NB_OF_GRID;i++) {
		free(q_matrix[i]);
	}
	free(q_matrix);
}

/*Matrice Q qualité, chaque case correspond à une grille possible du morpion
on va utiliser une écriture en base 3 pour qu'à chaque grille corresponde
une case du tableau
Chaque case du tableau contient un tableau d'entiers de taille 9 qui
correspond aux 9 actions possibles (en général) avec à chaque case la valeur
associée à cette action*/
double** create_q_matrix() {
	double** q_matrix = malloc(NB_OF_GRID*(sizeof(double*))+1);
	if (q_matrix==NULL) {
		printf("Erreur mémoire");
		return NULL;
	}
	for (int i=0;i<NB_OF_GRID;i++) {
		q_matrix[i]=calloc(9,sizeof(double));
		if (q_matrix[i]==NULL) {
			printf("Erreur mémoire");
			free_q_matrix(q_matrix);
			return NULL;
		}
	}
	return q_matrix;
}

//create list of possible actions
struct int_list* create_list_actions() {
	struct int_list* possible_actions = malloc(sizeof(struct int_list));
	if (possible_actions==NULL) {
			printf("Erreur mémoire\n");
			return NULL;
	}
	possible_actions->val = 9;
	possible_actions->next= NULL;
	for (int i=8;i>0;i--) {
		struct int_list* new_possible_actions = malloc(sizeof(struct int_list));
		if (new_possible_actions==NULL) {
			free_int_list(possible_actions);
			printf("Erreur mémoire\n");
			return NULL;
		}
		new_possible_actions->val = i;
		new_possible_actions->next = possible_actions;
		possible_actions = new_possible_actions;
	}
	return possible_actions;
}


//max quality
double max_quality(double* quality_tab, struct int_list* possible_actions) {
	double max = quality_tab[possible_actions->val-1];
	while (possible_actions!=NULL) {
		if (quality_tab[possible_actions->val -1]>max) max=quality_tab[possible_actions->val -1];
		possible_actions = possible_actions->next;
	}
	return max;
}


/*Convertie une grille de morpion en entier, on une utilise une représentation
en base 3 de la grille de tel sorte que
X.O
...
... soit représenté par (102000000) et on convertie en entier*/
int grid_to_int(char** grid) {
	int int_of_grid=0;
	int mult=1;
	int val;
	for (int i=2;i>=0;i--) {
		for (int j=2;j>=0;j--) {
			switch(grid[i][j]) {
                case 'X':
                    val=1;
                    break;
                case 'O':
                	val=2;
                	break;
                case '.':
                    val=0;
                    break;
                default:
                    printf("Erreur grid_to_int");
                    break;
            }
			int_of_grid=int_of_grid + val*mult;
			mult=mult*3;
		}
	}
	return int_of_grid;
}


// Renvoie l'indice d'une case valide (dans possible_actions) contenant la valeur val, si plusieurs choix possibles renvoie un indice aléatoire
int get_i_with_val(double val, double* quality_tab, struct int_list* possible_actions) {
	int tab_choices[9];
	int nb_choices = 0;
	while (possible_actions!=NULL) {
		if (quality_tab[possible_actions->val-1]==val) {
			tab_choices[nb_choices]=possible_actions->val;
			nb_choices++;
		}
		possible_actions=possible_actions->next;
	}
	return tab_choices[rand()%nb_choices];
}

//return action following eps_greedy method and remove it from possible_actions
int eps_greedy(double eps, double* quality_tab, struct int_list** possible_actions_p,
				int nb_possible_actions) {
	//Exploration
	if ((rand() % 100000)< 100000*eps) {
		int randomNumber=rand()%nb_possible_actions;
		return del_i_indix_from_list(randomNumber, possible_actions_p);
	//Exploitation, choix aléatoire parmi ceux de qualité max
	} else {
		struct int_list* possible_actions = *possible_actions_p;
		double max_q = max_quality(quality_tab, possible_actions);
		int randomChoice=get_i_with_val(max_q, quality_tab, possible_actions);
		del_val_from_list(randomChoice, possible_actions_p);
		return randomChoice;
	}
}

void action_to_coord(int* i, int* j, int action) {
	*i = (action-1)/3;
	*j = (action-1)%3;
}

//Reward selon une backpropagation, si victoire alors bon reward sinon mauvais reward qu'on fait propager en arrière avec un coefficient
double reward_function(bool game_over, int nb_coup) {
	//Si égalité
	if (!game_over && nb_coup==9) return 0;
	if (nb_coup%2 == 1) return 100;
	return -100;

}

/*
On suppose qu'on apprend que pour le joueur 1 (qui joue en premier)

1) On crée une grille de morpion vierge (A FREE A LA FIN)
2) On crée la Q-table, un élément de la Q-table correspond à une grille du morpion, et lui est attribuée une quality_tab qui nous donne le meilleur coup
	à jouer(A FREE A LA FIN)
======BOUCLE D'APPRENTISSAGE===============
3) On reset la grille du morpion
4) On joue une partie de morpion contre "joueur réel"/"aléatoire"/"ordinateur apprenant pour joueur 2"
	- On crée une pile (ou simplement une liste chaînée) contenant toutes les grilles du joueur 1 au fil du jeu (A FREE A LA FIN) avec l'action choisie
	- On crée la liste chaînée de toutes les actions possibles (A FREE A LA FIN)
	Tant qu'il n'y a pas de gagnant et qu'il y a encore des actions possibles
		- On effectue une action selon eps-greedy, on retire l'action de la liste chaînée, on ajoute la grille de morpion à la pile et
			l'action choisie, et on met à jour la grille
		- Vérifie si il a gagné ou fin (si oui break)
		- Joueur 2 joue selon sa méthode, on retire l'action de la liste chaînée et on met à jour la grille
		- Vérifie si il a gagné
	- On free la liste de toutes les actions possibles
5) On regarde qui a gagné pour déterminer le reward, si joueur 1 gagne --> bon reward, si égalité reward moyen (mieux vaut 
	faire égalité que perdre), si joueur 2 gagne mauvais reward
6) On rétropropage en arrière le reward dans la pile selon la méthode d'apprentissage, la rétropropagation est telle qu'à chaque étape la valeur du reward
	est diminuée (divisé par 2 ?)
========FIN BOUCLE=========================



7) On free la grille de morpion, on free la Q-table
*/


//Obtenir une action en fonction du joueur, PLAYER(0) pour joueur réel et COMPUTER(1) pour ordinateur, autre cas impossible normalement
int get_action(int player_type, struct int_list* possible_actions, double* quality_tab, int player_turn, int nb_possible_actions) {
	if (player_turn<0 || player_turn>2) {
		printf("Erreur player_turn\n");
		return -1000;
	}
	int action;
	double max_q;
	double* q_tab;
	switch (player_type) {
		case PLAYER :
			do {
				printf("Les actions possibles sont ");
				print_int_list(possible_actions);
				printf("Choix action ?\n");
				scanf("%d", &action);
				if (!is_in_list(action,possible_actions)) printf("Erreur choix action\n");
			} while (!is_in_list(action,possible_actions));
			break;
		case COMPUTER :
			q_tab=quality_tab;
			max_q=max_quality(q_tab, possible_actions);
			/*for (int i=0;i<9;i++) {
				printf("Case %d est %lf\n", i+1, q_tab[i]);
			}
			printf("Le max est %lf\n", max_q);*/
			action=get_i_with_val(max_q, q_tab, possible_actions);
			break;
		case RANDOM :
			for (int i=0;i<rand()%nb_possible_actions;i++) {
				possible_actions = possible_actions->next;
			}
			action = possible_actions->val;
			break;
		default :
			printf("Erreur type joueur\n");
			action = -1000;
			break;
	}
	return action;
}

//renvoie l'action jouée
int play(char** grid, double eps, double** q_matrix,
			struct int_list** p_possible_actions, struct int_list** p_grid_history, int* p_nb_possible_actions,
			int player, bool is_player_learning, int player_turn ) {
	int i, j, action;
	int int_of_grid = grid_to_int(grid);
	int nb_possible_actions = *p_nb_possible_actions;
	struct int_list* possible_actions = *p_possible_actions;
	switch (player) {
		case COMPUTER : {
			if (is_player_learning) {
				action = eps_greedy(eps, q_matrix[int_of_grid], p_possible_actions, nb_possible_actions);
			} else {
				action=get_action(player, possible_actions, q_matrix[int_of_grid], player_turn, nb_possible_actions);
				del_val_from_list(action, p_possible_actions);
			}
			break;
		}
		case RANDOM : {
			action = del_i_indix_from_list(rand()%nb_possible_actions, p_possible_actions);
			break;
		}
		case PLAYER : {
			action=get_action(player, possible_actions, q_matrix[int_of_grid], player_turn, nb_possible_actions);
			del_val_from_list(action, p_possible_actions);
			break;
		}
		default : {
			printf("Erreur player_type\n");
			break;
		}
	}
	add_head_list(action, p_grid_history);
	add_head_list(int_of_grid, p_grid_history);
	action_to_coord(&i, &j, action);
	if (player_turn==1) {
		place_on_grid('X', grid, i, j);
	} else {
		place_on_grid('O', grid, i, j);
	}
	(*p_nb_possible_actions)--;
	return action;
}

void learning(double alpha, double gamma, double eps, double** q_matrix_1, double** q_matrix_2, 
				int nb_loop, int player_1, int player_2, bool is_player_1_learning, bool is_player_2_learning) {
	char** grid = create_grid();
	for (int m=0; m<nb_loop;m++) {
		reset_grid(grid);
		struct int_list* possible_actions = create_list_actions();
		int nb_possible_actions = 9;
//history of grid and action every turn of player 1
/* Pour ne pas avoir à créer une autre structure stockant 2 entiers à chaque liste, la liste sera faite de sorte que l'élément
2i correspond à la grille et l'élément 2i+1 à l'action choisie dans cette grille
*/
		struct int_list* grid_history = NULL;
		int int_of_grid, action;
		while (true) {
			//JOUEUR 1
			play(grid, eps, q_matrix_1, &possible_actions, &grid_history, &nb_possible_actions, player_1, is_player_1_learning, 1);
			if (is_game_over(grid) || nb_possible_actions==0) break;

			//JOUEUR 2 JOUE ALEATOIREMENT
			play(grid, eps, q_matrix_2, &possible_actions, &grid_history, &nb_possible_actions, player_2, is_player_2_learning, 2);
			if (is_game_over(grid)) break;
		}
		
		//APPRENTISSAGE
		double reward_1 = reward_function(is_game_over(grid), 9-nb_possible_actions);
		double reward_2 = -reward_1;
		int old_int_of_grid;
		int_of_grid=grid_to_int(grid);
		while(grid_history!=NULL) {
			old_int_of_grid = int_of_grid;
			int_of_grid = del_i_indix_from_list(0, &grid_history);
			action = del_i_indix_from_list(0, &grid_history);
			if (possible_actions==NULL) {
				add_head_list(action, &possible_actions);
				continue;
			}
			if (player_1==COMPUTER && is_player_1_learning==true) {
				double max_q=max_quality(q_matrix_1[old_int_of_grid], possible_actions);
				q_matrix_1[int_of_grid][action-1]= q_matrix_1[int_of_grid][action-1] + alpha * (reward_1 + gamma*max_q - q_matrix_1[int_of_grid][action-1]);
				add_head_list(action, &possible_actions);
				reward_1 = reward_1/2;
			}
			if (player_2==COMPUTER && is_player_2_learning==true) {
				double max_q=max_quality(q_matrix_2[old_int_of_grid], possible_actions);
				q_matrix_2[int_of_grid][action-1]= q_matrix_2[int_of_grid][action-1] + alpha * (reward_2 + gamma*max_q - q_matrix_2[int_of_grid][action-1]);
				add_head_list(action, &possible_actions);
				reward_2 = reward_2/2;
			}
		}
		free_int_list(grid_history);
		free_int_list(possible_actions);
	}
	free_grid(grid);
}

void verif_learning(double** q_matrix_1, double** q_matrix_2) {
	char** grid = create_grid();
	printf("Les choix d'actions pendant les parties sont sous la forme\n");
	for (int i = 2; i >=0; i--)
	{
		for (int j = 0; j < 3; j++)
		{
			printf("%d ", 3*i + j +1);
		}
		printf("\n");
	}
	reset_grid(grid);
	struct int_list* possible_actions=create_list_actions();
	int action, player_1, player_2;
	int nb_possible_actions = 9;
	struct int_list* grid_history = NULL;
	do {
		printf("Donner type du joueur 1 (0 pour joueur, 1 pour ordinateur, 2 aléatoire) :\n");
		scanf("%d", &player_1);
		if (player_1!=PLAYER && player_1!=COMPUTER && player_1!=RANDOM) printf("Erreur type joueur\n");
	} while (player_1!=PLAYER && player_1!=COMPUTER && player_1 != RANDOM);
	do {
		printf("Donner type du joueur 2 (0 pour joueur et 1 pour ordinateur, 2 aléatoire) :\n");
		scanf("%d", &player_2);
		if (player_2!=PLAYER && player_2!=COMPUTER && player_2 != RANDOM) printf("Erreur type joueur\n");
	} while (player_2!=PLAYER && player_2!=COMPUTER && player_2 != RANDOM);
	while(true) {
		//JOUEUR 1
		printf("=============Tour : Joueur 1=============\n");
		print_grid(grid);
		action=play(grid, 0, q_matrix_1, &possible_actions, &grid_history, &nb_possible_actions, player_1, false, 1);
		printf("Joueur 1 joue %d\n\n",action);
		if (is_game_over(grid) || nb_possible_actions==0) break;

		//JOUEUR 2 
		printf("=============Tour : joueur 2=============\n");
		print_grid(grid);
		action=play(grid, 0, q_matrix_2, &possible_actions, &grid_history, &nb_possible_actions, player_2, false, 2);
		printf("Joueur 2 joue %d\n\n",action);
		if (is_game_over(grid)) break;
	}
	printf("Fin de partie\n");
	print_grid(grid);
	if (!is_game_over(grid) && nb_possible_actions==0) {
		printf("Egalité\n");
	} else if (nb_possible_actions%2==0) {
		printf("Joueur 1 gagne\n");
	} else {
		printf("Joueur 2 gagne\n");
	}
	printf("\n\n");
	free_int_list(grid_history);
	free_grid(grid);
}

void stat(double** q_matrix_1, double** q_matrix_2, 
			int player_1, int player_2, int nb_parties, int* nb_victoire_joueur_1, int* nb_victoire_joueur_2, int* nb_egalite) {
	char** grid = create_grid();
	*nb_egalite = 0;
	*nb_victoire_joueur_1 = 0;
	*nb_victoire_joueur_2 = 0;
	for (int k=0; k<nb_parties; k++) {
		struct int_list* grid_history = NULL;
		struct int_list* possible_actions=create_list_actions();
		int nb_possible_actions = 9;
		reset_grid(grid);
		while(true) {
			//JOUEUR 1
			play(grid, 0, q_matrix_1, &possible_actions, &grid_history, &nb_possible_actions, player_1, false, 1);
			if (is_game_over(grid) || nb_possible_actions==0) break;

			//JOUEUR 2 
			play(grid, 0, q_matrix_2, &possible_actions, &grid_history, &nb_possible_actions, player_2, false, 2);
			if (is_game_over(grid)) break;
		}
		if (!is_game_over(grid) && nb_possible_actions==0) {
			(*nb_egalite)++;
		} else if (nb_possible_actions%2==0) {
			(*nb_victoire_joueur_1)++;
		} else {
			(*nb_victoire_joueur_2)++;
		}
		free_int_list(grid_history);
	}
	free_grid(grid);
}

void learning_until_perfect(double alpha, double gamma, double eps, double** q_matrix_1, double** q_matrix_2) {
	char** grid = create_grid();
	int arret = 0;
	int player_1 = COMPUTER;
	int player_2 = COMPUTER;
	int nb_loop = 10000;
	bool is_player_1_learning = true;
	bool is_player_2_learning = true;
	time_t begin = time( NULL );
	while (!arret) {
		learning(alpha, gamma, eps, q_matrix_1, q_matrix_2, nb_loop, player_1, player_2, is_player_1_learning, is_player_2_learning);
		for (int k=0; k<100000; k++) {
			struct int_list* grid_history = NULL;
			struct int_list* possible_actions=create_list_actions();
			int nb_possible_actions = 9;
			reset_grid(grid);
			while(true) {
				//JOUEUR 1
				play(grid, 0, q_matrix_1, &possible_actions, &grid_history, &nb_possible_actions, player_1, false, 1);
				if (is_game_over(grid) || nb_possible_actions==0) break;

				//JOUEUR 2 
				play(grid, 0, q_matrix_2, &possible_actions, &grid_history, &nb_possible_actions, RANDOM, false, 2);
				if (is_game_over(grid)) break;
			}
			if (nb_possible_actions%2==1) {
				arret=true;
				break;
			}
			free_int_list(grid_history);
		}
		arret = !arret;
	}
	printf("Temps écoulé pour apprentissage du joueur 1: %lf \n",difftime( time(NULL), begin ));
	printf("Voulez vous continuer l'apprentissage du joueur 2 ? 0 si oui (Ne fonctionne pas et s'arrêtera au bout de 30 sec)\n");
	scanf("%d", &arret);
	is_player_1_learning = false;
	begin = time(NULL);
	while (!arret && difftime( time(NULL), begin ) <30) {
		learning(alpha, gamma, eps, q_matrix_1, q_matrix_2, nb_loop, RANDOM, player_2, false, is_player_2_learning);
		for (int k=0; k<100000; k++) {
			struct int_list* grid_history = NULL;
			struct int_list* possible_actions=create_list_actions();
			int nb_possible_actions = 9;
			reset_grid(grid);
			while(true) {
				//JOUEUR 1
				play(grid, 0, q_matrix_1, &possible_actions, &grid_history, &nb_possible_actions, RANDOM, false, 1);
				if (is_game_over(grid) || nb_possible_actions==0) break;

				//JOUEUR 2 
				play(grid, 0, q_matrix_2, &possible_actions, &grid_history, &nb_possible_actions, player_2, false, 2);
				if (is_game_over(grid)) break;
			}
			if (is_game_over(grid) && nb_possible_actions%2==0) {
				arret=true;
				break;
			}
			free_int_list(grid_history);
		}
		arret = !arret;
	}
	free_grid(grid);
}

int main() {
	srand( time( NULL ) );

	double alpha = 0.1; // learning rate
    double gamma = 0.7; // discount factor
    double eps = 0.9; // parameter to determine exploitation/exploration


	double** q_matrix_1 = create_q_matrix();
	double** q_matrix_2 = create_q_matrix();
	
	int arret=1;
	while (arret!=0) {
		printf("Choix : \n0.Arrêt \n1.Apprentissage \n2.Apprentissage jusqu'à que ce soit parfait \n3.Jeu \n4.Stats\n");
		scanf("%d",&arret);
		switch (arret) {
			case 0 :
				break;
			case 1 : {
				time_t begin = time( NULL );
				bool is_player_1_learning = false;
				bool is_player_2_learning = false;
				unsigned int nb_loop;
				printf("Combien de boucles pour apprentissage ? (Conseil : Au moins 10 000)\n");
				scanf("%d", &nb_loop);
				int player_1, player_2;
				do {
					printf("Donner type du joueur 1 (1 pour ordinateur, 2 aléatoire) :\n");
					scanf("%d", &player_1);
					if (player_1!=COMPUTER && player_1!=RANDOM) printf("Erreur type joueur\n");
					if (player_1==COMPUTER) is_player_1_learning = true;
				} while (player_1!=COMPUTER && player_1 != RANDOM);
				do {
					printf("Donner type du joueur 2 (1 pour ordinateur, 2 aléatoire) :\n");
					scanf("%d", &player_2);
					if (player_2!=COMPUTER && player_2 != RANDOM) printf("Erreur type joueur\n");
					if (player_2==COMPUTER) is_player_2_learning = true;
				} while (player_2!=COMPUTER && player_2 != RANDOM);
				learning(alpha, gamma, eps, q_matrix_1, q_matrix_2, nb_loop, player_1, player_2, is_player_1_learning, is_player_2_learning);
				printf("Temps écoulé pour %d boucles : %lf \n", nb_loop, difftime( time(NULL), begin ));
				break;
			}
			case 2 :
				learning_until_perfect(alpha, gamma, eps, q_matrix_1, q_matrix_2);
				break;
			case 3 :
				verif_learning(q_matrix_1, q_matrix_2);
				break;
			case 4 : {
				int player_1, player_2, nb_victoire_joueur_1, nb_victoire_joueur_2, nb_egalite;
				unsigned int nb_parties;
				printf("Combien de parties jouer ? \n");
				scanf("%d", &nb_parties);
				do {
					printf("Donner type du joueur 1 (1 pour ordinateur, 2 aléatoire) :\n");
					scanf("%d", &player_1);
					if (player_1!=COMPUTER && player_1!=RANDOM) printf("Erreur type joueur\n");
				} while (player_1!=COMPUTER && player_1 != RANDOM);
				do {
					printf("Donner type du joueur 2 (1 pour ordinateur, 2 aléatoire) :\n");
					scanf("%d", &player_2);
					if (player_2!=COMPUTER && player_2 != RANDOM) printf("Erreur type joueur\n");
				} while (player_2!=COMPUTER && player_2 != RANDOM);
				stat(q_matrix_1, q_matrix_2, player_1, player_2, nb_parties, &nb_victoire_joueur_1, &nb_victoire_joueur_2, &nb_egalite);
				printf("Nombre de parties : %d\nNombre de victoires du joueur 1 : %d\nNombre de victoires du joueur 2 : %d\nNombre d'égalités : %d\n", nb_parties, nb_victoire_joueur_1, nb_victoire_joueur_2, nb_egalite);
				break;
			}
			default :
				printf("Erreur choix\n");
				break;
		}
		
	}

//===================FREE=========================
	free_q_matrix(q_matrix_1);
	free_q_matrix(q_matrix_2);

}
