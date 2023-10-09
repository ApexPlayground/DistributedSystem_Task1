// Distributed Systems - 2023
// Assignment 1 Part 1 - template
// TODO: Divine Eboigbe - 3046155
// TODO: Complete the tasks as per the specification
// TODO: Don't forget to delete TODO comments and fully comment your code

#include <iostream>
#include <fstream>
#include <mpi.h>
#include <string>
#include <vector>

//Global variables
const static int arraySize = 400;
float gradesArray[arraySize];
int rank;
int world_size;

//TODO: Complete these helper methods as detailed in specification
// print array function
void printArray(float* arrayValues, int arraySize) {
    for (int i = 0; i < arraySize; i++) {
        std::cout << arrayValues[i] << " ";
    }
    std::cout << std::endl;

}

//Array sum function
float sum(float* arrayValues, int size) {
    float mySum = 0;
    //loops the array values
    for (int i = 0; i < size; i++) {
        //new sum is sum of prevoius index + sum of current index at each iteration
        mySum = mySum + arrayValues[i];
    }
    return mySum;
}

//Array Average function
float getAvg(float sum, int size) {
    float myAvg = 0;
    //Use average fomular
    myAvg = sum / size;
    return myAvg;
}

//Array Max Function

float getMax(float* arrayValues, int size) {
    float myMax = arrayValues[0];
    //loops through the values 
    for (int i = 1; i < size; ++i) {
        // checks the index that is larger than the first index at each iteration and assigns the largest value to myMax
        if (arrayValues[i] > myMax) {
            myMax = arrayValues[i];
        }
    }
    return myMax;
}

float getMin(float* arrayValues, int size) {
    float myMin = arrayValues[0];
    //loops through the values 
    for (int i = 1; i < size; ++i) {
        // checks the index that is lesser than the first index at each iteration and assigns the lowest value to myMin
        if (arrayValues[i] < myMin) {
            myMin = arrayValues[i];
        }
    }
    return myMin;
}


//You do not need to modify this method
void createData(std::string filename) {
    /*
    Populates an array with the data read from file.
    */
    std::ifstream file;
    file.open(filename, std::ios::in);
    if (!file) {
        std::cout << "Error reading file: " << filename << std::endl;
        std::cout << "Make sure file is saved in the same directory as your executable for this project." << std::endl;
    }
    else {
        std::cout << "Reading file: " << filename << std::endl;
        std::string line;
        //use a while loop with getLine() to read file line by line and store into array
        int i = 0;
        while (getline(file, line, ' '))
        {
            // add text from file into array
            gradesArray[i] = std::stof(line);
            //std::cout << std::to_string(gradesArray[i]) << std::endl; //debug
            i++;
        }
        file.close();
    }
}


//Main method - work to be done here
int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    
    // Initialize the array to hold data
    float* grades = nullptr;
    int localSize = 0;

    //Node 0 reads from moduleGrades.txt
    if (rank == 0) {
        // Node 0 reads data from the file
        
        std::ifstream inputFile(argv[1]);

        if (!inputFile.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Read the data from the file and store it in gradesArray
        int i = 0;
        while (i < arraySize && inputFile >> gradesArray[i]) {
            i++;
        }

        // Determine the size of the data to be sent to each process
        int dataSize = i;
        localSize = dataSize / world_size;
        if (rank == world_size - 1) {
            // Adjust local size for the last process to include any remaining data
            localSize = dataSize - (world_size - 1) * localSize;
        }

        // Allocate memory for the local grades array on Node 0
        grades = new float[localSize];

        // Scatter the data to all processes
        MPI_Scatter(gradesArray, localSize, MPI_FLOAT, grades, localSize, MPI_FLOAT, 0, MPI_COMM_WORLD);

        inputFile.close();
    }
    else {
        // Receive the data from Node 0
        MPI_Scatter(nullptr, localSize, MPI_FLOAT, grades, localSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();

    return 0;
    //TODO: Complete tasks b to h in this method
    // N.B. You do not need to create Coordinator()/Participant() methods for this assignment
}
