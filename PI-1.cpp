#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<time.h>

inline long double f(long double x) {
	return 4 / (1 + x*x);
}

 int main(int argc, char* argv[])
 {
	 int myid, numprocs, namelen;
	 long double pi, width, sum, x,start, end;
	 long long n;
	 char processor_name[MPI_MAX_PROCESSOR_NAME];
	 long double pi_norm = 3.14159265358793238462643;
	 
	 MPI_Init(&argc, &argv);        // starts MPI
     MPI_Comm_rank(MPI_COMM_WORLD, &myid);  // get current process id
	 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);      // get number of processes
	 MPI_Get_processor_name(processor_name, &namelen);
	 n = 0;
	 if (myid == 0) {
//		 printf("number of processes: %d\n...", numprocs);
//		 printf("please enter the number of section:");
		 
		 scanf("%lld", &n);
		 start = MPI_Wtime();
	 }
	 MPI_Bcast(&n, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD); //¹ã²¥
	 width = 1 / (long double)n;
	 sum = 0.0;
	 for (long long i = myid + 1; i <= n; i += numprocs) {
		 x = width*((double)i - 0.5);
		 sum += f(x);
	 }
	 sum *= width;
//     printf("%s: Hello world from process %d \n", processor_name, myid);
	 MPI_Reduce(&sum, &pi, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	 if (myid == 0) {
		 end = MPI_Wtime();
		 printf("pi=%0.26f\n", pi);
		 printf("ERROR is %e\n", pi - pi_norm);
		 printf("cost %f s", end - start);
	 }
		 
     MPI_Finalize();

     return 0;
 }