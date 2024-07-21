#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

#define MAX_SIZE 2048

int N;
int K;
int (*A)[MAX_SIZE];   // 2D array for input matrix
int (*result)[MAX_SIZE];  // 2D array for result matrix

typedef struct {
    int start_row;  // Struct to hold thread-specific data
} ThreadArgs;

void* multiplication(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int start_row = args->start_row;

    // Matrix multiplication
    for (int i = start_row - 1; i < N; i += K) {
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
    FILE* input = fopen("inp.txt", "r");
    fscanf(input, "%d %d", &N, &K);

    // Allocate memory for matrices
    A = malloc(N * sizeof(int[MAX_SIZE]));
    result = malloc(N * sizeof(int[MAX_SIZE]));

    // Read input matrix from file
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(input, "%d", &A[i][j]);
        }
    }
    fclose(input);

    // Allocate memory for threads and thread arguments
    pthread_t* threads = malloc(K * sizeof(pthread_t));
    ThreadArgs* args = malloc(K * sizeof(ThreadArgs));

    clock_t starting_time = clock();

    // Create threads for matrix multiplication
    for (int i = 0; i < K; i++) {
        args[i].start_row = i + 1;
        pthread_create(&threads[i], NULL, multiplication, (void*)&args[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t ending_time = clock();
    double total_time = ((double)(ending_time - starting_time)) / CLOCKS_PER_SEC;

    // Write result matrix and execution time to output file
    FILE* output = fopen("out.txt", "w");

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(output, "%d ", result[i][j]);
        }
        fprintf(output, "\n");
    }

    fprintf(output, "\nTime taken to compute the square matrix: %lf seconds\n", total_time);
    printf("%d %d %lf\n", N, K, total_time);

    fclose(output);

    // Free allocated memory
    free(A);
    free(result);
    free(threads);
    free(args);

    return 0;
}

