/***************************************************************/
/*P1 assignment                                                */
/*Samuel Akerman, COSI21, 2016                                 */
/****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "Puzzle.h"
#include "Output.h"

//////////////////////////////////////////////////////////////
//Define the four possible directions as integers
#define North 0
#define East  1
#define South 2
#define West  3

/*The following typedef/malloc definitions were created by modifying code provided on the COSI21 website*/
/*Define a hash vertex as a vertex in a bucket, containing: 
1) a linked list pointer 
2) a pointer to the parent vertex
3) the piece that was moved to create this position
4) the direction in which the piece was moved
5) an array representing the current positioning of pieces*/
typedef struct hash_vertex {
	struct hash_vertex *next;      //pointer to next vertex in bucket (bucket linked list)
	struct hash_vertex *parent;		//pointer to vertex with previous board position
	char moved_piece;
	int movement_direction;
	char position[BoardSize];
} TypeHashVertex;


/*Define the origin of a bucket as a pointer to its first vertex
and keep track of the number of vertices in the bucket*/
typedef struct bucket_head{
	int elements_in_bucket;		//counter for number of vertices in bucket
	struct hash_vertex *first;	//pointer to first vertex in bucket
} TypeHashListHead;


/*Function to allocate memory for a new vertex*/
TypeHashVertex *allocateHashVertex() {
	TypeHashVertex *p = malloc(sizeof(struct hash_vertex));
	if (p==NULL) {printf("Malloc failed."); exit(1);}
	return p;
}

/*Function to allocate memory for a new bucket pointer*/

TypeHashListHead *allocateHashListHead() {
	TypeHashListHead *p = malloc(sizeof(struct bucket_head));
	if (p==NULL) {printf("Malloc failed."); exit(1);}
	return p;
}

/***********************************************************************************************************************/
/***********************************************************************************************************************/
//From this point on, I wrote every line of code in this file without using COSI21 website code
//utility functions are defined here:

//utility function to calculate the power of a number given a base and exponent
int power(int base, int exp){

	if (exp == 0) {
		return 1;
	} else {
		int result = 1;
		for(int i = 0; i <exp;i++) {
			result = base*result;
		}
		return result;
	}
}

//utility function to compare two string arrays. Used to compare two board positions
int compare_positions(char string1[],char string2[]){
	//flah = 0 strings are the same. flag=1 they're not
	int flag = 0; //hopefuly, the two strings are the same
	int i;
	for(i=0; i<BoardSize;i++){

		if (string1[i]!=string2[i]) {
			flag = 1;				//well, they're not the same
		}
	}
	return flag;
}

//utility function to copy a string of chars

void copy_array(char *pointer_to_string[],char string2[],int size){
	for (int i=0;i<size;i++){

		*(*pointer_to_string+i) = string2[i];
	}
}

/***********************************************************************************************************************/
//**************************************Hash table related functions***************************************//////////////
//global hash variables, hash array and number of elements in hash
TypeHashListHead *hash_table[HashArraySize];
int current_elements_h=0;

int HashFunction(char current_position[BoardSize]){

	unsigned int sum=0;
	int i;

	//calculates sum = position[0]+10*position[1]+100*position[2]...
	//for most board combinations, the number will be different
	//hence, yielding a different sum mod HashArraySize (array position)
	for(i=0;i< BoardSize;i++) {
		sum = sum + power(10,i)*current_position[i];
	}
	return (sum % HashArraySize);
}


TypeHashVertex *MemberHash(char current_position[BoardSize], TypeHashListHead *H[HashArraySize]) {
	//checks if an element is contained in the hash.
	//returns pointer to vertex, if found or NULL if not.

	int bucket = HashFunction(current_position);

	if (*(H+bucket)==NULL){

		return NULL;
	} 
	else {


		TypeHashListHead *pointer;	//linked list pointer
		pointer = *(H+bucket);

		TypeHashVertex *p;
		p = pointer->first;			//first element in the bucket

		TypeHashVertex *found_vertex;

		int foundflag = 0;

		while ((p !=NULL) && (foundflag==0)) {		//iterates until end of bucket or element found

			if (compare_positions(current_position,p-> position)==0) {
				foundflag = 1;
				found_vertex = p;
			}

			p = p->next;
		} 

		if (foundflag == 1) {
			return found_vertex;
		} else
		{ 
			return NULL;
		}
	}
}


TypeHashVertex *InsertHash(char v_position[BoardSize], TypeHashListHead *H[HashArraySize], TypeHashVertex *v_parent, char v_moved_piece, int v_movement_direction) {
	//inserts a new vertex in hash by traversing the linked list
	//pointed by the pointer stored in the hash array

	int bucket = HashFunction(v_position);

	//create vertex and populate its fields
	TypeHashVertex *new_vertex = allocateHashVertex();		//create new vertex to insert in the hash table
	new_vertex -> moved_piece = v_moved_piece;
	new_vertex -> next = NULL;
	new_vertex -> parent = v_parent;
	new_vertex -> movement_direction = v_movement_direction;

	for (int i=0;i<BoardSize;i++){
		new_vertex -> position[i] = v_position[i];
	}

	if (*(H+bucket)==NULL){			//if corresponding position in array is empty, make it point to new vertex

		TypeHashListHead *pointer = allocateHashListHead();		//new linked list pointer to bucket
		pointer->first = new_vertex;
		pointer-> elements_in_bucket=1;							//this is the first element inserted in bucket
		*(H+bucket) = pointer;
		current_elements_h++;									//increase counter of elements in hash
		return new_vertex;			//return pointer to newly created vertex
	} 
	else {						//else, go to the last element in the linked list and make it point to the new vertex

		TypeHashListHead *pointer;		//linked list pointer
		pointer = *(H+bucket);

		TypeHashVertex *p;		//first element in bucket
		p = pointer->first;
		pointer->elements_in_bucket++;		//increase counter of elements in hash

		while (p->next !=NULL) {
			p = p->next;				//finding the last element in bucket
		} 

		p->next = new_vertex;
		return new_vertex;			//return pointer to newly created vertex
	}
}


void init_hash_table(TypeHashListHead *H[HashArraySize]){
	//makes all positions in the array point to NULL
	for(int i=0;i<HashArraySize;i++){

		*(H+i)=NULL;				//in the beginning, all cells in the hash table point to NULL
	}
}

void bucket_counter(TypeHashListHead *H[HashArraySize]){
	//reverses the hash array and counts how many buckets of a given size there are

	TypeHashListHead *pointer;
	int counters[HashStatsMAX];			//keep track of HashStatsMAX hash bucket size statistics
	int i;

	for(i=0;i<HashStatsMAX;i++){
		counters[i]=0;					//all counters init to 0
	}

	for(int bucket=0;bucket<HashArraySize;bucket++){
		pointer = *(H+bucket);
		if (pointer != NULL){

			counters[pointer->elements_in_bucket]++;	//counter of number of buckets with 1 or more elements
		} 
		else {
			counters[0]++;		//counter of the number of empty buckets
		}
	}

	i = 0;
	while (counters[i]!=0) {
		i++;					//how many sizes of buckets with at least 1 element there are
	}

	HashStats(HashArraySize, current_elements_h, i-1);		//print hash stat info

	for(i=0;counters[i]!=0;i++){
		BucketStat(i,counters[i]);				//print bucket size stats
	}
}

TypeHashVertex *reverse_pointers(TypeHashVertex *solution_vertex){

	//revert pointers from GoalBoard to StartBoard

	TypeHashVertex *p;
	TypeHashVertex *v;
	p = solution_vertex;
	p->next = NULL;

	do {
		v = p->parent;
		v->next = p;
		p = p->parent;
	} while (v->parent != NULL);

	return v;
}

void show_output(TypeHashVertex *solution_vertex) {
	//prints all positions starting on the origin board to the solution board
	TypeHashVertex *solution;					
	solution = reverse_pointers(solution_vertex);	//solution is a linked list with elements of path from StartBoard to GoalBoard
	int step=0;

	while (step != MinSolution+1)
	{	
		OutputPosition(solution->position, step, solution->moved_piece, solution->movement_direction);
		solution = solution->next; 
		step++;
	} 
}


/////////////////////////////////////////////////////////////////////
//***********Queue related functions*******************//////////////
TypeHashVertex *queue[QueueArraySize];
int tail=0;
int head=0;
int current_elements_q=0;
int max_elements_q=0;

//circular queue implementation
int enqueue(TypeHashVertex *new_vertex){
	//inserts element in the queue
	if ((head + 1 == tail) || ((head==QueueArraySize-1)&&(tail==0))){
		return 0; 			//queue is full
	} 
	else {

		*(queue+head) = new_vertex;

		if (head<QueueArraySize) {    
			head = head +1;
		}
		else {					//queue array overflow to beginning of array
			head = 0;
		}
		current_elements_q++;					//keep track of # of elements in queue
		if (current_elements_q>max_elements_q){
			max_elements_q = current_elements_q;   //keep track of max # of elements ever present in queue
		}

		return 1;
	}

}

TypeHashVertex *dequeue(){
	//removes element from the queue
	if (head == tail) {

		return NULL; 		//queue is empty
	}
	else {
		
		TypeHashVertex *dequeued_vertex;
		dequeued_vertex= *(queue+tail);

		if (tail<QueueArraySize-1) {
			tail = tail +1;
		} 
		else {				//queue array overflow to beginning of array
			tail = 0;
		}
		current_elements_q--;		//keep track of # of elements in queue
		return dequeued_vertex;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//***********Board traversing and piece movement logic*******************//////////////

void check_and_move(TypeHashVertex *p){
	//this method traverses each position in the board. 
	//It determines if a valid char (!="$" and !=".") can be moved in any of the 4 directions
	//if it is possible, it will create the board produced by moving that piece
	//and then it will insert it in both the hash table and the queue
	char temp[BoardSize];
	char *arr_point[BoardSize]; 
	*arr_point = temp;
	TypeHashVertex *v;

	for (int i=0;i<BoardSize;i++) {		//traverse each piece in the board
		//check if the piece can be moved north, east, south or west
		if ((i>BoardWidth-1) && (p->position[i-BoardWidth]==SymbolBlank) && (p->position[i]!=SymbolFixed) && (p->position[i]!=SymbolBlank)) {
			
			copy_array(arr_point,p->position,BoardSize);
			temp[i-BoardWidth] = p->position[i];
			temp[i] = SymbolBlank;

			if (MemberHash(temp,hash_table)==NULL){
				v = InsertHash(temp, hash_table, p, p->position[i], North);		
				enqueue(v);
			}
		}
		if (((i+1)%BoardWidth!=0) && (p->position[i+1]==SymbolBlank) && (p->position[i]!=SymbolFixed)&& (p->position[i]!=SymbolBlank)) {
			
			copy_array(arr_point,p->position,BoardSize);
			temp[i+1] = p->position[i];
			temp[i] = SymbolBlank;

			if (MemberHash(temp,hash_table)==NULL){
				v = InsertHash(temp, hash_table, p, p->position[i], East);
				enqueue(v);
			}
		}
		if ((i<(BoardHeight-1)*BoardWidth) && (p->position[i+BoardWidth]==SymbolBlank) && (p->position[i]!=SymbolFixed)&& (p->position[i]!=SymbolBlank)) {
			
			copy_array(arr_point,p->position,BoardSize);
			temp[i+BoardWidth] = p->position[i];
			temp[i] = SymbolBlank;
			if (MemberHash(temp,hash_table)==NULL){

				v = InsertHash(temp, hash_table, p, p->position[i], South);
				enqueue(v);
			}
		} if ((i%BoardWidth!=0) && (p->position[i-1]==SymbolBlank) && (p->position[i]!=SymbolFixed)&& (p->position[i]!=SymbolBlank)) {
			copy_array(arr_point,p->position,BoardSize);
			temp[i-1] = p->position[i];
			temp[i] = SymbolBlank;

			if (MemberHash(temp,hash_table)==NULL){
				v = InsertHash(temp, hash_table, p, p->position[i], West);
				enqueue(v);
			}
		}		
	}

}

/////////////////////////////////////////////////////////////////////
/*************************Main**************************************/
/////////////////////////////////////////////////////////////////////


int main() {

init_hash_table(hash_table);     //every cell in the hash table points to NULL

TypeHashVertex *p;
p = InsertHash(StartBoard, hash_table, NULL, 0, 0);		//insert the starting position into the hash table
enqueue(p);												//enqueue startint pasition

while (compare_positions(p->position,GoalBoard)!=0) {
	p = dequeue();
	check_and_move(p);
}

show_output(p);   //show all movements
QueueStats(QueueArraySize, max_elements_q, head, tail);  //show queue stats
bucket_counter(hash_table);								//show hash stats

return 0;
}