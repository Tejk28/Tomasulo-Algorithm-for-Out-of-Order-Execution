# Tomasulo's Algorithm for Out of Order Execution
 
Author: Tejashree Kulkarni

University: North Carolina State University;  
Professor: Eric Rotenberg 
Course: ECE 563; Architecture of Microprocessor;  
Duration: Aug 2022- December2022 

The technology used: OOPs concepts, instruction stages implementation

# Project Overview:

This project simulates a fully pipelined CPU that implements Tomasulo’s Algorithm for out-of-order execution of instructions. 
The objective was to create a superscalar processor model capable of processing multiple instructions per cycle while eliminating false dependencies using register renaming with the help of a Reorde Buffer (ROB). Performance metrics, such as Instructions Per Cycle (IPC), were measured by varying the sizes of the ROB, Issue Queue, and Superscalar width.


# Project Accomplishment:

- Simulating a fully pipelined CPU that implements Tomasulo’s algorithm for out of order execution of instructions.  
- Modeled out-of-order superscalar processor that processes N instructions per cycle. 
- The model eliminated false dependencies by using Register renaming with the help of Reorder buffer. 
- Studied the performance of the model by measuring IPC for various sizes of Reorder Buffer, Issue Queue, and Superscalar width. 

# Project Structure:

Directory Details:

- pipeline.c: Core implementation of Tomasulo's algorithm.
- sim_proc.h: Defines data structures, constants, and prototypes for core simulation functionality.
- provided_files: Contains various implementations (C, C++, Java) and support files to facilitate testing and comparison.
	- cpp_files: C++ implementation and traces.
	- java_files: Java-based implementation of the algorithm.
	- tool: Additional tools for analyzing and executing simulations.
	- c_files: Contains C implementation and testing files.
	- validation: Collection of validation files for verifying the accuracy and performance of the simulation.
docs: Documentation and project reports, including details of the design, experiments, and performance results.

