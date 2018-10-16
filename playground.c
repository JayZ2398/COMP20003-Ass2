// cd command for cygwin because it's quite useless
// cd '/cygdrive/c/Users/Jack Zezula/Documents/GitHub/COMP20003-Ass2'

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define INFTY INT_MAX
#define TILES 16

int blank_pos;
struct node initial_node;

typedef struct node{
	int state[16];
	int g;
	int f;
} node;

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

/*Compute the linear conflicts for a given state for the heuristic function.*/
int linear_conflicts(int *state) {
	int sum = 0, i;
	int tile, above, prev = INFTY;

  for (i = 0; i < TILES; i++) {
    tile = state[i];
    // Detect conflicts in row
    if (tile == prev - 1 && tile/4 == i/4) {
      sum += 2;
      printf("Yay\n");
    }
    // Detect conflicts in columns
    if (i > 4) {
      above = state[i - 4];
      if (tile == above - 4) {
        sum += 2;
      }
    }
    prev = tile;
  }

	return sum;
}

int main( int argc, char **argv )
{
	int i, conflicts;

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

  conflicts = linear_conflicts(initial_node.state);

  printf("Conflicts: %d\n", conflicts);

	return( 0 );
}

/*
// Skip testing move if not applicable
if (!applicable(move)) {
  continue;
}

// To-be-moved tile taxicab dist
slot = blank_pos;
t = blank_pos + (move == 0 ? -1 : (move == 1 ? 1 : (move == 2 ? -4 : 4)));
h1 = taxicab(t, node->state[t]);

// Generate the new node, its cost (n.g + 1) and its estimated cost f = g+h
generated++;
apply(node, move);

// Moved tile taxicab dist
h2 = taxicab(slot, node->state[slot]);

node->g ++;
node->f = node->g - h1 + h2;
printf("node->f = %d\n", node->f);
*/
