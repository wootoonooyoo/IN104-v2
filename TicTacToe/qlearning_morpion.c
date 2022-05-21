#include "qlearning_morpion.h"

void free_q_matrix(double** q_matrix) {
	for (int i=0;i<NB_OF_GRID;i++) {
		free(q_matrix[i]);
	}
	free(q_matrix);
}

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

double max_quality(double* quality_tab, struct int_list* possible_actions) {
	double max = quality_tab[possible_actions->val-1];
	while (possible_actions!=NULL) {
		if (quality_tab[possible_actions->val -1]>max) max=quality_tab[possible_actions->val -1];
		possible_actions = possible_actions->next;
	}
	return max;
}

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

int get_i_with_val(double val, double* quality_tab, struct int_list* possible_actions) {
	int tab_choices[9]; //Tableau utilisé pour traiter le cas où il y aurait plusieurs choix possibles
	int nb_choices = 0; //Nombre de choix possibles
	while (possible_actions!=NULL) {
		if (quality_tab[possible_actions->val-1]==val) {
			tab_choices[nb_choices]=possible_actions->val;
			nb_choices++;
		}
		possible_actions=possible_actions->next;
	}
	return tab_choices[rand()%nb_choices];
}

int eps_greedy(double eps, double* quality_tab, struct int_list** p_possible_actions,
				int nb_possible_actions) {
	//Exploration
	if ((rand() % 100000)< 100000*eps) {
		int randomNumber=rand()%nb_possible_actions;
		return del_i_indix_from_list(randomNumber, p_possible_actions);
	//Exploitation, choix aléatoire parmi ceux de qualité max
	} else {
		struct int_list* possible_actions = *p_possible_actions;
		double max_q = max_quality(quality_tab, possible_actions);
		int randomChoice=get_i_with_val(max_q, quality_tab, possible_actions);
		del_val_from_list(randomChoice, p_possible_actions);
		return randomChoice;
	}
}

void action_to_coord(int* i, int* j, int action) {
	*i = (action-1)/3;
	*j = (action-1)%3;
}

double reward_function(bool game_over, int nb_coup) {
	if (!game_over && nb_coup==9) return 0; //Cas d'égalité
	if (nb_coup%2 == 1) return 100;	//Cas de victoire
	return -100; //Cas de défaite

}

int get_action(int player_type, struct int_list* possible_actions, double* quality_tab, int player_turn, int nb_possible_actions) {
	if (player_turn<0 || player_turn>2) { //Cas qui n'arrive jamais normalement
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

int play(char** grid, double eps, double** q_matrix,
			struct int_list** p_possible_actions, struct int_list** p_grid_history, int* p_nb_possible_actions,
			int player_type, bool is_player_learning, int player_turn ) {
	int i, j, action;
	int int_of_grid = grid_to_int(grid);
	int nb_possible_actions = *p_nb_possible_actions;
	struct int_list* possible_actions = *p_possible_actions;
	switch (player_type) {
		case COMPUTER : {
			if (is_player_learning) { //Si l'ordinateur apprend renvoie une méthode selon eps_greedy
				action = eps_greedy(eps, q_matrix[int_of_grid], p_possible_actions, nb_possible_actions);
			} else { //Si l'ordinateur n'apprend pas renvoie l'action de qualité max
				action=get_action(player_type, possible_actions, q_matrix[int_of_grid], player_turn, nb_possible_actions);
				del_val_from_list(action, p_possible_actions);
			}
			break;
		}
		case RANDOM : {
			action = del_i_indix_from_list(rand()%nb_possible_actions, p_possible_actions);
			break;
		}
		case PLAYER : {
			action=get_action(player_type, possible_actions, q_matrix[int_of_grid], player_turn, nb_possible_actions);
			del_val_from_list(action, p_possible_actions);
			break;
		}
		default : {
			printf("Erreur player_type\n");
			break;
		}
	}
	//On met à jour l'historique des coups (pas toujours utile en pratique vu notre utilisation de la fonction) pour l'apprentissage
	add_head_list(action, p_grid_history);
	add_head_list(int_of_grid, p_grid_history);
	//On met à jour la grille
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
		
		//Historique de tous les coups joués
		/*
		Pour ne pas avoir à créer une autre structure stockant 2 entiers à chaque liste, la liste sera faite de sorte que l'élément
		2i correspond à la grille et l'élément 2i+1 à l'action choisie dans cette grille
		*/
		struct int_list* grid_history = NULL;
		int int_of_grid, action;
		while (true) { //Simulation d'une partie
			//JOUEUR 1
			play(grid, eps, q_matrix_1, &possible_actions, &grid_history, &nb_possible_actions, player_1, is_player_1_learning, 1);
			if (is_game_over(grid) || nb_possible_actions==0) break;

			//JOUEUR 2
			play(grid, eps, q_matrix_2, &possible_actions, &grid_history, &nb_possible_actions, player_2, is_player_2_learning, 2);
			if (is_game_over(grid)) break;
		}
		//APPRENTISSAGE
		double reward_1 = reward_function(is_game_over(grid), 9-nb_possible_actions);
		double reward_2 = -reward_1;
		int old_int_of_grid;
		int_of_grid=grid_to_int(grid);
		while(grid_history!=NULL) { //Mise à jour du tableau qualité suivant la méthode q_learning
			old_int_of_grid = int_of_grid;
			int_of_grid = del_i_indix_from_list(0, &grid_history);
			action = del_i_indix_from_list(0, &grid_history);
			/*
			Cas où la grille est complète après le dernier coup, on n'a pas besoin de mettre à jour la valeur de qualité car aucune
			action n'est possible
			*/
			if (possible_actions==NULL) { 
				add_head_list(action, &possible_actions);
				continue;
			}
			if (player_1==COMPUTER && is_player_1_learning==true) { //Mis à jour de la matrice qualité du joueur 1 s'il apprend
				double max_q=max_quality(q_matrix_1[old_int_of_grid], possible_actions);
				q_matrix_1[int_of_grid][action-1]= q_matrix_1[int_of_grid][action-1] + alpha * (reward_1 + gamma*max_q - q_matrix_1[int_of_grid][action-1]);
				add_head_list(action, &possible_actions);
				reward_1 = reward_1/2;
			}
			if (player_2==COMPUTER && is_player_2_learning==true) { //Mis à jour de la matrice qualité du joueur 2 s'il apprend
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
	//On définit qui joue contre qui
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
	while (!arret) { //Apprentissage joueur 1 et joueur 2 sur nb_loop parties
		learning(alpha, gamma, eps, q_matrix_1, q_matrix_2, nb_loop, player_1, player_2, is_player_1_learning, is_player_2_learning);
		//On fait jouer le joueur 1 100 000 parties contre aléatoire, dès qu'il en perd une, on arrête et on continue l'apprentissage
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
	while (!arret && difftime( time(NULL), begin ) <30) { //Apprentissage joueur 2 contre aléatoire (non fonctionnel donc arrêt au bout de 30 s)
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