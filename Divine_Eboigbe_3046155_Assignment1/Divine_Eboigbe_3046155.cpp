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
#include  <iomanip>

//Global variables
const static int arraySize = 400;
float gradesArray[arraySize];
int rank;
int world_size;


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


    
  
    int dataChunkSize = arraySize / world_size; // Calculate the size of the data chunk for each node
    float* nodeDataValues = new float[dataChunkSize]; // Create a dynamically allocated array to hold the chunk/piece of data for each node

    if (rank == 0) {
        // Node 0 reads the data and stores it into an array
        createData("moduleGrades.txt");
    }

    // Scatter data to all nodes, including Node 0
    MPI_Scatter(gradesArray, dataChunkSize, MPI_FLOAT, nodeDataValues, dataChunkSize, MPI_FLOAT, 0, MPI_COMM_WORLD);





    //Task C: Helper methods perform calculation
    float nodeSum = sum(nodeDataValues, dataChunkSize);
    float nodeAvg = getAvg(nodeSum, dataChunkSize);
    float nodeMax = getMax(nodeDataValues, dataChunkSize);
    float nodeMin = getMin(nodeDataValues, dataChunkSize);

    // Task D: Each node outputs its calculated values 
    std::cout << "Node " << rank << ":\n";
    std::cout << "Highest grade: " << nodeMax << "\n";
    std::cout << "lowest grade: " << nodeMin << "\n";
    std::cout << "Average: " << std::fixed << std::setprecision(2) << nodeAvg << "\n";
    

    // Task E: Collect node averages on Node 1
    float* nodeAvgArray = new float[world_size]; // Array to store node averages

    // Use MPI_Gather to collect the node averages from all nodes on Node 1
    MPI_Gather(&nodeAvg, 1, MPI_FLOAT, nodeAvgArray, 1, MPI_FLOAT, 1, MPI_COMM_WORLD);

    float total_sum = 0;
    float overall_avg = 0;

    // Synchronize all nodes using MPI_Barrier
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 1) {
        // Calculate the total sum of node averages
        for (int i = 0; i < world_size; i++) {
            total_sum += nodeAvgArray[i];
        }
        // Calculate overall average on Node 1
        overall_avg = total_sum / world_size;


    }

    // Use MPI_Bcast to broadcast overall_avg from Node 1 to all nodes
    MPI_Bcast(&overall_avg, 1, MPI_FLOAT, 1, MPI_COMM_WORLD);

   
    // Print rank and overall_avg
    std::cout << "Node " << rank << ": Overall Average: " << std::fixed << std::setprecision(2) << overall_avg << "\n";

    // Task F: Collect highest and lowest grades on Node 2
    // Define arrays to gather max and min grades from all nodes
    float* max_grades = new float[world_size];
    float* min_grades = new float[world_size];

    // Use MPI_Gather to collect the highest grades from all nodes
    MPI_Gather(&nodeMax, 1, MPI_FLOAT, max_grades, 1, MPI_FLOAT, 2, MPI_COMM_WORLD);

    // Use MPI_Gather to collect the lowest grades from all nodes
    MPI_Gather(&nodeMin, 1, MPI_FLOAT, min_grades, 1, MPI_FLOAT, 2, MPI_COMM_WORLD);

    if (rank == 2) {
        // Calculate the overall highest grade and lowest grade
        float max_grade = 0.0;
        float min_grade = 100.0;

        for (int i = 0; i < world_size; ++i) {
            if (max_grade < max_grades[i]) {
                max_grade = max_grades[i];
            }
            if (min_grade > min_grades[i]) {
                min_grade = min_grades[i];
            }
        }

        // Node 2 will have the overall highest (max_grade) and lowest (min_grade) values
        std::cout << "Node 2: Overall Highest Grade: " << max_grade << "\n";
        std::cout << "Node 2: Overall Lowest Grade: " << min_grade << "\n";
    }

    //clear memmory
    



    //Task G: clear memory 
    delete[] nodeDataValues;
    delete[] nodeAvgArray;
    delete[] max_grades;
    delete[] min_grades;



    std::cout << " ";
    
    









    
   

    MPI_Finalize();

    return 0;
    //TODO: Complete tasks b to h in this method
    // N.B. You do not need to create Coordinator()/Participant() methods for this assignment
}
