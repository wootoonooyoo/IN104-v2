#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

char** create_grid() {
	char** grid = malloc(3*sizeof(char*));
	if (grid==NULL) {
		printf("Mem error\n");
		return NULL;
	}
	for (int i=0;i<3;i++) {
		grid[i]=malloc(3*sizeof(char));
		if (grid[i]==NULL) {
			printf("Mem error\n");
			free(grid);
			return NULL;
		}
		for (int j=0;j<3;j++) {
			grid[i][j]='.';
		}
	}
	return grid;
}

void free_grid(char** grid) {
	for (int i=0;i<3;i++) {
		free(grid[i]);
	}
	free(grid);
}

void reset_grid(char** grid) {
	for (int i=0;i<3;i++) {
		for (int j=0;j<3;j++) {
			grid[i][j]='.';
		}
	}
}

void print_grid(char** grid) {
	for (int i=0;i<3;i++) {
		for (int j=0;j<3;j++) {
			printf("%c",grid[i][j]);
		}
		printf("\n");
	}
}

bool is_game_over(char** grid) {
	if ((grid[1][1]!='.' && grid[1][1]==grid[2][2] && grid[1][1]==grid[3][3])
		|| (grid[1][3]!='.' && grid[1][3]==grid[2][2] && grid[1][3]==grid[3][1])) 
		return true;
	for (int i=0;i<3;i++) {
		if ((grid[i][1]!='.' && grid[i][1]==grid[i][2] && grid[i][1]==grid[i][3])
		|| (grid[1][i]!='.' && grid[1][i]==grid[2][i] && grid[1][i]==grid[3][i])) 
		return true;
	}
	return false;
}

//determine if a square is occuped, 0 <= i,j <= 2
bool is_square_occuped(char** grid, int i, int j) {
	if (grid[i][j]='.') return false;
	return true;
}

//place value in the grid on the square i,j, the square is supposed not occuped
void place_on_grid(char value, char** grid, int i, int j) {
	grid[i][j]=value;
}

int main() {
	char** grid = create_grid();
	print_grid(grid);
}