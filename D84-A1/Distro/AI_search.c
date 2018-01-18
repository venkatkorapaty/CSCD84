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

#pragma region dead code
typedef struct LList{
	struct LList* next;
	struct LList* prev;
	struct LList* last;
	int value;
};

void addList(LList** list, int value, LList* prev) {
	// initialize node to add
	LList* node = (LList*)malloc(sizeof(LList));
	node->value = value;
	node->next = NULL;
	node->last = NULL;
	if (prev != NULL) {
		node->prev = prev;
	} else {
		node->prev = NULL;
	}
	LList* temp = *list;
	if (temp == NULL) {
		(*list) = node;
		node->last = node;
		return;
	}
	temp->last = node;
	if (temp->next == NULL) {
		temp->next = node;
	} else {
		// traverse to end of list
		while(temp->next != NULL) {

			temp = temp->next;
		}
	}
	temp->next = node;

	

}

LList* queuePop(LList** list) {
	if ((*list) == NULL) {
		return NULL;
	}
	LList* temp = *list;
	if ((*list)->next != NULL) {
		*list = (*list)->next;
	}
	else {
		*list = NULL;
	}

	temp->next = NULL;
	return temp;
}

LList* stackPop(LList** list) {
	if ((*list) == NULL) {
		return NULL;
	}
	LList* temp = *list;

	if (temp->next == NULL) {
		*list = NULL;
		return temp;
	}
	while (temp->next->next != NULL) {
		temp = temp->next;
	}
	LList* ret = temp->next;
	temp->next = NULL;
	return ret;
	
}


void printQueue(LList** list) {
	LList* temp = (*list);
	if (temp == NULL) {
		fprintf(stderr, "lsit is empty!\n");
	} else {
		int i = 0;
		while (temp != NULL) {
			fprintf(stderr, "index %d with value %d\n", i, temp->value);
			i++;
			temp = temp->next;
		}
	}
	return;
}

void traceBack(LList** end, int path[1024][2]) {
	int i = 0;
	int reversePath[1024];
	LList* temp = (*end);
	if (temp == NULL) {
		fprintf(stderr, "lsit is empty!\n");
	} else {
		while (temp != NULL) {
			reversePath[i] = temp->value;
			temp = temp->prev;
			i++;
		}
	}
	int x;
	int y;
	for (int j = 0; j < i; j++) {
		x = reversePath[j]%size_X;;
		y = reversePath[j]/size_Y;
		path[i - j - 1][0] = x;
		path[i - j - 1][1] = y;
	}
	for (int j = i; j < i + 20; j++) {
		path[j][0] = reversePath[0]%size_X;
		path[j][1] = reversePath[0]/size_Y;
	}

	/*for (int j = 0; j < i + 20; j++) {
		traced[indexed + j][0] = path[j][0];
		traced[indexed + j][1] = path[j][1];
	}
	indexed = indexed + i + 20;*/
	// exit(1);
	

	// for (int j = 0; j < i; j++) {
	// 	fprintf(stderr, "path (%d, %d)\n", path[j][0]	indexed = indexed + i + 20;, path[j][1]);
	// }
	// exit(1);
	return;
}
#pragma endregion

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
			fprintf(stderr, "CHEESE LOCATION FOUND: (%d, %d)\n", x, y);
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
			fprintf(stderr, "found a cheese\n");
			return i;
		}
		i++;
	}
	return -1;
}

static int traced[1044][2];
static int indexed;

void traceBack2(int pred[1024], int path[1024][2], int current, int mouse_loc[1][2]) {
	int i = 0;
	int reversePath[1024];

	while (pred[current] != -1 ) {
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

int cmp_paths(int p1[graph_size][2], int p2[graph_size][2]) {
	for (int i = 0; i < graph_size; i++) {
		if (p1[i][0] != p2[i][0] || p1[i][1] != p2[i][1]) {
			return 0;
		}
	}
	return 1;
}


static int prevPath[1024][2];

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
&& prevX != xCord && prevY != yCord
		(all of the above are *integer* operations!)

		A path is a sequence of (x,y) grid locations. We store it using an array of size	
		1024 x 2 (since there are 1024 locations, this is the maximum length of any
		path that visits locations only once).

		Agent locations are coordinate pairs (x,y)	

	Arguments:indexed
			gr[graph_size][4]   - This is an adjacency list for the maze
			path[graph_size][2] - An initially empty path for your code to fill.
						In this case, empty means all entries are in
		LList** queRef = &queue;
		queue = (LList*)malloc(sizeof(LList));
		printQueue(queRef);itially -1
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
	*				expansion, do so in the order top, right, bottom,
		LList** queRef = &queue;
		queue = (LList*)malloc(sizeof(LList));
		printQueue(queRef); left.
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

	// Stub so that the code compiles/runs - The code below will be removed and replaced by your code!

	
	// if (mode == 1) {
	// 	struct LList* queue = NULL;indexed
	// 	LList** queRef = &queue;
	// 	addList(queRef, mouse_loc[0][0] + (32 * mouse_loc[0][1]), NULL);
	// 	addList(queRef, 101, NULL);
	// 	addList(queRef, 204, NULL);
	// 	printQueue(queRef);
	// 	fprintf(stderr, "\n");
	// 	queuePop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	addList(queRef, 234, NULL);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	queuePop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	queuePop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	queuePop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	queuePop(queRef);
	// 	printQueue(queRef);

	// 	addList(queRef, mouse_loc[0][0] + (32 * mouse_loc[0][1]), NULL);
	// 	addList(queRef, 101, NULL);
	// 	addList(queRef, 204, NULL);
	// 	printQueue(queRef);
	// 	fprintf(stderr, "\n");
	// 	stackPop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	addList(queRef, 234, NULL);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	stackPop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	stackPop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	stackPop(queRef);
	// 	printQueue(queRef);

	// 	fprintf(stderr, "\n");
	// 	stackPop(queRef);
	// 	printQueue(queRef);
	// 	exit(1);
	// }


	//exit(1);

	// fprintf(stderr, "during trace path second step is (%d, %d)\n", traced[1][0], traced[1][1]);
	// BFS

/*
	if ((is_beside_cheese(mouse_loc[0][0],  mouse_loc[0][1] - 1, cheese_loc, cheeses) != -1 && gr[mouse_loc[0][0] + (mouse_loc[0][1]*size_Y)][0])
	|| (is_beside_cheese(mouse_loc[0][0] + 1,  mouse_loc[0][1], cheese_loc, cheeses) != -1 && gr[mouse_loc[0][0] + (mouse_loc[0][1]*size_Y)][1])
	|| (is_beside_cheese(mouse_loc[0][0],  mouse_loc[0][1] + 1, cheese_loc, cheeses) != -1 && gr[mouse_loc[0][0] + (mouse_loc[0][1]*size_Y)][2])
	||  (is_beside_cheese(mouse_loc[0][0] - 1,  mouse_loc[0][1], cheese_loc, cheeses) != -1) && gr[mouse_loc[0][0] + (mouse_loc[0][1]*size_Y)][3]) {
		// memset(traced, 0, sizeof(array[0][0]) * 1034 * 2);
		fprintf(stderr, "resetting\n");
		for(int j = 0; j < 2; j++)
		{
			for(int i = 0; i < 1034; i++)
			{  
				traced[i][j] = 0;
			}
		}

		for(int j = 0; j < size_Y; j++)
		{
			for(int i = 0; i < size_X; i++)
			{  
				visited[i + (32*j)] = 0;
			}
		}
		indexed = 0;
	}
	for (int i = 0; i < indexed; i++) {
		int x = traced[i][0];
		int y = traced[i][1];
		
		visited[x + (y*size_Y)] = 1;
		if (x == mouse_loc[0][0] && y == mouse_loc[0][1]) {
			indexed = i;
			break;
		}
	}*/
	fprintf(stderr, "MOUSE_LOC: (%d, %d)\n", mouse_loc[0][0], mouse_loc[0][1]);
	if (mode == 0 || mode == 1) {
		int visited[graph_size];
		int pred[graph_size];
		
		int v = 1;
		struct LList* queue;
		int queueMain[graph_size];
		int queueIndex = 0;
		int stackIndex = 0;
		
		for (int i = 0; i < graph_size; i++) {
			visited[i] = 0;
			pred[i] = -1;
		}
		int found = 0;
		int current;
		queueMain[stackIndex] = mouse_loc[0][0] + (size_Y*mouse_loc[0][1]);
		visited[mouse_loc[0][0] + (size_Y*mouse_loc[0][1])] = 1;
		stackIndex++;

		while (queueIndex < stackIndex && cheeses > 0) {
			// fprintf(stderr, "here1\n");

			LList* curr;
			
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

			int xCord = current % size_X;
			int yCord = current / size_Y;
			// fprintf(stderr, "checking values (%d, %d), and (%d, %d)\n", xCord, yCord, xCord2, yCord2);
			// fprintf(stderr, "here2\n");
			//fprintf(stderr, "# OF CHEESE: %d\n", cheeses);
			int catCheeseLoc = is_cat_or_cheese(xCord, yCord , cat_loc, cats, cheese_loc, cheeses);
			if (catCheeseLoc == CHEESE) {
				//trace backthe array
				if (mouse_loc[0][0] == xCord && mouse_loc[0][1] == yCord) {
					fprintf(stderr, "WE OUT HERE\n");
					path[0][0] = mouse_loc[0][0];
					path[0][1] = mouse_loc[0][1];
					path[1][0] = mouse_loc[0][0];
					path[1][1] = mouse_loc[0][1];
				}
				else {
					traceBack2(pred, path, current, mouse_loc);
				}

				return;
			}

			visit_order[xCord][yCord] = v;
			v++;
			double *loc = gr[xCord + (yCord*size_X)];
			
			// fprintf(stderr, "here4\n");
			if (loc[0]) {
				// fprintf(stderr, "here5\n");
				if (!visited[xCord + ((yCord - 1) * size_Y)]){
					found = 1;
					int catCheeseLoc = is_cat_or_cheese(xCord, yCord - 1, cat_loc, cats, cheese_loc, cheeses);
					if (catCheeseLoc != CAT) {
						//addList(queRef, xCord + ((yCord-1)*size_X), curr);
						queueMain[stackIndex] = xCord + ((yCord - 1) * size_Y);
						visited[queueMain[stackIndex]] = 1;
						pred[queueMain[stackIndex]] = current;
						stackIndex++;
					}
					
				}
			}
			if (loc[1]) {
				// fprintf(stderr, "here5\n");
				if (!visited[(xCord + 1) + ((yCord) * size_Y)]) {
					found = 1;
					int catCheeseLoc = is_cat_or_cheese(xCord + 1, yCord, cat_loc, cats, cheese_loc, cheeses);
					if (catCheeseLoc != CAT) {
						//addList(queRef, (xCord+1) + (yCord*size_X), curr);
						queueMain[stackIndex] = (xCord+1) + (yCord*size_X);
						visited[queueMain[stackIndex]] = 1;
						pred[queueMain[stackIndex]] = current;
						stackIndex++;
					}
					
				}
			}
			if (loc[2]) {
				// fprintf(stderr, "here5\n");
				if(!visited[xCord + ((yCord + 1) * size_Y)]) {
					found =1;
					int catCheeseLoc = is_cat_or_cheese(xCord, yCord + 1, cat_loc, cats, cheese_loc, cheeses);
					if (catCheeseLoc != CAT) {
						//addList(queRef, xCord + ((yCord+1)*size_X), curr);
						queueMain[stackIndex] = xCord + ((yCord+1)*size_X);
						visited[queueMain[stackIndex]] = 1;
						pred[queueMain[stackIndex]] = current;
						stackIndex++;
					}
					
					
				}
			}
			if (loc[3]) {
				// fprintf(stderr, "here5\n");
				if(!visited[(xCord - 1) + ((yCord) * size_Y)]){
					found = 1;
					int catCheeseLoc = is_cat_or_cheese(xCord - 1, yCord, cat_loc, cats, cheese_loc, cheeses);
					 if (catCheeseLoc != CAT) {
						//addList(queRef, (xCord-1) + (yCord*size_X), curr);
						queueMain[stackIndex] = (xCord-1) + (yCord*size_X);
						visited[queueMain[stackIndex]] = 1;
						pred[queueMain[stackIndex]] = current;
						stackIndex++;
					}
				}
			}

		}

	}
	//exit(1);
	//Couldn't find the cheese, accepting our fate
	path[0][0] = mouse_loc[0][0];
	path[0][1] = mouse_loc[0][1];
	path[1][0] = mouse_loc[0][0];
	path[1][1] = mouse_loc[0][1];
	fprintf(stderr, "--------------------------------\n");
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

 return(1);		// <-- Evidently you will need to update this.
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

 return(1);		// <-- Evidently you will need to update this.
}

