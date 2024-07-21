#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <ctime>

using namespace std;

#define MAX_SIZE 2048

int N;
int K;
int A[MAX_SIZE][MAX_SIZE];   // 2D array for input matrix
int result[MAX_SIZE][MAX_SIZE];  // 2D array for result matrix
int C;  // Number of logical cores
int BT; // Number of bounded threads

typedef struct {
    int start_row;  // Struct to hold thread-specific data
    int threadId;
    int core_id;  // Added core_id
} ThreadArgs;

void* multiplication(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    if (args->threadId < BT) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(args->core_id, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    }

    int start_row = args->start_row;

    // Matrix multiplication
    for (int i = start_row - 1; i < N; i += K) {
        for (int j = 0; j < N; j++) {
            int temp_result = 0;
            for (int k = 0; k < N; k++) {
                temp_result += A[i][k] * A[k][j];
            }
            result[i][j] += temp_result; // Accumulate the result
        }
    }
    pthread_exit(NULL);
}

int main() {
    ifstream input("inp.txt");
    input >> N >> K >> C >> BT;

    // Read input matrix from file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            input >> A[i][j];
        }
    }
    input.close();

    // Allocate memory for threads and thread arguments
    vector<pthread_t> threads(K);
    vector<ThreadArgs> args(K);

    clock_t starting_time = clock();

    int core_id = 0;
    int b = K / C;
    int count = 0;

    // Create threads for matrix multiplication
    for (int i = 0; i < K; i++) {
        args[i].start_row = i + 1;
        args[i].threadId = i;
        args[i].core_id = core_id;

        pthread_create(&threads[i], NULL, multiplication, (void*)&args[i]);

        count++;
        if (count == b) {
            core_id++;
            count = 0;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t ending_time = clock();
    double total_time = static_cast<double>(ending_time - starting_time) / CLOCKS_PER_SEC;

    // Write result matrix and execution time to the output file
    ofstream output("out.txt");

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            output << result[i][j] << " ";
        }
        output << "\n";
    }

    output << "\nTime taken to compute the square matrix (mixed) : " << total_time << " seconds\n";
    cout << N << " " << K << " " << C << " " << BT << " " << total_time << endl;

    output.close();

    return 0;
}

