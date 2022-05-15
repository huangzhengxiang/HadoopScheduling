#include<iostream>
#include<fstream>

// #include "../hFiles/ResourceScheduler.h"
// #include "../hFiles/Util.h"

#include "ResourceScheduler.h"
#include "Util.h"

int main() {
	// Needs to ensure there is a file named "task_$taskType_case_$caseID.txt" in 'input' filefolder.
	// You can test more cases if you want.
	freopen("output/task_1_case_1.txt", "w", stdout);
	int taskType = 1, caseID = 1;

	// Load data from file "task_$taskType_case_$caseID.txt" to class ResourceScheduler instance object.
	ResourceScheduler rs(taskType, caseID); 

	// Carefully! It will write the randomly generated data into file "task_$taskType_case_$caseID.txt".
	// generator(rs, taskType, caseID); 

	// The main task you need to do is to implement this function.
	rs.schedule(); 

	rs.outputSolutionFromBlock();
	rs.outputSolutionFromCore();

	rs.validFromBlock(); // Not necessary && Not Sufficient.

	return 0;
}

