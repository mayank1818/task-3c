#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Function to sort a vector using bubble sort algorithm
void sortVector(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int main(int argc, char ** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int numElements = 20;
    const int chunkSize = numElements / size;

    vector<int> allElements(numElements);
    vector<int> localChunk(chunkSize);

    // Generating random numbers on all processes
    srand(time(NULL) + rank);
    for (int i = 0; i < numElements; i++) {
        allElements[i] = rand() % 100;
    }

    // Displaying the initial unsorted array only by rank 0 process
    if (rank == 0) {
        cout << "Initial array: ";
        for (int i = 0; i < numElements; i++) {
            cout << allElements[i] << " ";
        }
        cout << endl;
    }

    double startTime, endTime;

    // Recording start time
    startTime = MPI_Wtime();

    // Scatter the array chunks to different processes
    MPI_Scatter(&allElements[0], chunkSize, MPI_INT, &localChunk[0], chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process sorts its own chunk
    sortVector(localChunk);

    // Gather all the sorted chunks back to the root process
    MPI_Gather(&localChunk[0], chunkSize, MPI_INT, &allElements[0], chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Displaying the sorted array only by rank 0 process
    if (rank == 0) {
        cout << "Sorted array: ";
        for (int i = 0; i < numElements; i++) {
            cout << allElements[i] << " ";
        }
        cout << endl;
    }

    // Recording end time
    endTime = MPI_Wtime();

    // Outputting execution time only by rank 0 process
    if (rank == 0) {
        cout << "Execution time: " << endTime - startTime << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}