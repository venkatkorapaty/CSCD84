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
  // // fprintf(stderr, " a: %d s: %d ", a,s);
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
  // (i+(j*size_X)) + ((k+(l*size_X))*graph_size) + ((m+(n*size_X))*graph_size*graph_size)
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

  // fprintf(stderr, "..13..");
  int maxAct = 0;
  // fprintf(stderr, "\n %d, %d\n", s_new, maxAct);
  
  double maxxx = get_Q(QTable, s_new, maxAct);
  // fprintf(stderr, "..14..");
  // - get_Q(QTable, s, maxAct);
  for (int a_p = 1; a_p < 4; a_p++) {
    double maxxxer = (double)get_Q(QTable, s_new, a_p);
    if (maxxxer > maxxx) {
      maxAct = a_p;
      maxxx = maxxxer;
    }
  }
  // fprintf(stderr, "%f\n", (double)alpha*((double)r + (double)lambda*(double)maxxx - (double)get_Q(QTable, s, a)));
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
        2 - move down
        3 - move left
        
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
  if (prob_rand - pct ) {
    do {
      // do rand act that's also valid
      a = rand()%4;

      // srand(time(NULL));
    } while(!(gr[mouse_pos[0][0] + size_X*mouse_pos[0][1]][a]));
  }
  else {
    // dun do rand act
    double vals[] = {get_Q(QTable, s, 0), get_Q(QTable, s, 1), get_Q(QTable, s, 2), get_Q(QTable, s, 3)};
    double max = -100000.0;
    int newer_mouse[2] = {mouse_pos[0][0], mouse_pos[0][1]};
    
    for (int i = 0; i < 4; i ++) {
      if (i % 2 == 0) {
        newer_mouse[1] -= 1 - i;
      } else {
        newer_mouse[0] += 2 - i;
      }
      if ((gr[mouse_pos[0][0] + mouse_pos[0][1]*size_X][a]) && (newer_mouse[0] >= 0 && newer_mouse[0] < size_X) && (newer_mouse[1] >= 0 && newer_mouse[1] < size_X) && vals[i] > max) {
        max = vals[i];
        a = i;
      }
    }
  }

  int new_mouse[2] = {mouse_pos[0][0], mouse_pos[0][1]};
  if (a % 2 == 0) {
    new_mouse[1] -= 1 - a;
  } else {
    new_mouse[0] += 2 - a;
  }
  int fuck[1][2] = {{new_mouse[0], new_mouse[1]}};
  double r = QLearn_reward(gr, fuck, cats, cheeses, size_X, graph_size);
  //double r = QLearn_reward(gr, mouse_pos, cats, cheeses, size_X, graph_size);
  
  int s_p = get_s(new_mouse, cats[0], cheeses[0], size_X, graph_size);
  
  QLearn_update(s, a, r, s_p, QTable);
  
  int maxAct = 4;
  double a_jabest = -100000.0;
  
  for (int i = 0; i < 4; i++) {
    double a_i = (double)get_Q(QTable, s, i);
    new_mouse[0] = mouse_pos[0][0];
    new_mouse[1] = mouse_pos[0][1];
    if (i % 2 == 0) {
      new_mouse[1] -= 1 - i;
    } else {
      new_mouse[0] += 2 - i;
    }
    int valid_spot = gr[mouse_pos[0][0] + mouse_pos[0][1]*size_X][i];
    int better = a_i > a_jabest;
    int out_x = new_mouse[0] >= 0 && new_mouse[0] < size_X;
    int out_y = new_mouse[1] >= 0 && new_mouse[1] < size_X;

    if ((valid_spot) && (better) && (out_x) && (out_y)) {

      maxAct = i;
      a_jabest = a_i;
    }
  }

  // if (maxAct != 4) {
  //   // fprintf(stderr, "a..");
  //   return maxAct;
  // }

  // fprintf(stderr, "returning..");
  if(0)
  return(0);		// <--- of course, you will change this!
  return maxAct;
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

  if (mouse_pos[0][0] == cats[0][0] && mouse_pos[0][1] == cats[0][1]) {
    return -50.0;
  }
  if (mouse_pos[0][0] == cheeses[0][0] && mouse_pos[0][1] == cheeses[0][1]) {
    return 50.0;
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

  /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/        

  return(0);		// <--- replace this while you're at it!

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
   
   You can have up to 5 cats and up to 5 cheese chunks, and array entries for the remaining cats/cheese
   will have a value of -1 - check this when evaluating your features!
  */

   /***********************************************************************************************
   * TO DO: Complete this function
   ***********************************************************************************************/      
   
}

double Qsa(double weights[25], double features[25])
{
  /*
    Compute and return the Qsa value given the input features and current weights
   */

  /***********************************************************************************************
  * TO DO: Complete this function
  ***********************************************************************************************/  
  
  return(0);		// <--- stub! compute and return the Qsa value
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
   * TO DO: Complete this function
   ***********************************************************************************************/  
 
  *maxU=0;	// <--- stubs! your code will compute actual values for these two variables!
  *maxA=0;
  return;
   
}

/***************************************************************************************************
 *  Add any functions needed to compute your features below 
 *                 ---->  THIS BOX <-----
 * *************************************************************************************************/
