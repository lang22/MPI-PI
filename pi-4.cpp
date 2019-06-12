#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/random.hpp>

using namespace boost::multiprecision;

#define PRECISION 26


typedef number<cpp_bin_float<PRECISION> > longdouble;
#define MPI_LONGDOUBLE ((MPI_Datatype)longdouble)

//每一项的计算函数f(X)，使用内联函数以节约时间
inline long long f(long long i) {
	long double x = (long double)rand() / (long double)(RAND_MAX);
	long double y = (long double)rand() / (long double)(RAND_MAX);
	if ((x)*(x) + (y)*(y) <= long double(1))
		return 1;
	else return 0;
}

int main(int argc, char* argv[])
{
	srand((unsigned)time(0)); 
	int myid, numprocs, namelen;
	longdouble pi;
	long long sum, all;
	double start, end;
	long long n;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	char* pi_norm = "3.141592653589793238462643";

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
	MPI_Bcast(&n, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD); //广播
	sum = 0, pi = 0.0;
	for (long long i = myid; i < n; i += numprocs) {
		sum += f(i);
	}

	//     printf("%s: Hello world from process %d \n", processor_name, myid);
	MPI_Reduce(&sum, &all, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
//	MPI_Gather(&sum, sizeof(sum), MPI_BYTE, temp, sizeof(sum), MPI_BYTE, 0, MPI_COMM_WORLD);
	if (myid == 0) {
		end = MPI_Wtime();
		std::cout << all << std::endl;
		pi = longdouble(all * 4) / longdouble(n);
		std::cout << std::defaultfloat << std::setprecision(PRECISION);
		std::cout << "pi is approximately " << pi << std::endl;
		std::cout << std::scientific << std::setprecision(5);
		std::cout << "Error is " << boost::multiprecision::fabs(pi - longdouble(pi_norm)) << std::endl;
		printf("cost %f s", end - start);

	}

	MPI_Finalize();

	return 0;
}