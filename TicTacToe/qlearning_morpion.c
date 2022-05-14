#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "int_list.c" //A changer
#include "morpion.c" 

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
	double** q_matrix = malloc(NB_OF_GRID*(sizeof(double*))+1);
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
	//printf("Nouvelle évaluation avec liste des actions possibles ");
	//print_int_list(possible_actions);
	//printf("La valeur évaluée est %d\n", possible_actions->val -1);
	//printf("Initialisation avec valeur à %lf\n", quality_tab[possible_actions->val -1]);
	double max = quality_tab[possible_actions->val-1];
	while (possible_actions!=NULL) {
	//	printf("L'indice évalué est %d et sa valeur dans quality_tab est %lf\n", possible_actions->val -1, quality_tab[possible_actions->val -1]);
		if (quality_tab[possible_actions->val -1]>max) max=quality_tab[possible_actions->val -1];
		possible_actions = possible_actions->next;
	}
	//printf("La fonction termine\n");
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
/* CORRIGER COMME SHAWN
if(randomNumber < 100000*epsilon*pow(0.7,loopNo))
Plus l'algorithme tourne et moins on essaie de faire de l'exploration
mais plus de la perfection ?
*/
int eps_greedy(double eps, double* quality_tab, struct int_list** possible_actions_p,
				int nb_possible_actions) {
	//Exploration
	if ((rand() % 100000)< 100000*eps) {
	//	printf("Exploration avec %d actions possibles\n", nb_possible_actions);
		int randomNumber=rand()%nb_possible_actions;
		return del_i_indix_from_list(randomNumber, possible_actions_p);
	//Exploitation, random choice among those with highest quality
	} else {
	//	printf("Exploitation\n");
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

//Corriger reward_function() de tel sorte que si P1 joue un coup et que P2 puisse gagner au prochain coup alors mauvais rewar
//Reward selon une backpropagation, si victoire alors bon reward sinon mauvais reward qu'on fait propager en arrière avec un coefficient
//Reward positif si gagnant, négatif si perdant, nul si égalité comme ça apprentissage valide pour les deux joueurs
double reward_function(int nb_coup) {
	//Si égalité
	if (nb_coup==9) return 0;
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

void learning(double alpha, double gamma, double eps, int nb_loop, char** grid, double** q_matrix) {
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
			//printf("Eps_greedy termine\n");
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
			if (is_game_over(grid)) break;
			//				print_grid(grid);
		}
		
		//APPRENTISSAGE
		//	printf("L'historique est : \n");
		//					print_int_list(grid_history);
		//printf("Rétropropagation\n");
		double reward = reward_function(9-nb_possible_actions);
		//	printf("Le reward est %lf\n", reward);
		int old_int_of_grid, int_of_grid, action;
		int_of_grid=grid_to_int(grid);
		//print_grid(grid);
		while(grid_history!=NULL) {
			//printf("La liste des actions possibles est ");
			//print_int_list(possible_actions);
			old_int_of_grid = int_of_grid;
			//printf("La valeur de la grille est %d\n", old_int_of_grid);
			int_of_grid = del_i_indix_from_list(0, &grid_history);
			action = del_i_indix_from_list(0, &grid_history);
			if (possible_actions==NULL) {
				add_head_list(action, &possible_actions);
				continue;
			}
			//printf("La valeur de la grille est %d\n", old_int_of_grid);
			//for (int j=0;j<9;j++) printf("%lf \n", q_matrix[old_int_of_grid][j]);
			//printf("\n");
			double max_q=max_quality(q_matrix[old_int_of_grid], possible_actions);
			q_matrix[int_of_grid][action-1]= q_matrix[int_of_grid][action-1] + alpha * (reward + gamma*max_q - q_matrix[int_of_grid][action-1]);
			//	printf("La nouvelle valeur de grille %d à action %d est %lf\n", int_of_grid, action, q_matrix[int_of_grid][action-1]);
			add_head_list(action, &possible_actions);
			reward = reward/2;
		}

		free_int_list(grid_history);
		free_int_list(possible_actions);
	}
}

int main() {
	time_t begin = time( NULL );
	srand( time( NULL ) );


	char** grid = create_grid();
	double** q_matrix = create_q_matrix();

//==========BOUCLE D'APPRENTISSAGE================
	
	double alpha = 0.1; // learning rate
    double gamma = 0.7; // discount factor
    double eps = 0.9; // parameter to determine exploitation/exploration
	int nb_loop;
	printf("Combien de boucles pour apprentissage ? (Conseil : Au moins 100 000)\n");
	scanf("%d", &nb_loop);
	learning(alpha, gamma, eps, nb_loop, grid, q_matrix);
//=================FIN BOUCLE=====================

	printf("Temps écoulé pour %d boucles : %lf \n", nb_loop, difftime( time(NULL), begin ));


//==============VERIFICATION APPRENTISSAGE========
	printf("Les choix d'actions pendant les parties sont sous la forme\n");
	for (int i = 2; i >=0; i--)
	{
		for (int j = 0; j < 3; j++)
		{
			printf("%d ", 3*i + j +1);
		}
		printf("\n");
	}
	int arret;
	printf("Choix : 0 jouer sinon arrêt\n");
	scanf("%d", &arret);
	while (arret==0) {
		reset_grid(grid);
		int action;
		struct int_list* possible_actions=create_list_actions();
		int int_of_grid;
		int i,j;
		int nb_possible_actions = 9;
		while(true) {
			printf("Tour : Joueur 1\n");
			int_of_grid = grid_to_int(grid);
			printf("Les actions possibles sont ");
			print_int_list(possible_actions);
			for (int i=0;i<9;i++) {
				printf("Case %d est %lf\n", i+1, q_matrix[int_of_grid][i]);
			}		
			double max_q=max_quality(q_matrix[int_of_grid], possible_actions);
			printf("Le max est %lf\n", max_q);
			//CORRIGER POUR BIEN PRENDRE UN MAX ALEATOIRE
			action=get_i_with_val(max_q, q_matrix[int_of_grid], possible_actions);
			action_to_coord(&i, &j, action);
			printf("On joue %d qui correspond à [%d][%d]\n", action, i ,j);
			place_on_grid('X', grid, i, j);
			del_val_from_list(action, &possible_actions);
			nb_possible_actions--;
			print_grid(grid);
			printf("\n");
			if (is_game_over(grid) || nb_possible_actions==0) break;


			//JOUEUR 2 
			printf("Tour : joueur 2\nAction joueur 2 : ");
			printf("Les actions possibles sont ");
			print_int_list(possible_actions);
			printf("Choix action ?\n");
			scanf("%d", &action);
			while (!is_in_list(action,possible_actions)) {
				printf("Erreur choix action, les actions possibles sont ");
				print_int_list(possible_actions);
				printf("Choix action ?\n");
				scanf("%d", &action);
			}
			//				printf("Joueur 2 action %d\n", action);
			action_to_coord(&i, &j, action);
			place_on_grid('O', grid, i, j);
			print_grid(grid);
			printf("\n");
			del_val_from_list(action, &possible_actions);
			nb_possible_actions--;
			if (is_game_over(grid)) break;
		}
		printf("Fin de partie\n");
		if (nb_possible_actions==0) {
			printf("Egalité\n");
		} else if (nb_possible_actions%2==0) {
			printf("Joueur 2 gagne\n");
		} else {
			printf("Joueur 1 gagne\n");
		}
		printf("\n\n");
		printf("Choix action : 0 jouer sinon arrêt\n");
		scanf("%d", &arret);	
	}




//===================FREE=========================
	free_q_matrix(q_matrix);
	free_grid(grid);

}
