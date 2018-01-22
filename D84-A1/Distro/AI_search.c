/*
	CSC D84 - Unit 1 - Search

	This file contains stubs for implementing the different search
	algorithms covered in the course. Please read the assignment
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

	Starter code: F.J.E., Jul. 15
	Updated: F.J.E., Jan. 18
*/

/**********************************************************************
% COMPLETE THIS TEXT BOX:
%
% 1) Student Name: Shevlin Bisesar	
% 2) Student Name: Venkat Korapaty
%
% 1) Student number:1001307766
% 2) Student number:1001365060
% 
% 1) UtorID: bisesars
% 2) UtorID: korapaty
% 
% We hereby certify that the work contained here is our own
%
%    Shevlin Bisesar             	  Venkat Korapaty
% (sign with your name)            (sign with your name)
***********************************************************************/

#include "AI_search.h"
#include <unistd.h>

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

int is_beside_cheese(int x, int y, int cheese_loc[10][2], int cheeses) {
	/*
	*/
	int i = 0;
	while (i < cheeses) {
		if (cheese_loc[i][0] == x && cheese_loc[i][1] == y) {
			return i;
		}
		i++;
	}
	return -1;
}

void traceBack2(int pred[1024], int path[1024][2], int current, int mouse_loc[1][2]) {
	int i = 0;
	int reversePath[1024];

	while (pred[current] != -1 ) {
		// fprintf(stderr, "CURR: (%d, %d), PRED: (%d, %d)\n", current%size_X, current/size_Y, pred[current]%size_X, pred[current]/size_Y);
		reversePath[i] = current;
		current = pred[current];
		i++;
	}

	int x;
	int y;
	for (int j = 0; j < i; j++) {
		x = reversePath[j]%size_X;
		y = reversePath[j]/size_Y;
		path[i - j][0] = x;
		path[i - j][1] = y;
	}
	path[0][0] = mouse_loc[0][0];
	path[0][1] = mouse_loc[0][1];

	// for (int j = 0; j <= i; j++) {
	// 	fprintf(stderr, "PATH: (%d, %d) \n", path[j][0], path[j][1]);
	// }
}

void addHeap(int *heap, double weights[graph_size], int actWeights[graph_size], int val, int size) {
	int parent;

	// heap[size] = loc;

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
	// lame case
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


void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]))
{
	/*
	This function is the interface between your solution for the assignment and the driver code. The driver code
	in AI_search_core_GL will call this function once per frame, and provide the following data

	Board and game layout:
		
		The game takes place on a grid of size 32x32, the file board_layout.h specifies the size and defines two
		constants 'size_X' and 'size_Y' for the horizontal and vertical size of the board, respectively. For our
		purposes, the grid of locations is represented by a graph with one node per grid location, so given
		the 32x32 cells, the graph has 1024 nodes. 

		To create a maze, we connect cell locations in the grid in such a way that a) there is a path from any
		grid location to any other grid location (i.e. there are no disconnected subsets of nodes in the graph),
		and b) there are loops.

		Since each node represents a grid location, each node can be connected to up to 4 neighbours in the
		top, right, bottom, and left directions respectively:

				node at (i,j-1)
					^
					|
		(node at i-1, j) <- node at (i,j) -> node at (i+1, j)
					|
					v
				node at (i,j+1)

		The graph is there&& prevX != xCord && prevY != yCorddore stored as an adjacency list with size 1024 x 4, with one row per node in the
		graph, and 4 columns corresponding to the weight of an edge linking the node with each of its 4
		possible neighbours in the order towp, right, bottom, left (clockwise from top).

		Since all we care is whether nodes are connected. Weights will be either 0 or 1, if the weight is
		1, then the neighbouring nodes are connected, if the weight is 0, they are not. For example, if

		graph[i][0] = 0 
		graph[i][1] = 1
		graph[i][2] = 0
		graph[i][3] = 1

		then node i is connected to the right and left neighbours, but not to top or bottom.

		The index in the graph for the node corresponding to grid location (x,y) is

		index = x + (y*size_X) 		or in this case		index = x + (y*32)

		Conversely, if you have the index and want to figure out the grid location,

		x = index % size_X		or in this case		x = index % 32
		y = index / size_Y		or in this case		y = index / 32
		(all of the above are *integer* operations!)

		A path is a sequence of (x,y) grid locations. We store it using an array of size	
		1024 x 2 (since there are 1024 locations, this is the maximum length of any
		path that visits locations only once).

		Agent locations are coordinate pairs (x,y)	

	Arguments:indexed
			gr[graph_size][4]   - This is an adjacency list for the maze
			path[graph_size][2] - An initially empty path for your code to fill.
						In this case, empty means all entries are initially -1
			visit_order[size_X][size_Y]	- An array in which your code will store the
							*order* in which grid locations were 
							visited during search. For example, while
							doing BFS, the initial location is the
							start location, it's visit order is 1.
							Thenpath[0][0] = mouse_loc[0][0];
		path[0][1] = mouse_loc[0][1]; the search would expand the immediate
							neighbours of the initial node in some order,
							these would get a visit order of 2, 3, 4, and
							5 respectively, and so on.
						
							This array will be used to display the search
							pattern generated by each search method.

			cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
						but there can be fewer). Only valid cat locations are 0 to (cats-1)
			cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
							but possibly fewer). Valid locations are 0 to (cheeses-1)
			mouse_loc[1][2] - Mouse location - there can be only one!
			mode - Search mode selection:
						mode = 0 	- BFS
						mode = 1	- DFS
						mode = 2	- A*

			(*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[10][2], int cats, int cheeses)
					- This is a pointer to one of the heuristic functions you will implement, either H_cost()
					or H_cost_nokitty(). The driver in AI_search_core_GL will pass the appropriate pointer
					depending on what search the user wants to run.
				
					If the mode is 0 or 1, this pointer is NULL

					* How to call the heuristic function from within this function : *
						- Like any other function:
							h = heuristic( x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses);

	Return values:
			Your search code will directly update data passedfprintf(stderr, "test!\n");-in as arguments:

			- path[graph_size][2]	: Your search code will update this array to contain the path from
						the mouse to one of the cheese chunks. The order matters, so
						path[0][:] must be the mouse's current location, path[1][:]
						is the next move for the mouse. Each successive row will contain
						the next move toward the cheese, and the path ends at a location
						whose coordinates correspond to one of the cheese chunks.
						Any entries beyond that must remain set to -1
			- visit_order[size_X][size_Y] 	:  Your search code will update this array to contain the
							order in which each location in the grid was expanded
							during search. This means, when that particular location
							was checked for being a goal, and if not a goal, had its
							neighbours added as candidates for future expansion (in
							whatever order is dictated by the search mode).

							Note that since there are 1024 locations, the values in 
							this array must always be in [0, 1023]. The driver code
							will then display search order as a yellow-colored
							brightness map where nodes expanded earlier will look
							brighter.

			* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
				or the location of the mouse - if you try, the driver code will know it *
				
			That's that, now, implement your solution!
	*/
	/********************************************************************************************************
	 * 
	 * TO DO:	Implement code to carry out the different types of search depending on the
	 *		mode.
	*
	*		You can do this by writing code within this single function (and being clever about it!)
	*		Or, you can use this function as a wpath[0][0] = mouse_loc[0][0];
		path[0][1] = mouse_loc[0][1];rapper that calls a different search function
	*		 (BFS, DFS, A*) depending on the mode. Note that in the latter case, you will have
	*	    	  to inform your A* function somehow of what heuristic it's supposed to use.
	*
	*		Visiting Order: When adding the neighbours of a node to your list of candidates for
	*				expansion, do so in the order top, right, bottom, left.
	*
	*		NOTE: Your search functions should be smart enough to not choose a path that goes
	*		      through a cat! this is easily done without any heuristics.
	* 
	*		How you design your solution is up to you. But:
	*
	*		- Document your implementation by addingarray concise and clear comments in this file
	*		- Document your design (how you implemented the solution, and why) in the report
	*
	********************************************************************************************************/

	// // Stub so that the code compiles/runs - The code below will be removed and replaced by your code!
	int visited[graph_size];
	int pred[graph_size];
	for (int i = 0; i < graph_size; i++) {
		visited[i] = 0;
		pred[i] = -1;
	}


	// fprintf(stderr, "MOUSE_LOC: (%d, %d)\n", mouse_loc[0][0], mouse_loc[0][1]);
	if (mode >= 0 && mode <= 3) {
		
		
		
		int v = 1;
		int queueMain[graph_size];
		int queueIndex = 0;
		int stackIndex = 0;
		int *heap = (int*)malloc(sizeof(int)*(graph_size)*2);
		double weights[graph_size];
		int actWeights[graph_size];
		int heap_visited[graph_size];
		for (int i = 0; i < graph_size; i++) {
			actWeights[i] = 0;
			heap_visited[i] = 0;
		}
		actWeights[mouse_loc[0][0] + (mouse_loc[0][1]*size_Y)] = -1;
		if (mode == 2 || mode == 3 ) {
			for (int i = 0; i < graph_size; i++) {
				heap[i] = -1;
			}
			addHeap(heap, weights, actWeights, mouse_loc[0][0] + mouse_loc[0][1]*size_X, queueIndex);
			visited[mouse_loc[0][0] + (size_Y*mouse_loc[0][1])] = 1;
			queueIndex++;
		} else {
			queueMain[stackIndex] = mouse_loc[0][0] + (size_Y*mouse_loc[0][1]);
			visited[mouse_loc[0][0] + (size_Y*mouse_loc[0][1])] = 1;
			stackIndex++;
		}

		int current = -1;
		

		while ((((queueIndex < stackIndex)  && (mode == 1 || mode == 0)) || mode == 2 || mode == 3) && cheeses > 0) {
			if (mode == 0) {
				current = queueMain[queueIndex];
				queueIndex++;
			}
			else if (mode == 1) {
				current = queueMain[stackIndex - 1];
				if (stackIndex != 0) {
					stackIndex--;
				}
			}
			else if (mode == 2 || mode == 3) {
				current = extract_min(heap, weights, actWeights, queueIndex);
				queueIndex--;
				if (current == -1) {
					// Couldn't find the cheese, accepting our fate from the fur devils
					path[0][0] = mouse_loc[0][0];
					path[0][1] = mouse_loc[0][1];
					path[1][0] = mouse_loc[0][0];
					path[1][1] = mouse_loc[0][1];
					free(heap);
					return;
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
						return;
					}
				}

				heap_visited[current] = 1;
				
			}

			int xCord = current % size_X;
			int yCord = current / size_Y;
			int catCheeseLoc = is_cat_or_cheese(xCord, yCord , cat_loc, cats, cheese_loc, cheeses);
			if (catCheeseLoc == CHEESE) {
				//trace backthe array
				if (mouse_loc[0][0] == xCord && mouse_loc[0][1] == yCord) {
					path[0][0] = mouse_loc[0][0];
					path[0][1] = mouse_loc[0][1];
					path[1][0] = mouse_loc[0][0];
					path[1][1] = mouse_loc[0][1];
				}
				else {
					traceBack2(pred, path, current, mouse_loc);
				}
 				if (mode == 2 || mode == 3)
					free(heap);
				return;
			}
			visit_order[xCord][yCord] = v;
			v++;
			double *loc = gr[xCord + (yCord*size_X)];
			int adj_Cords[4][2] = {{xCord, yCord-1},
			{xCord + 1, yCord},
			{xCord, yCord + 1},
			{xCord - 1, yCord}
			};

			for (int i = 0; i < 4; i++) {
				if (loc[i]) {
					if(!visited[(adj_Cords[i][0]) + ((adj_Cords[i][1]) * size_Y)]){
						int catCheeseLoc = is_cat_or_cheese(adj_Cords[i][0], adj_Cords[i][1], cat_loc, cats, cheese_loc, cheeses);
						if (catCheeseLoc != CAT) {
							if (mode == 0 || mode == 1) {
								queueMain[stackIndex] = (adj_Cords[i][0]) + (adj_Cords[i][1]*size_X);
								visited[queueMain[stackIndex]] = 1;
								pred[queueMain[stackIndex]] = current;
								stackIndex++;
							}
							else {
								visited[(adj_Cords[i][0]) + (adj_Cords[i][1] * size_X)] = 1;
								pred[(adj_Cords[i][0]) + (adj_Cords[i][1] * size_X)] = current;
								if (actWeights[xCord + (yCord*size_Y)] == -1) {
									actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] = 1;
								} else {
									actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] = 1 + actWeights[xCord + (yCord*size_Y)];
								}
								weights[(adj_Cords[i][0]) + (adj_Cords[i][1] * size_X)] = heuristic(adj_Cords[i][0], adj_Cords[i][1], cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
								addHeap(heap, weights, actWeights, (adj_Cords[i][0]) + (adj_Cords[i][1] * size_X), queueIndex);
								queueIndex++;
							}
						}
					} else if ((mode == 2 || mode == 3)  && actWeights[xCord + ((yCord)*size_Y)] == -1) {
						if (actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] > 1 + actWeights[xCord + (yCord*size_Y)]) {
							actWeights[adj_Cords[i][0] + ((adj_Cords[i][1]) * size_Y)] = 1 + actWeights[xCord + (yCord*size_Y)];
							pred[(adj_Cords[i][0]) + adj_Cords[i][1] * size_X] = current;
							addHeap(heap, weights, actWeights, (adj_Cords[i][0]) + (adj_Cords[i][1] * size_X), queueIndex);
							queueIndex++;
						}
					}
				}
			}
			// }
			// else if (mode == 1) {
			// 	for (int i = 3; i > -1; i--) {
			// 		if (loc[i]) {
			// 			if(!visited[(adj_Cords[i][0]) + ((adj_Cords[i][1]) * size_Y)]){
			// 				int catCheeseLoc = is_cat_or_cheese(adj_Cords[i][0], adj_Cords[i][1], cat_loc, cats, cheese_loc, cheeses);
			// 				if (catCheeseLoc != CAT) {
			// 					queueMain[stackIndex] = (adj_Cords[i][0]) + (adj_Cords[i][1]*size_X);
			// 					visited[queueMain[stackIndex]] = 1;
			// 					pred[queueMain[stackIndex]] = current;
			// 					stackIndex++;
			// 				}
			// 			}
			// 		}
			// 	}
			// }

		}

	}
	//Couldn't find the cheese, accepting our fate from the devils of fur
	path[0][0] = mouse_loc[0][0];
	path[0][1] = mouse_loc[0][1];
	path[1][0] = mouse_loc[0][0];
	path[1][1] = mouse_loc[0][1];

	return;
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
		int xCord = abs(x - cheese_loc[i][0]);
		int yCord = abs(y - cheese_loc[i][1]);
		int eu_dist = (int)pow(xCord*xCord + yCord*yCord, 0.5);
		if (heur > eu_dist) {
			heur = eu_dist;
		}
	}
	fprintf(stderr, "heur: %d\n", heur);
	return heur;
}

int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
	/*
		This function computes and returns the heuristic cost for location x,y.
		As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
		The goal is cheese. 

		However - this time you want your heuristic function to help the mouse avoid being eaten.
		Therefore: You have to somehow incorporate knowledge of the cats' locations into your
		heuristic cost estimate. How well you do this will determine how well your mouse behaves
		and how good it is at escaping kitties.

		This heuristic *does not have to* be admissible.

		Input arguments have the same meaning as in the H_cost() function above.
	*/
	int normCost = H_cost(x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);
	normCost++;
	int heur = 9999;
	int closeCats = 0;
	int deadlyCats = 0;
	int warningCats = 0;
	int closestDist = 99;
	for (int i = 0; i < cats; i++) {
		int xCord = abs(x - cat_loc[i][0]);
		int yCord = abs(y - cat_loc[i][1]);
		if (xCord + yCord <= 5) {
			deadlyCats++;
		} else if (xCord + yCord <= 10) {
			closeCats++;
		} else if (xCord + yCord <= 20) {
			warningCats++;
		}

		if (closestDist > xCord + yCord) {
			closestDist = xCord + yCord;
		}
	}

	int catMultiplier = deadlyCats*10 + closeCats*3 + abs(50 - closestDist);

	int closestCheese = 9999;
	int closeCheeses = 0;
	for (int i = 0; i < cheeses; i++) {
		int xCord = abs(x - cheese_loc[i][0]);
		int yCord = abs(y - cheese_loc[i][1]);
		if (xCord + yCord <= 5) {
			closeCheeses++;
		}
		if (closestCheese > xCord + yCord) {
			closestCheese = xCord + yCord;
		}
	}

	int cheeseMultiplier = closeCheeses*3 + abs(30 - closestCheese);




	for (int i = 0; i < cheeses; i++) {
		int man_hat_dist = abs(x - cheese_loc[i][0]) + abs(y - cheese_loc[i][1]);

		man_hat_dist = 4 * man_hat_dist;
		int t = 0;

		for( int i = 0; i < 4;i++) {
			t += gr[cheese_loc[i][0] + cheese_loc[i][1]*size_X][i];
		}
		if (t == 0)
			man_hat_dist = 10*man_hat_dist;
		else {
			man_hat_dist = man_hat_dist / t;
		}
		if (man_hat_dist < heur) {
			heur = man_hat_dist;
		}
	}
	if (cheeseMultiplier == 0) {
		cheeseMultiplier == 1;
	}
	fprintf(stderr, "%d\n", cheeseMultiplier);
	fprintf(stderr, "%d\n", catMultiplier);
	fprintf(stderr, "%d\n", heur);
	fprintf(stderr, "%d\n", normCost);
	// exit(0);
	// fprintf(stderr, "value: %d\n", (normCost + heur)*(catMultiplier/cheeseMultiplier));
	if (cheeseMultiplier == 0) {
		return (40)*catMultiplier;
	}
	normCost += heur;
	normCost *= 100;
	normCost *= catMultiplier;
	normCost /= cheeseMultiplier;
	return normCost;
	// heur;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	// int eu = 5000 + heur;

	// return 1;
}
