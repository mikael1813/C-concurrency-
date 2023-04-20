#include <iostream>
#include <vector>
#include "mpi.h"
#include <chrono>
#include <fstream>

using namespace std;

bool check() {
	ifstream f1("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar3.txt");
	ifstream f2("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar4.txt");

	int n, m;
	f1 >> n;
	f2 >> m;
	//cout << n << " " << m << endl;
	//cout << endl;
	if (n != m)
		return false;
	for (int i = 0; i < n; i++) {
		int x, y;
		f1 >> x;
		f2 >> y;
		if (x != y) {
			//cout << x << " " << y << endl;
			return false;
		}
	}
	return true;
}

void readFromFile(vector<int>& nr1, vector<int>& nr2) {
	ifstream f("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar1.txt");
	long long int n, x;
	f >> n;
	for (int i = 0; i < n; i++) {
		f >> x;
		nr1.push_back(x);
	}
	ifstream f2("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar2.txt");
	reverse(nr1.begin(), nr1.end());
	f2 >> n;
	for (int i = 0; i < n; i++) {
		f2 >> x;
		nr2.push_back(x);
	}
	reverse(nr2.begin(), nr2.end());
	f.close();
	f2.close();
}

void writeToFile(vector<int> result) {
	ofstream g("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar4.txt");
	g << result.size() << endl;
	for (int i = 0; i < result.size(); i++) {
		g << result.at(i) << " ";
	}
}

void sequentially(vector<int> nr1, vector<int> nr2) {
	int len;
	if (nr1.size() < nr2.size()) len = nr1.size();
	else len = nr2.size();

	vector<int> result;
	int cf = 0;
	for (int i = 0; i < len; i++) {
		int sum = 0;
		sum = nr1.at(i) + nr2.at(i) + cf;
		if (sum >= 10) {
			sum = sum % 10;
			cf = 1;
		}
		else {
			cf = 0;
		}
		result.push_back(sum);
	}

	if (len < nr2.size()) {
		for (int i = len; i < nr2.size(); i++) {
			result.push_back(nr2.at(i) + cf);
			cf = 0;
		}
	}

	if (len < nr1.size()) {
		for (int i = len; i < nr1.size(); i++) {
			result.push_back(nr1.at(i) + cf);
			cf = 0;
		}
	}

	reverse(result.begin(), result.end());
	writeToFile(result);
}

void varianta1_MPI() {
	auto start_time = chrono::high_resolution_clock::now();
	int rank, nr_procese, id_procces_curent = 1;
	vector<int> numar1; vector<int> numar2;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		cout << "err init MPI" << endl;
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	readFromFile(numar1, numar2);

	MPI_Comm_size(MPI_COMM_WORLD, &nr_procese);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		int n, m, len, rest;
		//ifstream f1("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar1.txt");
		//ifstream f2("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar2.txt");

		n = numar1.size();
		//f1 >> n;
		//cout << n << endl;
		m = numar2.size();
		//f2 >> m;
		//cout << m << endl;

		if (n > m) len = n;
		else len = m;
		rest = len % (nr_procese - 1);
		for (int i = 1; i < nr_procese; i++) {
			vector<int> nr1, nr2;
			int x = len / (nr_procese - 1), a, b;
			if (rest > 0) {
				x++;
				rest--;
			}
			for (int j = 0; j < x; j++) {
				if (numar1.size() > 0) {
					a = numar1.at(0);
					numar1.erase(numar1.begin());
				}
				else {
					a = 0;
				}
				if (numar2.size() > 0) {
					b = numar2.at(0);
					numar2.erase(numar2.begin());
				}
				else {
					b = 0;
				}

				/*if (!f1.eof()) {
					f1 >> a;
				}
				else {
					a = 0;
				}
				if (!f2.eof()) {
					f2 >> b;
				}
				else {
					b = 0;
				}*/
				nr1.push_back(a);
				nr2.push_back(b);
			}

			MPI_Send(&x, 1, MPI_INT, id_procces_curent, 0, MPI_COMM_WORLD);
			MPI_Send(&nr1[0], x, MPI_INT, id_procces_curent, 1, MPI_COMM_WORLD);
			MPI_Send(&nr2[0], x, MPI_INT, id_procces_curent, 2, MPI_COMM_WORLD);
			id_procces_curent++;

		}
		//f1.close();
		//f2.close();
	}
	else {
		int len;
		MPI_Status status;
		vector<int> nr1, nr2, result;

		MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		nr1.resize(len);
		nr2.resize(len);
		MPI_Recv(&nr1[0], len, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&nr2[0], len, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

		if (rank == 1) {
			int sum, cf = 0;
			for (int i = 0; i < nr1.size(); i++) {
				sum = nr1.at(i) + nr2.at(i) + cf;
				sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
				result.push_back(sum);
				//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
			}
			MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}

		else if (rank < nr_procese - 1) {
			int sum, cf;
			MPI_Recv(&cf, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			for (int i = 0; i < nr1.size(); i++) {
				sum = nr1.at(i) + nr2.at(i) + cf;
				sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
				result.push_back(sum);
			}
			MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
		else {
			int sum, cf;
			MPI_Recv(&cf, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			for (int i = 0; i < nr1.size(); i++) {
				sum = nr1.at(i) + nr2.at(i) + cf;
				sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
				result.push_back(sum);
			}
			MPI_Send(&cf, 1, MPI_INT, 0, 9, MPI_COMM_WORLD);
		}

		//int cf = 0, sum = 0, cf_primit = 0, gata = 0, gata_anterior = 0;
		//for (int i = 0; i < nr1.size(); i++) {
		//	sum = nr1.at(i) + nr2.at(i) + cf;
		//	sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
		//	result.push_back(sum);
		//}
		//if (cf == 1 && rank == nr_procese - 1) {
		//	result.push_back(1);
		//}
		//do {
		//	cf = cf_primit;
		//	if (gata_anterior == 1) { gata = 1; }
		//	for (int i = 0; i < nr1.size(); i++) {
		//		int nr = result.at(i);
		//		nr = nr + cf;
		//		nr >= 10 ? cf = 1, nr = nr % 10 : cf = 0;
		//		result.at(i) = nr;
		//	}
		//	if (rank == 1) gata = 1;
		//	if (rank < nr_procese - 1) {
		//		MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		//		MPI_Send(&gata, 1, MPI_INT, rank + 1, 2, MPI_COMM_WORLD);
		//	}
		//	if (rank > 1 && gata != 1) {
		//		MPI_Recv(&cf_primit, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
		//		MPI_Recv(&gata_anterior, 1, MPI_INT, rank - 1, 2, MPI_COMM_WORLD, &status);
		//		//cout << rank << " " << gata_anterior << endl;
		//		//if (gata_anterior == 1) gata = 1;
		//		//cout << rank << " cf= " << cf << endl;
		//	}


		//} while (gata == 0);
		int x = result.size();
		MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&result[0], x, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	if (rank == 0) {
		MPI_Status status;
		vector<int> finalResult, aux;
		for (int i = 1; i < nr_procese; i++) {
			int len;

			MPI_Recv(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			aux.resize(len);
			MPI_Recv(&aux[0], len, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
			for (int i = 0; i < aux.size(); i++) {
				finalResult.push_back(aux.at(i));
			}
			for (int j = 0; j < aux.size(); j++) {
				//cout << aux[j] << " ";
			}
			//cout << endl;
			aux.clear();
		}

		int cf;
		MPI_Recv(&cf, 1, MPI_INT, nr_procese - 1, 9, MPI_COMM_WORLD, &status);
		if (cf == 1)
			finalResult.push_back(cf);

		reverse(finalResult.begin(), finalResult.end());
		ofstream g("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar3.txt");
		g << finalResult.size() << endl;
		for (int i = 0; i < finalResult.size(); i++) {
			//cout << finalResult.at(i) << " ";
			g << finalResult.at(i) << " ";
		}
		//cout << endl << finalResult.size();
		g.close();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto time = end_time - start_time;

		if (check()) {
			cout << time / std::chrono::milliseconds(1) << "ms to run.\n";
		}
		else {
			cout << "ERROR\n";
		}
	}


	//MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);




	MPI_Finalize();
}


void varianta2_MPI() {
	auto start_time = chrono::high_resolution_clock::now();
	int rank, nr_procese, id_procces_curent = 1, len, chunck;
	vector<int> numar1_local, numar2_local, resultatFinal;
	vector<int> numar1, numar2;
	bool ok = false;
	MPI_Status status;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		cout << "err init MPI" << endl;
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	readFromFile(numar1, numar2);

	MPI_Comm_size(MPI_COMM_WORLD, &nr_procese);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		while (numar1.size() < numar2.size()) {
			numar1.push_back(0);
		}
		while (numar1.size() > numar2.size()) {
			numar2.push_back(0);
		}

		len = numar1.size() % nr_procese;

		if (len != 0) {
			len = (len + 1) * nr_procese - numar1.size();
		}

		for (int i = 0; i < len; i++) {
			numar1.push_back(0);
			numar2.push_back(0);
		}

		resultatFinal.resize(numar1.size());
		int k = numar1.size() / nr_procese;
		chunck = k;
		len = numar1.size();
		if (numar2.size() > len) len = numar2.size();
		//MPI_Scatter(&k, 1, MPI_INT, &chunck, 1, MPI_INT, 0, MPI_COMM_WORLD);

	}
	MPI_Bcast(&chunck, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//if (chunck * nr_procese + rank < len) {
	//	//chunck++;
	//}
	//cout << chunck << endl;
	numar1_local.resize(chunck);
	numar2_local.resize(chunck);

	MPI_Scatter(numar1.data(), chunck, MPI_INT, numar1_local.data(), chunck, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(numar2.data(), chunck, MPI_INT, numar2_local.data(), chunck, MPI_INT, 0, MPI_COMM_WORLD);

	vector<int> result;

	if (rank == 0) {
		int sum, cf = 0;
		for (int i = 0; i < numar1_local.size(); i++) {
			sum = numar1_local.at(i) + numar2_local.at(i) + cf;
			sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
			result.push_back(sum);
			//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
		}
		MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
	}

	else if (rank < nr_procese - 1) {
		int sum, cf;
		MPI_Recv(&cf, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
		for (int i = 0; i < numar1_local.size(); i++) {
			sum = numar1_local.at(i) + numar2_local.at(i) + cf;
			sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
			result.push_back(sum);
		}
		MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
	}
	else {
		int sum, cf;
		MPI_Recv(&cf, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
		for (int i = 0; i < numar1_local.size(); i++) {
			sum = numar1_local.at(i) + numar2_local.at(i) + cf;
			sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
			result.push_back(sum);
		}
		/*if (cf == 1) {
			result.push_back(cf);
			ok = true;
		}*/
		MPI_Send(&cf, 1, MPI_INT, 0, 9, MPI_COMM_WORLD);
	}

	MPI_Gather(result.data(), chunck, MPI_INT, resultatFinal.data(), chunck, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		int cf;
		MPI_Recv(&cf, 1, MPI_INT, nr_procese - 1, 9, MPI_COMM_WORLD, &status);
		if (cf == 1)
			resultatFinal.push_back(cf);
		reverse(resultatFinal.begin(), resultatFinal.end());
		while (resultatFinal.at(0) == 0) {
			resultatFinal.erase(resultatFinal.begin());
		}
		ofstream g("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar3.txt");
		g << resultatFinal.size() << endl;
		for (int i = 0; i < resultatFinal.size(); i++) {
			g << resultatFinal[i] << " ";
			//cout << resultatFinal[i] << " ";
		}
		g.close();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto time = end_time - start_time;

		if (check()) {
			cout << time / std::chrono::milliseconds(1) << "ms to run.\n";
		}
		else {
			cout << "ERROR\n";
		}
	}


	MPI_Finalize();
}


void varianta3_MPI() {
	auto start_time = chrono::high_resolution_clock::now();
	int rank, nr_procese, id_procces_curent = 1, x, lenn;
	vector<int> numar1; vector<int> numar2;
	vector<int> size;

	MPI_Request n1, n2, carry_flag;
	MPI_Request requests[12];
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		cout << "err init MPI" << endl;
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	readFromFile(numar1, numar2);

	MPI_Comm_size(MPI_COMM_WORLD, &nr_procese);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		int n, m, len, rest;
		size.resize(nr_procese);
		//ifstream f1("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar1.txt");
		//ifstream f2("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar2.txt");

		n = numar1.size();
		//f1 >> n;
		//cout << n << endl;
		m = numar2.size();
		//f2 >> m;
		//cout << m << endl;

		if (n > m) len = n;
		else len = m;
		lenn = len;
		rest = len % (nr_procese - 1);
		for (int i = 1; i < nr_procese; i++) {
			vector<int> nr1, nr2;
			x = len / (nr_procese - 1); int a, b;
			if (rest > 0) {
				x++;
				rest--;
			}
			for (int j = 0; j < x; j++) {
				if (numar1.size() > 0) {
					a = numar1.at(0);
					numar1.erase(numar1.begin());
				}
				else {
					a = 0;
				}
				if (numar2.size() > 0) {
					b = numar2.at(0);
					numar2.erase(numar2.begin());
				}
				else {
					b = 0;
				}

				/*if (!f1.eof()) {
					f1 >> a;
				}
				else {
					a = 0;
				}
				if (!f2.eof()) {
					f2 >> b;
				}
				else {
					b = 0;
				}*/
				nr1.push_back(a);
				nr2.push_back(b);
			}
			//x = x / 100;
			//cout << x << endl;
			MPI_Send(&x, 1, MPI_INT, id_procces_curent, 0, MPI_COMM_WORLD);
			//cout << x << endl;
			/*for (int i = 0; i < x; i++) {
				cout << nr2[i] << endl;
			}*/
			size[id_procces_curent] = x;
			//Bcout << 1;
			MPI_Send(&nr1[0], x, MPI_INT, id_procces_curent, 1, MPI_COMM_WORLD);
			//MPI_Wait(&n1, MPI_STATUS_IGNORE);
			//cout << 1;
			MPI_Send(&nr2[0], x, MPI_INT, id_procces_curent, 2, MPI_COMM_WORLD);
			//MPI_Wait(&n1, MPI_STATUS_IGNORE);
			id_procces_curent++;
		}
	}

	else {

		int len;
		MPI_Status status;
		vector<int> nr1, nr2, result;


		MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);


		nr1.resize(len);
		nr2.resize(len);
		MPI_Irecv(nr1.data(), len, MPI_INT, 0, 1, MPI_COMM_WORLD, &n1);
		MPI_Irecv(nr2.data(), len, MPI_INT, 0, 2, MPI_COMM_WORLD, &n2);
		MPI_Wait(&n2, MPI_STATUS_IGNORE);
		//cout << nr2[0] << endl;
		/*for (int i = 0; i < len; i++) {
			cout << nr2[i] << endl;
		}*/
		if (rank == 1) {
			//cout << "breakpoint " << rank << endl;
			MPI_Wait(&n1, MPI_STATUS_IGNORE);
			MPI_Wait(&n2, MPI_STATUS_IGNORE);


			//cout << "breakpoint " << rank << endl;
			int sum, cf = 0;
			for (int i = 0; i < nr1.size(); i++) {
				sum = nr1.at(i) + nr2.at(i) + cf;
				//cout << sum << endl;
				sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
				result.push_back(sum);
				//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
			}
			//MPI_Isend(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &cf);
			MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}

		else if (rank < nr_procese - 1) {
			//cout << "breakpoint " << rank << endl;
			int cf;
			int flag, arrays1, arrays2;

			MPI_Irecv(&cf, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &carry_flag);

			while (true) {

				MPI_Test(&carry_flag, &flag, MPI_STATUS_IGNORE);

				MPI_Test(&n1, &arrays1, MPI_STATUS_IGNORE);
				MPI_Test(&n2, &arrays2, MPI_STATUS_IGNORE);

				if (flag) {
					//cout << "rank=" << rank << " cf=" << cf << endl;
					MPI_Wait(&n1, MPI_STATUS_IGNORE);
					MPI_Wait(&n2, MPI_STATUS_IGNORE);

					int sum;
					for (int i = 0; i < nr1.size(); i++) {
						sum = nr1.at(i) + nr2.at(i) + cf;
						sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
						result.push_back(sum);
						//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
					}
					//MPI_Isend(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &cf);
					MPI_Send(&cf, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
					break;
				}

				if (arrays1 == 1 && arrays2 == 1) {
					int sum, cff = 0;
					for (int i = 0; i < nr1.size(); i++) {
						sum = nr1.at(i) + nr2.at(i) + cff;
						sum >= 10 ? sum = sum % 10, cff = 1 : cff = 0;
						result.push_back(sum);
						//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
					}

					MPI_Wait(&carry_flag, MPI_STATUS_IGNORE);
					//cout << "rank=" << rank << " cf=" << cf << endl;
					int i = 0;
					while (cf == 1 and i < result.size()) {
						sum = result.at(i) + cf;
						sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
						result.at(i) = sum;
					}

					MPI_Send(&cff, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
					break;
				}

			}
			//cout << "breakpoint " << rank << endl;
		}
		else {
			int cf;
			int flag, arrays1, arrays2;

			MPI_Irecv(&cf, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &carry_flag);

			while (true) {

				MPI_Test(&carry_flag, &flag, MPI_STATUS_IGNORE);

				MPI_Test(&n1, &arrays1, MPI_STATUS_IGNORE);
				MPI_Test(&n2, &arrays2, MPI_STATUS_IGNORE);

				if (flag == 1) {
					MPI_Wait(&n1, MPI_STATUS_IGNORE);
					MPI_Wait(&n2, MPI_STATUS_IGNORE);

					int sum;
					for (int i = 0; i < nr1.size(); i++) {
						sum = nr1.at(i) + nr2.at(i) + cf;
						sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
						result.push_back(sum);
						//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
					}
					MPI_Send(&cf, 1, MPI_INT, 0, 9, MPI_COMM_WORLD);
					break;
				}

				if (arrays1 == 1 && arrays2 == 1) {
					int sum, cff = 0;
					for (int i = 0; i < nr1.size(); i++) {
						sum = nr1.at(i) + nr2.at(i) + cff;
						sum >= 10 ? sum = sum % 10, cff = 1 : cff = 0;
						result.push_back(sum);
						//cout << "sum= " << sum << " nr1= " << nr1[i] << " nr2= " << nr2[i] << " cf= " << cf << endl;
					}

					MPI_Wait(&carry_flag, MPI_STATUS_IGNORE);

					int i = 0;
					while (cf == 1 and i < result.size()) {
						sum = result.at(i) + cf;
						sum >= 10 ? sum = sum % 10, cf = 1 : cf = 0;
						result[i] = sum;
					}

					MPI_Send(&cf, 1, MPI_INT, 0, 9, MPI_COMM_WORLD);
					break;
				}

			}
		}


		//x = result.size();
		//cout << "Send from " << rank << " value " << x << endl;
		//MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		//cout << "rank = " << rank << endl;
		//for (int i = 0; i < result.size(); i++) {
			//cout << rank << " : " << result[i] <<" : " << i<< endl;
		//}
		//MPI_Isend(&result[0], result.size(), MPI_INT, 0, 1, MPI_COMM_WORLD, &requests[rank]);
		MPI_Send(&result[0], result.size(), MPI_INT, 0, 1, MPI_COMM_WORLD);

	}


	if (rank == 0) {
		//cout << "GATATATA" << endl;

		MPI_Status status;
		vector<int> finalResult;
		finalResult.resize(lenn);
		//cout << finalResult[0] << " = default" << endl;
		//vector<int> size;
		vector<vector<int>> aux;

		aux.resize(nr_procese);
		/*size.resize(nr_procese);

		for (int i = 0; i < size.size(); i++) {
			size[i] = 0;
		}*/
		//MPI_Recv(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		for (int i = 1; i < nr_procese; i++) {
			int k;
			//cout << i << endl;
			//MPI_Recv(&k, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			aux[i].resize(size[i]);
			//cout << size[i] << " " << i << endl;
		}

		for (int i = 1; i < nr_procese; i++) {
			MPI_Irecv(aux[i].data(), size[i], MPI_INT, i, 1, MPI_COMM_WORLD, &requests[i]);

		}

		bool ok = true;
		int ax[12];
		for (int i = 0; i < nr_procese; i++) {
			ax[i] = 0;
		}
		while (ok) {
			ok = false;

			for (int i = 1; i < nr_procese; i++) {
				if (ax[i] != 1) {
					ok = true;
					//cout << i << " a ajuns pana la test" << endl;
					MPI_Test(&requests[i], &ax[i], MPI_STATUS_IGNORE);
					//cout << i << " a trecut de test " << ax[i] << endl;
					if (ax[i] == 1) {
						int start = 0;
						for (int j = 1; j < i; j++) {
							start += size[j];
						}
						for (int j = start; j < start + size[i]; j++) {
							finalResult[j] = aux.at(i).at(j - start);
						}
						//cout << i << " =rank start= " << start << " end=" << start + size[i] << endl;
					}
				}
			}
		}

		int cf;
		MPI_Recv(&cf, 1, MPI_INT, nr_procese - 1, 9, MPI_COMM_WORLD, &status);
		if (cf == 1)
			finalResult.push_back(cf);

		reverse(finalResult.begin(), finalResult.end());
		ofstream g("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/Numar3.txt");
		g << finalResult.size() << endl;
		for (int i = 0; i < finalResult.size(); i++) {
			//cout << finalResult.at(i) << " ";
			g << finalResult.at(i) << " ";
		}
		//cout << endl << finalResult.size();
		g.close();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto time = end_time - start_time;

		if (check()) {
			cout << time / std::chrono::milliseconds(1) << "ms to run.\n";
		}
		else {
			cout << "ERROR\n";
		}
	}



	MPI_Finalize();
}

int main() {

	vector<int> numar1, numar2;



	readFromFile(numar1, numar2);

	sequentially(numar1, numar2);



	//varianta1_MPI();

	//varianta2_MPI();

	varianta3_MPI();

	/*ofstream f("C:/Info An 3/Sem 1/LFTC/Lab3/Lab3/Lab3/date2.txt");
	f << 100000 << endl;
	for (int i = 0; i < 100000; i++) {
		int v1, v2,v3,v4;
		v1 = rand() % 10;
		v2 = rand() % 10;
		v3 = rand() % 10;
		v4 = rand() % 10;

		f << v4 << " ";

	}
	f.close();*/

	return 0;
}