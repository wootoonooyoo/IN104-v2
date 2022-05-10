#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "morpion.c" //A changer

#define NB_OF_GRID 19683 //Number of different grid = 3^9


void free_q_matrix(int** q_matrix) {
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
	double** q_matrix = malloc(NB_OF_GRID*sizeof(int*));
	if (q_matrix==NULL) {
		printf("Mem error");
		return NULL;
	}
	for (int i=0;i<NB_OF_GRID;i++) {
		q_matrix[i]=calloc(9,sizeof(int));
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
			int_of_grid=int_of_grid + val*mult;
			mult=mult*3;
			}
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
	struct int_list* possible_actions;
	for (int j=0;j<i;j++) {
		possible_actions=possible_actions->next;
	}
	int val = (possible_actions->next)->val;
	struct int_list* tmp = possible_actions->next;
	possible_actions->next = (possible_actions->next)->next;
	free(tmp);
	return val;
}

//remove val from possible_actions, val is supposed in possible_actions
void del_val_from_list(int val, struct int_list** possible_actions_p) {
	if (val==*possible_actions_p->val) {
		struct int_list* frst = *possible_actions_p;
		*possible_actions_p=(*possible_actions_p)->next;
		free(frst);
	} else {
		struct int_list* possible_actions;
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

int eps_greedy(double eps, double* quality_tab, struct int_list* possible_actions,
				int nb_possible_actions) {
	if (rand() % 100000< 100000*eps) {
		int randomNumber=rand()%nb_possible_actions;
		return del_i_indix_from_list(randomNumber, &possible_actions);
	} else {
		double max_q = max_quality(quality_tab);
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
		del_val_from_list(randomChoice, &possible_actions);
		return randomChoice;
	}
}



void testeur(struct int_list* possible_actions) {
	*possible_actions=*(possible_actions->next);
	//possible_actions->next = (possible_actions->next)->next;
}

//Create quality matrix
//Max quality
//Reward Function
//Best Action function

int main() {
	time_t begin = time(NULL);
	int** test = calloc(19683,sizeof(int*));
	if (test[0]==NULL) printf("C'est bon\n");
	unsigned long secondes = (unsigned long) difftime( time(NULL), begin );
	printf("Fini en %ld secondes\n", secondes);
	free(test);

	struct int_list* possible_actions = create_list_actions();
	printf("%d\n", possible_actions->val);
	//testeur(possible_actions);
	printf("%d\n", del_i_indix_from_list(1,&possible_actions));
	printf("%d\n", (possible_actions)->val);
	//free_int_list(possible_actions);
}