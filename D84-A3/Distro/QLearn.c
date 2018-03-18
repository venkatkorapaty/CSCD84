/*
	CSC D84 - Unit 3 - Reinforcement Learning
	
	This file contains stubs for implementing the Q-Learning method
	for reinforcement learning as discussed in lecture. You have to
	complete two versions of Q-Learning.
	
	* Standard Q-Learning, based on a full-state representation and
	  a large Q-Table
	* Feature based Q-Learning to handle problems too big to allow
	  for a full-state representation
	    
	Read the assignment handout carefully, then implement the
	required functions below. Sections where you have to add code
	are marked

	**************
	*** TO DO:
	**************

	If you add any helper functions, make sure you document them
	properly and indicate in the report.txt file what you added.
	
	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Jan. 16
*/

#include <time.h>
#include "QLearn.h"

double get_Q(double *QTable, int s, int a) {
  return *(QTable + 4*s + a);
}

void set_Q(double *QTable, int s, int a, double val) {
  *(QTable + 4*s + a) = val;
}

void cum_sum_set_Q(double *QTable, int s, int a, double val) {
  *(QTable + 4*s + a) += val;
}

int get_s(int mouse[2], int cat[2], int cheese[2], int size_X, int graph_size) {
  return (mouse[0]+(mouse[1]*size_X)) + ((cat[0]+(cat[1]*size_X))*graph_size) + ((cheese[0]+(cheese[1]*size_X))*graph_size*graph_size);
}

void QLearn_update(int s, int a, double r, int s_new, double *QTable)
{
 /*
   This function implementes the Q-Learning update as stated in Lecture. It 
   receives as input a <s,a,r,s'> tuple, and updates the Q-table accordingly.
   
   Your work here is to calculate the required update for the Q-table entry
   for state s, and apply it to the Q-table
     
   The update involves two constants, alpha and lambda, which are defined in QLearn.h - you should not 
   have to change their values. Use them as they are.
     
   Details on how states are used for indexing into the QTable are shown
   below, in the comments for QLearn_action. Be sure to read those as well!
 */
 
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  //finding the actions for the given s_new that yields the highest Q(s_new,a_prime) value
  //then update the Q table for Q(s,a) with it's a number influenced by the immediate reward for s
  //and the reward for s_new
  int maxAct = 0;  
  double maxxx = get_Q(QTable, s_new, maxAct);
  for (int a_p = 1; a_p < 4; a_p++) {
    double maxxxer = (double)get_Q(QTable, s_new, a_p);
    if (maxxxer > maxxx) {
      maxAct = a_p;
      maxxx = maxxxer;
    }
  }
  cum_sum_set_Q(QTable, s, a, alpha*(r + (lambda*maxxx) - get_Q(QTable, s, a)));
}

int QLearn_action(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, double *QTable, int size_X, int graph_size)
{
  /*
     This function decides the action the mouse will take. It receives as inputs
     - The graph - so you can check for walls! The mouse must never move through walls
     - The mouse position
     - The cat position
     - The chees position
     - A 'pct' value in [0,1] indicating the amount of time the mouse uses the QTable to decide its action,
       for example, if pct=.25, then 25% of the time the mouse uses the QTable to choose its action,
       the remaining 75% of the time it chooses randomly among the available actions.
       
     Remember that the training process involves random exploration initially, but as training
     proceeds we use the QTable more and more, in order to improve our QTable values around promising
     actions.
     
     The value of pct is controlled by QLearn_core_GL, and increases with each round of training.
     
     This function *must return* an action index in [0,3] where
        0 - move up
        1 - move right
        2 - move down    // int out_x = new_mouse[0] >= 0 && new_mouse[0] < size_X;
    // int out_y = new_mouse[1] >= 0 && new_mouse[1] < size_X;
    //  && (out_y) && (out_x)
        3 - move left
        int distCat(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
     QLearn_core_GL will print a warning if your action makes the mouse cross a wall, or if it makes
     the mouse leave the map - this should not happen. If you see a warning, fix the code in this
     function!
     
   The Q-table has been pre-allocated and initialized to 0. The Q-table has
   a size of
   
        graph_size^3 x 4
        
   This is because the table requires one entry for each possible state, and
   the state is comprised of the position of the mouse, cat, and cheese. 
   Since each of these agents can be in one of graph_size positions, all
   possible combinations yield graph_size^3 states.
   
   Now, for each state, the mouse has up to 4 possible moves (up, right,
   down, and left). We ignore here the fact that some moves are not possible
   from some states (due to walls) - it is up to the QLearn_action() function
   to make sure the mouse never crosses a wall. 
   
   So all in all, you have a big table.
        
   For example, on an 8x8 maze, the Q-table will have a size of
   
       64^3 x 4  entries
       
       with 
       
       size_X = 8		<--- size of one side of the maze
       graph_size = 64		<--- Total number of nodes in the graph
       
   Indexing within the Q-table works as follows:
   
     say the mouse is at   i,j
         the cat is at     k,l
         the cheese is at  m,n
         
     state = (i+(j*size_X)) * ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
     ** Make sure you undestand the state encoding above!
     
     Entries in the Q-table for this state are

     *(QTable+(4*state)+a)      <-- here a is the action in [0,3]
     
     (yes, it's a linear array, no shorcuts with brackets!)
     
     NOTE: There is only one cat and once cheese, so you only need to use cats[0][:] and cheeses[0][:]
   */
  
  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/  

  //observer current state
  int s = get_s(mouse_pos[0], cats[0], cheeses[0], size_X, graph_size);
  // srand(time(NULL));
  double prob_rand = (double)rand()/(double)RAND_MAX;
  int a = 0;
  // fprintf(stderr, "%f\n", pct);
  if (prob_rand > pct) {
    do {
      // do rand act that's also valid
      a = rand()%4;

    } while(!(gr[mouse_pos[0][0] + size_X*mouse_pos[0][1]][a]));
  }
  else {
    // dun do rand act
    //take the actions that yields the largest Q(s,a) value
    double vals[] = {get_Q(QTable, s, 0), get_Q(QTable, s, 1), get_Q(QTable, s, 2), get_Q(QTable, s, 3)};
    double max = -100000.0;
    int newer_mouse[2] = {mouse_pos[0][0], mouse_pos[0][1]};
    
    for (int i = 0; i < 4; i ++) {
      if ((gr[mouse_pos[0][0] + mouse_pos[0][1]*size_X][i]) && vals[i] > max) {
        max = vals[i];
        a = i;
      }
    }
  }

  //find new mouse coords given the action we decided to take
  int new_mouse[2] = {mouse_pos[0][0], mouse_pos[0][1]};
  if (a % 2 == 0) {
    new_mouse[1] -= 1 - a;
  } else {
    new_mouse[0] += 2 - a;
  }

  //receive immediate reward
  int temp_mouse[1][2] = {{new_mouse[0], new_mouse[1]}};
  double r = QLearn_reward(gr, temp_mouse, cats, cheeses, size_X, graph_size);
  
  //observe s prime
  int s_p = get_s(new_mouse, cats[0], cheeses[0], size_X, graph_size);
  
  //update Q(s,a) with <s,a,r,s_p>
  QLearn_update(s, a, r, s_p, QTable);
  

  //once our table is updated, look at the possible actions to take from state s
  //and take the best one
  int maxAct = 4;
  double a_jabest = -100000.0;
  
  for (int i = 0; i < 4; i++) {
    double a_i = (double)get_Q(QTable, s, i);
    int valid_spot = gr[mouse_pos[0][0] + mouse_pos[0][1]*size_X][i];
    int better = a_i > a_jabest;

    if ((valid_spot) && (better)) {
      maxAct = i;
      a_jabest = a_i;
    }
  }

  if(0)
  return(0);		// <--- of course, you will change this!
  return maxAct; //we didn't change it!
}

double QLearn_reward(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function computes and returns a reward for the state represented by the input mouse, cat, and
    cheese position. 
    
    You can make this function as simple or as complex as you like. But it should return positive values
    for states that are favorable to the mouse, and negative values for states that are bad for the 
    mouse.
    
    I am providing you with the graph, in case you want to do some processing on the maze in order to
    decide the reward. 
        
    This function should return a maximim/minimum reward when the mouse eats/gets eaten respectively.      
   */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/

  int amountOfCheeses = 0;
  for (int i = 0; i < 5; i++) {
    if (cheeses[i][0] != -1)
      amountOfCheeses++;
  }

  int c_cat = 0;
  int cat_Val = 10000;
  int c_cheese = 0;
  int cheese_val = 10000;
  for (int i = 0; i < 5; i++) {
    if (mouse_pos[0][0] == cats[i][0] && mouse_pos[0][1] == cats[i][1]) {
      return -2.0;
    }
    if (mouse_pos[0][0] == cheeses[i][0] && mouse_pos[0][1] == cheeses[i][1]) {
      return 2.0;
    }

  }
  


  return(0);
  // return(fabs(mouse_pos[0][0] - cats[0][0]) + fabs(mouse_pos[0][1] - cats[0][1]) - (fabs(mouse_pos[0][0] - cheeses[0][0]) + fabs(mouse_pos[0][1] - cheeses[0][1])));		// <--- of course, you will change this as well!     
}

void feat_QLearn_update(double gr[max_graph_size][4],double weights[25], double reward, int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
    This function performs the Q-learning adjustment to all the weights associated with your
    features. Unlike standard Q-learning, you don't receive a <s,a,r,s'> tuple, instead,
    you receive the current state (mouse, cats, and cheese potisions), and the reward 
    associated with this action (this is called immediately after the mouse makes a move,
    so implicit in this is the mouse having selected some action)
    
    Your code must then evaluate the update and apply it to the weights in the weight array.    
   */
  
   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/       

  //create and evaluate the features array
  //double * features = (double*)malloc(sizeof(double)*25);
  double features[25];
  evaluateFeatures(gr, features, mouse_pos, cats, cheeses, size_X, graph_size);
  int *a;
  int act = 0;
  a = &act;
  // fprintf(stderr, "..WHYYY\n");
  double *max;
  double max_val = -100000.0;
  max = &max_val;

  
  int new_mouse[1][2] = {{mouse_pos[0][0], mouse_pos[0][1]}};
  if (act % 2 == 0) {
    new_mouse[0][1] -= 1 - act;
  } else {
    new_mouse[0][0] += 2 - act;
  }
  maxQsa(gr, weights, new_mouse, cats, cheeses, size_X, graph_size, max, a);
  double qs = Qsa(weights, features);
  // fprintf(stderr, "..WHYYY2\n");
  for (int i = 0; i < 25; i++) {
    weights[i] += alpha*(reward + (lambda * (max_val)) - qs) * features[i];
  } 

  //free(features);
  // fprintf(stderr, "..WHYYY3\n");
}

int feat_QLearn_action(double gr[max_graph_size][4],double weights[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], double pct, int size_X, int graph_size)
{
  /*
    Similar to its counterpart for standard Q-learning, this function returns the index of the next
    action to be taken by the mouse.
    
    Once more, the 'pct' value controls the percent of time that the function chooses an optimal
    action given the current policy.
    
    E.g. if 'pct' is .15, then 15% of the time the function uses the current weights and chooses
    the optimal action. The remaining 85% of the time, a random action is chosen.
    
    As before, the mouse must never select an action that causes it to walk through walls or leave
    the maze.    
   */

  /********************************************************************int***************************
   * TO DO: Complete this function
   ***********************************************************************************************/        
  //observer current state

  // srand(time(NULL));
  // fprintf(stderr, "PACOOO1...");
  double prob_rand = (double)rand()/(double)RAND_MAX;
  int *a;
  int act = 0;
  a = &act;
  //fprintf(stderr, "..WHYYY\n");  
  double *val;
  double max_val = 0.0;
  val = &max_val; 
  // fprintf(stderr, "%f\n", pct);
  if (prob_rand >= pct) {
    do {
      // do rand act that's also valid
      act = rand()%4;
    } while(!(gr[mouse_pos[0][0] + size_X*mouse_pos[0][1]][act]));
  }
  else {
    maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, val, a);
  }
  //find new mouse coords given the action we decided to take
  int new_mouse[2] = {mouse_pos[0][0], mouse_pos[0][1]};
  if (act % 2 == 0) {
    new_mouse[1] -= 1 - act;
  } else {
    new_mouse[0] += 2 - act;
  }
  //receive immediate reward
  int temp_mouse[1][2] = {{new_mouse[0], new_mouse[1]}};
  double reward = QLearn_reward(gr, temp_mouse, cats, cheeses, size_X, graph_size);
  //observe s prime
  feat_QLearn_update(gr, weights, reward, mouse_pos, cats, cheeses, size_X, graph_size);
  maxQsa(gr, weights, mouse_pos, cats, cheeses, size_X, graph_size, val, a);
  
  if (0)
    return(0);		// <--- replace this while you're at it!
  return act; //we didn't change it!
}

void evaluateFeatures(double gr[max_graph_size][4],double features[25], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size)
{
  /*
   This function evaluates all the features you defined for the game configuration given by the input
   mouse, cats, and cheese positions. You are free to define up to 25 features. This function will
   evaluate each, and return all the feature values in the features[] array.
   
   Take some time to think about what features would be useful to have, the better your features, the
   smarter your mouse!
   
   Note that instead of passing down the number of cats and the number of cheese chunks (too many parms!)
   the arrays themselves will tell you what are valid cat/cheese locations.
   
    if (i % 2 == 0) {
   You can have up to    // int out_x = new_mouse[0] >= 0 && new_mouse[0] < size_X;
    // int out_y = new_mouse[1] >= 0 && new_mouse[1] < size_X;
    //  && (out_y) && (out_x) 5 cats and up to 5 cheese chunks, and array entries for the remaining cats/cheese
   will have a value of -1 - check this when evaluating your features!
  */
  // features[2] = distCheese(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  // features[1] = distCat(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  // features[0] = mouseWalls(gr, mouse_pos, cats, size_X);
  // features[3] = distCatCheese(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  // features[4] = findAmountPaths(gr, mouse_pos, cats, cheeses, graph_size);
  for (int i = 0; i < 25; i++) {
    features[i] = 0;
  }
  //features[2] = distCheese(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  features[0] = distCat(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  //features[0] = mouseWalls(gr, mouse_pos, cats, size_X);
  //features[3] = distCatCheese(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  if (graph_size == 64) {
  double *l;
  double len = 0.0;
  l = &len;
  features[1] = findAmountPaths(gr, mouse_pos, cats, cheeses, graph_size, l);
  features[1] = len;
 } else {
	features[1] = distCheese(gr, mouse_pos, cats, cheeses, size_X, graph_size);
	//features[2] = mouseWalls(gr, mouse_pos, cats, size_X);
    features[2] = distCatCheese(gr, mouse_pos, cats, cheeses, size_X, graph_size);
 }
  // double totalFeat = features[0] + features[1] + features[2] + features[3] + features[4];
  // for (int i = 0; i < 5; i++) features[i] = features[i]/totalFeat;
  
   
}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */

  double ret = 0;
  //GRADIENT DESCENT
  for (int i = 0; i < 25; i++) {
    ret += (weights[i])*(features[i]);
    // fprintf(stderr, "weights i : %f\n", weights[i]);
  }
  
  return ret;		// <--- stub! compute and return the Qsa value
}

void maxQsa(double gr[max_graph_size][4],double weights[25],int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size, double *maxU, int *maxA)
{
 /*
   Given the state represented by the input positions for mouse, cats, and cheese, this function evaluates
   the Q-value at all possible neighbour states and returns the max. The maximum value is returned in maxU
   and the index of the action corresponding to this value is returned in maxA.
   
   You should make sure the function does not evaluate moves that would make the mouse walk through a
   wall. 
  */
 
   /***********************************************************************************************
   * TO DO: Complete this functionint size_X
   ***********************************************************************************************/  
 
  *maxU = -(pow (max_graph_size, 3));
  // int set = 0;
  
  
  for (int a_i = 0; a_i < 4; a_i++) {
    //if actions is available
    if (gr[mouse_pos[0][0] + (size_X*mouse_pos[0][1])][a_i]) {

      //calculate coords of mouse after actions is taken
      int new_mouse[1][2] = {{mouse_pos[0][0], mouse_pos[0][1]}};
      if (a_i % 2 == 0) {
        new_mouse[0][1] -= 1 - a_i;
      } else {
        new_mouse[0][0] += 2 - a_i;
      }

      //create and evaluate the features array
      //double * features = (double*)malloc(sizeof(double)*25);
	  double features[25];
      evaluateFeatures(gr, features, new_mouse, cats, cheeses, size_X, graph_size);
      //get Qsa for this move
      double maybeBetter = Qsa(weights, features);
      // fprintf(stderr, "%f..", maybeBetter);
      //free(features);

      //make sure we have the best one
      if (maybeBetter > *maxU) {
        // set = 1;
        *maxU = maybeBetter;
        *maxA = a_i;
      }
    }
    // if (set == 0) {
    //   fprintf(stderr, "fasdfasdf  %d\n", mouse_pos[0][0] + (size_X*mouse_pos[0][1]));
    // }
  }
  
  // *maxU=0;	// <--- stubs! your code will compute actual values for these two variables!
  // *maxA=0;
  return;
   
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below 
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/
double distCheese(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size) {
  //test code
  double shortest = 1000.0;
  for (int i = 0; i < 5; i++) {
    if (cheeses[i][0] < 0 || cheeses[i][1] < 0) {
      continue;
    }
    double temp = pow(pow(fabs(mouse_pos[0][0] - cheeses[i][0]), 2) + pow(fabs(mouse_pos[0][1] - cheeses[i][1]), 2),0.5);
    if (temp < shortest) {
      shortest = temp;
    }
  }
  return 1.0/(1.0 + shortest);
}


double distCat(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size) {
  //test code
  double shortest = 1000.0;
  for (int i = 0; i < 5; i++) {
    
    if (cats[i][0] < 0 || cats[i][1] < 0) {
      continue;
    }
    
    double temp = pow(pow(fabs(mouse_pos[0][0] - cats[i][0]), 2) + pow(fabs(mouse_pos[0][1] - cats[i][1]), 2),0.5);
    
    if (temp < shortest) {
      shortest = temp;
    }
   
  }
  return -1.0 * (1.0/(1.0+ shortest));
}

double mouseWalls(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int size_X) {
  int walls = 0;
  
  for (int i = 0; i < 4; i++) {
    int new_mouse[2] = {mouse_pos[0][0], mouse_pos[0][1]};
    if (i % 2 == 0) {
      new_mouse[1] -= 1 - i;
    } else {
      new_mouse[0] += 2 - i;
    }
    if (!gr[mouse_pos[0][0] + size_X*mouse_pos[0][1]][i]) {
      walls++;
    } else {
      for (int i = 0; i < 5; i++) {
        if (new_mouse[0] == cats[i][0] && new_mouse[1] == cats[i][1]) {
          walls++;
        }
      }
    }
  }
  return 1.0/(1.0+(double)walls);
}

double findAmountPaths(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int graph_size, double *l) {
  double amountOfPaths = 0.0;

  int size_X = (int)sqrt(graph_size);
  // int *pred = (int*)malloc(sizeof(int)*graph_size);
  int pred[graph_size];
  int amountOfCats = 0;
  int amountOfCheeses = 0;
  for (int i = 0; i < 5; i++) {
    if (cats[i][0] != -1)
      amountOfCats++;
    if (cheeses[i][0] != -1)
      amountOfCheeses++;
  }

  // find optimial path from mouse location to every other spot
  
  // fprintf(stderr, "start\n");
  // for (int i = 0; i < graph_size; i++) {
  //   fprintf(stderr, "%d: %d\n", i, pred[i]);
  // }

  int wallsAroundCheese[amountOfCheeses];
  int priority_cheese = 0;

  // find amount of walls around each cheese
  for (int cheese = 0; cheese < amountOfCheeses; cheese++) {
    for (int wall = 0; wall < 4; wall++) {
      if (!gr[cheeses[cheese][0] + cheeses[cheese][1]*size_X][wall])
        wallsAroundCheese[cheese]++;
    }
  }
  
  // find cheese with least amount of walls
  int resultWalls = 4;
  for (int cheese = 0; cheese < amountOfCheeses; cheese++) {
    if (wallsAroundCheese[cheese] < resultWalls) {
      resultWalls = wallsAroundCheese[cheese];
      priority_cheese = cheese;
    }
  }
  // int *p = (int*)malloc(sizeof(int)*max_graph_size);
  int p[graph_size];
  int pathLen = findDeWay(gr, cats, amountOfCats, cheeses, amountOfCheeses, mouse_pos, pred, graph_size, p);
  *l = 1.0/(1.0 + pathLen);
  // if (pathLen < 1)
  //   return pathLen;

  // int pathLen = traceBack2(pred, getLocation(cheeses[priority_cheese], size_X), mouse_pos[0], graph_size, path, p);
  // free(p);
  // fprintf(stderr, "%d\n", pathLen);
  // exit(0);
  // return pathLen;
  
  int mansNotHOt = abs(mouse_pos[0][0] - cheeses[priority_cheese][0]) + abs(mouse_pos[0][1] - cheeses[priority_cheese][1]);

  if (pathLen != graph_size) {
    amountOfPaths += 1.0;
  }


  // fprintf(stderr, "..IT\n");
  
  // free(p);
  // free(pred);
  // fprintf(stderr, "pred pls dund break\n");

  // paths[0] = pathLen;
  // paths[1] = amountOfPaths;
  // if (pathLen == -1) {
  //   paths[0] = paths[1] = 0;
  // }
  return 1.0/(1.0 + amountOfPaths);
}

int traceBack2(int *pred, int current, int origin[2], int graph_size, int *p) {
  int size_X = ((int)sqrt(graph_size));
  int size_Y = size_X;
	// uses the pred array to backtrack and figure out the path we found to the goal
	int i = 0;
	int reversePath[graph_size];

  // int mouseIndex = origin[0] + origin[1]*size_X;

  int goal = getLocation(origin, size_X);
  pred[goal] = -1;
  // fprintf(stderr, "start: %d, goal %d\n", pred[current], goal);
  // int prevLoc = -1;
	while (pred[current] != -1  && current != goal && i != graph_size) {//current != goal) {
    // fprintf(stderr, "%d\n", i);
    // fprintf(stderr, "Pre: (%d, %d) \n", current%size_X, current/size_X);
    // if (prevLoc != pred[current]) {
    //   fprintf(stderr, "Pre: (%d, %d, %d) \n", prevLoc, pred[current], current]);
    //   prevLoc = current;
    // }
    // else {
    //   fprintf(stderr, "Pre: (%d, %d, %d) \n", prevLoc, pred[current], current]);
    //   return 0;
    // }
    // fprintf(stderr, "IS IT..(%d, %d, %d)", i, current, graph_size);
		reversePath[i] = current;
    // fprintf(stderr, "..HERE?\n");
		current = pred[current];
    // fprintf(stderr, "Post: (%d, %d) \n", current%size_X, current/size_X);
    //something's happening sometimes we don't find a path to the goal
    // j++;
    // if (j > max_graph_size*max_graph_size*max_graph_size) {
    //   // fprintf(stderr, "stuck exiting...");
    //   return 0;
    // }
		i++;
	}
  if (i == graph_size) {
    return 0;
  }
	return i;
}

int findDeWay(double gr[max_graph_size][4], int cat_loc[5][2], int cats, int cheese_loc[5][2], int cheeses, int mouse_loc[1][2], int *pred, int graph_size, int *p)
{


  //declarations
	int v = 1;
	int queueMain[graph_size];
	int queueIndex = 0;
	int stackIndex = 0;
	int *heap = (int*)malloc(sizeof(int)*(graph_size)*2);
	//int heap[max_graph_size*2]
	double weights[graph_size];
	int actWeights[graph_size];
	int heap_visited[graph_size];
  int size_X = ((int)sqrt(graph_size));
  int size_Y = size_X;
	// // Stub so that the code compiles/runs - The code below will be removed and replaced by your code!
	int visited[graph_size];
	// int pred[graph_size];
	//initialize arrays because of C
	for (int i = 0; i < graph_size; i++) {
		visited[i] = 0;
		pred[i] = -1;
    actWeights[i] = 0;
		heap_visited[i] = 0;
    heap[i] = -1;
	}
  

	actWeights[mouse_loc[0][0] + (mouse_loc[0][1]*size_X)] = -1;

	//if A* we set all the elements in our heap to -1 and then add the first element (current location)
	addHeap(heap, weights, actWeights, mouse_loc[0][0] + mouse_loc[0][1]*size_X, queueIndex, graph_size);
	visited[mouse_loc[0][0] + (size_X*mouse_loc[0][1])] = 1;
	queueIndex++;

	int current = -1;
  // fprintf(stderr, "start..");
	while (queueIndex > 0/*cheeses > 0*/) {
		current = extract_min(heap, weights, actWeights, queueIndex, graph_size);
		queueIndex--;
		while (heap_visited[current] == 1) {
			current = extract_min(heap, weights, actWeights, queueIndex, graph_size);
			queueIndex--;
		}

    if (!(current <= graph_size && current >= 0)) {
      break;
    }

		heap_visited[current] = 1;
		int xCord = current % size_X;
		int yCord = current / size_Y;

		int catCheeseLoc = is_cat_or_cheese(xCord, yCord , cat_loc, cats, cheese_loc, cheeses);
    if (catCheeseLoc == 1) {// 1 == cheese
      int pathLen = 0;
      //trace backthe array
      if (!(mouse_loc[0][0] == xCord && mouse_loc[0][1] == yCord)) {
        pathLen = traceBack2(pred, current, mouse_loc[0], graph_size, p);
      }
      free(heap);
      // fprintf(stderr, "..end\n");
      return pathLen;
    }
		v++;
		double *loc = gr[xCord + (yCord*size_X)];
		
		// holds grid coordinates around our current coordinate
		int adj_Cords[4][2] = {{xCord, yCord-1},
		{xCord + 1, yCord},
		{xCord, yCord + 1},
		{xCord - 1, yCord}
		};

		for (int i = 0; i < 4; i++) {
    // fprintf(stderr, "startloop %d..", i);
			int x = i;
      // fprintf(stderr, "TEST..");
      int t = loc[x];
      // fprintf(stderr, "..ING");
			// if valid location
      // fprintf(stderr, "test..%d..", current);
			if (t) {
        int blah = 0;
        // fprintf(stderr, "passed\n");
        // fprintf(stderr, "startlooploc..");
				// if we haven't visited
				if(!visited[(adj_Cords[x][0]) + ((adj_Cords[x][1]) * size_Y)]){
					int catCheeseLoc = is_cat_or_cheese(adj_Cords[x][0], adj_Cords[x][1], cat_loc, cats, cheese_loc, cheeses);
          if (catCheeseLoc != 2) {// if not a cat
            //add to heap and set weights
            visited[(adj_Cords[x][0]) + (adj_Cords[x][1] * size_X)] = 1;
            pred[(adj_Cords[x][0]) + (adj_Cords[x][1] * size_X)] = current;
            if (actWeights[xCord + (yCord*size_Y)] == -1) {
              //root is current (act weight is -1)
              actWeights[adj_Cords[x][0] + ((adj_Cords[x][1]) * size_Y)] = 1;
            } else {
              actWeights[adj_Cords[x][0] + ((adj_Cords[x][1]) * size_Y)] = 1 + actWeights[xCord + (yCord*size_Y)];
            }
            weights[(adj_Cords[x][0]) + (adj_Cords[x][1] * size_X)] = H_cost(adj_Cords[x][0], adj_Cords[x][1], cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr, graph_size);
            int ind = (adj_Cords[x][0]) + (adj_Cords[x][1] * size_X);
            // if (ind <= graph_size && ind >= 0) {
            //   blah = 1;
            addHeap(heap, weights, actWeights, ind, queueIndex, graph_size);
            queueIndex++;
            // }
            // if (!blah)
            //   fprintf(stderr, "ind %d %d\n", ind, graph_size);
          }
				}
				// When running UCS, and we come back to a node, see if it has faster path
				else if (actWeights[xCord + ((yCord)*size_Y)] != -1) {
					if (actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] >= 1 + actWeights[xCord + (yCord*size_Y)]) {
						actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] = 1 + actWeights[xCord + (yCord*size_Y)];
						pred[(adj_Cords[i][0]) + adj_Cords[i][1] * size_X] = current;
            int ind = (adj_Cords[i][0]) + (adj_Cords[i][1] * size_X);
            // fprintf(stderr, "ind %d\n", ind);
            // if (ind <= graph_size && ind >= 0) {
              // blah = 1;
            addHeap(heap, weights, actWeights, (adj_Cords[i][0]) + (adj_Cords[i][1] * size_X), queueIndex, graph_size);
            queueIndex++;
            // }
            // if (!blah)
            //   fprintf(stderr, "ind %d %d\n", ind, graph_size);
					}
				}
        // fprintf(stderr, "..endlooploc\n");
			}
      // fprintf(stderr, "..endloop %d\n", i);
		}
    // fprintf(stderr, "next\n");
	}
  // fprintf(stderr, "..end\n");
  // did not find cheese
  return graph_size;
}

void addHeap(int *heap, double weights[max_graph_size], int actWeights[max_graph_size], int val, int size, int graph_size) {
	// adds val to our heap data structure for the A* algorithms
	int parent;
	while (size > 0) {
		parent = (size - 1) / 2;
		
		if (weights[val] + actWeights[val] >= weights[heap[parent]] + actWeights[heap[parent]]) {
			heap[size] = val;
			return;
		}
		heap[size] = heap[parent];
		size = parent;
	}
	heap[0] = val;
}

void propagate(int *heap, double weights[max_graph_size], int actWeights[max_graph_size], int node, int size, int graph_size) {
	// when extracting min from the heap we need to reorganize our heap
	if (size == 2) {
		if (weights[heap[0]] + actWeights[heap[0]] > weights[heap[1]] + actWeights[heap[1]]) {
			int temp = heap[0];
			heap[0] = heap[1];
			heap[1] = temp;
		}
		return;
	}
	int l = node*2 + 1;
	int r = node*2 + 2;
	int min = weights[heap[node]] + actWeights[heap[node]];
	int mindex = node;

	if (l < size - 1 && (weights[heap[l]] + actWeights[heap[l]]) < (weights[heap[r]] + actWeights[heap[r]])) {
		min = weights[heap[l]] + actWeights[heap[l]];
		mindex = l;
	}
	if (r < size - 1 && (weights[heap[r]] + actWeights[heap[r]]) < (weights[heap[mindex]] + actWeights[heap[mindex]])) {
		min = weights[heap[r]] + actWeights[heap[r]];
		mindex = r;
	}
	if (mindex != node) {
		int temp = heap[node];
		heap[node] = heap[mindex];
		heap[mindex] = temp;
		propagate(heap, weights, actWeights, mindex, size, graph_size);
	}
}

int extract_min(int *heap, double weights[max_graph_size], int actWeights[max_graph_size], int size, int graph_size) {
	//get the first element in the heap and then re-sort it
	int min;
	int last_elem;
	if (size == 0) {
		return -1;
	}
	if (size == 1) {
		min = heap[0];
		heap[0] = -1;
		return min;
	}

	min = heap[0];
	last_elem = heap[size - 1];
	heap[0] = last_elem;
	heap[size - 1] = -1;

	propagate(heap, weights, actWeights, 0, size - 1, graph_size);

	return min;
}

int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[max_graph_size][4], int graph_size)
{

	int heur = 99999;
	for (int i = 0; i < cheeses; i++) {
		int eu_dist = abs(x - cheese_loc[i][0]) + abs(y - cheese_loc[i][1]);
		if (heur > eu_dist) {
			heur = eu_dist;
		}
	}
	return heur;
}

int getLocation(int coords[2], int size_Y) {
	return coords[0] + (coords[1]*size_Y);
}

int is_cat_or_cheese(int x, int y, int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses) {
	/*
	  Return 0 for safe spot
	  1 if cheese is there
	  2 if fur devil is there
	*/
	int bound = fmax(cats, cheeses);
	int i = 0;
	while (i < bound) {
		if (i < cats && cat_loc[i][0] == x && cat_loc[i][1] == y) {
			return 2;
		}
		if (i < cheeses && cheese_loc[i][0] == x && cheese_loc[i][1] == y) {
			return 1;
		}
		i++;
	}
	return 0;
}

double distCatCheese(double gr[max_graph_size][4], int mouse_pos[1][2], int cats[5][2], int cheeses[5][2], int size_X, int graph_size) {
  int cheese = 0;
  double shortest = 1000.0;
  //pick the prioritized cheese
  for (int i = 0; i < 5; i++) {
    if (cheeses[i][0] < 0 || cheeses[i][1] < 0) {
      continue;
    }
    double temp = pow(pow(fabs(mouse_pos[0][0] - cheeses[i][0]), 2) + pow(fabs(mouse_pos[0][1] - cheeses[i][1]), 2),0.5);
    if (temp < shortest) {
      shortest = temp;
      cheese = i;
    }
  }

  int num_cats = 0;
  double total = 0.0;

  for (int i = 0; i < 5; i++) {
    
    if (cats[i][0] < 0 || cats[i][1] < 0) {
      continue;
    }
    
    double temp = pow(pow(fabs(mouse_pos[0][0] - cats[i][0]), 2) + pow(fabs(mouse_pos[0][1] - cats[i][1]), 2),0.5);
    total += temp;
    num_cats++;
   
  }

  return 1.0/(1.0 + (total/num_cats));

}