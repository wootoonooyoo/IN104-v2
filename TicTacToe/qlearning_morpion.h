#ifndef QLEARNING_MORPION_H
#define QLEARNING_MORPION_H

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "int_list.h"
#include "morpion.h" 

#define NB_OF_GRID 19683 //Number of different grid = 3^9

#define PLAYER 0
#define COMPUTER 1
#define RANDOM 2

//free le tableau qualité q_matrix
void free_q_matrix(double** q_matrix);

//Crée (alloue la mémoire pour) le tableau qualité utilisé pour l'apprentissage 
/*
Chaque case correspond à une grille possible du morpion et contient un tableau
(possible mais pas forcément atteignable lors d'une partie 
i.e une grille avec que des croix a une case dans le tableau)
Chaque tableau est de taille 9 correspondant aux 9 actions possibles et 
à chauqe action est associée une valeur de qualité
*/
double** create_q_matrix();

//Crée (alloue la mémoire pour) une liste de toutes les actions possibles i.e [1,2,...,9]
struct int_list* create_list_actions();


//Détermine la valeur maximale quality_tab parmi toutes les actions possibles
double max_quality(double* quality_tab, struct int_list* possible_actions);

//Convertie une grille de morpion grid en un entier
/*
On une utilise une représentation en base 3 de la grille de tel sorte que
X.O 					1 0 2
...						0 0 0
... soit représenté par 0 0 0 puis (102000000) et on convertie en un entier
*/
int grid_to_int(char** grid);


//Renvoie l'indice d'une case valide (i.e dans possible_actions) contenant la valeur val dans quality_tab, si plusieurs cases possibles renvoie une valeur aléatoire
int get_i_with_val(double val, double* quality_tab, struct int_list* possible_actions);

//Renvoie une action suivant la méthode eps-greedy et la supprime de la liste possible_actions
int eps_greedy(double eps, double* quality_tab, struct int_list** p_possible_actions,
				int nb_possible_actions);

//Renvoie la position [i][j] correspondant à la valeur action
/*
On utilise une représentation du type 
7 8 9
4 5 6
1 2 3
de tel sorte que 4 correspond à [0][1]
*/
void action_to_coord(int* i, int* j, int action);

//Fonction qui donne renvoie un reward en fonction du résultat de la partie
/*
game_over indique s'il y a un gagnant (i.e pas d'égalité) et nb_coup indique le nombre de coups joués dans la grille
On fixe le reward de la façon suivante :
Victoire --> reward = 100
Egalite --> reward = 0
Défaite --> reward = -100
*/
double reward_function(bool game_over, int nb_coup);

//Renvoie une action en fonction de player_type
/*
On a les cas suivants selon la valeur de player_type :
player_type = PLAYER : On demande une action à l'utilisateur à rentrer au clavier
player_type = COMPUTER : On renvoie la meilleure action d'après le tableau qualité
player_type = RANDOM : On renvoie une action aléatoire
*/
int get_action(int player_type, struct int_list* possible_actions, double* quality_tab, int player_turn, int nb_possible_actions);

//Simule un tour de jeu et renvoie l'action jouée, met à jour l'historique des coups joués
/*
On a les cas suivants selon la valeur de player_type :
player_type = PLAYER : On demande une action à l'utilisateur à rentrer au clavier
player_type = COMPUTER : Si is_player_learning = true on renvoie une action selon eps_greedy sinon on renvoie la meilleure action d'après le tableau qualité
player_type = RANDOM : On renvoie une action aléatoire
*/
int play(char** grid, double eps, double** q_matrix,
			struct int_list** p_possible_actions, struct int_list** p_grid_history, int* p_nb_possible_actions,
			int player_type, bool is_player_learning, int player_turn );

//Effectue un apprentissage sur nb_loop parties
/*
L'apprentissage est effectif si player_i = COMPUTER et is_player_i_learning = true
*/
void learning(double alpha, double gamma, double eps, double** q_matrix_1, double** q_matrix_2, 
				int nb_loop, int player_1, int player_2, bool is_player_1_learning, bool is_player_2_learning);

//Simulation d'une partie de morpion
/*
On choisit qui joue contre qui et on a une vue sur le déroulé de la partie
*/
void verif_learning(double** q_matrix_1, double** q_matrix_2);

//Simulation d'un grand nombre de parties (sans apprentissage) et retour du nombre de victoire de chaque joueur et du nombre d'égalité
void stat(double** q_matrix_1, double** q_matrix_2, 
			int player_1, int player_2, int nb_parties, int* nb_victoire_joueur_1, int* nb_victoire_joueur_2, int* nb_egalite);

//Apprentissage jusqu'à que joueur 1 ne perde jamais, de même ensuite pour joueur 2 (non fonctionnel pour joueur 2)
void learning_until_perfect(double alpha, double gamma, double eps, double** q_matrix_1, double** q_matrix_2);

#endif /* QLEARNING_MORPION_H */