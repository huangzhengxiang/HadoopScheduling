## Hadoop Scheduling Problem

Kai Liu,  kai\_liu@sjtu.edu.cn

Rong Shan, shanrong@sjtu.edu.cn

Zhengxiang Huang, huangzhengxiang@sjtu.edu.cn

### Introduction
The course project focuses on resource scheduling problem on single or multiple hosts in Hadoop. Since the Load Balancing Problem is a NP-hard problem, existing algorithms can not solve the optimal solution with polynomial time complexity. Our group tried to design an algorithm with polynomial time complexity to approximate the optimal solution, based on the idea of greedy algorithm. 

In Problem 1, we proved the NP hardness of our problem, we computed the approximation ratio of simple greedy approximation, explored some tricky cases, and finally, did some analysis and visualization jobs. 

In Problem 2, we further employed the heuristic algorithm of Stochastic Greedy Local Search with Random Walk to made up the disadvantages when applying intuitive single-core allocation to deal with the much more complicated Comprehensive Version. We theoretically analyzed the performance of our algorithm and test it on the test cases we generated randomly. Finally, we visualized the results.

We, as a team, learned a lot in the very days of cooperation, thanks.

### Compilation

调整main.cpp中的文件路径，选择想要选用的scheduler，编译运行即可输出。
会生成visualization.txt文档,将其放至pythonFile文档下，运行visualiztion.py程序即可生成可视化
可以调用pythonFiles文件夹下generator.py程序自动化生成参数，具体数据范围可自行设置，记得调整main.cpp文件路径


### Data Input

1. Job 数，主机数，g 函数的 alpha 值，传输速度(Task1 中不会输入速度)
2. 每个主机的核数
3. 每个 job 的块数
4. 每个 job 的速度
5. 每个 job 的每个数据块的大小（一行对应一个 job）
6. 每个 job 的每个数据块的初始位置（一行对应一个 job）

### Results

Greedy Algorithm works pretty well on simple cases, where its approximation rate < 2.

<img src=".\Figures\single-host-good.png" alt="single-host-good" style="zoom:77%;" />


However, for more difficult cases, simple greedy performs poor, especially for those that job number < core number, where naive greedy leaves cores idle.

In worst case, there is no bound for approximation rate. In other words, in the worst case, the results may be arbitrarily bad for naive greedy.

<img src=".\Figures\single-host-bad.png" alt="single-host-bad" style="zoom:77%;" />

Therefore, we introduce our Stochastic Local Search + Random Walk Algorithm, whose performance exceeds Naive Greedy by large.

The following figures compare <b>Ours</b> SLS+Random Walk with Naive Greedy.

<img src=".\Figures\single-host-COM.png" alt="single-host-COM" style="zoom:77%;" />


<img src=".\Figures\multi-host-COM.png" alt="single-host-COM" style="zoom:77%;" />

### Conclusion
To sum up, our Simple Greedy Algorithm performs well for some \textit{Favourable Cases} while the data blocks are distributed evenly. However, the performance of this trivial algorithm can degrade heavily when some \textit{Bad Cases} happen.

To compare with it, when the decay factor is not too large, no matter how the data blocks or cores on hosts distribute, our SLS + Walk algorithm can always give a pretty good result which is a very close approximation of the optimal solution, since the algorithm can nearly traverse through the feasible solution space.  

However, the decay factor and big-size data can challenge our SLS + Random Walk Algorithm a lot. From our analyzed time complexity, \textit{MAX\_TRY} and \textit{MAX\_FLIP} influence the performance a lot. With larger randomization iteration times, we may get better results, but the time cost goes up. Moreover, with larger decay factor, the performance of parallel processing suffers a lot and sequential running is better.

From the analysis above, it hinds on us that in real world, when handling similar load-balancing problem or resource scheduling in distributed system, we should take factors like transmission time and processing speed decay when doing parallel computing into consideration, which may influence a lot to our decision whether parallel computing is better. And the trade-off between time complexity and performance is another question we must consider.

Last but not least, there isn't a cure-all algorithm for this problem in the foreseeable future. However, the basic concept of greedy approximation and random/heuristic algorithm is always an option to us for an acceptable good solution.

### Acknowledgement

In writing this paper, we have benefited from the presence of our teacher and our classmates a lot. They generously helped us figure out some hard problems and made many invaluable suggestions. We hereby extend out grateful thanks for their kind help, without which the paper would not have been what it is.
    
To be honest, the beginning is the most difficult. Because there is not a clear direction, we tried many possible ways but all failed. Later, we just jumped out of the limitation of the methods that we had already learnt, tried to learn some new methods to solve the problem. And that how we come up these fascinating algorithm. We have to admit that the most powerful algorithm sometimes comes from simple ideas, like the random walk algorithm and greedy search algorithm. Also, we learnt how to analyse the approximation rate and the NP related problem. In sum, we really benefited a lot from this project.
    
As for suggestions, we hope that there could be some useful reference or possible methods to solve the problem, otherwise we really have to spend a lot of time on designing the algorithm. It feels bad when an algorithm doesn't work. But the designing process and see the excellent output is really attractive.
    
Thank you very much! Let it be our ending.