#include<iostream>
#include<fstream>
#include <string>
#include <cstring>
#include <ctime>
// #include "../hFiles/ResourceScheduler.h"
// #include "../hFiles/Util.h"

#include "ResourceScheduler.h"
#include "Util.h"
#include<cstdlib>

int main() {
	srand(time(NULL));
	// three optional input files
	freopen("../input/task1_case1.txt", "r", stdin);
	// freopen("../input/task2_case1.txt", "r", stdin);
	// freopen("../input/input.txt", "r", stdin);


	// // // freopen("/output/task_1_case_1.txt", "w", stdout);
	//don't know why that output doesn't work


	// // // Needs to ensure there is a file named "task_$taskType_case_$caseID.txt" in 'input' filefolder.
	// // // You can test more cases if you want.
	// // //scheduler = [0: test, 1: greedy approximation, 2: modified greedy, 3:stochastic greedy local search with random walk]
	// normally,scheduler 3 is the best
	// caseID doesn't work
	int scheduler = 2,taskType = 1, caseID = 1;

// ResourceScheduler instance object.
	ResourceScheduler rs(scheduler,taskType, caseID);

	// // // // The main task you need to do is to implement this function.
	rs.schedule();
	
	rs.outputSolutionFromCore();
	rs.outputSolutionFromBlock();

	rs.visualization();


	// exp 2 // show comparasion between greedy approximation and modified greedy
	// mainly to show the performance
	// freopen("../input/input.txt", "r", stdin);
	// ResourceScheduler rs1(2, 2, 1);
	// rs1.schedule();
	// rs1.outputSolutionFromCore();
	// rs1.outputSolutionFromBlock();
	// fclose(stdin);

	// srand(time(NULL));
	// freopen("../input/input.txt", "r", stdin);
	// ResourceScheduler rs2(3, 2, 1);
	// rs2.schedule();
	// rs2.outputSolutionFromCore();
	// rs2.outputSolutionFromBlock();
	// fclose(stdin);


	// // exp 3 //show comparison between greedy approximation and modified greedy
	// // you can adjust the graph number in visualization.py to 2
	// freopen("../input/input.txt", "r", stdin);
	// freopen("../pythonFiles/visualization.txt", "r", stdout);
	// fclose(stdout);
	// ResourceScheduler rs1(2, 2, 1);
	// rs1.schedule();
	// rs1.visualization();
	// fclose(stdin);

	// srand(time(NULL));
	// freopen("../input/input.txt", "r", stdin);
	// ResourceScheduler rs2(3, 2, 1);
	// rs2.schedule();
	// rs2.visualization();
	// fclose(stdin);


	
	return 0;
}
