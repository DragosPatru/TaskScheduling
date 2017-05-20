#include <stdio.h>  //For printf()
#include <stdlib.h> //for exit() and atoi()

const int num_expected_args = 2;
const unsigned UINT32_MAX = 65536;

int main( int argc, char* argv[] ){

	unsigned index_one,index_two,index; //loop indicies
	unsigned array_size;
	double *Array;
	double temp;

	if( argc != num_expected_args ){
		printf("Usage: ./bubble_random_array <size of Array>\n");
		exit(-1);
	}

	array_size = atoi(argv[1]);
	
	if( array_size > UINT32_MAX ){
		printf("ERROR: Array size must be between zero and 65536!\n");
		exit(-1);
	}

	printf("Generating random array...\n");

	Array = (double*) malloc( sizeof(double) * array_size );

	for( index = 0; index < array_size; index++ ){
		Array[index] = (double) rand();
	}

	printf("apply bubble sort...\n");
	 // loop through all numbers 
   for(index_one = 0; index_one < array_size-1; index_one++) { 
      
      for(index_two = 0; index_two < array_size-1-index_one; index_two++) {
         // check if next number is lesser than current no
         //   swap the numbers. 
         //  (Bubble up the highest number)
			
         if(Array[index_two] > Array[index_two+1]) {
            temp = Array[index_two];
            Array[index_two] = Array[index_two+1];
            Array[index_two+1] = temp;
         }
			
      }
  }

	printf("Bubble sort done!\n");
	 for(index_one = 0; index_one < array_size; index_one++) {
      printf("%.2f ",Array[index_one]);
   }
	return 0;
}