#ifndef INT_LIST_H
#define INT_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//Liste chaînée d'entiers
struct int_list{
	int val;
	struct int_list* next;
};

//free une liste chaînée
void free_int_list(struct int_list* list);

//Ajoute elt en tête de list
void add_head_list(int elt, struct int_list** list);

//Supprime le i-ème élément de la list et renvoie sa valeur
int del_i_indix_from_list(int i, struct int_list** p_list);

//Supprime la première occurence de val dans list, val est supposée dans list lors de l'appel de cette fonction
void del_val_from_list(int val, struct int_list** p_list);

//Affiche tous les éléments de list
void print_int_list(struct int_list* list);

//Vérifie si elt est dans list
bool is_in_list(int elt, struct int_list* list);

#endif /* INT_LIST_H */