/*
MIS v1.0: This code computes a maximal independent set using Luby's algorithm.

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
#include <limits.h>
#include <sys/time.h>

static void findmins(const int* const nidx, const int* const nlist, const int nodes, const int edges, unsigned int* const random, char* const nstat, int* updated)
{
  const unsigned int rand_max = 0xffffffff;
  for (int v = 0; v < nodes; v++) {
    if (random[v] != rand_max) {
      // check all direct neighbors for a lower random number
      int i = nidx[v];
      while ((i < nidx[v + 1]) && ((random[v] < random[nlist[i]]) || ((random[v] == random[nlist[i]]) && (v < nlist[i])))) {
        i++;
      }
      // if v is lowest include it
      if (i >= nidx[v + 1]) {
        *updated = 1;
        nstat[v] = 1;
        random[v] = rand_max;
        for (int i = nidx[v]; i < nidx[v + 1]; i++) {
          random[nlist[i]] = rand_max;
          nstat[nlist[i]] = 2;
        }
      }
    }
  }
}

static unsigned int hash(unsigned int val)
{
  // generate a pseudo random number
  val = ((val >> 16) ^ val) * 0x45d9f3b;
  val = ((val >> 16) ^ val) * 0x45d9f3b;
  val = ((val >> 16) ^ val);
  return val;
}

static void luby(const int* const nidx, const int* const nlist, const int nodes, const int edges, char* const nstat)
{
  // set up pseudo random numbers
  unsigned int* random = (unsigned int*)malloc(nodes * sizeof(unsigned int));
  if (random == NULL) {fprintf(stderr, "out of memory\n"); exit(-1);}
  for (int i = 0; i < nodes; i++) {
    random[i] = hash(i);
  }

  // iterate until no more updates
  int updated;
  do {
    updated = 0;
    findmins(nidx, nlist, nodes, edges, random, nstat, &updated);
  } while (updated != 0);

  free(random);
}

int main(int argc, char* argv[])
{
  printf("Maximal Independent Set v1.0 [serial]\n");

  // check command line
  if (argc != 2) {fprintf(stderr, "usage: %s input_file_name\n", argv[0]); exit(-1);}

  // open input file
  FILE* f = fopen(argv[1], "rt");  if (f == NULL) {fprintf(stderr, "could not open file %s\n", argv[1]); exit(-1);}

  // read number of nodes and edges
  int nodes, edges;
  int cnt = fscanf(f, "%d %d", &nodes, &edges);
  if ((cnt != 2) || (nodes < 1) || (edges < 1)) {fprintf(stderr, "failed to parse first line\n"); exit(-1);}

  // allocate memory for graph
  int* neighborindex = (int*)calloc(nodes + 1, sizeof(int));
  int* neighborlist = (int*)malloc(edges * sizeof(int));
  char* nodestatus = (char*)calloc(nodes, sizeof(char));
  if ((neighborindex == NULL) || (neighborlist == NULL) || (nodestatus == NULL)) {fprintf(stderr, "out of memory\n"); exit(-1);}

  // read in graph
  neighborindex[0] = 0;
  for (int i = 0; i < edges; i++) {
    int src, dst;
    cnt = fscanf(f, "%d %d", &src, &dst);
    if (cnt != 2) {fprintf(stderr, "failed to parse line\n"); exit(-1);}
    if ((src < 0) || (src >= nodes)) {fprintf(stderr, "source out of range\n"); exit(-1);}
    if ((dst < 0) || (dst >= nodes)) {fprintf(stderr, "destination out of range\n"); exit(-1);}
    neighborindex[src + 1] = i + 1;
    neighborlist[i] = dst;
  }
  fclose(f);
  printf("configuration: %d nodes and %d edges (%s)\n", nodes, edges, argv[1]);

  // check graph
  for (int i = 1; i <= nodes; i++) {
    if (neighborindex[i] == 0) {fprintf(stderr, "found zero\n"); exit(-1);}
  }

  // time computation
  struct timeval start, end;
  gettimeofday(&start, NULL);
  luby(neighborindex, neighborlist, nodes, edges, nodestatus);
  gettimeofday(&end, NULL);

  // print performance info
  double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
  printf("compute time: %.4f s\n", runtime);
  printf("mega_nodes/sec: %.3f\n", nodes * 0.000001 / runtime);

  // verify result
  int count = 0;
  for (int i = 0; i < nodes; i++) {
    if (nodestatus[i] == 0) {fprintf(stderr, "found unprocessed node\n"); exit(-1);}
    if (nodestatus[i] == 1) {
      count++;
    }
  }

  // output result
  printf("number of elements in maxmimal independent set: %d (%.1f%%)\n", count, 100.0 * count / nodes);

  free(neighborindex);  free(neighborlist);  free(nodestatus);
  return 0;
}

