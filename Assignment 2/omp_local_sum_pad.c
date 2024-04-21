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

typedef struct { double val; char pad[128]; } t_vals; 

double omp_local_sum(double *x, size_t size)
{
    int max_threads = omp_get_max_threads();
    t_vals local_sum[max_threads];
    #pragma omp parallel shared(local_sum)
    {
      int id = omp_get_thread_num();
	    local_sum[id].val = 0;
      #pragma omp for
      for (size_t i = 0; i < size; i++) {
        local_sum[id].val += x[i];
      }
    }
    double sum_val = 0;
    for (int i = 0; i < max_threads; i++) {
       sum_val += local_sum[i].val;
    }

    return sum_val;
}

int main() {
    double *input_array;
    double t0, total_time, avg_time[4], result;
    size_t size = 10E7;
    int n_threads[] = {1, 32, 64, 128};
    double time[ITER];
    double std_dev[4];

    input_array = (double *) malloc(size*sizeof(double));
    
    generate_random(input_array, size);       //generate a random input

    for (int j=0; j<4; j++) {
        omp_set_num_threads(n_threads[j]);    //set the number of threads

        total_time = 0;                       //initialize for each execution 
        double std_sum = 0;

        for (int i=0; i<ITER; i++) {
            t0 = omp_get_wtime();
            result = omp_local_sum(input_array, size);
            time[i] = omp_get_wtime() - t0;
            total_time += time[i];            
        }

        //Average execution time and std deviation
        avg_time[j] = total_time / ITER;
                
        for (int i=0; i<ITER; i++) {
            std_sum += pow(time[i]-avg_time[j], 2);
        }
        
        std_dev[j] = sqrt((std_sum / ITER));

        printf("Serial = %f, Parallel = %f \n", serial_sum(input_array, size), result);
    }

    for (int j=0; j<4; j++) {
        printf("Num of threads: %d, Mean execution time: %fs, Std deviation: %.15f \n", n_threads[j], avg_time[j], std_dev[j]);
    }

}