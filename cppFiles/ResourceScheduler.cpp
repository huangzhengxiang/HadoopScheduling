#include "ResourceScheduler.h"

ResourceScheduler::ResourceScheduler(int tasktype, int caseID) {
	taskType = tasktype;
	string filePath = "../input/task" + to_string(taskType) + "_case" + to_string(caseID) + ".txt";
	freopen(filePath.c_str(), "r", stdin);
	cin >> numJob >> numHost >> alpha;
	if (taskType == 2)
		cin >> St;

	hostCore.resize(numHost);
	for (int i = 0; i < numHost; i++)
		cin >> hostCore[i];

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

void ResourceScheduler::schedule() {
	// ���´�����Ϊ�˲��ԣ����Ǻ�������

	vector<vector<int>> hostCoreBlock(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreBlock[i].resize(hostCore[i], 0);

	int hid, cid; // hostId, coreId
	for (int i = 0; i < numJob; i++) {
		set<pair<int, int>> allocatedJobCore;
		double jobDataSize = 0.0;
		for (int j = 0; j < jobBlock[i]; j++) {
			jobDataSize += dataSize[i][j];
			if (g(allocatedJobCore.size() + 1) < 0) { // �ټ��º˵����ٶ�Ϊ��
				set<pair<int,int>>::const_iterator position(allocatedJobCore.begin());
				advance(position, rand() % allocatedJobCore.size()); // ���ȡһ���ѷ���˼��㵱ǰ���ݿ�
				hid = position->first;
				cid = position->second;
			}
			else {
				hid = rand() % numHost;
				cid = rand() % hostCore[hid];
				allocatedJobCore.insert({ hid,cid });
			}
			runLoc[i][j] = make_tuple(hid, cid, ++hostCoreBlock[hid][cid]); // rank ��1��ʼ
		}

		jobFinishTime[i] = jobDataSize / (Sc[i] * g(allocatedJobCore.size()));
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

// �����ݿ���ӽǴ�ӡ�������
void ResourceScheduler::outputSolutionFromBlock() {
	cout << "\nTask" << taskType << " Solution (Block Perspective) of Teaching Assistant:\n\n";
	for (int i = 0; i < numJob; i++) {
		double speed = g(jobCore[i]) * Sc[i];
		cout << "Job" << i << " obtains " << jobCore[i] << " cores (speed=" << speed << ") and finishes at time " << jobFinishTime[i] << ": \n";
		for (int j = 0; j < jobBlock[i]; j++) {
			cout << "\tBlock" << j << ": H" << get<0>(runLoc[i][j]) << ", C" << get<1>(runLoc[i][j]) << ", R" << get<2>(runLoc[i][j]) << " (time=" << fixed << setprecision(2) << dataSize[i][j] / speed << ")" << " \n";
		}
		cout << "\n";
	}

	cout << "The maximum finish time: " << *max_element(jobFinishTime.begin(), jobFinishTime.end()) << "\n";
	cout << "The sum of response time: " << accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0) << "\n\n";
}

// �Ժ˵��ӽǴ�ӡ�������?
void ResourceScheduler::outputSolutionFromCore() {
	cout << "\nTask" << taskType << " Solution (Core Perspective) of Teaching Assistant:\n\n";
	double maxHostTime = 0, totalRunningTime = 0.0;
	for (int i = 0; i < numHost; i++) {
		double hostTime = *max_element(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end());
		maxHostTime = max(hostTime, maxHostTime);
		totalRunningTime += accumulate(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end(), 0.0);
		cout << "Host" << i << " finishes at time " << hostTime << ":\n\n";
		for (int j = 0; j < hostCore[i]; j++) {
			cout << "\tCore" << j << " has " << hostCoreTask[i][j].size() << " tasks and finishes at time " << hostCoreFinishTime[i][j] << ":\n";
			for (int k = 0; k < hostCoreTask[i][j].size(); k++) {
				cout << "\t\tJ" << setw(2) << setfill('0') << get<0>(hostCoreTask[i][j][k]) << ", B" << setw(2) << setfill('0') << get<1>(hostCoreTask[i][j][k]) << ", runTime " << fixed << setprecision(1) << setw(5) << setfill('0') << get<2>(hostCoreTask[i][j][k]) << " to " << fixed << setprecision(1) << setw(5) << setfill('0') << get<3>(hostCoreTask[i][j][k]) << "\n";
			}
			cout << "\n";
		}
		cout << "\n\n";
	}
	cout << "The maximum finish time of hosts: " << maxHostTime << "\n";
	cout << "The total efficacious running time: " << totalRunningTime << "\n";
	cout << "Utilization rate: " << totalRunningTime / accumulate(hostCore.begin(), hostCore.end(), 0.0) / maxHostTime << "\n\n";
}

// �����ݿ���ӽǽ������?
void ResourceScheduler::validFromBlock() {

	// 1. ��֤jobFinishTime: "jobFinishTime�ĺ�" Ӧ��С�� "����ִ��ÿ��job, �������ݿ��ڸ��Գ�ʼ����������ͬһ������ֱ�����е�ʱ��ĺ�?"
	double actualTime = accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0);
	double maxRunningTime = 0.0;
	vector<unordered_set<int>> jobInitLocSet(numJob); // ÿ��job��ʼ��ɢ���ļ���������
	vector<int> jobTotalSize(numJob, 0); // ÿ��job�������ݿ��С�?��
	_for(i, 0, numJob) {
		_for(j, 0, location[i].size()) {
			jobInitLocSet[i].insert(location[i][j]);
			jobTotalSize[i] += dataSize[i][j];
		}
		maxRunningTime += (double)jobTotalSize[i] / (Sc[i]) / g(jobInitLocSet[i].size());
	}
	assert(maxRunningTime >= actualTime);

	// 2. ��֤jobCore: �����ÿ��Job�ĺ���Ӧ���� [1,�ܺ���] ��
	int numCore = accumulate(hostCore.begin(), hostCore.end(), 0);
	_for(i, 0, numJob)
		assert(0 < jobCore[i] && jobCore[i] <= numCore);

	// 3. ��֤runLoc: ÿ�����ڵ����ݿ����д�������ͬ, ������[1,...,n]��һ��ȫ����, n��������ϱ����ȵ����ݿ����
	//    ˳��Ѵӿ��ӽǵĴ��?��Ϊ�Ӻ��ӽǵĴ�

	// ����ÿ���˱������˶������ݿ�
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

	// ��ʼ������-�˵������б�����
	_for(i, 0, numHost)
		_for(j, 0, hostCore[i])
		hostCoreTask[i][j].resize(hostCoreBlock[i][j], make_tuple(-1, -1, -1, -1));

	// ���Խ�ÿ�����ݿ���䵽����?-�˵������б���
	_for(i, 0, numJob) {
		_for(j, 0, jobBlock[i]) {
			int h = get<0>(runLoc[i][j]);
			int c = get<1>(runLoc[i][j]);
			int r = get<2>(runLoc[i][j]);

			if (h < 0 || h >= numHost || c<0 || c >= hostCore[h] || r <= 0 || r>hostCoreBlock[h][c])
				cerr << "Error: Host " << h << " core " << c << " rank " << r << " should not be allocated by job " << i << " block " << j << "\n";
			else if (get<0>(hostCoreTask[h][c][r - 1]) != -1) // ע�� r ��Ҫ��һ
				cerr << "Error: Host " << h << " core " << c << " rank " << r << " is already allocated by job " << get<0>(hostCoreTask[h][c][r - 1])
				<< " block " << get<1>(hostCoreTask[h][c][r - 1]) << ": " << get<2>(hostCoreTask[h][c][r - 1]) << " ~ " << get<3>(hostCoreTask[h][c][r - 1])
				<< " when allocate job " << i << " block " << j << "\n";

			hostCoreTask[h][c][r - 1] = make_tuple(i, j, -1.0, -1.0);
		}
	}

	// ��������-���ϵ��������?, ���԰Ѵ����б�transferListҲ�����?

	// ��ʼ��hostCoreFinishTime, ��ģ��˵�ǰ���е����ĸ�ʱ��?
	hostCoreFinishTime.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreFinishTime[i].resize(hostCore[i], 0.0);

	int blockFinished = 0;
	int numTotalBlock = accumulate(jobBlock.begin(), jobBlock.end(), 0);
	vector<double> jobStartTime(numJob, 0.0); // ��¼ÿ��Job�Ŀ�ʼʱ

	while (blockFinished < numTotalBlock) {
		// 1. ���������ʱ��ĺ�, ���Ե���������ϵ���һ�����Ӧ��Job���������ݿ�
		//    �жϲ���¼��Ҫ����Ŀ鵽transferMap��

		// 2. ������ ���job�� ÿ�����ݿ������ȵĺ��������һ�����job���ݿ�Ľ���ʱ��?, 
		//    �����ʱ�����?��ǰJob����ʼʱ��jobStartTime

		// 3. ģ����ɵ�ǰJob���������ݿ�
		//    ����hostCoreTask��hostCoreFinishTime��blockFinished

	}

}

void ResourceScheduler::validFromCore() {
	// 1. hostCoreTask���Ƿ��������Job���������ݿ�������ظ�?, �õ�runLoc


	// 2. hostCoreTask���������ݿ������ʱ�䲻�����ص�?, �õ�jobStartTime��Job������˳��


	// 3. ����ÿ��Job�ĺ���, hostCoreTask��ÿ�����ݿ������ʱ��Ӧ���ں�������?


}

void ResourceScheduler::visualization() {

}

double ResourceScheduler::g(int e) {
	return 1 - alpha * (e - 1);
}
