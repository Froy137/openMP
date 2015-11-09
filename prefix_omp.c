/*
Author: Froylan Morales

Author: Martin Burtscher <burtscher@txstate.edu>
*/
#pragma warning( disable : 161 )
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

static int f(int val)
{
  return (val - 17) * 19;
}

static void prefixSumA(int data[], const int size)
{
  for (int i = 1; i < size; i++) {
    data[i] += data[i - 1];
  }
}


int main(int argc, char* argv[])
{
  printf("Prefix Sum v1.0 [OpenMP]\n");

  // check command line
  if (argc != 2) {fprintf(stderr, "usage: %s size\n", argv[0]); exit(-1);}
  int size = atoi(argv[1]);
  if (size < 1) {fprintf(stderr, "size is too small: %d\n", size); exit(-1);}
  printf("configuration: %d elements\n", size);

  // allocate arrays
  int* arrayA = (int*)malloc(sizeof(int) * size);  if (arrayA == NULL) {fprintf(stderr, "cannot allocate arrayA\n");  exit(-1);}
  int* arrayB = (int*)malloc(sizeof(int) * size);  if (arrayB == NULL) {fprintf(stderr, "cannot allocate arrayB\n");  exit(-1);}
  int* carry  = (int*)malloc(sizeof(int) * 244);  if (carry == NULL) {fprintf(stderr, "cannot allocate partialPrefix\n");exit(-1);}
  // initialize
  for (int i = 0; i < size; i++) {
    arrayA[i] = arrayB[i] = f(i);
  }

  // time the prefix sum computation
  struct timeval start, end;
  gettimeofday(&start, NULL);


long long my_rank;

#pragma omp parallel default(none) shared(arrayB,size,carry)
 {
#ifdef _OPENMP
#define my_rank omp_get_thread_num()
#define thread_count omp_get_num_threads()
# else
#define my_rank 0
#define thread_count 1
#endif


	//int my_rank = omp_get_thread_num();
	int my_start = my_rank * size / thread_count;
	int my_end = (my_rank + 1) * size / thread_count;


	 	 //partial prefix sum
	 	 prefixSumA(&arrayB[my_start],my_end-my_start);

	 	  //write last element into carry array
	 	  carry[my_rank]=arrayB[my_end-1];

			# pragma omp barrier

	 	     //calculating the prefix of carries
	 	  	  if(my_rank==0)
	 	  	  {
	 	  		  prefixSumA(carry,thread_count);
	 	  	  }

	 	  	  # pragma omp barrier

		 	  //recomputing the values with carries
		 	  if(my_rank != 0)
		 	  {
		 		  //for(int i=my_start; i < my_start+(my_end-my_start) ; i++){
		 		 for(int i=my_start; i < my_end ; i++){
		 		  arrayB[i]= carry[my_rank-1] + arrayB[i];
		 		  }
		 	  }

}

  gettimeofday(&end, NULL);

  // print performance info
  double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
  printf("compute time: %.4f s\n", runtime);
  printf("mega_elements/sec: %.3f\n", size * 0.000001 / runtime);

  // compare results
  prefixSumA(arrayA, size);
  for (int i = 0; i < size; i++) {
    if (arrayA[i] != arrayB[i]) {fprintf(stderr, "result mismatch at position %d\n", i);  exit(-1);}
  }

  free(arrayA);  free(arrayB); free(carry);
  return 0;
}

