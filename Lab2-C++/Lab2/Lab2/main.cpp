#include <iostream>
#include <thread>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "barrier.h"


using namespace std;

#define N 10
#define M 10000
#define THREADS 2
#define K 5
#define KK (K/2)*2
#define DINAMIC 0

my_barrier barrier(THREADS);

void run_convolution_dinamic(int** matrix, int** kernel, int nr, int NN, int MM, int X, int Y) {

	if (N <= 10 && M <= 10) {
		int** finalMatrix;
		//declare finalMatrix
		finalMatrix = new int* [10];
		for (int i = 0; i < 10; i++)
			finalMatrix[i] = new int[10];
		for (int i = nr + NN; i < MM + nr; i++) {
			for (int j = nr + X; j < Y + nr; j++) {
				int sum = 0;
				for (int x = i - nr; x < i + nr + 1; x++) {
					for (int y = j - nr; y < j + nr + 1; y++) {
						sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
					}
				}
				finalMatrix[i - nr - NN][j - nr] = sum;
			}
		}



		//bariera problema
			//cout << "Astept urmatorul thread: " << NN << " " << MM << endl;
		barrier.wait();
		//cout << "Done: " << NN << " " << MM << endl;

		for (int i = NN + nr; i < MM + nr; i++) {
			for (int j = nr + X; j < Y + nr; j++) {
				matrix[i][j] = finalMatrix[i - nr - NN][j - nr];
			}
		}

		//delete finalMatrix
		for (int i = 0; i < 10; i++)
			delete[] finalMatrix[i];
		delete[] finalMatrix;

	}

	else {
		int** frontiera_sus, ** frontiera_jos;

		//declare frontiera_sus
		frontiera_sus = new int* [K / 2];
		for (int i = 0; i < K / 2; i++)
			frontiera_sus[i] = new int[10004];

		//declare frontiera_jos
		frontiera_jos = new int* [K / 2];
		for (int i = 0; i < K / 2; i++)
			frontiera_jos[i] = new int[10004];


		if (N >= M) {
			int count = 0;
			for (int i = nr + NN; i < nr + NN + nr; i++) {

				for (int j = nr + X; j < Y + nr; j++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_sus[count][j - nr] = sum;
				}
				count++;
			}
			count = 0;
			for (int i = MM; i < MM + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_jos[count][j - nr] = sum;

				}
				count++;
			}

			int** aux;
			//declare aux
			aux = new int* [(K / 2) + 1];
			for (int i = 0; i < (K / 2) + 1; i++)
				aux[i] = new int[10004];

			count = 0;
			for (int i = nr + NN + nr; i < MM; i++) {
				if (count == 1 + nr) {
					for (int j = nr + X; j < Y + nr; j++) {
						matrix[i - 1 - nr][j] = aux[0][j - nr - X];
					}
					for (int j = 0; j < 1 + nr - 1; j++) {
						for (int k = 0; k < Y - X; k++) {
							aux[j][k] = aux[j + 1][k];
						}
					}
					count--;
				}

				for (int j = nr + X; j < Y + nr; j++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					aux[count][j - nr] = sum;
				}
				count++;

			}
			for (int i = 0; i < 1 + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					matrix[MM - 1 - nr + i][j] = aux[i][j - nr - X];
				}
			}
			//delete aux
			for (int i = 0; i < (K / 2) + 1; i++)
				delete[] aux[i];
			delete[] aux;

			//bariera problema
			//cout << "Astept urmatorul thread: " << NN << " " << MM << endl;
			barrier.wait();
			//cout << "Done: " << NN << " " << MM << endl;
			int k = 0;
			for (int i = nr + NN; i < nr + NN + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					matrix[i][j] = frontiera_sus[k][j - nr];
				}
				k++;
			}
			k = 0;
			for (int i = MM; i < MM + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					matrix[i][j] = frontiera_jos[k][j - nr];
				}
				k++;
			}

		}
		else {
			int count = 0;
			for (int j = nr + X; j < nr + X + nr; j++) {

				for (int i = nr + NN; i < MM + nr; i++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_sus[count][i - nr] = sum;
				}
				count++;
			}
			count = 0;
			for (int j = Y; j < Y + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_jos[count][i - nr] = sum;

				}
				count++;
			}

			int** aux;
			//declare aux
			aux = new int* [(K / 2) + 1];
			for (int i = 0; i < (K / 2) + 1; i++)
				aux[i] = new int[10000];

			count = 0;
			for (int j = nr + X + nr; j < Y; j++) {
				if (count == 1 + nr) {
					for (int i = nr + NN; i < MM + nr; i++) {
						matrix[i][j - 1 - nr] = aux[0][i - nr];

					}
					//cout << matrix[nr + NN][j - 1 - nr] << " " << aux[0][i - nr] << endl;
					for (int i = 0; i < 1 + nr - 1; i++) {
						for (int k = 0; k < MM - NN; k++) {
							aux[i][k] = aux[i + 1][k];
						}
					}
					count--;
				}

				for (int i = nr + NN; i < MM + nr; i++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					aux[count][i - nr] = sum;
				}
				count++;

			}
			for (int j = 0; j < 1 + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					matrix[i][Y - 1 - nr + j] = aux[j][i - nr];
				}
			}
			//delete aux
			for (int i = 0; i < (K / 2) + 1; i++)
				delete[] aux[i];
			delete[] aux;

			//bariera problema
			//cout << "Astept urmatorul thread: " << NN << " " << MM << endl;
			barrier.wait();
			//cout << "Done: " << NN << " " << MM << endl;
			int k = 0;
			for (int j = nr + X; j < nr + X + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					matrix[i][j] = frontiera_sus[k][i - nr];
				}
				k++;
			}
			k = 0;
			for (int j = Y; j < Y + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					matrix[i][j] = frontiera_jos[k][i - nr];
				}
				k++;
			}
		}
		//delete frontiera_sus
		for (int i = 0; i < K / 2; i++)
			delete[] frontiera_sus[i];
		delete[] frontiera_sus;
		//delete frontiera_jos
		for (int i = 0; i < K / 2; i++)
			delete[] frontiera_jos[i];
		delete[] frontiera_jos;
	}


	//for (int i = nr + NN; i < MM + nr; i++) {

	//	for (int j = nr + X; j < Y + nr; j++) {
	//		int sum = 0;
	//		for (int x = i - nr; x < i + nr + 1; x++) {
	//			for (int y = j - nr; y < j + nr + 1; y++) {
	//				sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
	//				//kernel.get(x - (i - nr)).get(y - (j - nr));
	//			}
	//		}
	//		//endMatrix[i - nr][j - nr] = sum;
	//	}
	//}
}


void run_convolution(int matrix[N + KK][M + KK], int kernel[K][K], int nr, int NN, int MM, int X, int Y) {

	if (N <= 10 && M <= 10) {
		int finalMatrix[10][10];
		for (int i = nr + NN; i < MM + nr; i++) {
			for (int j = nr + X; j < Y + nr; j++) {
				int sum = 0;
				for (int x = i - nr; x < i + nr + 1; x++) {
					for (int y = j - nr; y < j + nr + 1; y++) {
						sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
					}
				}
				finalMatrix[i - nr - NN][j - nr] = sum;
			}
		}

		//bariera problema
			//cout << "Astept urmatorul thread: " << NN << " " << MM << endl;
		barrier.wait();
		//cout << "Done: " << NN << " " << MM << endl;

		for (int i = NN + nr; i < MM + nr; i++) {
			for (int j = nr + X; j < Y + nr; j++) {
				matrix[i][j] = finalMatrix[i - nr - NN][j - nr];
			}
		}


	}

	else {
		int frontiera_sus[K / 2][10000], frontiera_jos[K / 2][10000];

		if (N >= M) {
			int count = 0;
			for (int i = nr + NN; i < nr + NN + nr; i++) {

				for (int j = nr + X; j < Y + nr; j++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_sus[count][j - nr] = sum;
				}
				count++;
			}
			count = 0;
			for (int i = MM; i < MM + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_jos[count][j - nr] = sum;

				}
				count++;
			}

			int aux[(K / 2) + 1][10000];
			count = 0;
			for (int i = nr + NN + nr; i < MM; i++) {
				if (count == 1 + nr) {
					for (int j = nr + X; j < Y + nr; j++) {
						matrix[i - 1 - nr][j] = aux[0][j - nr - X];
					}
					for (int j = 0; j < 1 + nr - 1; j++) {
						for (int k = 0; k < Y - X; k++) {
							aux[j][k] = aux[j + 1][k];
						}
					}
					count--;
				}

				for (int j = nr + X; j < Y + nr; j++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					aux[count][j - nr] = sum;
				}
				count++;

			}
			for (int i = 0; i < 1 + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					matrix[MM - 1 - nr + i][j] = aux[i][j - nr - X];
				}
			}


			//bariera problema
			//cout << "Astept urmatorul thread: " << NN << " " << MM << endl;
			barrier.wait();
			//cout << "Done: " << NN << " " << MM << endl;
			int k = 0;
			for (int i = nr + NN; i < nr + NN + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					matrix[i][j] = frontiera_sus[k][j - nr];
				}
				k++;
			}
			k = 0;
			for (int i = MM; i < MM + nr; i++) {
				for (int j = nr + X; j < Y + nr; j++) {
					matrix[i][j] = frontiera_jos[k][j - nr];
				}
				k++;
			}

		}
		else {
			int count = 0;
			for (int j = nr + X; j < nr + X + nr; j++) {

				for (int i = nr + NN; i < MM + nr; i++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_sus[count][i - nr] = sum;
				}
				count++;
			}
			count = 0;
			for (int j = Y; j < Y + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					frontiera_jos[count][i - nr] = sum;

				}
				count++;
			}

			int aux[(K / 2) + 1][10000];
			count = 0;
			for (int j = nr + X + nr; j < Y; j++) {
				if (count == 1 + nr) {
					for (int i = nr + NN; i < MM + nr; i++) {
						matrix[i][j - 1 - nr] = aux[0][i - nr];

					}
					//cout << matrix[nr + NN][j - 1 - nr] << " " << aux[0][i - nr] << endl;
					for (int i = 0; i < 1 + nr - 1; i++) {
						for (int k = 0; k < MM - NN; k++) {
							aux[i][k] = aux[i + 1][k];
						}
					}
					count--;
				}

				for (int i = nr + NN; i < MM + nr; i++) {
					int sum = 0;
					for (int x = i - nr; x < i + nr + 1; x++) {
						for (int y = j - nr; y < j + nr + 1; y++) {
							sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
						}
					}
					aux[count][i - nr] = sum;
				}
				count++;

			}
			for (int j = 0; j < 1 + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					matrix[i][Y - 1 - nr + j] = aux[j][i - nr];
				}
			}


			//bariera problema
			//cout << "Astept urmatorul thread: " << NN << " " << MM << endl;
			barrier.wait();
			//cout << "Done: " << NN << " " << MM << endl;
			int k = 0;
			for (int j = nr + X; j < nr + X + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					matrix[i][j] = frontiera_sus[k][i - nr];
				}
				k++;
			}
			k = 0;
			for (int j = Y; j < Y + nr; j++) {
				for (int i = nr + NN; i < MM + nr; i++) {
					matrix[i][j] = frontiera_jos[k][i - nr];
				}
				k++;
			}
		}
	}


	//for (int i = nr + NN; i < MM + nr; i++) {

	//	for (int j = nr + X; j < Y + nr; j++) {
	//		int sum = 0;
	//		for (int x = i - nr; x < i + nr + 1; x++) {
	//			for (int y = j - nr; y < j + nr + 1; y++) {
	//				sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
	//				//kernel.get(x - (i - nr)).get(y - (j - nr));
	//			}
	//		}
	//		//endMatrix[i - nr][j - nr] = sum;
	//	}
	//}
}

void run_sequentally_dinamic(int** matrix, int** endMatrix, int** kernel, int nr) {
	for (int i = nr + 0; i < N + nr; i++) {

		for (int j = nr + 0; j < M + nr; j++) {
			int sum = 0;
			for (int x = i - nr; x < i + nr + 1; x++) {
				for (int y = j - nr; y < j + nr + 1; y++) {
					sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
					//kernel.get(x - (i - nr)).get(y - (j - nr));
				}
			}
			endMatrix[i - nr][j - nr] = sum;
		}
	}
}

void run_sequentally(int matrix[N + KK][M + KK], int endMatrix[N][M], int kernel[K][K], int nr) {
	for (int i = nr + 0; i < N + nr; i++) {

		for (int j = nr + 0; j < M + nr; j++) {
			int sum = 0;
			for (int x = i - nr; x < i + nr + 1; x++) {
				for (int y = j - nr; y < j + nr + 1; y++) {
					sum = sum + matrix[x][y] * kernel[x - (i - nr)][y - (j - nr)];
					//kernel.get(x - (i - nr)).get(y - (j - nr));
				}
			}
			endMatrix[i - nr][j - nr] = sum;
		}
	}
}

void run_concurent_by_rows_dinamic(int** matrix, int** kernel, int nr) {
	vector<thread> t;
	int unused = N;

	int k = unused / THREADS;


	int start = 0, end = start + k, rest = unused % THREADS;
	for (size_t i = 0; i < THREADS; i++)
	{
		if (rest > 0) {
			end++;
			rest--;
		}
		thread thr = thread(run_convolution_dinamic, matrix, kernel, nr, start, end, 0, M);
		t.push_back(move(thr));
		start = end;
		end = end + k;
	}

	for (size_t i = 0; i < t.size(); i++)
	{
		if (t[i].joinable())
			t[i].join();
	}
}


void run_concurent_by_rows(int matrix[N + KK][M + KK], int kernel[K][K], int nr) {
	vector<thread> t;
	int unused = N;
	int k = unused / THREADS;
	int start = 0, end = start + k, rest = unused % THREADS;
	for (size_t i = 0; i < THREADS; i++)
	{
		if (rest > 0) {
			end++;
			rest--;
		}
		thread thr = thread(run_convolution, matrix, kernel, nr, start, end, 0, M);
		t.push_back(move(thr));
		start = end;
		end = end + k;
	}

	for (size_t i = 0; i < t.size(); i++)
	{
		if (t[i].joinable())
			t[i].join();
	}
}

void run_concurent_by_columns_dinamic(int** matrix, int** kernel, int nr) {
	vector<thread> t;
	int unused = M;
	int k = unused / THREADS;
	int start = 0, end = start + k, rest = unused % THREADS;
	for (size_t i = 0; i < THREADS; i++)
	{
		if (rest > 0) {
			end++;
			rest--;
		}
		thread thr = thread(run_convolution_dinamic, matrix, kernel, nr, 0, N, start, end);
		t.push_back(move(thr));
		start = end;
		end = end + k;
	}

	for (size_t i = 0; i < t.size(); i++)
	{
		if (t[i].joinable())
			t[i].join();
	}
}

void run_concurent_by_columns(int matrix[N + KK][M + KK], int kernel[K][K], int nr) {
	vector<thread> t;
	int unused = M;
	int k = unused / THREADS;
	int start = 0, end = start + k, rest = unused % THREADS;
	for (size_t i = 0; i < THREADS; i++)
	{
		if (rest > 0) {
			end++;
			rest--;
		}
		thread thr = thread(run_convolution, matrix, kernel, nr, 0, N, start, end);
		t.push_back(move(thr));
		start = end;
		end = end + k;
	}

	for (size_t i = 0; i < t.size(); i++)
	{
		if (t[i].joinable())
			t[i].join();
	}
}

void expand_dinamic(int** matrix, int** endMatrix, int nr) {
	int* firstlist, * lastlist;

	//declare firstlist
	firstlist = new int[M + KK];
	//declare firstlist
	lastlist = new int[M + KK];

	for (int j = nr; j < M + KK - nr; j++) {
		firstlist[j] = matrix[0][j - nr];
		lastlist[j] = matrix[N - 1][j - nr];
	}
	for (int i = 0; i < nr; i++) {
		for (int j = nr; j < M + KK - nr; j++) {
			endMatrix[i][j] = firstlist[j];
			endMatrix[N + KK - i - 1][j] = lastlist[j];
		}
	}
	for (int i = nr; i < N + KK - nr; i++) {
		for (int j = nr; j < M + KK - nr; j++) {
			endMatrix[i][j] = matrix[i - nr][j - nr];
		}
	}
	for (int i = 0; i < N + KK; i++) {
		for (int j = 0; j < nr; j++) {
			endMatrix[i][j] = endMatrix[i][nr];
			endMatrix[i][M + KK - j - 1] = endMatrix[i][M + KK - 1 - nr];
		}
	}
	/*for (size_t i = 0; i < N + KK; i++)
	{
		for (size_t j = 0; j < M + KK; j++)
		{
			cout << endMatrix[i][j] << " ";
		}
		cout << endl;
	}*/

	delete[] firstlist;
	delete[] lastlist;

}

void expand(int matrix[N][M], int endMatrix[N + KK][M + KK], int nr) {
	int firstlist[M + KK], lastlist[M + KK];
	for (int j = nr; j < M + KK - nr; j++) {
		firstlist[j] = matrix[0][j - nr];
		lastlist[j] = matrix[N - 1][j - nr];
	}
	for (int i = 0; i < nr; i++) {
		for (int j = nr; j < M + KK - nr; j++) {
			endMatrix[i][j] = firstlist[j];
			endMatrix[N + KK - i - 1][j] = lastlist[j];
		}
	}
	for (int i = nr; i < N + KK - nr; i++) {
		for (int j = nr; j < M + KK - nr; j++) {
			endMatrix[i][j] = matrix[i - nr][j - nr];
		}
	}
	for (int i = 0; i < N + KK; i++) {
		for (int j = 0; j < nr; j++) {
			endMatrix[i][j] = endMatrix[i][nr];
			endMatrix[i][M + KK - j - 1] = endMatrix[i][M + KK - 1 - nr];
		}
	}
	/*for (size_t i = 0; i < N + KK; i++)
	{
		for (size_t j = 0; j < M + KK; j++)
		{
			cout << endMatrix[i][j] << " ";
		}
		cout << endl;
	}*/
}


int main(int argc, char** argv) {
	//auto start_time = std::chrono::high_resolution_clock::now();
	if (DINAMIC == 0) {
		int kernel[K][K], nr = K / 2;
		ifstream f("kernel5.txt");
		for (size_t i = 0; i < K; i++)
		{
			for (size_t j = 0; j < K; j++)
			{
				f >> kernel[i][j];
			}
		}
		f.close();
		int matrix[N][M], matrixExpanded[N + KK][M + KK];
		ifstream f2("file1010000.txt");
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				f2 >> matrix[i][j];

			}
		}
		f2.close();
		/*for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}*/
		expand(matrix, matrixExpanded, nr);
		/*for (size_t i = 0; i < N + KK; i++)
		{
			for (size_t j = 0; j < M + KK; j++)
			{
				cout << matrixExpanded[i][j] << " ";
			}
			cout << endl;
		}*/
		auto start_time = std::chrono::high_resolution_clock::now();
		if (THREADS == 0) {
			int endMatrix1[N][M];
			run_sequentally(matrixExpanded, endMatrix1, kernel, nr);
		}
		else {
			if (N >= M) {
				run_concurent_by_rows(matrixExpanded, kernel, nr);
			}
			else {
				run_concurent_by_columns(matrixExpanded, kernel, nr);
			}
		}
		auto end_time = std::chrono::high_resolution_clock::now();

		auto time = end_time - start_time;
		cout << "took " << time / std::chrono::milliseconds(1) << "ms to run.\n";
		ofstream ggg("temporary.txt");
		for (size_t i = 0; i < N + KK; i++)
		{
			for (size_t j = 0; j < M + KK; j++)
			{
				ggg << matrixExpanded[i][j] << " ";
			}
			ggg << endl;
		}

		int endMatrix[N][M];
		expand(matrix, matrixExpanded, nr);
		run_sequentally(matrixExpanded, endMatrix, kernel, nr);
		ofstream gg("test.txt");
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				gg << endMatrix[i][j] << " ";
			}
			gg << endl;
		}
		gg.close();
		ifstream f3("temporary.txt");
		int temporaryMatrix[N + KK][M + KK];
		for (size_t i = 0; i < N + KK; i++)
		{
			for (size_t j = 0; j < M + KK; j++)
			{
				f3 >> temporaryMatrix[i][j];
				/*if (endMatrix[i][j] != x) {
					cout << x << " " << endMatrix[i][j] << " " << i << " " << j << endl;
				}*/
			}
		}
		f3.close();

		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				if (endMatrix[i][j] != temporaryMatrix[i + nr][j + nr]) {
					cout << temporaryMatrix[i + nr][j + nr] << " " << endMatrix[i][j] << " " << i << " " << j << endl;
				}
			}
		}
	}
	else {
		int** kernel, nr = K / 2;
		kernel = new int* [K];
		for (int i = 0; i < K; i++)
			kernel[i] = new int[K];
		ifstream f("kernel5.txt");
		for (size_t i = 0; i < K; i++)
		{
			for (size_t j = 0; j < K; j++)
			{
				f >> kernel[i][j];
			}
		}
		f.close();
		int** matrix, ** matrixExpanded;
		//declare matrix
		matrix = new int* [N];
		for (int i = 0; i < N; i++)
			matrix[i] = new int[M];
		//declare matrixExpanded
		matrixExpanded = new int* [N + KK];
		for (int i = 0; i < N + KK; i++)
			matrixExpanded[i] = new int[M + KK];
		ifstream f2("file1000010.txt");
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				f2 >> matrix[i][j];

			}
		}
		f2.close();
		/*for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}*/
		expand_dinamic(matrix, matrixExpanded, nr);
		/*for (size_t i = 0; i < N + KK; i++)
		{
			for (size_t j = 0; j < M + KK; j++)
			{
				cout << matrixExpanded[i][j] << " ";
			}
			cout << endl;
		}*/
		auto start_time = std::chrono::high_resolution_clock::now();
		if (THREADS == 0) {
			int** endMatrix1;
			//declare endMatrix
			endMatrix1 = new int* [N];
			for (int i = 0; i < N; i++)
				endMatrix1[i] = new int[M];
			run_sequentally_dinamic(matrixExpanded, endMatrix1, kernel, nr);
			//delete endMatrix1
			for (int i = 0; i < N; i++)
				delete[] endMatrix1[i];
			delete[] endMatrix1;
		}
		else {
			if (N >= M) {
				run_concurent_by_rows_dinamic(matrixExpanded, kernel, nr);
			}
			else {
				run_concurent_by_columns_dinamic(matrixExpanded, kernel, nr);
			}
		}
		auto end_time = std::chrono::high_resolution_clock::now();

		auto time = end_time - start_time;
		cout << "took " << time / std::chrono::milliseconds(1) << "ms to run.\n";
		ofstream ggg("temporary.txt");
		for (size_t i = 0; i < N + KK; i++)
		{
			for (size_t j = 0; j < M + KK; j++)
			{
				ggg << matrixExpanded[i][j] << " ";
			}
			ggg << endl;
		}

		int** endMatrix;
		//declare endMatrix
		endMatrix = new int* [N];
		for (int i = 0; i < N; i++)
			endMatrix[i] = new int[M];
		expand_dinamic(matrix, matrixExpanded, nr);
		run_sequentally_dinamic(matrixExpanded, endMatrix, kernel, nr);
		ofstream gg("test.txt");
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				gg << endMatrix[i][j] << " ";
			}
			gg << endl;
		}
		gg.close();
		ifstream f3("temporary.txt");
		int** temporaryMatrix;
		//declare temporaryMatrix
		temporaryMatrix = new int* [N + KK];
		for (int i = 0; i < N + KK; i++)
			temporaryMatrix[i] = new int[M + KK];
		for (size_t i = 0; i < N + KK; i++)
		{
			for (size_t j = 0; j < M + KK; j++)
			{
				f3 >> temporaryMatrix[i][j];
				/*if (endMatrix[i][j] != x) {
					cout << x << " " << endMatrix[i][j] << " " << i << " " << j << endl;
				}*/
			}
		}
		f3.close();

		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
			{
				if (endMatrix[i][j] != temporaryMatrix[i + nr][j + nr]) {
					//cout << temporaryMatrix[i + nr][j + nr] << " " << endMatrix[i][j] << " " << i << " " << j << endl;
				}
			}
		}


		//delete matrix
		for (int i = 0; i < N; i++)
			delete[] matrix[i];
		delete[] matrix;
		//delete kernel
		for (int i = 0; i < K; i++)
			delete[] kernel[i];
		delete[] kernel;
		//delete matrixExpanded
		for (int i = 0; i < N + KK; i++)
			delete[] matrixExpanded[i];
		delete[] matrixExpanded;
		//delete endMatrix
		for (int i = 0; i < N; i++)
			delete[] endMatrix[i];
		delete[] endMatrix;
		//delete temporaryMatrix
		for (int i = 0; i < N + KK; i++)
			delete[] temporaryMatrix[i];
		delete[] temporaryMatrix;
	}

}