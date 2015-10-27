/*
PrefixSum v1.0: This code computes the inclusive prefix sum of an array.
 
Copyright (c) 2015, Texas State University. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted for academic, research, experimental, or personal use provided
that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   * Neither the name of Texas State University nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

For all other uses, please contact the Office for Commercialization and Industry
Relations at Texas State University <http://www.txstate.edu/ocir/>.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Martin Burtscher <burtscher@txstate.edu>
*/


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

static void prefixSumB(int data[], const int size)
{
  for (int dist = 1; dist < size; dist *= 2) {
    for (int i = size - 1; i >= dist; i--) {
      data[i] += data[i - dist];
    }
  }
}

int main(int argc, char* argv[])
{
  printf("Prefix Sum v1.0 [serial]\n");

  // check command line
  if (argc != 2) {fprintf(stderr, "usage: %s size\n", argv[0]); exit(-1);}
  int size = atoi(argv[1]);
  if (size < 1) {fprintf(stderr, "size is too small: %d\n", size); exit(-1);}
  printf("configuration: %d elements\n", size);

  // allocate arrays
  int* arrayA = (int*)malloc(sizeof(int) * size);  if (arrayA == NULL) {fprintf(stderr, "cannot allocate arrayA\n");  exit(-1);}
  int* arrayB = (int*)malloc(sizeof(int) * size);  if (arrayB == NULL) {fprintf(stderr, "cannot allocate arrayB\n");  exit(-1);}

  // initialize
  for (int i = 0; i < size; i++) {
    arrayA[i] = arrayB[i] = f(i);
  }

  // time the prefix sum computation
  struct timeval start, end;
  gettimeofday(&start, NULL);
  prefixSumB(arrayB, size);
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

  free(arrayA);  free(arrayB);
  return 0;
}

