#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_SIZE 2048

// Global variables
int N;  // Size of the matrix
int K;  // Number of threads
int (*A)[MAX_SIZE];       // Input matrix
int (*result)[MAX_SIZE];  // Result matrix

// Structure to hold arguments for each thread
typedef struct {
    int chunk_size;
    int start_row;
} ThreadArgs;

// Function for matrix multiplication in a specified range
void* multiplication(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int chunk_size = args->chunk_size;
    int start_row = args->start_row;

    // Matrix multiplication
    for (int i = start_row; i < start_row + chunk_size && i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] += A[i][k] * A[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    // File input
    FILE* input = fopen("inp.txt", "r");
    fscanf(input, "%d %d", &N, &K);

    // Memory allocation for matrices
    A = malloc(N * sizeof(int[MAX_SIZE]));
    result = malloc(N * sizeof(int[MAX_SIZE]));

    // Reading matrix from file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(input, "%d", &A[i][j]);
        }
    }
    fclose(input);

    // Thread and argument allocation
    pthread_t* threads = malloc(K * sizeof(pthread_t));
    ThreadArgs* args = malloc(K * sizeof(ThreadArgs));

    // Determining chunk size for each thread
    int chunk_size = N / K;
    int remainder = N % K;

    // Record starting time
    clock_t starting_time = clock();

    // Creating threads for matrix multiplication
    for (int i = 0; i < K - 1; i++) {
        args[i].chunk_size = chunk_size;
        args[i].start_row = i * chunk_size;
        pthread_create(&threads[i], NULL, multiplication, (void*)&args[i]);
    }

    // Last thread may have a larger chunk size
    args[K - 1].chunk_size = chunk_size + remainder;
    args[K - 1].start_row = (K - 1) * chunk_size;
    pthread_create(&threads[K - 1], NULL, multiplication, (void*)&args[K - 1]);

    // Waiting for threads to finish
    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }

    // Record ending time
    clock_t ending_time = clock();
    double cpu_time_used = ((double)(ending_time - starting_time)) / CLOCKS_PER_SEC;

    // File output
    FILE* output = fopen("out.txt", "w");

    // Writing result matrix to file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(output, "%d ", result[i][j]);
        }
        fprintf(output, "\n");
    }

    // Writing time taken to compute the square matrix
    fprintf(output, "\nTime taken to compute the square matrix: %lf seconds\n", cpu_time_used);
    printf("%d %d %lf\n", N, K, cpu_time_used);

    fclose(output);

    // Memory deallocation
    free(A);
    free(result);
    free(threads);
    free(args);

    return 0;
}

