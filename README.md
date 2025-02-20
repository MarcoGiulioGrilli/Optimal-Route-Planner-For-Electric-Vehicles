#🚀 Optimal Route Planning on a Highway with Electric Vehicles

📌 Description

This project implements an efficient algorithm for optimal route planning on a highway with service stations offering electric vehicles for rent. Given a pair of stations, the goal is to determine the shortest route with the fewest stops, considering vehicle autonomy constraints and prioritizing the shortest distance in case of equivalent routes.

🎯 Objectives

✔️ Minimize the number of stops to reach the destination
✔️ Ensure the shortest path selection in case of ties
✔️ Support operations for managing stations and electric vehicles

🏆 Results

✅ Tested on over 100 test cases with flawless results
✅ High-performance implementation optimized for efficiency
✅ Final grade: 30/30 with honors in Algorithms & Data Structures

⚙️ Features
	•	Add and remove service stations
	•	Manage the fleet of electric vehicles (add, remove)
	•	Plan the optimal route between two stations
	•	Parse text files with specific commands

📂 Project Structure
	•	pathPlanner.c → Implementation of the algorithm and project logic
	•	test_cases/ → Folder containing multiple test cases for validation
  	•	Each test case consists of:
	    •	open_#.txt → Input file with commands
	    •	open_#.output.txt → Expected output file
 
🚀 Technologies Used

🔹 C language to maximize performance and optimize memory management
🔹 Graph Search Algorithms Greedy BFS for route planning
🔹 Data structures for efficient station and vehicle management

🛠️ How to Run the Project

# Compilation
gcc pathPlanner.c -o pathPlanner

# Execution with an input file
./pathPlanner < open_#.txt > outputFile
diff ./open_#.output.txt ./outputFile
