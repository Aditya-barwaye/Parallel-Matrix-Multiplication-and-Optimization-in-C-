#include <iostream>
#include <pthread.h>
#include <vector>
#include <ctime>
#include <fstream>
#include <atomic>

using namespace std;

#define MAX_SIZE 2048

// Global variables
int N;  // Size of the matrix
int K;  // Number of threads
int **A;       // Input matrix
int **result;  // Result matrix
int rowInc;
int C = 0; // Shared variable 
int lock;
int key;
bool waiting;

// Function for matrix multiplication in a specified range
void* multiplication(void *arg) {
    int thread_id = *(int*)arg;

	while(C<N){
		waiting = true;
		key = 1;
		while (waiting && key == 1){
			key = __sync_bool_compare_and_swap(&lock,0,1);
		}
		waiting = false;

		// Critical section
		int start_row = C;
		C = C + rowInc;
		int end_row = C;

		int q = (thread_id+1)%K;
		while(q != thread_id && !waiting) {
			q = (q+1)%K;
		}

		if (q==thread_id){
			lock = 0;
		}
		else {
			waiting = false;
		}
		

		for (int i = start_row ; i < end_row && i < N; i++) {
			for (int j = 0; j < N; j++) {
				result[i][j] = 0;
				for (int k = 0; k < N; k++) {
					result[i][j] += A[i][k] * A[k][j];
				}
			}
		}
	}
	
    pthread_exit(NULL);
}


int main() {
    // File input
    ifstream input("inp.txt");
    input >> N >> K >> rowInc;

    // Memory allocation for matrices
    A = new int*[N];
    result = new int*[N];

	for (int i=0 ;i<N;i++){
		A[i]=new int[N];
		result[i]=new int[N];
	}
    // Reading matrix from file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            input >> A[i][j];
        }
    }
    input.close();

    // Thread and argument allocation
    vector<pthread_t> threads(K);
	vector<int> thread_ids(K);

    // Record starting time
    clock_t starting_time = clock();

    // Creating threads for matrix multiplication
    for (int i = 0; i < K ; i++) {
		thread_ids[i] = i;
        pthread_create(&threads[i],NULL, multiplication, (void*)&thread_ids[i]);
    }

    // Waiting for threads to finish
    for (int i = 0; i < K; i++) {
        pthread_join(threads[i],NULL);
    }

    // Record ending time
    clock_t ending_time = clock();
    double cpu_time_used = double(ending_time - starting_time) / CLOCKS_PER_SEC;

    // File output
    ofstream output("out.txt");

    // Writing result matrix to file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            output << result[i][j] << " ";
        }
        output << "\n";
    }

    // Writing time taken to compute the square matrix
    output << "\nTime taken to compute the square matrix: " << cpu_time_used << " seconds\n";
    cout << N << " " << K << " " << cpu_time_used << endl;

    output.close();

    // Memory deallocation
    
	for(int i = 0; i < N; i++)
	{
		delete[] A[i];
		delete[] result[i];
	}
	delete[] A;
	delete[] result;

    return 0;
}
