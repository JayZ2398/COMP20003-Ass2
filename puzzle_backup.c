#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>


/**
 * READ THIS DESCRIPTION
 *
 * node data structure: containing state, g, f
 * you can extend it with more information if needed
 */
typedef struct node{
	int state[16];
	int g;
	int f;
} node;

/* Jack's functions */
void copy_state(node* to, node* from);
int min(int a, int b);

/**
 * Global Variables
 */

// used to track the position of the blank in a state,
// so it doesn't have to be searched every time we check if an operator is applicable
// When we apply an operator, blank_pos is updated
int blank_pos;

// Initial node of the problem
node initial_node;

// Statistics about the number of generated and expendad nodes
unsigned long generated;
unsigned long expanded;
unsigned long level;

/**
 * The id of the four available actions for moving the blank (empty slot). e.x.
 * Left: moves the blank to the left, etc.
 */

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

#define TILES 16
#define INFTY INT_MAX
#define MIN_MOVES 2
#define MAX_MOVES 4
#define CHARS 4 // maybe remove?

/*
 * Helper arrays for the applicable function
 * applicability of operators: 0 = left, 1 = right, 2 = up, 3 = down
 */
int ap_opLeft[]  = { 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 };
int ap_opRight[]  = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
int ap_opUp[]  = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int ap_opDown[]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
int *ap_ops[] = { ap_opLeft, ap_opRight, ap_opUp, ap_opDown };

char *moves[CHARS] = {"LEFT", "RIGHT", "UP", "DOWN"};

/* print state */
void print_state( int* s )
{
	int i;

	for( i = 0; i < 16; i++ )
		printf( "%2d%c", s[i], ((i+1) % 4 == 0 ? '\n' : ' ') );
}

void printf_comma (long unsigned int n) {
    if (n < 0) {
        printf ("-");
        printf_comma (-n);
        return;
    }
    if (n < 1000) {
        printf ("%lu", n);
        return;
    }
    printf_comma (n/1000);
    printf (",%03lu", n%1000);
}

/* return the sum of manhattan distances from state to goal */
int manhattan( int* state )
{
	int sum = 0, i = 0, pi, fi, taxicab;

	for (i = 0; i < TILES; i++) {
		pi = i, fi = state[i];
		// Disregard blank misplacement
		if (fi == 0) continue;
		// Compute Manhattan dist (distance between present and final rows and cols)
		taxicab = abs((fi / 4) - (pi / 4)) + abs((fi % 4) - (pi % 4));
		sum += taxicab;
	}

	return( sum );
}


/* return 1 if op is applicable in state, otherwise return 0 */
int applicable( int op )
{
       	return( ap_ops[op][blank_pos] );
}


/* apply operator */
void apply( node* n, int op )
{
	int t;

	//find tile that has to be moved given the op and blank_pos
	t = blank_pos + (op == 0 ? -1 : (op == 1 ? 1 : (op == 2 ? -4 : 4)));

	//apply op
	n->state[blank_pos] = n->state[t];
	n->state[t] = 0;

	//update blank pos
	blank_pos = t;
}

/* Recursive IDA */
node* ida( node* node, int threshold, int* newThreshold )
{
	struct node *new = NULL;
	struct node *r = NULL;
	int blank_pos_backup = blank_pos;

	// Generate possible actions
	int *ap_actions = malloc(MIN_MOVES * sizeof(int));
	int actions = 0, i;
	for (i = 0; i < MAX_MOVES; i++) {
		if (applicable(i)) {
			// Increase size of actions array if necessary
			if (actions >= 2) {
				ap_actions = realloc(ap_actions, (actions+1)*sizeof(int));
			}
			ap_actions[actions++] = i;
		}
	}

	// For each action
	for (i = 0; i < actions; i++) {
		// Keep track of original blank_pos
		blank_pos = blank_pos_backup;
		// Generate the new node, its cost (n.g + 1) and its estimated cost f = g+h
		new = malloc(sizeof(struct node));
		generated++;
		copy_state(new, node);
		apply(new, ap_actions[i]);
		new->g = node->g + 1;
		new->f = new->g + manhattan(new->state);

		// If f(n') is greater than the threshold
		if (new->f > threshold) {
			// Update *newThreshold to min(f(n'), *newThreshold)
			*newThreshold = min(*newThreshold, new->f);
		}
		// Otherwise, investigate
		else {
			// If heuristic is zero, solution found, return n'
			if (new->f == new->g) return new;
			// Run IDA on n' with B and B'
			expanded++; level++;
			r = ida(new, threshold, newThreshold);
			level--;
			// If r isn't NULL (i.e. found solution), return r
			if (r != NULL) return r;
		}
	}
	free(new);
	blank_pos = blank_pos_backup;
	return( NULL );
}


/* main IDA control loop */
int IDA_control_loop(  ){
	node* r = NULL;

	int threshold, newThreshold;

	/* initialize statistics */
	generated = 0;
	expanded = 0;
	level = 0;

	/* compute initial threshold B */
	initial_node.f = threshold = manhattan( initial_node.state );

	printf( "Initial Estimate = %d\nThreshold = \n", threshold );

	while (r == NULL) {
		printf("### Threshold = %d | blank_pos = %d \n# Initial node:\n", threshold, blank_pos);
		print_state(initial_node.state);
		newThreshold = INFTY;
		// need to reassign intial state to initial_node here?
		initial_node.g = 0;
		r = ida(&initial_node, threshold, &newThreshold);
		if (r == NULL) threshold = newThreshold;
	}

	if(r) {
		print_state(r->state);
		return r->g;
	}	else
		return -1;
}


static inline float compute_current_time()
{
	struct rusage r_usage;

	getrusage( RUSAGE_SELF, &r_usage );
	float diff_time = (float) r_usage.ru_utime.tv_sec;
	diff_time += (float) r_usage.ru_stime.tv_sec;
	diff_time += (float) r_usage.ru_utime.tv_usec / (float)1000000;
	diff_time += (float) r_usage.ru_stime.tv_usec / (float)1000000;
	return diff_time;
}

int main( int argc, char **argv )
{
	int i, solution_length;

	/* check we have a initial state as parameter */
	if( argc != 2 )
	{
		fprintf( stderr, "usage: %s \"<initial-state-file>\"\n", argv[0] );
		return( -1 );
	}


	/* read initial state */
	FILE* initFile = fopen( argv[1], "r" );
	char buffer[256];

	if( fgets(buffer, sizeof(buffer), initFile) != NULL ){
		char* tile = strtok( buffer, " " );
		for( i = 0; tile != NULL; ++i )
			{
				initial_node.state[i] = atoi( tile );
				blank_pos = (initial_node.state[i] == 0 ? i : blank_pos);
				tile = strtok( NULL, " " );
			}
	}
	else{
		fprintf( stderr, "Filename empty\"\n" );
		return( -2 );

	}

	if( i != 16 )
	{
		fprintf( stderr, "invalid initial state\n" );
		return( -1 );
	}

	/* initialize the initial node */
	initial_node.g=0;
	initial_node.f=0;

	print_state( initial_node.state );


	/* solve */
	float t0 = compute_current_time();

	solution_length = IDA_control_loop();

	float tf = compute_current_time();

	/* report results */
	printf( "\nSolution = %d\n", solution_length);
	printf( "Generated = ");
	printf_comma(generated);
	printf("\nExpanded = ");
	printf_comma(expanded);
	printf( "\nTime (seconds) = %.2f\nExpanded/Second = ", tf-t0 );
	printf_comma((unsigned long int) expanded/(tf+0.00000001-t0));
	printf("\n\n");

	/* aggregate all executions in a file named report.dat, for marking purposes */
	FILE* report = fopen( "report.dat", "a" );

	fprintf( report, "%s", argv[1] );
	fprintf( report, "\n\tSoulution = %d, Generated = %lu, Expanded = %lu", solution_length, generated, expanded);
	fprintf( report, ", Time = %f, Expanded/Second = %f\n\n", tf-t0, (float)expanded/(tf-t0));
	fclose(report);

	return( 0 );
}

/* Jack's functions */
void copy_state(node* to, node* from) {
	int i;
	for (i = 0; i < TILES; i++) to->state[i] = from->state[i];
	return;
}

int min(int a, int b) {
	return (a > b) ? b : a;
}
