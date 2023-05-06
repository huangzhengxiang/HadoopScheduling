import numpy as np
import matplotlib.pyplot as plt

graph_number = 2
job_number = 0
host_number = 0
core_number = []
job_block_num=[]
pos = []
block_size=[]
block_begin=[]
finish_time=0.0

show_Core_name = True
show_block_name = False

width = 0.85 
job_color = ['#264653','#2A9D8E','#E9C46B','#F3A261','#E66F51','#90C9E7',
             '#219EBC','#136783','#FEB704','#FA8600','#EF4143','#457B9D','#FEB3AE']
Title = ['Single Core','SLS+Random Walk']

file = 'visualization.txt'
mode = 'r'

if(graph_number == 2):
    plt.subplots(1,2,sharey=True)

with open(file,mode) as f:
    for g in range(graph_number):
        job_number = int(f.readline().split()[0])
        host_number = int(f.readline().split()[0])
        core_number=[int (x) for x in f.readline().split()]
        job_block_num=[int (x) for x in f.readline().split()]
        
        pos.clear()
        for i in range(job_number):
            pos.append([int (x) for x in f.readline().split()])
        
        block_size.clear()
        for i in range(job_number):
            block_size.append([float (x) for x in f.readline().split()])
            
        block_begin.clear()
        for i in range(job_number):
            block_begin.append([float (x) for x in f.readline().split()])
   
        finish_time=float(f.readline())

        # print(job_number,core_number,job_block_num,pos,block_size,block_begin,sep='\n')
        xticks = ['C'+str(j) for i in range(host_number) for j in range(core_number[i])]

        if(graph_number == 2):
            plt.subplot(1,2,g+1)

        for i in range(job_number):
            # plt.text(pos[i][0], block_begin[i][0]+sum(block_size[i])/2, 'J{}'.format(i), ha='center', va='bottom')
            for b in range(job_block_num[i]):
                p = plt.bar(pos[i][b], block_size[i][b], width, bottom=block_begin[i][b],color=job_color[(i+3)%13])
                p = plt.bar(pos[i][b], 0.05, width, bottom=block_begin[i][b],color='#FFFFFF')
                if show_block_name:
                    plt.text(pos[i][b], block_begin[i][b]+block_size[i][b]/2, 'J{} B{}'.format(i,b), ha='center', va='bottom')
        # plt.text(sum(core_number)/2-0.5, finish_time*1.1, 'Finish Time: {}'.format(finish_time), ha='center', va='bottom')
        plt.ylabel('Time')
        plt.title(Title[g%2]+'\nFinish Time: {}'.format(finish_time))
        # plt.title('Finish Time: {}'.format(finish_time))
        if show_Core_name:
            plt.xticks(range(sum(core_number)), xticks)
        if graph_number != 2:
            plt.yticks(np.arange(0, finish_time*1.2, max(1,int(finish_time/10))))

plt.show()