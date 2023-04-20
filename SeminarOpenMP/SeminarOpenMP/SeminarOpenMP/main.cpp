#include <omp.h>
#include <iostream>

using namespace std;

int main() {

	int x = 9;
	int a[] = { 1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10 };
	int b[] = { 1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10 };
	int c[20];


	printf("1 - %d\n", x);
	//printf("1 - %d - %d\n", x, omp_get_num_threads());

	omp_set_num_threads(3); // proritate mai mare ca variabila globala

#pragma omp parallel num_threads(6) // cea mai mare prioritate
	{
		printf("2 - %d - %d\n", x, omp_get_num_threads());
		printf("2 - %d - %d\n", x, omp_get_thread_num());
	}//join
	//printf("3 - %d - %d\n", x, omp_get_num_threads());
	printf("3 - %d\n", x);

#pragma omp parallel
	{
		printf("2 - %d - %d\n", x, omp_get_num_threads());
		printf("2 - %d - %d\n", x, omp_get_thread_num());
	}//join

	//in total se vor crea 10 thread-uri(6 + 4)


#pragma omp parallel private(x)
	{
		x = 40;
		printf("2 - %d - %d\n", x, omp_get_thread_num());
	}//join



#pragma omp parallel default(none) shared(a,b,c) private(x)
	{
		x = 40;
		//#pragma omp for schedule(static,2)
#pragma omp for schedule(static,2) collapse(4) //nowait
		for (int i = 0; i < 20; i++)
		{
			c[i] = a[i] + b[i];
			printf("sum - %d - %d\n", i, omp_get_thread_num());
		}//bariera de sincroniza daca nu folosim nowait

		printf("Fin for\n");
		//printf("2 - %d - %d\n", x, omp_get_thread_num());
	}//join

	for (int i = 0; i < 10; i++) {
		printf("%d ", c[i]);
	}


	printf("ID %d\n", omp_get_thread_num());
#pragma omp parallel
	{
#pragma omp parallel sections
		{
#pragma omp section
			{
				printf("id = %d, \n", omp_get_thread_num());
			}

#pragma omp section
			{
				printf("id = %d, \n", omp_get_thread_num());
			}
#pragma omp section
			{
				printf("id = %d, \n", omp_get_thread_num());
			}
		}
	}


	return 0;
}