#include <iostream>
#include <algorithm>

using namespace std;

#define N 1024
#define RADIUS 3
#define BLOCK_SIZE 16

__global__void stencil_1d(int *in, int *out) {
    __shared__ int temp[BLOCK_SIZE + 2*RADIUS];
    int gindex = threadldx.x + blockldx.x * blockDim.x;
    int lindex = threadldx.x + RADIUS;

    // Lee los elementos de entrada en la memoria compartida
    // Read input elements into shared memory

    temp[lindex] = in[gindex];
    if (threadldx.x < RADIUS) {
        temp[lindex - RADIUS] = in[gindex - RADIUS];
        temp[lindex + BLOCK_SIZE] = in[gindex + BLOCK_SIZE];
    }

    // Sincroniza (asegurando que todos los datos esten disponibles)
    // Synchronize (ensure all the data is available)

    __syncthreads();

    // Aplica la plantilla
    // Apply the stencil
    int result = o;
    for (int offset = -RADIUS; offset <= RADIUS; offset++)
        result += temp[lindex + offset];

    // Almacena el resultado
    // Store the result
    out[gindex] = result;

    }

    void fill_ints(int *x, int n) {
        fill_n(x, n,1);
    }

    int main (void) {
        int *in, *out;
        int *d_in, *d_out;
        int size = (N + 2*RADIUS)* sizeof(int);


        // Asigna el espacio para copias de host y los valores de configuración
        // Alloc space for host copies and setup values

        in = (int *)malloc(size); fill_ints(in, N + 2*RADIUS);
        out = (int *) malloc(size); fill_ints(out, N + 2*RADIUS);

        // Asigna el espacio para copias de dispositivos
        // Alloc space for device copies
        cudaMalloc((void **)&d_in,size);
        cudaMalloc((void **)&d_out,size);

        // Copia al dispositivo
        //Copy to device

        cudaMemcpy(d_in, in, size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_out, out, size, cudaMemcpyHostToDevice);

        // Inicia stencil_1d() en el kernel del GPU
        //Launch stencil_1d() kernel on GPU
        stencil_1d <<<N/BLOCK_SIZE, BLOCK_SIZE>>>(d_in + RADIUS, d_out + RADIUS);

        // Copia el resultado de vuelta al host.
        //Copy result back to host
        cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);

        // Limpia la memoria
        //Cleanup
        free(in); free(out);
        cudaFree(d_in); cudaFree(d_out);
        return 0;


        }
