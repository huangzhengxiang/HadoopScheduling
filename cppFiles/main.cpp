#include<iostream>
#include<fstream>

// #include "../hFiles/ResourceScheduler.h"
// #include "../hFiles/Util.h"

#include "ResourceScheduler.h"
#include "Util.h"

int main() {
    srand(time(0));
	// Needs to ensure there is a file named "task_$taskType_case_$caseID.txt" in 'input' filefolder.
	// You can test more cases if you want.
	//scheduler = [0: test, 1: greedy approximation, 2: modified greedy, 3: stochastic greedy search with random wlk]
	int scheduler = 3,taskType = 2, caseID = 2;

	// Load data from file "task_$taskType_case_$caseID.txt" to class ResourceScheduler instance object.
	ResourceScheduler rs(scheduler,taskType, caseID);

	// Carefully! It will write the randomly generated data into file "task_$taskType_case_$caseID.txt".
	// generator(rs, taskType, caseID);

	// The main task you need to do is to implement this function.
	rs.schedule();
	rs.outputSolutionFromCore();
	rs.outputSolutionFromBlock();
	rs.visualization();
//	rs.validFromBlock();

	return 0;
}
