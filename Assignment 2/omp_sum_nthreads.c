#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define ITER 100

// Initialize the input input_array
void generate_random(double *input, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

double serial_sum(double *x, size_t size)
{
  double sum_val = 0.0;
  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  return sum_val;
}

double omp_sum(double *x, size_t size)
{
    double sum_val = 0.0;
    #pragma omp parallel for
        for (size_t i = 0; i < size; i++) {
            sum_val += x[i];
        }
    return sum_val;
}

int main() {
    double *input_array;
    double t0, total_time, avg_time[9], result;
    size_t size = 10E7;
    int n_threads[] = {1, 2, 4, 8, 16, 20, 24, 28, 32};
    double time[ITER];
    double std_dev[9];

    input_array = (double *) malloc(size*sizeof(double));
    
    generate_random(input_array, size);       //generate a random input

    for (int j=0; j<9; j++) {
        omp_set_num_threads(n_threads[j]);    //set the number of threads

        total_time = 0;                       //initialize for each execution 
        double std_sum = 0;

        for (int i=0; i<ITER; i++) {
            t0 = omp_get_wtime();
            result = omp_sum(input_array, size);
            time[i] = omp_get_wtime() - t0;
            total_time += time[i];            
        }

        // Average execution time and std deviation

        avg_time[j] = total_time / ITER;
        
        for (int i=0; i<ITER; i++) {
            std_sum += pow(time[i]-avg_time[j], 2);
        }
        
        std_dev[j] = sqrt((std_sum / ITER));
    }

    for (int j=0; j<9; j++) {
        printf("Num of threads: %d, Mean execution time: %fs, Std deviation: %.15f \n", n_threads[j], avg_time[j], std_dev[j]);
    }

}