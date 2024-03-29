#pragma once
#include<vector>
#include<algorithm>
#include<iostream>
#include<iomanip>
#include<numeric>
#include<ctime>
#include<set>
#include<unordered_set>
#include<unordered_map>
#include <sstream>
#include <assert.h>
#include <queue>


using namespace std;

class ResourceScheduler {
public:
    int Scheduler; //0, 1, 2
	int taskType; // 1 or 2
	int caseID;   // 1
	int numJob; // No. 0 ~ numJob-1
	int numHost;// No. 0 ~ numHost-1
	int numCore;
	double St;  // Speed of Transimision
	double alpha;   // g(e)=1-alpha(e-1) alpha>0, e is the number of cores allocated to a single job
	vector<int> hostCore;              // The number of cores for each host
	vector<int> jobBlock;              // The number of blocks for each job
	vector<double> Sc;                    // Speed of calculation for each job
	vector<vector<double>> dataSize;      // Job-> block number-> block size
	vector<vector<int>> location;         // Job-> block number-> block location (host number)

	vector<double> jobFinishTime;                 // The finish time of each job
	vector<int> jobCore;                       // The number of cores allocated to each job.
	vector<vector<tuple<int, int, int>>> runLoc;
	// Block perspective: job number->block number->(hostID, coreID,rank), rank=1 means that block is
	// the first task running on that core of that host

	vector<vector<vector<tuple<int, int, double, double>>>> hostCoreTask;
	// Core perspective: host->core->task-> <job,block,startRunningTime,endRunningTime>
	vector<vector<double>> hostCoreFinishTime;
	// host->core->finishTime
	unordered_map<int, unordered_map<int, tuple<int, int, double>>> transferMap;
	// Transmission situation: (job, block) --> (initHost, destinationHost, propagation time)

	ResourceScheduler(int,int, int);
	void get_sched(struct sol*);
	double __schedule(struct sol*);
	void schedule();
	void outputSolutionFromBlock();
	void outputSolutionFromCore();
	void validFromBlock();
	void validFromCore();
	void visualization(); // An optional function.
	double g(int);
	void init_rand_sol(struct sol*);
};