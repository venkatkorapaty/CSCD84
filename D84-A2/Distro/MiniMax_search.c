/*
	CSC D84 - Unit 2 - MiniMax search and adversarial games

	This file contains stubs for implementing a MiniMax search
        procedure with alpha-beta pruning. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E. Sep. 15
*/

#include "MiniMax_search.h"
#include <unistd.h>

static int MAX_DIST_FROM_TWO_POINTS = 46;
static int CHEESE = 1;
static int CAT = 2;

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

int traceBack2(int pred[1024], int path[1024][2], int current, int mouse_loc[1][2]) {
	// uses the pred array to backtrack and figure out the path we found to the goal
	int i = 0;
	int reversePath[1024];

	while (pred[current] != -1 ) {
		
		reversePath[i] = current;
		current = pred[current];
		i++;
	}
	// return i;

	int x;
	int y;
	double total = 0;
	for (int j = 0; j < i; j++) {
		x = reversePath[j]%size_X;
		y = reversePath[j]/size_Y;
		path[i - j][0] = x;
		path[i - j][1] = y;
	}
	path[0][0] = mouse_loc[0][0];
	path[0][1] = mouse_loc[0][1];
	return i;
}

void addHeap(int *heap, double weights[graph_size], int actWeights[graph_size], int val, int size) {
	int parent;

	// adds val to our heap data structure for the A* algorithms

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

void propagate(int *heap, double weights[graph_size], int actWeights[graph_size], int node, int size) {
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
		propagate(heap, weights, actWeights, mindex, size);
	}
}

int extract_min(int *heap, double weights[graph_size], int actWeights[graph_size], int size) {
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

	propagate(heap, weights, actWeights, 0, size - 1);

	return min;
}

int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
	/*
		This function computes and returns the heuristic cost for location x,y.
		As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
		The goal is cheese. Which cheese is up to you.
		Whatever you code here, your heuristic must be admissible.

		Input arguments:

			x,y - Location for which this function will compute a heuristic search cost
			cat_loc - Cat locations
			cheese_loc - Cheese locations
			mouse_loc - Mouse location
			cats - # of cats
			cheeses - # of cheeses
			gr - The graph's adjacency list for the maze

			These arguments are as described in the search() function above
	*/
	int heur = 99999;
	for (int i = 0; i < cheeses; i++) {
		// int xCord = abs(x - cheese_loc[i][0]);
		// int yCord = abs(y - cheese_loc[i][1]);
		// int eu_dist = (int)pow(xCord*xCord + yCord*yCord, 0.5);
		int eu_dist = abs(x - cheese_loc[i][0]) + abs(y - cheese_loc[i][1]);
		if (heur > eu_dist) {
			heur = eu_dist;
		}
	}
	// fprintf(stderr, "heur: %d\n", heur);
	return heur;
}

int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{

	int heur = 0;

	// calculate heuristic distance from x y to every cat
	int loc[10][2];
	for (int i = 0; i < cats; i++) {
		loc[0][0] = cat_loc[i][0];
		loc[0][1] = cat_loc[i][1];
		int mouseCost = H_cost(x, y, loc, loc, mouse_loc, 1, 1, gr);
		// We use 46 as that's an upperboundd for the maximum size of our euclidean distance
		heur += (MAX_DIST_FROM_TWO_POINTS - mouseCost);
	}

	// amount of walls enclosing a cheese, checks if it's too
	// enclosed in a hallways
	int cheeseWalls[10][2];
	for (int i = 0; i < cheeses; i++) {
		loc[0][0] = cheese_loc[i][0];
		loc[0][1] = cheese_loc[i][1];
		cheeseWalls[i][1] = H_cost(x, y, loc, loc, mouse_loc, 1, 1, gr);
		for (int j = 0; j < 4; j++) {
			cheeseWalls[i][0] += gr[cheese_loc[i][0] + (size_X*cheese_loc[i][1])][j];
		}
	}
	
	int closestCheese = 999;
	int index;
	int highest_walls = 10;
	int prioritized_cheese = 10;
	// Get cheese that has the least amount of walls enclosing a cheese
	for (int i = 0; i < cheeses; i++) {
		if (cheeseWalls[i][0] <= highest_walls) {
			prioritized_cheese = i;
		}
	}

	loc[0][0] = cheese_loc[prioritized_cheese][0];
	loc[0][1] = cheese_loc[prioritized_cheese][1];
	// We use 46 as that's an upperboundd for the maximum size of our euclidean distance
	return heur + (MAX_DIST_FROM_TWO_POINTS - H_cost(x, y, loc, loc, mouse_loc, 1, 1, gr));
}

int search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2])
{
	// // Stub so that the code compiles/runs - The code below will be removed and replaced by your code!
	int visited[graph_size];
	int pred[graph_size];
	//initialize arrays because of C
	for (int i = 0; i < graph_size; i++) {
		visited[i] = 0;
		pred[i] = -1;
	}

	//declarations
	int v = 1;
	int queueMain[graph_size];
	int queueIndex = 0;
	int stackIndex = 0;
	int *heap = (int*)malloc(sizeof(int)*(graph_size)*2);
	double weights[graph_size];
	int actWeights[graph_size];
	int heap_visited[graph_size];


	//set all the initial actual weights and visited nodes to 0 for A* modes
	for (int i = 0; i < graph_size; i++) {
		actWeights[i] = 0;
		heap_visited[i] = 0;
	}

	actWeights[mouse_loc[0][0] + (mouse_loc[0][1]*size_Y)] = -1;

	//if A* we set all the elements in our heap to -1 and then add the first element (current location)
	for (int i = 0; i < graph_size; i++) {
		heap[i] = -1;
	}
	addHeap(heap, weights, actWeights, mouse_loc[0][0] + mouse_loc[0][1]*size_X, queueIndex);
	visited[mouse_loc[0][0] + (size_Y*mouse_loc[0][1])] = 1;
	queueIndex++;

	int current = -1;

	while (cheeses > 0) {
		current = extract_min(heap, weights, actWeights, queueIndex);
		queueIndex--;
		if (current == -1) {
			// Couldn't find the cheese, accepting our fate from the fur devils
			path[0][0] = mouse_loc[0][0];
			path[0][1] = mouse_loc[0][1];
			path[1][0] = mouse_loc[0][0];
			path[1][1] = mouse_loc[0][1];
			free(heap);
			return 2;
		}
		while (heap_visited[current] == 1) {
			current = extract_min(heap, weights, actWeights, queueIndex);
			queueIndex--;
			if (current == -1) {
				// Couldn't find the cheese, accepting our fate from the fur devils
				path[0][0] = mouse_loc[0][0];
				path[0][1] = mouse_loc[0][1];
				path[1][0] = mouse_loc[0][0];
				path[1][1] = mouse_loc[0][1];
				free(heap);
				return 2;
			}
		}

		heap_visited[current] = 1;

		int xCord = current % size_X;
		int yCord = current / size_Y;
		int catCheeseLoc = is_cat_or_cheese(xCord, yCord , cat_loc, cats, cheese_loc, cheeses);
		if (catCheeseLoc == CHEESE) {
			int dist = 0;
			//trace backthe array
			if (mouse_loc[0][0] == xCord && mouse_loc[0][1] == yCord) {
				path[0][0] = mouse_loc[0][0];
				path[0][1] = mouse_loc[0][1];
				path[1][0] = mouse_loc[0][0];
				path[1][1] = mouse_loc[0][1];
				dist = 2;
			}
			else {
				// if we find cheese, compute the proper path
				dist = traceBack2(pred, path, current, mouse_loc);
				// dist += weights[path[1][0] + path[1][1]*size_X];
			}

			free(heap);
			return dist;
		}

		// keep track of visit order
		visit_order[xCord][yCord] = v;
		v++;
		double *loc = gr[xCord + (yCord*size_X)];
		
		// holds grid coordinates around our current coordinate
		int adj_Cords[4][2] = {{xCord, yCord-1},
		{xCord + 1, yCord},
		{xCord, yCord + 1},
		{xCord - 1, yCord}
		};

		for (int i = 0; i < 4; i++) {
			int x = i;
			// if valid location
			if (loc[x]) {
				// if we haven't visited
				if(!visited[(adj_Cords[x][0]) + ((adj_Cords[x][1]) * size_Y)]){
					int catCheeseLoc = is_cat_or_cheese(adj_Cords[x][0], adj_Cords[x][1], cat_loc, cats, cheese_loc, cheeses);
					if (catCheeseLoc != CAT) {
						//add to heap and set weights
						visited[(adj_Cords[x][0]) + (adj_Cords[x][1] * size_X)] = 1;
						pred[(adj_Cords[x][0]) + (adj_Cords[x][1] * size_X)] = current;
						if (actWeights[xCord + (yCord*size_Y)] == -1) {
							//root is current (act weight is -1)
							actWeights[adj_Cords[x][0] + ((adj_Cords[x][1]) * size_Y)] = 1;
						} else {
							actWeights[adj_Cords[x][0] + ((adj_Cords[x][1]) * size_Y)] = 1 + actWeights[xCord + (yCord*size_Y)];
						}
						weights[(adj_Cords[x][0]) + (adj_Cords[x][1] * size_X)] = H_cost_nokitty(adj_Cords[x][0], adj_Cords[x][1], cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
						addHeap(heap, weights, actWeights, (adj_Cords[x][0]) + (adj_Cords[x][1] * size_X), queueIndex);
						queueIndex++;
						
					}
				}
				// When running UCS, and we come back to a node, see if it has faster path
				else if (actWeights[xCord + ((yCord)*size_Y)] != -1) {
					if (actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] >= 1 + actWeights[xCord + (yCord*size_Y)]) {
						actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] = 1 + actWeights[xCord + (yCord*size_Y)];
						pred[(adj_Cords[i][0]) + adj_Cords[i][1] * size_X] = current;
						addHeap(heap, weights, actWeights, (adj_Cords[i][0]) + (adj_Cords[i][1] * size_X), queueIndex);
						queueIndex++;
					}
				}
			}
		}


	}
	//Couldn't find the cheese, accepting our fate from the devils of fur
	path[0][0] = mouse_loc[0][0];
	path[0][1] = mouse_loc[0][1];
	path[1][0] = mouse_loc[0][0];
	path[1][1] = mouse_loc[0][1];

	return 2;
}

int start = 0;
double MiniMax(double gr[graph_size][4], int path[1][2], double minmax_cost[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, double (*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]), int agentId, int depth, int maxDepth, double alpha, double beta)
{
 /*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in MiniMax_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:

	Exactly the same as for Assignment 1 - have a look at your code if you need a reminder of how the adjacency
	list and agent positions are stored.	

	Note that in this case, the path will contain a single move - at the top level, this function will provide
	an agent with the 'optimal' mini-max move given the game state.


   IMPORTANT NOTE: Mini-max is a recursive procedure. This function will need to fill-in the mini-max values for 
	 	   all game states down to the maximum search depth specified by the user. In order to do that,
		   the function needs to be called with the correct state at each specific node in the mini-max
		   search tree.

		   The game state is composed of:

			* Mouse, cat, and cheese positions (and number of cats and cheeses)

		   At the top level (when this function is called by the mini-max driver code), the game state
		   correspond to the current situation of the game. But once you start recursively calling
		   this function for lower levels of the search tree the positions of agents will have changed.

		   Therefore, you will need to define local variables to keep the game state at each node of the
		   mini-max search tree, and you will need to update this state when calling recursively so that
		   the search does the right thing.

		   This function *must check* whether:
			* A candidate move results in a terminal configuration (cat eats mouse, mouse eats cheese)
			  at which point it calls the utility function to get a value
	 		* Maximum search depth has been reached (depth==maxDepth), at which point it will also call
			  the utility function to get a value
			* Otherwise, call recursively using the candidate configuration to find out what happens
			  deeper into the mini-max tree.

   Arguments:
		gr[graph_size][4]   		- This is an adjacency list for the maze
		path[1][2] 			- Your function will return the optimal mini-max move in this array.
		minmax_cost[size_X][size_Y]	- An array in which your code will store the
						  minimax value for maze locations expanded by
						  the search *when called for the mouse, not
						  for the cats!*

						  This array will be used to provide a visual 
						  display of minimax values during the game.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- No alpha-beta pruning
					mode = 1	- Alpha-beta pruning

		(*utility)(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4]);
				- This is a pointer to the utility function which returns a value for a specific game configuration

				   NOTE: Unlike the search assignment, this utility function also gets access to the graph so you can do any processing 					 that requires knowledge of the maze for computing the utility values.

				  * How to call the utility function from within this function : *
					- Like any other function:
						u = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
						
		agentId: Identifies which agent we are doing MiniMax for. agentId=0 for the mouse, agentId in [1, cats] for cats. Notice that recursive calls
                         to this function should increase the agentId to reflect the fact that the next level down corresponds to the next agent! For a game
                         with two cats and a mouse, the agentIds for the recursion should look like 0, 1, 2, 0, 1, 2, ...
	
		depth: Current search depth - whether this is a MIN or a MAX node depends both on depth and agentId.
		
		maxDepth: maximum desired search depth - once reached, your code should somehow return
			  a minimax utility value for this location.

		alpha. beta: alpha and beta values passed from the parent node to constrain search at this
			     level.

   Return values:
		Your search code will directly update data passed-in as arguments:
		
		- Mini-Max value	: Notice this function returns a double precision number. This is
					  the minimax value at this level of the tree. It will be used 
					  as the recursion backtracks filling-in the mini-max values back
					  from the leaves to the root of the search tree. 

		- path[1][2]		: Your MiniMax function will return the location for the agent's 
					  next location (i.e. the optimal move for the agent). 
		- minmax_cost[size_X][size_Y] 	:  Your search code will update this array to contain the
						   minimax value for locations that were expanded during
						   the search. This must be done *only* for the mouse.

						   Values in this array will be in the range returned by
						   your utility function.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

 /********************************************************************************************************
 * 
 * TO DO:	Implement code to perform a MiniMax search. This will involve a limited-depth BFS-like
 *              expansion. Once nodes below return values, your function will propagate minimax utilities
 *		as per the minimax algorithm.
 *	
 *		Note that if alpha-beta pruning is specified, you must keep track of alphas and betas
 *		along the path.
 *
 *		You can use helper functions if it seems reasonable. Add them to the MiniMax_search.h
 *		file and explain in your code why they are needed and how they are used.
 *
 *		Recursion should appear somewhere.
 *
 *		MiniMax cost: If the agentId=0 (Mouse), then once you have a MiniMax value for a location
 *		in the maze, you must update minmax_cost[][] for that location.
 *
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

 // Stub so that the code compiles/runs - This will be removed and replaced by your code!

	// path[0][0]=mouse_loc[0][0];
	// path[0][1]=mouse_loc[0][1];
	// fprintf(stderr, "DEPTH: %d\n", depth);
	// double num = 10000;
	// if (num == 10000.00000) {
	// 	fprintf(stderr, "thing");

	// }
	// 		exit(0);
	int *curr_player;
	if (agentId == 0) {
		curr_player = mouse_loc[agentId];
	} else {
		curr_player = cat_loc[agentId - 1];
	}
	int ded = checkForTerminal(mouse_loc, cat_loc, cheese_loc, cats, cheeses);
	// fprintf(stderr, "move is %d depth %d --(%d, %d)\n", agentId, depth, curr_player[0], curr_player[1]);
	// fprintf(stderr, "DED: %d\n", ded);
	// fprintf(stderr, "test\n");
	if (ded == 2) {
		// fprintf(stderr, "test1\n");
		//some high value
		minmax_cost[curr_player[0]][curr_player[1]] = 100;
		return minmax_cost[curr_player[0]][curr_player[1]];
	} else if (ded == 1) {
		// fprintf(stderr, "test2\n");
		//some low value
		minmax_cost[curr_player[0]][curr_player[1]] = -10000;
		return minmax_cost[curr_player[0]][curr_player[1]];
	} else if (depth == maxDepth) {
		// fprintf(stderr, "test3\n");
		//utility function
		minmax_cost[curr_player[0]][curr_player[1]] = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
		return minmax_cost[curr_player[0]][curr_player[1]];
	}
	// fprintf(stderr, "test8\n");
	

	int x = curr_player[0];
	int y = curr_player[1];
	int adj_list[4][2] = {{x, y-1},
						{x+1, y},
						{x, y+1},
						{x-1, y}};
	// fprintf(stderr, "test4\n");
	double val = 10000;
	int optX = 0;
	int optY = 0;
	double vals[4][3];
	for (int i = 0; i < 4; i++) {
		if (agentId == 0)
			vals[i][0] = -10000.0;
		else
			vals[i][0] = 10000.0;
	}
	for (int i = 0; i < 4; i++) {
		// fprintf(stderr, "test5\n");
		if (gr[x + (y * size_Y)][i]) {
			// fprintf(stderr, "test6\n");
			//check for terminal using a possible move of the current player
			int cordX = curr_player[0];
			int cordY = curr_player[1];
			// int *temp = &curr_player[0];
			// (*temp) = adj_list[i][0];
			// // curr_player[0] = adj_list[i][0];
			// temp = &curr_player[1];
			// (*temp) = adj_list[i][1];
			if (agentId == 0) {
				// curr_player = mouse_loc[agentId];
				mouse_loc[0][1] = adj_list[i][1];
				mouse_loc[0][0] = adj_list[i][0];
			} else {
				cat_loc[agentId - 1][1] = adj_list[i][1];
				cat_loc[agentId - 1][0] = adj_list[i][0];
			}
			// curr_player[1] = adj_list[i][1];
			int cat_agent = agentId;
			if (cat_agent < cats) {
				cat_agent++;
			} else {
				cat_agent = 0;
			}
			double childVal = MiniMax(gr, path, minmax_cost, cat_loc, cats, cheese_loc, cheeses, mouse_loc, mode, *utility, cat_agent, depth+1, maxDepth, alpha, beta);
			// fprintf(stderr, "CHILDVAL: %f\n", childVal);
			if (val == 10000.0 || val == -10000.0) {
				val = childVal;
				optX = adj_list[i][0];
				optY = adj_list[i][1];
			}
			// if (agentId == 0) {
			// 	if (childVal > val) {
			// 		val = childVal;
			// 		optX = adj_list[i][0];
			// 		optY = adj_list[i][1];
			// 	}
			// }
			// else {
			// 	if (childVal < val) {
			// 		val = childVal;
			// 		optX = adj_list[i][0];
			// 		optY = adj_list[i][1];
			// 	}
			// }
			vals[i][0] = childVal;
			vals[i][1] = adj_list[i][0];
			vals[i][2] = adj_list[i][1];
			if (agentId == 0) {
				// curr_player = mouse_loc[agentId];			fprintf(stderr, "checking %d %d\n", x, y);
				mouse_loc[0][1] = y;
				mouse_loc[0][0] = x;
			} else {
				cat_loc[agentId-1][1] = y;
				cat_loc[agentId-1][0] = x;
			}
			// fprintf(stderr, "%d value: %f\n", i, childVal);
		}
	}
	// fprintf(stderr, "test7\n");
	if (depth == 0) {
		// for (int i = 0; i < 4; i++) {
		// 	fprintf(stderr, "%f VALUES\n", vals[i][0]);
		// }
		// exit(0);
		
		// fprintf(stderr, "val %f, (%d, %d)\n", val, optX, optY);

		// if (start < 3) {
		// 	start++;
		// }
		// else {
		// 	exit(1);
		// }
	}
	
	for (int i = 0; i < 4; i++) {
		if (vals[i][0] != 10000.0 || vals[i][0] != -10000.0) {
			if (agentId == 0) {
				if (val < vals[i][0]) {
					val = vals[i][0];
					optX = vals[i][1];
					optY = vals[i][2];
				}
			} else {
				if (val > vals[i][0]) {
					val = vals[i][0];
					optX = vals[i][1];
					optY = vals[i][2];
				}
			}
		}
	}

	path[0][0] = optX;
	path[0][1] = optY;
	for (int i = 0; i < 4; i++) {
		fprintf(stderr, "%f VALUES\n", vals[i][0]);
	}
	if (agentId == 0) {
		fprintf(stderr, "max node, returning %f\n", val);
	} else {
		fprintf(stderr, "min node, returning %f\n", val);
	}
	minmax_cost[curr_player[0]][curr_player[1]] = val;
	return val;
}

double utility(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4])
{
	/*
	This function computes and returns the utility value for a given game configuration.
	As discussed in lecture, this should return a positive value for configurations that are 'good'
	for the mouse, and a negative value for locations that are 'bad' for the mouse.

	How to define 'good' and 'bad' is up to you. Note that you can write a utility function
	that favours your mouse or favours the cats, but that would be a bad idea... (why?)

	Input arguments:

		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		depth - current search depth
		gr - The graph's adjacency list for the maze

		These arguments are as described in A1. Do have a look at your solution!
	*/
	int visit_order[32][32];
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			visit_order[i][j] = 0;
		}
	}
	int path[1024][2];
	for (int i = 0; i < 1024; i++) {
		path[i][0] = -1;
		path[i][1] = -1;
	}
	int mouse_len = search(gr, path, visit_order, cat_loc, cats, cheese_loc, cheeses, mouse_loc);
	// mouse_len = 64 - mouse_len;
	int cat_len;
	int catnum[1][2];
	int ignoreCats[1][2] = {{-1, -1}};
	int bestest = 1000000;
	for (int i = 0; i < cats; i++) {
		catnum[0][0] = cat_loc[i][0];
		catnum[0][1] = cat_loc[i][1];
		cat_len = search(gr, path, visit_order, ignoreCats, 0, mouse_loc, 1, catnum);
		if (bestest > cat_len) {
			bestest = cat_len;
		}
	}

	return 128-mouse_len;  // <--- Obviously, this will be replaced by your computer utilities
}

int checkForTerminal(int mouse_loc[1][2],int cat_loc[10][2],int cheese_loc[10][2],int cats,int cheeses)
{
 /* 
   This function determines whether a given configuration constitutes a terminal node.
   Terminal nodes are those for which:
     - A cat eats the mouse
     or
     - The mouse eats a cheese
   
   If the node is a terminal, the function returns 1, else it returns 0
 */

 // Check for cats having lunch
 for (int i=0; i<cats; i++)
  if (mouse_loc[0][0]==cat_loc[i][0]&&mouse_loc[0][1]==cat_loc[i][1]) return(1);

 // Check for mouse having lunch
 for (int i=0; i<cheeses; i++)
  if (mouse_loc[0][0]==cheese_loc[i][0]&&mouse_loc[0][1]==cheese_loc[i][1]) return(2);

 return(0);

}

