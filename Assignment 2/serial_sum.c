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

int main() {
    double *input_array;
    double t0, total_time, avg_time, result;
    size_t size = 10E7;
    double time[ITER];

    input_array = (double *) malloc(size*sizeof(double));
    
    generate_random(input_array, size);
        
    for (int i=0; i<ITER; i++) {
        t0 = omp_get_wtime();
        result = serial_sum(input_array, size);
        time[i] = omp_get_wtime() - t0;
        total_time += time[i];            
    }

    // Average execution time and std deviation
    avg_time = total_time / ITER;
    
    double std_sum = 0;
    
    for (int i=0; i<ITER; i++) {
        std_sum += pow(time[i]-avg_time, 2);
    }
    
    double std_dev = sqrt((std_sum/ITER));

    printf("Mean execution time of the serial code was %fus with a std deviation of %.15f \n", avg_time*1000000, std_dev);
}