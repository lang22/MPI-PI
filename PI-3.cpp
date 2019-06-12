#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <boost/multiprecision/cpp_bin_float.hpp>

using namespace boost::multiprecision;

#define PRECISION 3000
 
typedef number<cpp_bin_float<PRECISION> > longdouble;
#define MPI_LONGDOUBLE ((MPI_Datatype)longdouble)

//每一项的计算函数f(X)，使用内联函数以节约时间
inline longdouble f(longdouble x) {
	longdouble y = 2 * x + 1;
	longdouble z = pow(-1, x);
	
	return longdouble(4) * z / y*(longdouble(4) / pow(longdouble(5), y) - 1 / pow(longdouble(239), y));
}

int main(int argc, char* argv[])
{
	int myid, numprocs, namelen;
	longdouble pi, sum, x, *temp;
	double start, end;
	long long n;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	char* pi_norm = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420198938";
	temp = NULL;
	MPI_Init(&argc, &argv);        // starts MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);  // get current process id
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);      // get number of processes
	MPI_Get_processor_name(processor_name, &namelen);
	n = 0;

	if (myid == 0) {
		//		 printf("number of processes: %d\n...", numprocs);
		//		 printf("please enter the number of section:");
		temp = (longdouble*)malloc(sizeof(longdouble)*numprocs);
		scanf("%lld", &n);
		start = MPI_Wtime();
	}
	MPI_Bcast(&n, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD); //广播
	sum = 0.0, pi = 0.0;
	for (long long i = myid; i <= n; i += numprocs) {
		sum += f(i);
	}

	//     printf("%s: Hello world from process %d \n", processor_name, myid);
//	MPI_Reduce(&sum, &pi, 1, MPI_LONGDOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Gather(&sum, sizeof(sum), MPI_BYTE, temp, sizeof(sum), MPI_BYTE, 0, MPI_COMM_WORLD);
	if (myid == 0) {
		end = MPI_Wtime();
		for (int i = 0; i < numprocs; i++) {
			pi += temp[i];
		}
		std::cout << std::defaultfloat << std::setprecision(PRECISION);
		std::cout << "pi is approximately " << pi << std::endl;
		std::cout << std::scientific << std::setprecision(5);
		std::cout << "Error is " << boost::multiprecision::fabs(pi - longdouble(pi_norm)) << std::endl;
		printf("cost %f s", end - start);
		free(temp);
	}

	MPI_Finalize();

	return 0;
}