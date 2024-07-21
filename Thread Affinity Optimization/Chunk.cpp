#include <iostream>
#include <fstream>
#include <vector>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

#define MAX_SIZE 2048

int N;  // Size of the matrix
int K;  // Number of threads
int C;  // Number of logical cores
int BT; // Number of bounded threads
int A[MAX_SIZE][MAX_SIZE];
int result[MAX_SIZE][MAX_SIZE];

typedef struct {
    int chunk_size;
    int start_row;
    int threadId;
} ThreadArgs;

void multiplication(ThreadArgs* args, int core_id) {
    if (args->threadId < BT) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    }

    int chunk_size = args->chunk_size;
    int start_row = args->start_row;
    int thread_id = args->threadId;

    for (int i = start_row; i < start_row + chunk_size && i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] += A[i][k] * A[k][j];
            }
        }
    }
}

int main() {
    ifstream input("inp.txt");

    input >> N >> K >> C >> BT;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            input >> A[i][j];
        }
    }
    input.close();

    vector<pthread_t> threads;
    vector<ThreadArgs> args(K);

    int chunk_size = N / K;
    int remainder = N % K;
    int core_id = 0;
    int b = K / C;
    int count = 0;

    clock_t starting_time = clock();

    for (int i = 0; i < K - 1; i++) {
        args[i].chunk_size = chunk_size;
        args[i].start_row = i * chunk_size;
        args[i].threadId = i;

        pthread_t thread;
        pthread_create(&thread, nullptr, reinterpret_cast<void*(*)(void*)>(multiplication), &args[i]);
        threads.push_back(thread);

        count++;
        if (count == b) {
            core_id++;
            count = 0;
        } 
    }

    args[K - 1].chunk_size = chunk_size + remainder;
    args[K - 1].start_row = (K - 1) * chunk_size;
    args[K - 1].threadId = K - 1;

    pthread_t last_thread;
    pthread_create(&last_thread, nullptr, reinterpret_cast<void*(*)(void*)>(multiplication), &args[K - 1]);
    threads.push_back(last_thread);

    for (auto& thread : threads) {
        pthread_join(thread, nullptr);
    }

    clock_t ending_time = clock();
    double cpu_time_used = ((double)(ending_time - starting_time)) / CLOCKS_PER_SEC;

    ofstream output("out.txt");

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            output << result[i][j] << " ";
        }
        output << "\n";
    }

    output << "\nTime taken to compute the square matrix (chunks) : " << cpu_time_used << " seconds\n";
    cout << N << " " << K << " " << C << " " << BT << " " << cpu_time_used << endl;

    output.close();

    return 0;
}

