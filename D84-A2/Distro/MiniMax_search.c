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

int getLocation(int coords[2]) {
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

int traceBack2(int pred[1024], int current, int origin[2]) {
	// uses the pred array to backtrack and figure out the path we found to the goal
	int i = 0;
	int reversePath[1024];

	while (pred[current] != -1 && current != getLocation(origin)) {
		reversePath[i] = current;
		current = pred[current];
		i++;
	}
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

	int heur = 99999;
	for (int i = 0; i < cheeses; i++) {
		int eu_dist = abs(x - cheese_loc[i][0]) + abs(y - cheese_loc[i][1]);
		if (heur > eu_dist) {
			heur = eu_dist;
		}
	}
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

void search(double gr[graph_size][4], int path[graph_size][2], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int *pred)
{
	// // Stub so that the code compiles/runs - The code below will be removed and replaced by your code!
	int visited[graph_size];
	// int pred[graph_size];
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

	while (queueIndex > 0/*cheeses > 0*/) {
		current = extract_min(heap, weights, actWeights, queueIndex);
		queueIndex--;
		while (heap_visited[current] == 1) {
			current = extract_min(heap, weights, actWeights, queueIndex);
			queueIndex--;
		}

		heap_visited[current] = 1;

		int xCord = current % size_X;
		int yCord = current / size_Y;

		int catCheeseLoc = is_cat_or_cheese(xCord, yCord , cat_loc, cats, cheese_loc, cheeses);

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
}

int start = 0;
static int * pred;// = (int*)malloc(graph_size*sizeof(int));
static int ** cheese_paths;// = (int**)malloc(10*sizeof(int*));
static int prevMove[10][2] = {{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1},{-1, -1}};
		// pred = (int*)malloc(graph_size*sizeof(int));
		// cheese_paths = (int**)malloc(10*sizeof(int*));

static int distanceBase = 0;
static int turns = 0;

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
static int visited[graph_size];

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

	if (depth == 0) {
		if (distanceBase == 0) {
			distanceBase = abs(mouse_loc[0][0] - cat_loc[0][0])+abs(mouse_loc[0][1] + cat_loc[0][1]);
		} else if (turns < 5) {
			int temp = abs(mouse_loc[0][0] - cat_loc[0][0])+abs(mouse_loc[0][1] + cat_loc[0][1]);
			if (abs(temp - distanceBase) > 5) {
				distanceBase = -1;
			}
			turns++;
		}
		int path1[graph_size][2];
		for (int i = 0; i < graph_size; i++) {
			path1[i][0] = -1;
			path1[i][1] = -1;
		}

		for (int i = 0; i < 10; i++) {
			if (start == 0 || cheese_loc[i][0] != prevMove[i][0] ||cheese_loc[i][1] != prevMove[i][1]) {
				if (start != 0) {
					for (int j = 0; j < cheeses; j++) {
						free(cheese_paths[j]);
					}
					free(cheese_paths);
				}
				cheese_paths = (int**)malloc(10*sizeof(int*));
				for (int j = 0; j < cheeses; j++) {
					cheese_paths[j] = (int*)malloc(graph_size*sizeof(int));
					int player[1][2];
					player[0][0] = cheese_loc[j][0];
					player[0][1] = cheese_loc[j][1];
					int cats_loc[10][2];
					for (int k = 0; k < 10; k++) {
						cats_loc[k][0] = -1;
						cats_loc[k][1] = -2;
					}
					search(gr, path1, cats_loc, 0, mouse_loc, 1, player, cheese_paths[j]);
				}
				start = 1;

				prevMove[i][0] = cheese_loc[i][0];
				prevMove[i][1] = cheese_loc[i][1];
			}

		}
		pred = (int*)malloc(graph_size*sizeof(int));
		start++;
		search(gr, path1, cat_loc, cats, cheese_loc, cheeses, mouse_loc, pred);
		int shitter2[1][2] = {{cheese_loc[0][0], cheese_loc[0][1]}};

		traceBack2(cheese_paths[0], getLocation(mouse_loc[0]), shitter2[0]);
	}

	int *curr_player;
	if (agentId == 0) {
		curr_player = mouse_loc[agentId];
	} else {
		curr_player = cat_loc[agentId - 1];
	}

	int ded = checkForTerminal(mouse_loc, cat_loc, cheese_loc, cats, cheeses);
	if (ded == 2) {
		//some high value
		if (agentId == 0) {
			minmax_cost[curr_player[0]][curr_player[1]] = 9999;
			return 9999/(depth + 1);
		}
		return 9999/(depth+1);
	} else if (ded == 1) {
		//some low value
		if (agentId == 0) {
			minmax_cost[curr_player[0]][curr_player[1]] = -9999;
			return -9999/(depth+1);
		}
		return -9999/(depth+1);
	} else if (depth == maxDepth) {
		//utility function
		int x = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
		if (agentId == 0) {
			minmax_cost[curr_player[0]][curr_player[1]] = utility(cat_loc, cheese_loc, mouse_loc, cats, cheeses, depth, gr);
			return minmax_cost[curr_player[0]][curr_player[1]];
		}
		return x;
	}

	int x = curr_player[0];
	int y = curr_player[1];
	int adj_list[4][2] = {{x, y-1},
						{x+1, y},
						{x, y+1},
						{x-1, y}};
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
	int prune[4];
	prune[0]=1;
	prune[1]=1;
	prune[2]=1;
	prune[3]=1;

	for (int i = 0; i < 4; i++) {
		if (mode == 1 && prune[i] != 1) {
			break;
		}
		if (gr[x + (y * size_Y)][i] && prune[i] == 1) {
			//check for terminal using a possible move of the current player

			if (agentId == 0) {
				mouse_loc[0][1] = adj_list[i][1];
				mouse_loc[0][0] = adj_list[i][0];
			} else {
				cat_loc[agentId - 1][1] = adj_list[i][1];
				cat_loc[agentId - 1][0] = adj_list[i][0];
			}
			int cat_agent = agentId;
			if (cat_agent < cats) {
				cat_agent++;
			} else {
				cat_agent = 0;
			}

			double childVal = MiniMax(gr, path, minmax_cost, cat_loc, cats, cheese_loc, cheeses, mouse_loc, mode, *utility, cat_agent, depth+1, maxDepth, alpha, beta);
			
			// for pruning, check if we should prune the following branches
			if (i != 3 && mode == 1) {
				// for max nodes
				if (agentId == 0) {
					if (beta < childVal) {
						prune[i + 1] = 0;
					}
				}
				// for min nodes
				else {
					if (alpha > childVal) {
						prune[i + 1] = 0;
					}
				}
			}

			if (val == 10000.0 || val == -10000.0) {
				val = childVal;
				optX = adj_list[i][0];
				optY = adj_list[i][1];
			}
			vals[i][0] = childVal;
			vals[i][1] = adj_list[i][0];
			vals[i][2] = adj_list[i][1];
			if (agentId == 0) {
				// for pruning, update our alphas for that parent node
				if (mode == 1 && childVal > alpha) {
					alpha = childVal;
				}
				mouse_loc[0][1] = y;
				mouse_loc[0][0] = x;
			} else {
				// for pruning update our betas for that parent node
				if (mode == 1 && childVal < beta) {
					beta = childVal;
				}
				cat_loc[agentId-1][1] = y;
				cat_loc[agentId-1][0] = x;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		// if we reached a point where we pruned, don't bother checking rest of values
		if (prune[i] != 1) {
			break;
		}
		if (vals[i][0] != 10000.0 || vals[i][0] != -10000.0) {
			if (agentId == 0) {
				// if(depth == 0) {
					// if (curr_player[0] == 10 && curr_player[1] == 10)
					// 	fprintf(stderr, "(10, 10), i: %d, val: (%f, %f, %f)\n", i, vals[i][0], vals[i][1], vals[i][2]);
					//else if (curr_player[0] == 13 && curr_player[1] == 12)
					//	fprintf(stderr, "(13, 10), i: %d, val: (%f, %f, %f)\n", i, vals[i][0], vals[i][1], vals[i][2]);
				// }
				if (val < vals[i][0]) {
					val = vals[i][0];
					optX = (int)vals[i][1];
					optY = (int)vals[i][2];
				}
				// we found an equally good move, pick the one
				// that's closer in path length heuristic
				else if (val == vals[i][0]) {
					int cheese_len = 10000;
					int ind = 0;
					for (int l = 0; l < cheeses; l++) {
						int temp = pow(pow(x - cheese_loc[l][0], 2) + pow(y - cheese_loc[l][1], 2), 0.5);
						if (temp < cheese_len) {
							ind = l;
						}
					}
					if (pow(pow((int)optX-cheese_loc[ind][0], 2)+pow((int)optY-cheese_loc[ind][1], 2),0.5) > pow(pow(vals[i][1]-cheese_loc[ind][0], 2)+pow(vals[i][2]-cheese_loc[ind][1], 2), 0.5)) {
						val = vals[i][0];
						optX = (int)vals[i][1];
						optY = (int)vals[i][2];
					}
				}
			} else {
				if (val > vals[i][0]) {
					val = vals[i][0];
					optX = (int)vals[i][1];
					optY = (int)vals[i][2];
				}
				// we found an equally good move, pick the one
				// that's closer in path length heuristic
				else if (val == vals[i][0]) {
					int cheese_len = 10000;
					int ind = 0;
					for (int l = 0; l < cheeses; l++) {
						int temp = abs(x - cheese_loc[l][0]) + abs(y - cheese_loc[l][1]);
						if (temp < cheese_len) {
							ind = l;
						}
					}
					if (abs((int)optX-cheese_loc[ind][0])+abs((int)optY-cheese_loc[ind][1]) < abs(vals[i][1]-cheese_loc[ind][0])+abs(vals[i][2]-cheese_loc[ind][1])) {
						val = vals[i][0];
						optX = (int)vals[i][1];
						optY = (int)vals[i][2];
					}
				}
			}
		}
	}

	if (depth == 0) {
		free(pred);
		pred = NULL;
		// fprintf(stderr, "BEST VAL: (%f, %d, %d)\n", val, optX, optY);
	}

	path[0][0] = optX;
	path[0][1] = optY;
	if (agentId == 0) {
		minmax_cost[curr_player[0]][curr_player[1]] = val;
	}
	return val;
}

int isCorneredCheese(int gr[graph_size][4], int x, int y, int chsX, int chsY) {
	int modGr[graph_size][4];

	for (int i = 0; i < graph_size; i++) {
		for (int j = 0; j < 4; j++) {
			modGr[i][j] = gr[i][j];
		}
	}

	if (x + 1 == chsX) {
		modGr[chsX + chsY*size_Y][3] = 1;
	}
	else if (x - 1 == chsX) {
		modGr[chsX + chsY*size_Y][1] = 1;
	}
	else if (y + 1 == chsY) {
		modGr[chsX + chsY*size_Y][0] = 1;
	}
	else
		modGr[chsX = chsY*size_Y][2] = 1;

	int adj_list[4][2];
	adj_list[0][0] = chsX;
	adj_list[0][1] = chsY-1;
	adj_list[1][0] = chsX+1;
	adj_list[1][1] = chsY;
	adj_list[2][0] = chsX;
	adj_list[2][1] = chsY+1;
	adj_list[3][0] = chsX-1;
	adj_list[3][1] = chsX;
	for (int i = 0; i < 4; i++) {
		if (!gr[chsX + chsY*size_Y][i]) {

		}
	}

}

int checkHallway(int freeX, int freeY, int x, int y, double gr[graph_size][4]) {
	int temp = 0;
	int adj_list[4][2];
	adj_list[0][0] = x;
	adj_list[0][1] = y-1;
	adj_list[1][0] = x+1;
	adj_list[1][1] = y;
	adj_list[2][0] = x;
	adj_list[2][1] = y+1;
	adj_list[3][0] = x-1;
	adj_list[3][1] = x;
	int nextX = -1;
	int nextY = -1;
	for (int i = 0; i < 4; i++) {
		int curr[2] = {x, y};
		if (gr[getLocation(curr)][i]) {
			temp++;
		} else if (adj_list[0][0] != freeX && adj_list[0][1] != freeY){
			nextX = adj_list[i][0];
			nextY = adj_list[i][0];
		}
	}

	if (temp == 2) {
		return 1 + checkHallway(x, y, nextX, nextY, gr);
	}

	return 0;
}

int wallz(double gr[graph_size][4], int x, int y) {
	int temp = 0;
	int freeX = -1;
	int freeY = -1;
	int adj_list[4][2];
	adj_list[0][0] = x;
	adj_list[0][1] = y-1;
	adj_list[1][0] = x+1;
	adj_list[1][1] = y;
	adj_list[2][0] = x;
	adj_list[2][1] = y+1;
	adj_list[3][0] = x-1;
	adj_list[3][1] = x;
	for (int i = 0; i < 4; i++) {
		int curr[2] = {x, y};
		if (gr[getLocation(curr)][i]) {
			temp++;
		} else {
			freeX = adj_list[i][0];
			freeY = adj_list[i][0];
		}
	}

	if (temp == 3) {
		return 1 + checkHallway(x, y, freeX, freeY, gr);
	}

	return 0;
}



double utility(int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, int depth, double gr[graph_size][4])
{
	/*
	This function computes and returns the utility value for a given game configuration.
	As discussed in lecture, this should return a positive value for configurations that are 'good'
	for the mouse, and a negative value for locations that are 'bad' for the mouse.

	How to define 'good' and 'bad' is up to you. Note that you can write a utility function
	that favours your mouse or favours the cats, but that would be a bad idea... (why?)

	Input arguments:abs(mouse_loc[0][0] - cat_loc[0][0])+abs(mouse_loc[0][1] + cat_loc[0][1])

		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
					
		cats - # of cats
		cheeses - # of cheeses
		depth - current search depth
		gr - The graph's adjacency list for the maze

		These arguments are as described in A1. Do have a look at your solution!
	*/


	int cheese_len = 10000;
	int walls = 50;
	int cheeseNum = 0;

	int cheeseWalls = 0;
	int x = -1;
	int y = -1;
	int adj_list[4][2];
	int hallDepth = 0;
	
	for (int i = 0; i < cheeses; i++) {
		int temp = 0;
		x = cheese_loc[i][0];
		y = cheese_loc[i][1];
		adj_list[0][0] = x;
		adj_list[0][1] = y-1;
		adj_list[1][0] = x+1;
		adj_list[1][1] = y;
		adj_list[2][0] = x;
		adj_list[2][1] = y+1;
		adj_list[3][0] = x-1;
		adj_list[3][1] = x;
		int choice = 0;
		int tempLen = traceBack2(cheese_paths[i], getLocation(mouse_loc[0]), cheese_loc[i]);
		
		for (int j = 0; j < 4; j++) {
			if (gr[getLocation(cheese_loc[i])][j]) {
				for (int k = 0; k < 4; k++) {
					temp += !(gr[getLocation(adj_list[j])][k]);
				}
			} else {
				temp++;
			}
		}
		if (temp < walls) {
			walls = temp;
			if (tempLen < cheese_len) {
				choice = i;

				hallDepth = wallz(gr, cheese_loc[i][0], cheese_loc[i][1]);
				cheese_len = tempLen;
			}
		}
	}
	
	//pick the closest cat

	int catChoice = 11;
	int cat_len = 10000;
	int numCloseCats = 0;
	for (int i = 0; i < cats; i++) {
		int temp = traceBack2(pred, getLocation(cat_loc[i]), mouse_loc[0]);
		if (temp < cat_len) {
			if (temp < 5) {
				numCloseCats++;
			}
			cat_len = temp;
			catChoice = i;
		}
	}

	int mouseWalls = 0;
	x = mouse_loc[0][0];
	y = mouse_loc[0][1];
	
	adj_list[0][0] = x;
	adj_list[0][1] = y-1;
	adj_list[1][0] = x+1;
	adj_list[1][1] = y;
	adj_list[2][0] = x;
	adj_list[2][1] = y+1;
	adj_list[3][0] = x-1;
	adj_list[3][1] = x;

	for (int j = 0; j < 4; j++) {
		if (gr[getLocation(mouse_loc[0])][j]) {
			for (int i = 0; i < 4; i++) {
				int catCheeseLoc = is_cat_or_cheese(adj_list[j][0], adj_list[j][1] , cat_loc, cats, cheese_loc, cheeses);
				if ( !(gr[getLocation(adj_list[j])][i]) || (catCheeseLoc == 2)) {
					mouseWalls++;
				}
			}
		} else {
			mouseWalls++;
		}
	}

	int catWalls = 0;
	x = cat_loc[catChoice][0];
	y = cat_loc[catChoice][1];
	adj_list[0][0] = x;
	adj_list[0][1] = y-1;
	adj_list[1][0] = x+1;
	adj_list[1][1] = y;
	adj_list[2][0] = x;
	adj_list[2][1] = y+1;
	adj_list[3][0] = x-1;
	adj_list[3][1] = x;

	for (int j = 0; j < 4; j++) {
		if (gr[getLocation(cat_loc[catChoice])][j]) {
			catWalls += !gr[getLocation(adj_list[0])][j];
			catWalls += !gr[getLocation(adj_list[1])][j];
			catWalls += !gr[getLocation(adj_list[2])][j];
			catWalls += !gr[getLocation(adj_list[3])][j];
		}
		
	}

	int ret = (catWalls*cat_len) - (walls*cheese_len);

	if (distanceBase > 0 && turns == 5) {
		return 100 - cheese_len;
	}

	if (numCloseCats > 2) {
		if (ret > 0) {
			return ret/mouseWalls;
		}
		return ret*mouseWalls;
	}
	return ret;
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

