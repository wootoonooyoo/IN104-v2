#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "morpion.c" //A changer

#define NB_OF_GRID 19683 //Number of different grid = 3^9


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
	double** q_matrix = malloc(NB_OF_GRID*(sizeof(double*)));
	if (q_matrix==NULL) {
		printf("Mem error");
		return NULL;
	}
	for (int i=0;i<NB_OF_GRID;i++) {
		q_matrix[i]=calloc(9,sizeof(double));
		if (q_matrix[i]==NULL) {
			printf("Mem error");
			free_q_matrix(q_matrix);
			return NULL;
		}
	}
	return q_matrix;
}

//int list
struct int_list{
	int val;
	struct int_list* next;
};

void free_int_list(struct int_list* list) {
	struct int_list* l = list;
	while (list!=NULL) {
		l=list->next;
		free(list);
		list = l;
	}
}

//add elt at the head of the list
void add_head_list(int elt, struct int_list** list) {
	struct int_list* new_list = malloc(sizeof(struct int_list));
	if (new_list==NULL) {
		printf("Mem error\n");
		return;
	}
	new_list->val = elt;
	new_list->next = *list;
	*list = new_list;
}

//create list of possible actions
struct int_list* create_list_actions() {
	struct int_list* possible_actions = malloc(sizeof(struct int_list));
	if (possible_actions==NULL) {
			printf("Mem error\n");
			return NULL;
	}
	possible_actions->val = 9;
	possible_actions->next= NULL;
	for (int i=8;i>0;i--) {
		struct int_list* new_possible_actions = malloc(sizeof(struct int_list));
		if (new_possible_actions==NULL) {
			free_int_list(possible_actions);
			printf("Mem error\n");
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
	double max = quality_tab[possible_actions->val];
	while (possible_actions!=NULL) {
		if (quality_tab[possible_actions->val]>max) max=quality_tab[possible_actions->val];
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
                    printf("Error grid to int");
                    break;
            }
			int_of_grid=int_of_grid + val*mult;
			mult=mult*3;
		}
	}
	return int_of_grid;
}

//remove possible_actions[i] and return its value, i<nb_possible_actions
int del_i_indix_from_list(int i, struct int_list** possible_actions_p) {
	if (i==0) {
		struct int_list* frst = *possible_actions_p;
		int val = frst->val;
		*possible_actions_p=(*possible_actions_p)->next;
		free(frst);
		return val;
	}
	struct int_list* possible_actions = *possible_actions_p;
	for (int j=0;j<i-1;j++) {
		//printf("Marche\n");
		possible_actions=possible_actions->next;
	}
	//printf("Fin for\n");
	int val = (possible_actions->next)->val;
	struct int_list* tmp = possible_actions->next;
	possible_actions->next = (possible_actions->next)->next;
	free(tmp);
	return val;
}

//remove val from possible_actions, val is supposed in possible_actions
void del_val_from_list(int val, struct int_list** possible_actions_p) {
	if (val==(*possible_actions_p)->val) {
		struct int_list* frst = *possible_actions_p;
		*possible_actions_p=(*possible_actions_p)->next;
		free(frst);
	} else {
		struct int_list* possible_actions = *possible_actions_p;
		while ((possible_actions->next)->val != val) {
			possible_actions=possible_actions->next;
		}
		struct int_list* tmp = possible_actions->next;
		possible_actions->next = (possible_actions->next)->next;
		free(tmp);
	}
}

//return action following eps_greedy method and remove it from possible_actions
/* CORRIGER COMME SHAWN
if(randomNumber < 100000*epsilon*pow(0.7,loopNo))
Plus l'algorithme tourne et moins on essaie de faire de l'exploration
mais plus de la perfection ?
*/
int eps_greedy(double eps, double* quality_tab, struct int_list** possible_actions_p,
				int nb_possible_actions) {
	//Exploration
	if ((rand() % 100000)< 100000*eps) {
		int randomNumber=rand()%nb_possible_actions;
		return del_i_indix_from_list(randomNumber, possible_actions_p);
	//Exploitation, random choice among those with highest quality
	} else {
		struct int_list* possible_actions = *possible_actions_p;
		double max_q = max_quality(quality_tab, possible_actions);
		int tab_choices[9];
		int nb_choices = 0;
		while (possible_actions!=NULL) {
			if (quality_tab[possible_actions->val]==max_q) {
				tab_choices[nb_choices]=possible_actions->val;
				nb_choices++;
			}
			possible_actions=possible_actions->next;
		}
		int randomChoice=tab_choices[rand()%nb_choices];
		del_val_from_list(randomChoice, possible_actions_p);
		return randomChoice;
	}
}

void action_to_coord(int* i, int* j, int action) {
	*i = (action-1)/3;
	*j = (action-1)%3;
}

//Corriger reward_function() de tel sorte que si P1 joue un coup et que P2 puisse gagner au prochain coup alors mauvais rewar
//Reward selon une backpropagation, si victoire alors bon reward sinon mauvais reward qu'on fait propager en arrière avec un coefficient
double reward_function(int nb_coup) {
	//Si égalité
	if (nb_coup==9) return 5;
	if (nb_coup%2 == 1) return 100;
	return -0.5;

}

void print_int_list(struct int_list* l) {
	while (l!=NULL) {
		printf("%d ", l->val);
		l = l->next;
	}
	printf("\n");
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



X) On free la grille de morpion, on free la Q-table
*/

int main() {
	time_t begin = time( NULL );
	srand( time( NULL ) );


	char** grid = create_grid();
	double** q_matrix = create_q_matrix();

//==========BOUCLE D'APPRENTISSAGE================ CONTRE JOUEUR ALEATOIRE, A RAJOUTER LA PARTIE POUR RETROP DU REWARD
	
	double alpha = 0.1; // learning rate
    double gamma = 0.7; // discount factor
    double eps = 0.9; // parameter to determine exploitation/exploration
	int nb_loop = 10000;
	for (int m=0; m<nb_loop;m++) {
		reset_grid(grid);
		struct int_list* possible_actions = create_list_actions();
		//					print_int_list(possible_actions);
		int nb_possible_actions = 9;
		//					print_grid(grid);
		//history of grid and action every turn of player 1
		/* Pour ne pas avoir à créer une autre structure stockant 2 entiers à chaque liste, la liste sera faite de sorte que l'élément
		2i correspond à la grille et l'élément 2i+1 à l'action choisie dans cette grille
		*/
		struct int_list* grid_history = NULL;

		while (true) {
			//JOUEUR 1
			int int_of_grid = grid_to_int(grid);
			double* quality_tab = q_matrix[int_of_grid];
			int action = eps_greedy(eps, quality_tab, &possible_actions, nb_possible_actions);
			add_head_list(action, &grid_history);
			add_head_list(int_of_grid, &grid_history);
			//				printf("Joueur 1 action %d\n", action);
			int i,j;
			action_to_coord(&i, &j, action);
			place_on_grid('X', grid, i, j);
			nb_possible_actions--;
			//				print_grid(grid);
			if (is_game_over(grid) || nb_possible_actions==0) break;


			//JOUEUR 2 JOUE ALEATOIREMENT
			//				printf("Tour joueur 2\n");
			int_of_grid = grid_to_int(grid);
			action = del_i_indix_from_list(rand()%nb_possible_actions, &possible_actions);
			add_head_list(action, &grid_history);
			add_head_list(int_of_grid, &grid_history);
			//				printf("Joueur 2 action %d\n", action);
			action_to_coord(&i, &j, action);
			place_on_grid('O', grid, i, j);
			nb_possible_actions--;
			//				print_grid(grid);
		}
		
		//APPRENTISSAGE
		//	printf("L'historique est : \n");
		//					print_int_list(grid_history);

		double reward = reward_function(9-nb_possible_actions);
		//	printf("Le reward est %lf\n", reward);
		int old_int_of_grid, int_of_grid, action;
		while(grid_history!=NULL) {
			old_int_of_grid = int_of_grid;
			int_of_grid = del_i_indix_from_list(0, &grid_history);
			action = del_i_indix_from_list(0, &grid_history);
			if (possible_actions==NULL) {
				add_head_list(action, &possible_actions);
				continue;
			}
			double max_q=max_quality(q_matrix[old_int_of_grid], possible_actions);
			q_matrix[int_of_grid][action-1]= q_matrix[int_of_grid][action-1] + alpha * (reward + gamma*max_q - q_matrix[int_of_grid][action-1]);
			//	printf("La nouvelle valeur de grille %d à action %d est %lf\n", int_of_grid, action, q_matrix[int_of_grid][action-1]);
			add_head_list(action, &possible_actions);
			reward = reward/2;
		}

		free_int_list(grid_history);
		free_int_list(possible_actions);

	}
//=================FIN BOUCLE=====================

	for (int i=0;i<9;i++) {
		printf("Case %d est %lf\n", i, q_matrix[0][i]);
	}

//===================FREE=========================
	free_q_matrix(q_matrix);
	free_grid(grid);
	printf("Temps écoulé pour %d boucles : %lf \n", nb_loop, difftime( time(NULL), begin ));
}