#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "qlearning_morpion.h"


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
