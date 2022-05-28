#include "ResourceScheduler.h"

#define _for(i,a,b) for( int i=(a); i<(b); ++i)
#define __for(i,a,b) for( int i=(a); i<=(b); ++i)
#define mfor(i,a,b) for(int i=(a);i>(b);--i)
#define mmfor(i,a,b) for(int i=(a);i>=(b);--i)

using namespace std;
//MAX_CORE
#define MAX_CORE 10
// SLS iteration control
#define MAX_TRY 100
#define MAX_FLIP 500
//infinity
#define INF 0x3f3f3f3f
//Random Walk
#define Pf 0.2

int WALK(double p)
{
    //walk (1) for 1-p possibility!
    int possibility = p * RAND_MAX;
    return (rand() > possibility);
}

struct sol{
    vector<int> order;
    vector<int> core_num;
};

void init_sol(struct sol* cur_sol, int num_job)
{
    cur_sol->order.resize(num_job,0);
    cur_sol->core_num.resize(num_job,0);
}

void copy_sol(struct sol* from_sol, struct sol* to_sol,int num_job)
{
    //originally of the same size!
    for(int i = 0;i < num_job; ++i){
        to_sol->order[i] = from_sol->order[i];
        to_sol->core_num[i] = from_sol->core_num[i];
    }
}

void swap_sol_order(struct sol* cur_sol, int flip)
{
    int tmp = cur_sol->order[flip];
    cur_sol->order[flip] = cur_sol->order[flip+1];
    cur_sol->order[flip+1] = tmp;
}

void print_sol(struct sol* cur_sol, int num_job)
{
    cout << "This is the order!\n";
    for(int i = 0;i<num_job;++i){
        cout << cur_sol->order[i] << ' ';
    }
    cout << "\nThis is the core_num!\n";
    for(int i = 0;i<num_job;++i){
        cout << cur_sol->core_num[i] << ' ';
    }
    cout << endl;
}

struct cmp{
    double amt;
    size_t idx; //core
    size_t idh; //host
};
bool operator<(const cmp &a,const cmp &b)
{
    return a.amt < b.amt;
    //idle ones first!
}
bool operator>(const cmp &a,const cmp &b)
{
    return a.amt > b.amt;
    //idle ones first!
}


ResourceScheduler::ResourceScheduler(int scheduler,int tasktype, int caseID) {
	taskType = tasktype;
	Scheduler = scheduler;
	numCore = 0;
//	string filePath = "task" + to_string(taskType) + "_case" + to_string(caseID) + ".txt";
//    freopen(filePath, "r", stdin);
//	freopen("test2.txt", "r", stdin);
	cin >> numJob >> numHost >> alpha;
	if (taskType == 2)
		cin >> St;

	hostCore.resize(numHost);
	for (int i = 0; i < numHost; i++){
        cin >> hostCore[i];
        numCore += hostCore[i];
	}

	jobBlock.resize(numJob);
	for (int i = 0; i < numJob; i++)
		cin >> jobBlock[i];

	Sc.resize(numJob);
	for (int i = 0; i < numJob; i++)
		cin >> Sc[i];

	dataSize.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		dataSize[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++)
			cin >> dataSize[i][j];
	}

	location.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		location[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++)
			cin >> location[i][j];
	}

	jobFinishTime.resize(numJob, 0);
	jobCore.resize(numJob);

	runLoc.resize(numJob);
	for (int i = 0; i < numJob; i++)
		runLoc[i].resize(jobBlock[i]);

	hostCoreTask.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreTask[i].resize(hostCore[i]);

	hostCoreFinishTime.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreFinishTime[i].resize(hostCore[i], 0);
}

double ResourceScheduler::g(int e) {
	return 1 - (double)alpha * (e - 1);
}

void ResourceScheduler::get_sched(struct sol* cur_sol)
{
    /* We only record the scheduling answer for the best answer!
     * We only need to modify the __schedule() a little to suit it for recording!
     */
    //follow the order of cur_sol->order to calculate!
    priority_queue<cmp,vector<cmp>,greater<cmp>> CoreQueue;
    for(int i=0;i < numHost;++i){
        for(int j=0;j < hostCore[i];++j){
            cmp tem;
            tem.amt=0.0,tem.idx=j,tem.idh=i;
            CoreQueue.push(tem);
        }
    }
    //initialize the priority queue for greedy algorithm!
    vector<vector<int>> hostCoreBlock;
    hostCoreBlock.resize(numHost);
    for (int i = 0; i < numHost; i++)
        hostCoreBlock[i].resize(hostCore[i], 0);
    //hostCoreBlock is [hid,cid,the rank's block on the core]!

    for(int i = 0;i < numJob;++i){
        int cur_index = cur_sol->order[i];
        int cur_core = cur_sol->core_num[cur_index];
        jobCore[cur_index] = cur_core;
        //get the core list to run on!
        double group_start = 0;
        priority_queue<cmp,vector<cmp>,greater<cmp>> LocalCoreQueue;
        for(int j = 0;j < cur_core;++j){
            //pick the cores!
            cmp tem = CoreQueue.top();
            CoreQueue.pop();
            group_start = tem.amt;
            tem.amt = 0;
            LocalCoreQueue.push(tem);
        }

        //sub-optimization!
        vector<double> blockTime;
        blockTime.resize(jobBlock[cur_index],0);
        for (int j = 0; j < jobBlock[cur_index]; j++) {
            blockTime[j] = dataSize[cur_index][j] / Sc[cur_index] / g(cur_core);
        }
        vector<size_t> idx(blockTime.size());
        iota(idx.begin(),idx.end(),0);
        sort(idx.begin(),idx.end(),[&blockTime](double i1, double i2)->bool { return blockTime[i1] > blockTime[i2]; });
        for (int j = 0; j < jobBlock[cur_index]; j++) {
            cmp tem = LocalCoreQueue.top();
            LocalCoreQueue.pop();
            double starttime = tem.amt;
            double transTime = ((tem.idh != location[cur_index][idx[j]]) ? (dataSize[cur_index][idx[j]] / St) : 0);
            double endtime = starttime + blockTime[idx[j]] + transTime;
            tem.amt = endtime;
            LocalCoreQueue.push(tem);
            runLoc[cur_index][idx[j]] = make_tuple(tem.idh,tem.idx,++hostCoreBlock[tem.idh][tem.idx]);
            hostCoreTask[tem.idh][tem.idx].push_back(make_tuple(cur_index,idx[j],starttime+group_start,endtime+group_start));
            transferMap[cur_index][idx[j]] = make_tuple(location[cur_index][idx[j]],tem.idh,transTime);
        }

        //update the time!
        vector<cmp> CoreList;
        double group_delta = 0.0;
        for(int j = 0;j < cur_core;++j){
            cmp tem = LocalCoreQueue.top();
            LocalCoreQueue.pop();
            if(tem.amt > group_delta){
                group_delta = tem.amt;
            }
            CoreList.push_back(tem);
        }
        double group_finish = group_delta + group_start;
        for(int j = 0;j < cur_core;++j){
            cmp tem = CoreList.back();
            tem.amt = group_finish;
            CoreQueue.push(tem);
            CoreList.pop_back();
            hostCoreFinishTime[tem.idh][tem.idx] = tem.amt;
        }
        jobFinishTime[cur_index] = group_finish;
    }
}

double ResourceScheduler::__schedule(struct sol* cur_sol)
{
    /* Here we calculate  the time of a given scheduling sol
     * specifically with greedy approximation and random walk!
     */
    //follow the order of cur_sol->order to calculate!
    priority_queue<cmp,vector<cmp>,greater<cmp>> CoreQueue;
    for(int i=0;i < numHost;++i){
        for(int j=0;j < hostCore[i];++j){
            cmp tem;
            tem.amt=0.0,tem.idx=j,tem.idh=i;
            CoreQueue.push(tem);
        }
    }
    //initialize the priority queue for greedy algorithm!

    for(int i = 0;i < numJob;++i){
        int cur_index = cur_sol->order[i];
        int cur_core = cur_sol->core_num[cur_index];
        //get the core list to run on!
        double group_start = 0;
        priority_queue<cmp,vector<cmp>,greater<cmp>> LocalCoreQueue;
        for(int j = 0;j < cur_core;++j){
            //pick the cores!
            cmp tem = CoreQueue.top();
            CoreQueue.pop();
            group_start = tem.amt;
            tem.amt = 0;
            LocalCoreQueue.push(tem);
        }

        //sub-optimization!
        vector<double> blockTime;
        blockTime.resize(jobBlock[cur_index],0);
        for (int j = 0; j < jobBlock[cur_index]; j++) {
            blockTime[j] = dataSize[cur_index][j] / Sc[cur_index] / g(cur_core);
        }
        vector<size_t> idx(blockTime.size());
        iota(idx.begin(),idx.end(),0);
        sort(idx.begin(),idx.end(),[&blockTime](double i1, double i2)->bool { return blockTime[i1] > blockTime[i2]; });
        for (int j = 0; j < jobBlock[cur_index]; j++) {
            cmp tem = LocalCoreQueue.top();
            LocalCoreQueue.pop();
            tem.amt = tem.amt + blockTime[idx[j]] + \
            ((tem.idh != location[cur_index][idx[j]]) ? (dataSize[cur_index][idx[j]] / St) : 0);
            LocalCoreQueue.push(tem);
        }

        //update the time!
        vector<cmp> CoreList;
        double group_delta = 0.0;
        for(int j = 0;j < cur_core;++j){
            cmp tem = LocalCoreQueue.top();
            LocalCoreQueue.pop();
            if(tem.amt > group_delta){
                group_delta = tem.amt;
            }
            CoreList.push_back(tem);
        }
        double group_finish = group_delta + group_start;
        for(int j = 0;j < cur_core;++j){
            cmp tem = CoreList.back();
            tem.amt = group_finish;
            CoreQueue.push(tem);
            CoreList.pop_back();
        }
    }
    for(int i = 0;i<numCore-1;++i){
        CoreQueue.pop();
    }
    double ans = CoreQueue.top().amt;
    //test
//    cout << ans << endl;
    //test
    return ans;
}

void ResourceScheduler::init_rand_sol(struct sol* cur_sol)
{
    for(int i=0;i<numJob;++i){
        cur_sol->order[i] = i;
        //randomize the core_num to initiate our run!
        cur_sol->core_num[i] = rand() * min(min(jobBlock[i], MAX_CORE), numCore)/ RAND_MAX + 1; //1...jobBlock[i] or MAX_CORE
        while(g(cur_sol->core_num[i]) < 0){
            cur_sol->core_num[i]--;
        }
    }
    //the scheduling order of the jobs!
    random_shuffle(cur_sol->order.begin(),cur_sol->order.end());
}

void ResourceScheduler::schedule() {
    if(Scheduler==1){
        vector<double> jobTime;
        jobTime.resize(numJob,0);
        for(int i=0;i<numJob;++i){
            for (int j = 0; j < jobBlock[i]; j++) {
                jobTime[i] += dataSize[i][j];
            }
            // cout << i <<' '<< jobTime[i] << endl;
            jobTime[i] /= Sc[i];
        }
        vector<size_t> idx(jobTime.size());
        iota(idx.begin(),idx.end(),0);
        sort(idx.begin(),idx.end(),[&jobTime](size_t i1, size_t i2)->bool { return jobTime[i1] > jobTime[i2]; });
        //idx is the sorted index of jobTime, from big to small!

        vector<vector<int>> hostCoreBlock;
        hostCoreBlock.resize(numHost);
        for (int i = 0; i < numHost; i++)
            hostCoreBlock[i].resize(hostCore[i], 0);
        //hostCoreBlock is [hid,cid,the rank's block on the core]!

        //task 1
        for(int i=0;i < numHost;++i){
            priority_queue<cmp,vector<cmp>,greater<cmp>> CoreQueue;
            for(int j=0;j < hostCore[i];++j){
                cmp tem;
                tem.amt=0.0,tem.idx=j;
                CoreQueue.push(tem);
            }
            for(int j=0;j < numJob;++j){
                cmp coreSelection = CoreQueue.top();
                CoreQueue.pop();
                for(int k=0; k < jobBlock[idx[j]];++k){
                    double starttime = coreSelection.amt;
                    double endtime = coreSelection.amt + dataSize[idx[j]][k] / Sc[idx[j]] / g(1);
                    coreSelection.amt = endtime;
                    hostCoreTask[i][coreSelection.idx].push_back(make_tuple(idx[j],k,starttime,endtime));
                    runLoc[idx[j]][k] = make_tuple(i,coreSelection.idx,++hostCoreBlock[i][coreSelection.idx]);
                }
                jobFinishTime[idx[j]] = coreSelection.amt;
                jobCore[idx[j]] = 1;
                hostCoreFinishTime[i][coreSelection.idx] = coreSelection.amt;
                CoreQueue.push(coreSelection);
            }
        }
    }
    if(Scheduler==2){
        vector<double> jobTime;
        jobTime.resize(numJob,0);
        for(int i=0;i<numJob;++i){
            for (int j = 0; j < jobBlock[i]; j++) {
                jobTime[i] += dataSize[i][j];
            }
            // cout << i <<' '<< jobTime[i] << endl;
            jobTime[i] /= Sc[i];
        }
        vector<size_t> idx(jobTime.size());
        iota(idx.begin(),idx.end(),0);
        sort(idx.begin(),idx.end(),[&jobTime](size_t i1, size_t i2)->bool { return jobTime[i1] > jobTime[i2]; });
        //idx is the sorted index of jobTime, from big to small!

        vector<vector<int>> hostCoreBlock;
        hostCoreBlock.resize(numHost);
        for (int i = 0; i < numHost; i++)
            hostCoreBlock[i].resize(hostCore[i], 0);
        //hostCoreBlock is [hid,cid,the rank's block on the core]!

        //task 2
        priority_queue<cmp,vector<cmp>,greater<cmp>> CoreQueue;
        for(int i=0;i < numHost;++i){
            for(int j=0;j < hostCore[i];++j){
                cmp tem;
                tem.amt=0.0,tem.idx=j,tem.idh=i;
                CoreQueue.push(tem);
            }
        }

        for(int j=0;j < numJob;++j){
            cmp coreSelection = CoreQueue.top();
            size_t hid=coreSelection.idh,cid=coreSelection.idx;
            CoreQueue.pop();
            for(int k=0; k < jobBlock[idx[j]];++k){
                double starttime = coreSelection.amt;
                double endtime = coreSelection.amt + dataSize[idx[j]][k] / Sc[idx[j]] / g(1);

                double transTime=(location[idx[j]][k]!=hid)?(dataSize[idx[j]][k]/St):0;
                endtime+=transTime;
                transferMap[idx[j]][k]=make_tuple(location[idx[j]][k],hid,transTime);

                coreSelection.amt = endtime;
                hostCoreTask[hid][cid].push_back(make_tuple(idx[j],k,starttime,endtime));
                runLoc[idx[j]][k] = make_tuple(hid,cid,++hostCoreBlock[hid][cid]);
            }
            jobFinishTime[idx[j]] = coreSelection.amt;
            jobCore[idx[j]] = 1;
            hostCoreFinishTime[hid][cid] = coreSelection.amt;
            CoreQueue.push(coreSelection);
        }
    }
    if(Scheduler==3){
        double best_time = INF;
        struct sol best_sol;
        init_sol(&best_sol,numJob);
        for(int i = 0; i < MAX_TRY; ++i){
            struct sol cur_sol;
            double cur_time = 0;
            //randomize a possible solution field!
            init_sol(&cur_sol,numJob);
            init_rand_sol(&cur_sol);
            //test!
//            print_sol(&cur_sol,numJob);
            //test!
            cur_time = __schedule(&cur_sol);
            if(cur_time < best_time){
                copy_sol(&cur_sol,&best_sol,numJob);
                best_time = cur_time;
            }
            for(int j = 0; j<MAX_FLIP; ++j){
                int choice = rand()*2 / RAND_MAX; //0...1
                switch(choice){
                    case 0:
                    {
                        //change the order!
                        struct sol ori_sol;
                        int flip = rand() * (numJob-1) / RAND_MAX; //0...numJob-2
                        double ori_time = 0;
                        init_sol(&ori_sol,numJob);
                        copy_sol(&cur_sol,&ori_sol,numJob);
                        if(numJob == 1) break;
                        swap_sol_order(&cur_sol,flip);
                        ori_time = __schedule(&ori_sol);
                        cur_time = __schedule(&cur_sol);
                        if(cur_time < best_time){
                            copy_sol(&cur_sol,&best_sol,numJob);
                            best_time = cur_time;
                        }
                        //test!
//                        print_sol(&ori_sol,numJob);
//                        print_sol(&cur_sol,numJob);
                        //test!
                        //greedy or not, random walk!
                        int walk = WALK(Pf);
                        if(walk){
                            if(ori_time < cur_time){
                                cur_time =ori_time;
                                copy_sol(&ori_sol,&cur_sol,numJob);
                            }
                        }else
                        {   //take the worse step!
                            if(ori_time > cur_time){
                                cur_time =ori_time;
                                copy_sol(&ori_sol,&cur_sol,numJob);
                            }
                        }
                        break;
                    }
                    case 1:
                    {
                        //flip the core number!
                        struct sol oth_sol;
                        int flip = rand()*numJob / RAND_MAX; //0...numJob-1
                        double oth_time = 0;
                        init_sol(&oth_sol,numJob);
                        copy_sol(&cur_sol,&oth_sol,numJob);
                        //oth minus the designated core number
                        if(oth_sol.core_num[flip] - 1 > 0) oth_sol.core_num[flip]--;
                        oth_time = __schedule(&oth_sol);
                        //oth minus the designated core number
                        if(g(cur_sol.core_num[flip]+1) > 0 \
                           && cur_sol.core_num[flip]+1 <= min(min(MAX_CORE,jobBlock[flip]), numCore))
                            cur_sol.core_num[flip]++;
                        cur_time = __schedule(&cur_sol);
                        if(oth_time < best_time){
                            copy_sol(&oth_sol,&best_sol,numJob);
                            best_time = oth_time;
                        }
                        if(cur_time < best_time){
                            copy_sol(&cur_sol,&best_sol,numJob);
                            best_time = cur_time;
                        }
                        //test!
//                        print_sol(&oth_sol,numJob);
//                        print_sol(&cur_sol,numJob);
                        //test!
                        //greedy or not, random walk!
                        int walk = WALK(Pf);
                        if(walk){
                            if(oth_time < cur_time){
                                cur_time =oth_time;
                                copy_sol(&oth_sol,&cur_sol,numJob);
                            }
                        }else
                        {   //take the worse step!
                            if(oth_time > cur_time){
                                cur_time =oth_time;
                                copy_sol(&oth_sol,&cur_sol,numJob);
                            }
                        }
                        break;
                    }
                }
            }
        }
        get_sched(&best_sol);
//        cout << (__schedule(&best_sol) == best_time) << endl;
//        cout << best_time << endl;
    }
    if(Scheduler==0)
    {
        // 以下代码是为了测试，不是合理方案

        vector<vector<int>> hostCoreBlock(numHost);
        for (int i = 0; i < numHost; i++)
            hostCoreBlock[i].resize(hostCore[i], 0);

        int hid, cid; // hostId, coreId
        for (int i = 0; i < numJob; i++) {
            set<pair<int, int>> allocatedJobCore;
            double jobDataSize = 0.0;
            for (int j = 0; j < jobBlock[i]; j++) {
                jobDataSize += dataSize[i][j];
                if (g(allocatedJobCore.size() + 1) < 0) { // 再加新核导致速度为负
                    set<pair<int,int>>::const_iterator position(allocatedJobCore.begin());
                    advance(position, rand() % allocatedJobCore.size()); // 随机取一个已分配核计算当前数据块
                    hid = position->first;
                    cid = position->second;
                }
                else {
                    hid = rand() % numHost;
                    cid = rand() % hostCore[hid];
                    allocatedJobCore.insert({ hid,cid });
                }
                runLoc[i][j] = make_tuple(hid, cid, ++hostCoreBlock[hid][cid]); // rank 从1开始
            }

            jobFinishTime[i] = jobDataSize / (Sc[i] * g(allocatedJobCore.size()));// why is this?
            jobCore[i] = allocatedJobCore.size();
        }

        for (int i = 0; i < numHost; i++) {
            for (int j = 0; j < hostCore[i]; j++) {
                int numTask = rand() % 10 + 1;
                hostCoreTask[i][j].resize(numTask);
                for (int k = 0; k < numTask; k++) {
                    int jid = rand() % numJob;
                    int bid = rand() % jobBlock[jid];
                    int endTime = hostCoreFinishTime[i][j] + rand() % 100 + 1;
                    hostCoreTask[i][j][k] = make_tuple(jid, bid, hostCoreFinishTime[i][j], endTime);
                    hostCoreFinishTime[i][j] = endTime;
                }
            }
        }
    }
}

// 以数据块的视角打印解决方案
void ResourceScheduler::outputSolutionFromBlock() {
	cout << "\nTask" << taskType << " Solution (Block Perspective) of Teaching Assistant:\n\n";
	for (int i = 0; i < numJob; i++) {
		double speed = g(jobCore[i]) * Sc[i];
		cout << "Job" << i << " obtains " << jobCore[i] << " cores (speed=" << speed << ") and finishes at time " << jobFinishTime[i] << ": \n";
		for (int j = 0; j < jobBlock[i]; j++) {
			cout << "\tBlock" << j << ": H" << get<0>(runLoc[i][j]) << ", C" << get<1>(runLoc[i][j]) << ", R" << \
                get<2>(runLoc[i][j]) << " (time=" << fixed << setprecision(2) << dataSize[i][j] / speed << ")" << " \n";
		}
		cout << "\n";
	}

	cout << "The maximum finish time: " << *max_element(jobFinishTime.begin(), jobFinishTime.end()) << "\n";
	cout << "The sum of response time: " << accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0) << "\n\n";
}

void ResourceScheduler::outputSolutionFromCore() {
	cout << "\nTask" << taskType << " Solution (Core Perspective) of Our team:\n\n";
	double maxHostTime = 0, totalRunningTime = 0.0;
	for (int i = 0; i < numHost; i++) {
        // calculation!
		double hostTime = *max_element(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end());
		double HostFinishTime = 0.0;
		vector<double> CoreFinishTime;
		CoreFinishTime.resize(hostCore[i],0.0);
		maxHostTime = max(hostTime, maxHostTime);
        for (int j = 0; j < hostCore[i]; j++) {
			for (int k = 0; k < hostCoreTask[i][j].size(); k++) {
				CoreFinishTime[j] = get<3>(hostCoreTask[i][j][k]);
				totalRunningTime += get<3>(hostCoreTask[i][j][k]) - get<2>(hostCoreTask[i][j][k]);
			}
		}
		HostFinishTime = *max_element(CoreFinishTime.begin(),CoreFinishTime.end());
		// output!
		cout << fixed << setprecision(2) << "Host" << i << " finishes its jobs at "<< HostFinishTime <<", and is released at " << hostTime << ":\n\n";
		for (int j = 0; j < hostCore[i]; j++) {
			cout << fixed << setprecision(2)  << "\tCore" << j << " has " << hostCoreTask[i][j].size() << " tasks. It finishes its jobs at " << CoreFinishTime[j] << \
			", and is released at " << hostCoreFinishTime[i][j] << ":\n";
			for (int k = 0; k < hostCoreTask[i][j].size(); k++) {
				cout << fixed << setprecision(1) << "\t\tJ" << setw(2) << setfill('0') << get<0>(hostCoreTask[i][j][k]) << \
				", B" << fixed << setprecision(1) << setw(2) << setfill('0') << get<1>(hostCoreTask[i][j][k]) << ", runTime " << setw(5) << setfill('0') << get<2>(hostCoreTask[i][j][k]) << \
				" to " << fixed << setprecision(1) << setw(5) << setfill('0') << get<3>(hostCoreTask[i][j][k]) << "\n";
			}
			cout << "\n";
		}
		cout << "\n\n";
	}
	cout << "The overall finish time is: " << fixed << setprecision(2) << maxHostTime << "\n";
	cout << "The total efficacious running time: " << fixed << setprecision(2) << totalRunningTime << "\n";
	cout << "Utilization rate: " << fixed << setprecision(2) << totalRunningTime / numCore / maxHostTime << "\n\n";
}

// 从数据块的视角进行验证
void ResourceScheduler::validFromBlock() {

	// 1. 验证jobFinishTime: "jobFinishTime的和" 应该小于 "依次执行每个job, 所有数据块在各自初始所在主机的同一个核上直接运行的时间的和"
	// double actualTime = accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0);
	double actualTime = 0.0;
	for (int i = 0; i < numJob; i++) {
		actualTime = max(actualTime, jobFinishTime[i]);
	}
	double maxRunningTime = 0.0;
	vector<unordered_set<int>> jobInitLocSet(numJob); // 每个job初始分散在哪几个主机上
	vector<int> jobTotalSize(numJob, 0); // 每个job所有数据块大小之和
	_for(i, 0, numJob) {
		_for(j, 0, location[i].size()) {
			jobInitLocSet[i].insert(location[i][j]);
			jobTotalSize[i] += dataSize[i][j];
		}
		maxRunningTime += (double)jobTotalSize[i] / (Sc[i]) / g(jobInitLocSet[i].size());
	}
	assert(maxRunningTime >= actualTime);

	// 2. 验证jobCore: 分配给每个Job的核数应该在 [1,总核数] 内
	int numCore = accumulate(hostCore.begin(), hostCore.end(), 0);
	_for(i, 0, numJob)
		assert(0 < jobCore[i] && jobCore[i] <= numCore);

	// 3. 验证runLoc: 每个核内的数据块运行次序不能相同, 必须是[1,...,n]的一个全排列, n是这个核上被调度的数据块个数
	//    顺便把从块视角的答案转化为从核视角的答案

	// 计算每个核被分配了多少数据块
	vector<vector<int>> hostCoreBlock(numHost);
	_for(i, 0, numHost)
		hostCoreBlock[i].resize(hostCore[i], 0);

	_for(i, 0, numJob) {
		_for(j, 0, jobBlock[i]) {
			int h = get<0>(runLoc[i][j]);
			int c = get<1>(runLoc[i][j]);
			if (h < 0 || h >= numHost || c < 0 || c >= hostCore[h])
				cerr << "Error: Job" << i << " block " << j << " should not be allocated in host " << h << " core " << c << "\n";
			hostCoreBlock[h][c]++;
		}
	}

	// 初始化主机-核的任务列表长度
	_for(i, 0, numHost)
		_for(j, 0, hostCore[i])
		hostCoreTask[i][j].resize(hostCoreBlock[i][j], make_tuple(-1, -1, -1, -1));

	// 尝试将每个数据块分配到主机-核的任务列表中
	_for(i, 0, numJob) {
		_for(j, 0, jobBlock[i]) {
			int h = get<0>(runLoc[i][j]);
			int c = get<1>(runLoc[i][j]);
			int r = get<2>(runLoc[i][j]);

			if (h < 0 || h >= numHost || c<0 || c >= hostCore[h] || r <= 0 || r>hostCoreBlock[h][c])
				cerr << "Error: Host " << h << " core " << c << " rank " << r << " should not be allocated by job " << i << " block " << j << "\n";
			else if (get<0>(hostCoreTask[h][c][r - 1]) != -1) // 注意 r 需要减一
				cerr << "Error: Host " << h << " core " << c << " rank " << r << " is already allocated by job " << get<0>(hostCoreTask[h][c][r - 1])
				<< " block " << get<1>(hostCoreTask[h][c][r - 1]) << ": " << get<2>(hostCoreTask[h][c][r - 1]) << " ~ " << get<3>(hostCoreTask[h][c][r - 1])
				<< " when allocate job " << i << " block " << j << "\n";

			hostCoreTask[h][c][r - 1] = make_tuple(i, j, -1.0, -1.0);
		}
	}

	// 计算主机-核上的运行情况, 可以把传输列表transferList也算出来

	// 初始化hostCoreFinishTime, 逐步模拟核当前运行到了哪个时间
	hostCoreFinishTime.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreFinishTime[i].resize(hostCore[i], 0.0);

	int blockFinished = 0;
	int numTotalBlock = accumulate(jobBlock.begin(), jobBlock.end(), 0);
	vector<double> jobStartTime(numJob, 0.0); // 记录每个Job的开始时

	while (blockFinished < numTotalBlock) {
		// 1. 找最短运行时间的核, 尝试调度这个核上的下一个块对应的Job的所有数据块
		//    判断并记录需要传输的块到transferMap中

		// 2. 找最大的 这个job的 每个数据块所调度的核上完成上一个别的job数据块的结束时间,
		//    将这个时间作为当前Job的起始时间jobStartTime

		// 3. 模拟完成当前Job的所有数据块
		//    更新hostCoreTask、hostCoreFinishTime、blockFinished
		break;
	}
}

void ResourceScheduler::validFromCore() {
	// 1. hostCoreTask上是否包含所有Job的所有数据块而且无重复, 得到runLoc


	// 2. hostCoreTask上所有数据块的运行时间不能有重叠, 得到jobStartTime和Job的运行顺序


	// 3. 根据每个Job的核数, hostCoreTask上每个数据块的运行时间应该在合理长度


}

void ResourceScheduler::visualization() {
    cout << numJob << endl;
    cout << numHost << endl;
    vector<vector<int>> Coreindex;
    Coreindex.resize(numHost);
    for(int i=0;i < numHost;++i){
        cout << hostCore[i] << ' ';
        Coreindex[i].resize(hostCore[i],0);
        if(i == 0){
            for(int j = 0;j<hostCore[i];++j){
                Coreindex[i][j] = j;
            }
        }else
        {
            for(int j = 0;j<hostCore[i];++j){
                Coreindex[i][j] = Coreindex[i-1].back() + 1 + j;
            }
        }
    }
    cout << endl;
    for(int i = 0;i<numJob;++i){
        cout << jobBlock[i] << ' ';
    }
    cout << endl;
    for(int i = 0;i<numJob;++i){
        for(int j=0;j<jobBlock[i];++j){
            int cur_host = get<0>(runLoc[i][j]);
            int cur_core = get<1>(runLoc[i][j]);
            cout << Coreindex[cur_host][cur_core] << ' ';
        }
        cout << endl;
    }
    for(int i = 0;i<numJob;++i){
        for(int j=0;j<jobBlock[i];++j){
            int cur_host = get<0>(runLoc[i][j]);
            int cur_core = get<1>(runLoc[i][j]);
            int cur_rank = get<2>(runLoc[i][j]) - 1;
            cout << get<3>(hostCoreTask[cur_host][cur_core][cur_rank]) - get<2>(hostCoreTask[cur_host][cur_core][cur_rank]) << ' ';
        }
        cout << endl;
    }
    for(int i = 0;i<numJob;++i){
        for(int j=0;j<jobBlock[i];++j){
            int cur_host = get<0>(runLoc[i][j]);
            int cur_core = get<1>(runLoc[i][j]);
            int cur_rank = get<2>(runLoc[i][j]) - 1;
            cout << get<2>(hostCoreTask[cur_host][cur_core][cur_rank]) << ' ';
        }
        cout << endl;
    }
    double maxHostTime = 0;
    for(int i = 0;i < numHost;++i){
        double HostTime =  *max_element(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end());
        maxHostTime = max(maxHostTime, HostTime);
    }
    cout << maxHostTime << endl;
}
