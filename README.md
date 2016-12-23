# sliding-piece-puzzle
Sliding piece puzzle assignment for the Data Structures course. Brandeis University - Fall 2016

Given an MxN board and initial position, the objective of this algorithm is to reach a goal position. 
Within the board, there are fixed cells that cannot be moved.

For example, given a 3x4 board, where dollar signs represent fixed cells:

                                                  NITE
                                                  $.$.
                                                  DAY.

The algorithm finds the shortest path to a solution board, like:

                                                  DAY.
                                                  $.$.
                                                  NITE

This algorithm implements two main data structures in C:
1) A hash table containing all visited positions. Each visited position points to its "parent" position,
so that when the goal is found it can be traced back to the initial board.
2) A queue that stores reachable positions from current or past boards, not visited yet. 
Once a position is visited, it is removed from the queue.

The solution algorithm implements a breadth first search in which a position is taken from the queue,
and all reachable positions are stored in the hash table and queue. This process lasts until the goal position is taken from the queue.

When the goal position is found, the path to the original board is found by reversing the pointer to each position's parent.

The output shows all positions generated in order to reach the goal, as well as statistics of the data structures usage. For example, it shows how number of buckets in the hash table of size 0, 1, etc.
