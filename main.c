//
//  main.c

#include <stdio.h>
#include "mazeEnv.h"
#include "functions.c"
#include "functions.h"

void alloc_maze(void){
     maze = malloc(rows * sizeof(char*));

     for(int i=0; i<rows; i++) {
         maze[i] = malloc(cols * sizeof(char*));
     }
}

void maze_make(char* filename){
     char c;
     char rows_s[3] ={'\0'};
     char cols_s[3] ={'\0'};
     int rows_i = 0;
     int cols_i = 0;
     int swap = 0;

     FILE* file = fopen(filename, "r");

     if (file) {
         /* lire la premiere ligne avant EOF */
         while( (c=getc(file)) != EOF) {
               if(c== '\n'){
                      break;
               } else if (c==',') {
                      swap = 1;
               } else if (!swap) {
                      rows_s[rows_i]=c;
                      rows_i++;
               } else {
                      cols_s[cols_i]= c;
                      cols_i++;
               }
         }
     }

     /* convertir le string en nombre */
     rows = atoi(rows_s);
     cols = atoi(cols_s);

     alloc_maze();

     for (int i=0; i<rows; i++){
         for (int j=0; j < cols; j++){
             c = getc(file);

             if (c=='\n'){
                 c = getc(file);
             } else if (c == 's'){
               start_row = i;
               start_col = j;
             } else if ( c == 'g'){
               goal_row = i;
               goal_col = j;
             }

             maze[i][j] = c;
         }
     }

     fclose(file);
    
}

void maze_render(void){
     for (int i=0; i<rows; i++) {
         for (int j=0; j< cols; j++){
             printf("%c ", maze[i][j]);
         }
         printf("\n");
     }
     printf("\n");
    
}

void maze_reset(void){
     state_row = start_row;
     state_col = start_col;
}

/*
>> State
We shall define each position as a state.

>> Actions
In each state, there would be 4 actions: up, down, left, and right.

>> Q-Function
We need to initialise the Q (quality) variable and populate it.
At the start, for every state and every action, Q = 0.

>> Parameters
The Q Learning Algorithm consists of 2 parameters: alpha (learning rate) and gamma (discount factor).
We will arbitarily set both values with a value between 0 and 1 (exclusive of bounds).

>> Reward function
We need to design the reward function such that it reaches the destination "g" with the shortest path.
 
[Action]                     % [Penalty]
Taking 1 step                % -0.04
Moving into a wall           % -1
Moving into a visited square % -0.2 (not coded)
Moving back into s           % -0.5
Finding g                    % +1000
 
>> Timeout threshold
The cumulative score of the bot should not exceed a threshold value.
When it falls below the threshold value, timeout.
The value will be set arbitarily later.
 
*/

// Our code from here

int RNG(int largestValue){
    int k = rand() % (largestValue+1);
    return k;
}

int rowStateUpdater (int state_row, int action, int mode){
    
    // Mode: 1 for normal, -1 to undo
    if((mode != 1) && (mode!=-1)){
        printf("Invalid argument for mode! Use 1 or -1 only!\n");
        exit(0);
    }
    
    int new_state_row;
    
    if (action==0){
        new_state_row = state_row - 1 * mode; // moving up
    } else if (action == 1){
        new_state_row = state_row + 1 * mode; // moving down
    } else {
        new_state_row = state_row;
    }
    
    return new_state_row;
    
}

int colStateUpdater (int state_col, int action, int mode){
    
    // Mode: 1 for normal, -1 to undo
    if((mode != 1) && (mode!=-1)){
        printf("Invalid argument for mode! Use 1 or -1 only!\n");
        exit(0);
    }
    
    int new_state_col;
    
    if (action==2){
        new_state_col = state_col - 1 * mode; // moving up
    } else if (action == 3){
        new_state_col = state_col + 1 * mode; // moving down
    } else {
        new_state_col = state_col;
    }
    
    return new_state_col;
    
}


// Block refers to the reading at the new coordinate
double rewardFunction(char block){
    
    double reward;
    
    switch(block){
        
        // Moving into wall or boundary
        case '+':
            reward = -1.5;
            break;
        
        // Moving into visited square
        case '.':
            reward = -0.25;
            break;
            
        // Finding goal
        case 'g':
            reward = 1000;
            break;
        
        // Just moving around
        default:
            reward = -0.04;
            break;

    }
    
    return reward;
    
}

// Print out corresponding action direction
void actionToWord(int action){

    printf("The action taken is: ");

    switch(action){

        case 0:
            printf("Up\n");
            break;

        case 1:
            printf("Down\n");
            break;

        case 2:
            printf("Left\n");
            break;

        case 3:
            printf("Right\n");
            break;
    }


}

// A function to determine the maximum value of the Quality variable at that particular location
double maxQuality(int row, int col, double quality[][cols][4]){
    
    double qualityMax = quality[row][col][0];
    
    for (int k=1;k<4;k++){

        if(qualityMax < quality[row][col][k]){

            qualityMax = quality[row][col][k];

        }
       
    }
    return qualityMax;
    
}

// Determine the action with the highest quality. If multiple choices arise, randomly choose one of the option.
int bestActionFunction(int row, int col, double quality[][cols][4]){

    // initalise inital values
    int bestAction = 0;
    double bestQualityValue = quality[row][col][0];

    // in case of possible options
    int actionPossible[4];
    int choices = 0;
    actionPossible[choices++] = 0;
    
    // perform a comparison
    for (int k=1;k<4;k++){

        if(bestQualityValue < quality[row][col][k]){

            bestAction = k;
            bestQualityValue = quality[row][col][k];
            choices = 0;
            actionPossible[choices] = k;
            choices++;


        } else if (bestQualityValue == quality[row][col][k]){
            
            actionPossible[choices] = k;
            choices++;

        }
        

    }

    // if possible options exist, choose one at random
    if (choices > 1){
        
        int randomNo = RNG(--choices); // -- operator because it will overcount by 1
        bestAction = actionPossible[randomNo];

    }

    return bestAction;

}

void initaliseVariables(void){
    
    // [Step 1.1]
    // Declare the Q variable (quality)
    // Notation: double quality[row number][column number][action number];
    // action numbers: 0 - up, 1 - down, 2 - left, 3 - right
    double quality[rows][cols][4];

    // Declare the visited array as well
    int visited[rows][cols];
    
    // After declaring, intialise everything with 0
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            for (int k=0;k<4;k++){
                quality[i][j][k] = 0;
            }
            visited[i][j] = 0;
        }
    }

    // [Step 1.2]
    // Declare and initalise parameters
    // We shall set their values arbitarily.
    double alpha = 0.1; // learning rate
    double gamma = 0.2; // discount factor
    double epsilon = 0.95; // parameter to determine exploitation/exploration
    double score = 0; // score

    // [Step 1.3]
    // Define reward function
    // See above this block of comments
    int rowCounter = 0;
    
    // [Step 2]
    // Loop
    for (int b=0;b<100;b++){
        maze_reset();
        maze_make("maze.txt");
        //maze_render();
        rowCounter=0;
        //while(maze[state_row][state_col]!='g')
        while(maze[state_row][state_col]!='g'){

             //indicate current location
            maze[state_row][state_col] = 'x';

            // Debug purposes
            //printf("Loop number: %d\n",a);

            // BestAction variable
            int action;

            // Determine epsilon or greedy here by rolling a random number;
            int randomNumber = rand() % 1000;
            //printf("Rolled: %d\n",randomNumber);

            if(randomNumber < 1000*epsilon){
            // Exploration
                //printf("Exploring!\n");

                // Generate a random number between 0-3
                action = RNG(3);

            } else {
            // Exploitation
                //printf("Exploiting\n");

                // Find the action with the highest reward
                action = bestActionFunction(state_row,state_col,quality);

            } // end of epsilon-greedy

            // Debug
            //actionToWord(action);

            // Define 2 variables as placeholders for the inital coordinates
            int state_row_old = state_row;
            int state_col_old = state_col;
                
            // Update state
            printf("Before update -- Row: %d Col: %d \n",state_row,state_col);
            state_row = rowStateUpdater(state_row,action,1);
            state_col = colStateUpdater(state_col,action,1);
            printf("After update -- Row: %d Col: %d \n",state_row,state_col);

            // Debug - quality update
            printf("Quality of (%d,%d), Action %d -> %.2f\n",state_row_old,state_col_old,action,quality[state_row_old][state_col_old][action]);
            // Update quality value and score
            quality[state_row_old][state_col_old][action] = quality[state_row_old][state_col_old][action] + alpha * (rewardFunction(maze[state_row][state_col]) + gamma * maxQuality(state_row,state_col,quality) - quality[state_row_old][state_col_old][action]);
            // Debug - quality update
            printf("Quality of (%d,%d), Action %d -> %.2f\n",state_row_old,state_col_old,action,quality[state_row_old][state_col_old][action]);

            switch(maze[state_row][state_col]){

                case 'g':
                    printf("Goal!\n");
                    break;
                    
                // If it encounters a wall
                case '+':
                    printf("Wall!\n");

                    // Revert to original position
                    state_row = rowStateUpdater(state_row,action,-1);
                    state_col = colStateUpdater(state_col,action,-1);
                    break;

                case '.':
                    printf("Visited\n");
                    break;
                    
                // If it encounters a valid space (blank space)
                default:
                    printf("Valid space\n");
                    break;

            } // end of switch

            // indicate visited
            maze[state_row_old][state_col_old] = '.';
            maze_render();


            printf("\n");

            // Print maze
            //maze_render();
            rowCounter++;

        } // end of for loop
        //printf("Reached goal in %d loops\n",rowCounter);


    } // end of game loop
   

    // debug print
    for(int i=6; i<rows; i++){
        for(int j=0; j< cols; j++){
            for(int k=0;k<4;k++){
                if(quality[i][j][k]!=0){
                    printf("Quality (%d,%d) Action %d, Value: %.9f\n", i, j,k,quality[i][j][k]);

                }
            }
        }
    }

    maze_reset();
    maze_make("maze.txt");
    for (int q=0;q<50;q++){


        int action;

        //debug exploitation
        action = bestActionFunction(state_row,state_col,quality);

        // Define 2 variables as placeholders for the inital coordinates
        int state_row_old = state_row;
        int state_col_old = state_col;
            
        //print out quality values
        for(int k=0;k<4;k++){
            printf("Quality value for action %d: %.8f\n",k,quality[state_row][state_col][k]);
        }

        printf("The best action is %d\n",action);

        // Update state
        state_row = rowStateUpdater(state_row,action,1);
        state_col = colStateUpdater(state_col,action,1);
        printf("(%d,%d) -> (%d,%d)\n",state_row_old,state_col_old,state_row,state_col);

        // Update quality value and score
        quality[state_row_old][state_col_old][action] = quality[state_row_old][state_col_old][action] + alpha * (rewardFunction(maze[state_row][state_col]) + gamma * maxQuality(state_row,state_col,quality) - quality[state_row_old][state_col_old][action]);
        
        switch(maze[state_row][state_col]){

            case 'g':
                //printf("Goal!\n");
                break;
                
            // If it encounters a wall
            case '+':
                //printf("Wall!\n");

                // Revert to original position
                state_row = rowStateUpdater(state_row,action,-1);
                state_col = colStateUpdater(state_col,action,-1);
                break;

            case '.':
                //printf("Visited\n");
                break;
                
            // If it encounters a valid space (blank space)
            default:
                //printf("Valid space\n");
                break;

        } // end of switch


        // indicate visited
        maze[state_row_old][state_col_old] = '.';

        if(maze[state_row][state_col]=='g'){
            printf("Reached goal in %d steps!\n",q);
            //indicate current location
            maze[state_row][state_col] = 'x';
            exit(0);
        }


        //indicate current location
        maze[state_row][state_col] = 'x';

        printf("q = %d\n",q);
        maze_render();
        printf("\n");


        if(maze[state_row][state_col]=='g'){
            printf("Completed at loop %d\n",q);
            printf("Goal!!!!!\n");
            exit(0);
        }

    }
    

} // end of function


int main(void){
    
    //these lines work
    char* filename = "maze.txt";
    maze_make(filename);
    maze_render();
    initaliseVariables();


    
}
