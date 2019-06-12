#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <boost/multiprecision/cpp_bin_float.hpp>
using namespace boost::multiprecision;

#define PRECISION 26
typedef number<cpp_bin_float<PRECISION> > pt; //自定义高精度浮点数
//每个区间的计算式，使用内联函数以集约时间
inline pt f(pt x) {
	return pow(pt(-1),x)/(pt(2)*x+pt(1))*pt(4);
}

int main(int argc, char* argv[])
{
	int myid, numprocs, namelen;
	pt pi, sum, x, *temp;
	double start, end;
	long long n;
	char* pi_norm = "3.141592653589793238462643";
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);        // starts MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);  // get current process id
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);      // get number of processes
	MPI_Get_processor_name(processor_name, &namelen);
	n = 0, temp = NULL;
	if (myid == 0) {
		//		 printf("number of processes: %d\n...", numprocs);
		//		 printf("please enter the number of section:");
		temp = (pt*)malloc(sizeof(pt)*numprocs);
		scanf("%lld", &n);
		start = MPI_Wtime();
	}
	MPI_Bcast(&n, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD); //广播
	sum = 0.0;
	for (long long i = myid ; i <= n; i += numprocs) {
		sum += f(i);
	}

	//     printf("%s: Hello world from process %d \n", processor_name, myid);
	//MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Gather(&sum, sizeof(sum), MPI_BYTE, temp, sizeof(sum), MPI_BYTE, 0, MPI_COMM_WORLD);
	if (myid == 0) {
		end = MPI_Wtime();
		for (int i = 0; i<numprocs; i++) {
			pi += temp[i];
		}
		std::cout << std::defaultfloat << std::setprecision(PRECISION);
		std::cout << "pi is approximately " << pi << std::endl;
		std::cout << std::scientific << std::setprecision(5);
		std::cout << "Error is " << boost::multiprecision::fabs(pi - pt(pi_norm)) << std::endl;
		printf("cost %f s", end - start);
		free(temp);
	}

	MPI_Finalize();

	return 0;
}