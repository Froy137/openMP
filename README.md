# openMP

On the machines in the parallel lab, compile your OpenMP code as follows:
gcc -march=native -O3 -fopenmp fractal_omp.c -o fractal_cpu
On Stampede, compile your OpenMP code as follows:
icc -xhost -O3 -openmp fractal_omp.c -o fractal_cpu
Then compile the code again on Stampede as follows:
icc -mmic -O3 -openmp fractal_omp.c -o fractal_mic







-------------
On the machines in the parallel lab, compile your OpenMP code as follows:
gcc -O3 -march=native -fopenmp prefix_omp.c -o prefix_omp -std=c99
gcc -O3 -march=native prefix_omp.c -o prefix_ser -std=c99 -Wall
On Stampede, compile your OpenMP code as follows:
icc -xhost -O3 -openmp prefix_omp.c -o prefix_omp -std=c99
icc -xhost -O3 prefix_omp.c -o prefix_ser -std=c99


--------------------
On the machines in the parallel lab, compile your OpenMP code as follows:
gcc -march=native -O3 -std=c99 -fopenmp mis_omp.c -DSCHED=static -o mis_static
gcc -march=native -O3 -std=c99 -fopenmp mis_omp.c -DSCHED=dynamic -o mis_dynamic
gcc -march=native -O3 -std=c99 -fopenmp mis_omp.c -DSCHED=guided -o mis_guided
On Stampede, compile your OpenMP code as follows:
icc -xhost -O3 -std=c99 -openmp mis_omp.c -DSCHED=static -o mis_static
icc -xhost -O3 -std=c99 -openmp mis_omp.c -DSCHED=dynamic -o mis_dynamic
icc -xhost -O3 -std=c99 -openmp mis_omp.c -DSCHED=guided -o mis_guided
