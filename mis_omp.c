
#ifndef SCHED
#define SCHED "static"
#endif


#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

static const unsigned int rand_max = 0xffffffff;
static const int in = 1;
static const int out = 2;
int chunksize;

static void findmins(const int* const nidx, const int* const nlist, const int nodes, unsigned int* const random, char* const nstat, int *updated,int chunksize)
{
#pragma omp parallel for default(none) shared(updated,chunksize) schedule(SCHED,chunksize)
  for (int v = 0; v < nodes; v++) {
    if (nstat[v] == 0) {
      // check all direct neighbors for a lower random number
      int i = nidx[v];
      while ((i < nidx[v + 1]) && ((random[v] < random[nlist[i]]) || ((random[v] == random[nlist[i]]) && (v < nlist[i])))) {
        i++;
      }
      // if v is lowest include it
      if (i >= nidx[v + 1]) {
        *updated = 1;
        nstat[v] = in;
        for (int i = nidx[v]; i < nidx[v + 1]; i++) {
          random[nlist[i]] = rand_max;
          nstat[nlist[i]] = out;
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
  if (val == rand_max) val--;
  return val;
}

static void luby(const int* const nidx, const int* const nlist, const int nodes, char* const nstat,int chunksize)
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
    findmins(nidx, nlist, nodes, random, nstat, &updated,chunksize);
  } while (updated != 0);

  free(random);
}

int main(int argc, char* argv[])
{
  printf("Maximal Independent Set v1.1 [openMP]\n");

  // check command line
  if (argc != 3) {fprintf(stderr, "usage: %s input_file_name chunksize \n", argv[0]); exit(-1);}

  // open input file
  FILE* f = fopen(argv[1], "rt");  if (f == NULL) {fprintf(stderr, "could not open file %s\n", argv[1]); exit(-1);}

  // read number of nodes and edges
  int nodes, edges;
  int cnt = fscanf(f, "%d %d", &nodes, &edges);
  if ((cnt != 2) || (nodes < 1) || (edges < 1)) {fprintf(stderr, "failed to parse first line\n"); exit(-1);}


  chunksize = atoi(argv[2]);

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
  luby(neighborindex, neighborlist, nodes, nodestatus,chunksize);
  gettimeofday(&end, NULL);

  // print performance info
  double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
  printf("compute time: %.4f s\n", runtime);
  printf("mega_nodes/sec: %.3f\n", nodes * 0.000001 / runtime);

  // output result
  int count = 0;
  for (int v = 0; v < nodes; v++) {
    if (nodestatus[v] == in) {
      count++;
    }
  }
  printf("number of elements in maxmimal independent set: %d (%.1f%%)\n", count, 100.0 * count / nodes);

  // verify result
  for (int v = 0; v < nodes; v++) {
    if (nodestatus[v] == 0) {fprintf(stderr, "error: found unprocessed node in graph\n"); exit(-1);}
    if (nodestatus[v] == in) {
      for (int i = neighborindex[v]; i < neighborindex[v + 1]; i++) {
        if (nodestatus[neighborlist[i]] == in) {fprintf(stderr, "error: found adjacent nodes in MIS\n"); exit(-1);}
      }
    } else {
      if (nodestatus[v] != out) {fprintf(stderr, "error: encountered invalid node status\n"); exit(-1);}
      int flag = 0;
      for (int i = neighborindex[v]; i < neighborindex[v + 1]; i++) {
        if (nodestatus[neighborlist[i]] == in) {
          flag = 1;
        }
      }
      if (flag == 0) {fprintf(stderr, "error: set is not maximal\n"); exit(-1);}
    }
  }

  free(neighborindex);  free(neighborlist);  free(nodestatus);
  return 0;
}
