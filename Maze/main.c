#include <stdio.h>
#include "mazeEnv.h"
#include "functions.h"

//Additional for boltzmann policy
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>



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
See Reward function below
 
>> Timeout threshold
The cumulative score of the bot should not exceed a threshold value.
When it falls below the threshold value, timeout.
The value will be set arbitarily later.
 
*/ // Our code from here

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
            reward = -300;
            break;
        
        // Moving into visited square
        case '.':
            reward = -50;
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

// For boltzmann policy

//calculate boltzmann quality
double boltzmannQuality(double quality,double temperature){
    return exp(quality/temperature);
}

double boltzmannWeights(int row,int col,double quality[][cols][4], double temperature){

    //Calculate total
    double sum=0;
    for (int i=0;i<4;i++){
        sum += boltzmannQuality(quality[row][col][i],temperature);
    }
    return sum;

}

//calculate boltzmann probability for each action
double boltzmannProba(int row, int col,double quality[][cols][4],double temperature,int action){

    //Calculate specific action probability
    double individualWeight = boltzmannQuality(quality[row][col][action],temperature);

    //Calculate total weight
    double sum = boltzmannWeights(row,col,quality,temperature);

    return individualWeight/sum;
}

// Robert Jenkins' 96 bit Mix Function
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

int epsilonGreedyAction(int row,int col,double quality[][cols][4],double epsilon,int loopNo){

    int action;

    // Determine epsilon or greedy here by rolling a random number;
    int randomNumber = rand() % 100000;

    if(randomNumber < 100000*epsilon*pow(0.7,loopNo)){
    // Exploration
        // Generate a random number between 0-3
        action = RNG(3);

    } else {
    // Exploitation
        // Find the action with the highest reward
        action = bestActionFunction(row,col,quality);

    } // end of epsilon-greedy

    return action;
    
}

//Choose boltzmann action from weighted probability
int boltzmannAction(int row, int col, double quality[][cols][4], double temperature){

    //seed
    unsigned long seed = mix(clock(), time(NULL), getpid());

    //initalise rng
    srand(seed);

    //Set bounds
    double min = 0.0;
    double max = boltzmannWeights(row,col,quality, temperature);

    //Generate random number
    double range = (max - min); 
    double div = RAND_MAX / range;
    double randomNo = min + (rand() / div);
    //printf("max is = %f\n",max);
    //printf("random number = %f\n",randomNo);

    //Debug print - probability for each action
    // for(int i=0;i<4;i++){
    //     printf("action %d, value = %f\n",i,boltzmannProba(row,col,quality,temperature,i));
    // }
    
    //Choose action
    double weightCheck = max;
    for (int i=3;i>=0;i--){

        weightCheck -= boltzmannQuality(quality[row][col][i],temperature);
        if(randomNo >= weightCheck){
            //printf("Chosen action is = %d\n",i);
            return i;
        }
    }
    return 0; //for gcc
} 

int nextAction(int row, int col, double quality[][cols][4],double temperature){
    return boltzmannAction(row,col,quality,temperature);
}

void initaliseVariables(void){
    
    // [Step 1.1]
    // Declare the Q variable (quality)
    // Notation: double quality[row number][column number][action number];
    // action numbers: 0 - up, 1 - down, 2 - left, 3 - right
    double quality[rows][cols][4];

    // Declare the visited array as well
    //int visited[rows][cols];
    
    // After declaring, intialise everything with 0
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            for (int k=0;k<4;k++){
                quality[i][j][k] = 0;
            }
            //visited[i][j] = 0;
        }
    }

    // [Step 1.2]
    // Declare and initalise parameters - We shall set their values arbitarily.
    double alpha = 0.1; // learning rate
    double gamma = 0.7; // discount factor
    double epsilon = 0.9; // parameter to determine exploitation/exploration
    double temperature = 400;

    // Debug purposes
    int rowCounter = 0;
    
    // Choosing method
    int method = 0;
            do {
                printf("Choose method : \n1. EpsilonGreedy \n2. Boltzmann\n");
                scanf("%d", &method);
            } while (method !=1 && method !=2);

    // [Step 2]
    // Loop
    for (int b=0;b<15;b++){

        temperature = temperature * 0.7;
        //printf("temp = %f\n",temperature);
        maze_reset();
        maze_make("maze.txt");
        //maze_render();
        rowCounter=0;
        while(maze[state_row][state_col]!='g'){

            // Indicate current location
            maze[state_row][state_col] = 'x';

            // BestAction variable

            int action;
            if (method==1) {
                action = boltzmannAction(state_row,state_col,quality,temperature);
            } else if (method==2) {
                action = epsilonGreedyAction(state_row,state_col,quality,epsilon,b);
            }

            //printf("chosen action is %d\n",action);

            // Define 2 variables as placeholders for the inital coordinates
            int state_row_old = state_row;
            int state_col_old = state_col;
                
            // Update state
            state_row = rowStateUpdater(state_row,action,1);
            state_col = colStateUpdater(state_col,action,1);

            // Update quality value and score - Q Learning
            quality[state_row_old][state_col_old][action] = quality[state_row_old][state_col_old][action] + alpha * (rewardFunction(maze[state_row][state_col]) + gamma * maxQuality(state_row,state_col,quality) - quality[state_row_old][state_col_old][action]);
            
            // Update quality value and score - SARSA
            // quality[state_row_old][state_col_old][action] = quality[state_row_old][state_col_old][action] + alpha * (rewardFunction(maze[state_row][state_col]) + gamma * quality[state_row][state_col][nextAction(state_row,state_col,quality,temperature)] - quality[state_row_old][state_col_old][action]);

            switch(maze[state_row][state_col]){

                case 'g':
                    break;
                    
                // If it encounters a wall
                case '+':
                    // Revert to original position
                    state_row = rowStateUpdater(state_row,action,-1);
                    state_col = colStateUpdater(state_col,action,-1);
                    break;

                case '.':
                    break;
                    
                // If it encounters a valid space (blank space)
                default:
                    break;

            } // end of switch

            // indicate visited
            maze[state_row_old][state_col_old] = '.';
            //maze_render();
            rowCounter++;

        } // end of for loop
        printf("Loop %d - goal in %d loops\n",b,rowCounter);

    } // end of game loop
   
    // debug print to check quality table
    // for(int i=6; i<rows; i++){
    //     for(int j=0; j< cols; j++){
    //         for(int k=0;k<4;k++){
    //             if(quality[i][j][k]!=0){
    //                 printf("Quality (%d,%d) Action %d, Value: %.9f\n", i, j,k,quality[i][j][k]);

    //             }
    //         }
    //     }
    // }

    // debug print to check action
    maze_reset();

    // check quality at 6,3
    // for (int i=0;i<4;i++){
    //     printf("quality at (6,3), action %d // quality = %f // boltz = %f\n",i,quality[6][3][i],boltzmannQuality(quality[6][3][i],100));
    // }
    
    // // BestAction variable
    // int action = boltzmannAction(state_row,state_col,quality,temperature);

    // printf("best action at (%d,%d) is %d\n",state_row,state_col,action);

 
    

} // end of function


int main(void){
    //these lines work
    char* filename = "maze.txt";
    maze_make(filename);
    maze_render();
    initaliseVariables();

    // double k = boltzmannQuality(-1.5,100);
    // printf("value k = %f\n",k);


    
}
