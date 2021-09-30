import os
import sys
import time
import subprocess


command = ""
extension = ""
bar = ""
if sys.platform == 'win32':
    command = "del"
    extension = ".exe"
    bar = ".\\"
    #bar = "./"
else:
    command = "rm"
    extension = ""
    bar = "./"




os.system(f"cd ../results && {command} *.csv")
os.system("cd ../baseline")
print(os.popen("dir").read())
time_list = []

#removing larger sizes: '_n_20_m_100000000', '_n_10_m_100000000', '-a2_n_60_m_100000000',

datasets = ['_n_20_m_10000000', '_n_20_m_1000000', '_n_10_m_10000000', 
'_n_10_m_1000000', '-a2_n_60_m_10000000', '-a2_n_60_m_1000000']

threads = [1,2,4,8,16,32]



# STRONG SCALING EXPERIMENTS

def make_experiment(experiment):
    
    os.system('make -B {0}'.format(experiment))
    
    ex = experiment.split('_')[1]

    executable = bar + ex + extension

    for dataset in datasets:

        prefix = ''

        if '-a2' in dataset:
            prefix = 'PL'
        
        if ex == 'baseline':
            print('{0} ../datasets/strings{1}Input{2}.csv ../datasets/strings{1}Test{2}.csv 123 6 100000 >> ../results/{3}.csv'.format(executable, prefix , dataset, ex))
            os.system('{0} ../datasets/strings{1}Input{2}.csv ../datasets/strings{1}Test{2}.csv 123 6 100000 >> ../results/{3}.csv'.format(executable, prefix , dataset, ex))
            #p = subprocess.Popen(["powershell", '{0} ../datasets/strings{1}Input{2} ../datasets/strings{1}Test{2} 123 6 100000 >> ../results/{3}.csv'.format(executable, prefix , dataset, ex)])
            #p.kill()
        else:
            for thread in threads:
                print('{0} ../datasets/strings{1}Input{2}.csv ../datasets/strings{1}Test{2}.csv 123 6 100000 {3} >> ../results/{4}.csv'.format(executable, prefix , dataset, thread, ex))
                os.system('{0} ../datasets/strings{1}Input{2}.csv ../datasets/strings{1}Test{2}.csv 123 6 100000 {3} >> ../results/{4}.csv'.format(executable, prefix , dataset, thread, ex))
                #p = subprocess.Popen(["powershell", '{0} ../datasets/strings{1}Input{2} ../datasets/strings{1}Test{2} 123 6 100000 {3} >> ../results/{4}.csv'.format(executable, prefix , dataset, thread, ex)])
                #p.kill()

print("-----------------STARTING STRONG SCALING EXPERIMENTS-------------------\n\n\n")

for i in range(10):
    start = time.time()
    print("------ STRONG SCALING - ITERATION {0} -------\n\n".format(i))
    make_experiment("full_baseline")
    print("baseline done\n")
    make_experiment("full_omp")
    print("omp done\n")
    make_experiment("full_atomic")
    print("atomic done\n")
    make_experiment("full_reduction")
    print("reduction done\n")
    make_experiment("full_lockStriping")
    print("lockStriping done\n")
    end = time.time()
    print("Time elapsed: {0} min".format((end - start)/60))
    time_list.append((end - start)/60)
    print("=====================================================================\n")

my_sum = 0
for i in time_list:
    my_sum += i
print("Total time: {0}".format(my_sum))


#---------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------
#---------------------------------------------------------------------------------------------------------------------


threads = [1,2,4,8,16,32]
sizes = [100,1000,10000,100000,1000000,10000000]
datasets = ['_n_20', '_n_10', '-a2_n_60']
def make_weak_experiment(experiment):
    
    os.system('make -B {0}'.format(experiment))
    
    ex = experiment.split('_')[1]

    executable = bar + ex + extension

    for i in range(6):
        thread = threads[i]
        size = sizes[i]

        if ex == 'baseline':
            os.system('{0} ../datasets/stringsInput_n_20_m_{1}.csv ../datasets/stringsTest_n_20_m_{1}.csv 123 6 100000 >> ../results/weak_{2}.csv'.format(executable, size, ex))
            os.system('{0} ../datasets/stringsInput_n_10_m_{1}.csv ../datasets/stringsTest_n_10_m_{1}.csv 123 6 100000 >> ../results/weak_{2}.csv'.format(executable, size, ex))
            os.system('{0} ../datasets/stringsPLInput-a2_n_60_m_{1}.csv ../datasets/stringsPLTesta2_n_60_m_{1}.csv 123 6 100000 >> ../results/weak_{2}.csv'.format(executable, size,ex))
        else:  
            os.system('{0} ../datasets/stringsInput_n_20_m_{1}.csv ../datasets/stringsTest_n_20_m_{1}.csv 123 6 100000 {2} >> ../results/weak_{3}.csv'.format(executable, size, thread, ex))
            os.system('{0} ../datasets/stringsInput_n_10_m_{1}.csv ../datasets/stringsTest_n_10_m_{1}.csv 123 6 100000 {2} >> ../results/weak_{3}.csv'.format(executable, size, thread,ex))
            os.system('{0} ../datasets/stringsPLInput-a2_n_60_m_{1}.csv ../datasets/stringsPLTesta2_n_60_m_{1}.csv 123 6 100000 {2} >> ../results/weak_{3}.csv'.format(executable, size, thread,ex))







print("-----------------STARTING WEAK SCALING EXPERIMENTS-------------------\n\n\n")

for i in range(10):
    start = time.time()
    print("------ WEAK SCALING - ITERATION {0} -------\n\n".format(i))
    make_weak_experiment("full_baseline")
    print("baseline done\n")
    make_weak_experiment("full_omp")
    print("omp done\n")
    make_weak_experiment("full_atomic")
    print("atomic done\n")
    make_weak_experiment("full_reduction")
    print("reduction done\n")
    make_weak_experiment("full_lockStriping")
    print("lockStriping done\n")
    end = time.time()
    print("Time elapsed: {0} min".format((end - start)/60))
    time_list.append((end - start)/60)
    print("=====================================================================\n")

my_sum = 0
for i in time_list:
    my_sum += i
print("Total time: {0}".format(my_sum))

