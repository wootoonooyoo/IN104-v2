#ifndef MORPION_H
#define MORPION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//Crée (alloue l'espace pour) une grille de morpion vierge
char** create_grid();

//free la grille de morpion grid
void free_grid(char** grid);

//Remet à zéro la grille de morpion grid
void reset_grid(char** grid);

//Affiche la grille de morpion grid
void print_grid(char** grid);

//Renvoie true s'il y a un gagnant dans la grille de morpion grid 
//On vérifie uniquement le cas de victoire et pas le cas d'égalité
bool is_game_over(char** grid);

//Place symbol à la position grid[i][j], grid[i][j] est supposée non occupée et symbol est supposé être 'X' ou 'O'
void place_on_grid(char symbol, char** grid, int i, int j);

#endif /* MORPION_H */