import random
# remember to change the path to the correct one
fileAddress = "../input/input.txt"

# # below is the all you need to change    big data bad for visualization
# T_min, T_max =                                  1,  10
# num_jobs_min, num_jobs_max =                    100,  200
# num_hosts_min, num_hosts_max =                  10,  20
# alpha_min, alpha_max =                          1,  10       # 1 is 0.01 actually
# transmationSpeed_min, transmationSpeed_max =    10, 800
# core_min, core_max =                            1,  20
# block_min, block_max =                          20,  80
# speed_min,speed_max=                            10, 100
# size_min,size_max =                             1,  1000
# # above is the all you need to change

# below is the all you need to change  good for visualization
T_min, T_max =                                  1,  10
num_jobs_min, num_jobs_max =                    1,  10
num_hosts_min, num_hosts_max =                  1,  3
alpha_min, alpha_max =                          1,  10       # 1 is 0.01 actually
transmationSpeed_min, transmationSpeed_max =    1, 2
core_min, core_max =                            1,  5
block_min, block_max =                          1,  10
speed_min,speed_max=                            10, 100
size_min,size_max =                             1,  1000
# above is the all you need to change
 
# # below is the all you need to change
# num_jobs_min, num_jobs_max =                    10,  11
# num_hosts_min, num_hosts_max =                  2,  4
# alpha_min, alpha_max =                          1,  10       # 1 is 0.01 actually
# transmationSpeed_min, transmationSpeed_max =    10, 800
# core_min, core_max =                            2,  5
# block_min, block_max =                          3,  10
# speed_min,speed_max=                            10, 100
# size_min,size_max =                             1,  1000
# # above is the all you need to change

jb=[]
T =1# random.randint(T_min, T_max)
with open(fileAddress, "w") as f:
    # f.write(str(T)+'\n\n')
    
    for i in range(T):
        num_jobs = random.randint(num_jobs_min, num_jobs_max)
        num_hosts = random.randint(num_hosts_min, num_hosts_max)
        alpha = random.randrange(alpha_min,alpha_max)/100
        transmationSpeed = random.randint(transmationSpeed_min, transmationSpeed_max)
        
        f.write("{} {} {} {}\n".format(num_jobs, num_hosts, alpha, transmationSpeed))
        
        for h in range(num_hosts):
            f.write("{} ".format(random.randint(core_min, core_max)))# cores
        f.write('\n')
        
        for j in range(num_jobs):
            jb.append(random.randint(block_min,block_max))# blocks
            f.write("{} ".format(jb[j]))# blocks
        f.write("\n")

        for j in range(num_jobs):
            f.write("{} ".format(random.randint(speed_min,speed_max)))# speed
        f.write("\n")
        
        for j in range(num_jobs):
            for b in range(jb[j]):
                f.write("{} ".format(random.randint(size_min,size_max)))# size
            f.write("\n")
        
        for j in range(num_jobs):
            for b in range(jb[j]):
                f.write("{} ".format(random.randint(0,num_hosts-1)))#postion
            f.write("\n")
        f.write('\n')