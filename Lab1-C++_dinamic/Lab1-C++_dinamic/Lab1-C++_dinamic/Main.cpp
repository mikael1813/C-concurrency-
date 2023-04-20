#include <iostream>
#include <thread>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <fstream>


using namespace std;

#define N 10000
#define M 10
#define THREADS 8
#define K 5
#define KK (K/2)*2

void run_convolution(int** matrix, int** endMatrix, int** kernel, int nr, int NN, int MM, int X, int Y) {
	for (int i = nr + NN; i < MM + nr; i++) {

		for (int j = nr + X; j < Y + nr; j++) {
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

void run_sequentally(int** matrix, int** endMatrix, int** kernel, int nr) {
	run_convolution(matrix, endMatrix, kernel, nr, 0, N, 0, M);
}

void run_concurent_by_rows(int** matrix, int** endMatrix, int** kernel, int nr) {
	vector<thread> t;
	int k = N / THREADS;
	int start = 0, end = start + k, rest = N % THREADS;
	for (size_t i = 0; i < THREADS; i++)
	{
		if (rest > 0) {
			end++;
			rest--;
		}
		thread thr = thread(run_convolution, matrix, endMatrix, kernel, nr, start, end, 0, M);
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

void run_concurent_by_columns(int** matrix, int** endMatrix, int** kernel, int nr) {
	vector<thread> t;
	int k = M / THREADS;
	int start = 0, end = start + k, rest = N % THREADS;
	for (size_t i = 0; i < THREADS; i++)
	{
		if (rest > 0) {
			end++;
			rest--;
		}
		thread thr = thread(run_convolution, matrix, endMatrix, kernel, nr, 0, N, start, end);
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

void expand(int** matrix, int** endMatrix, int nr) {
	//int firstlist[M + KK], lastlist[M + KK];
	int* firstlist, * lastlist;
	//declare firstlist
	firstlist = new int[M + KK];
	//declare lastlist
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
}


int main(int argc, char** argv) {
	//auto start_time = std::chrono::high_resolution_clock::now();
	int** kernel, nr = K / 2;
	//declare kernel
	kernel = new int* [5];
	for (int i = 0; i < 5; i++)
		kernel[i] = new int[5];
	ifstream f("kernel5.txt");
	for (size_t i = 0; i < K; i++)
	{
		for (size_t j = 0; j < K; j++)
		{
			f >> kernel[i][j];
		}
	}
	f.close();
	int** matrix, ** matrixExpanded, ** finalMatrix;
	//declare matrix
	matrix = new int* [N];
	for (int i = 0; i < N; i++)
		matrix[i] = new int[M];
	//declare matrixExpanded
	matrixExpanded = new int* [N + KK];
	for (int i = 0; i < N + KK; i++)
		matrixExpanded[i] = new int[M + KK];
	//declare finalMatrix
	finalMatrix = new int* [N];
	for (int i = 0; i < N; i++)
		finalMatrix[i] = new int[M];
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
	if (N > M) {
		run_concurent_by_rows(matrixExpanded, finalMatrix, kernel, nr);
	}
	else {
		run_concurent_by_columns(matrixExpanded, finalMatrix, kernel, nr);
	}
	//run_convolution(matrixExpanded, finalMatrix, kernel, nr, 0, N, 0, M);
	auto end_time = std::chrono::high_resolution_clock::now();
	//run_concurent_by_rows(matrixExpanded, finalMatrix, kernel, nr);
	//run_concurent_by_columns(matrixExpanded, finalMatrix, kernel, nr);

	//expand(matrix, matrixExpanded, nr);
	/*for (size_t i = 0; i < N; i++)
	{
		for (size_t j = 0; j < M; j++)
		{
			cout << finalMatrix[i][j] << " ";
		}
		cout << endl;
	}*/

	//delete matrix
	for (int i = 0; i < N; i++)
		delete[] matrix[i];
	delete[] matrix;
	//delete kernel
	for (int i = 0; i < 3; i++)
		delete[] kernel[i];
	delete[] kernel;
	//delete matrixExpanded
	for (int i = 0; i < N + KK; i++)
		delete[] matrixExpanded[i];
	delete[] matrixExpanded;
	//delete matrix
	/*for (int i = 0; i < N; i++)
		delete[] finalMatrix[i];
	delete[] finalMatrix;*/

	//auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	cout << "took " << time / std::chrono::milliseconds(1) << "ms to run.\n";

	/*ifstream f3("test.txt");
	for (size_t i = 0; i < N; i++)
	{
		for (size_t j = 0; j < M; j++)
		{
			int x;
			f3 >> x;
			if (finalMatrix[i][j] != x) {
				cout << x << " " << finalMatrix[i][j] << " " << i << " " << j << endl;
			}
		}
	}
	f3.close();*/
	//delete matrix
	for (int i = 0; i < N; i++)
		delete[] finalMatrix[i];
	delete[] finalMatrix;
}