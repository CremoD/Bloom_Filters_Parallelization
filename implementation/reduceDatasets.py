import os
import subprocess

#datasets = ['_n_20_m_100000000', '_n_20_m_10000000', '_n_20_m_1000000', '_n_10_m_100000000', '_n_10_m_10000000', 
#'_n_10_m_1000000', '-a2_n_60_m_100000000', '-a2_n_60_m_10000000', '-a2_n_60_m_1000000']
datasets = ['_n_20_m_100000000', '_n_10_m_100000000',  '-a2_n_60_m_100000000']


for dataset in datasets:
    prefix = ''
    if '-a2' in dataset:
        prefix = 'PL'
    
    #file = open("../datasets/strings{0}Input{1}.csv".format(prefix, dataset))

    steps = [100,1000,10000, 100000, 1000000, 10000000]


    for i in steps:
        print("Dataset: ", dataset, " - Lines: ", i)
        subprocess.Popen(["powershell","Get-Content ../datasets/strings{0}Test{1}.csv | select -First {2} | Out-File ../datasets/{2}_strings{0}Test{1}.csv".format(prefix, dataset, i)])
        #os.system('powershell -NoProfile Get-Content ../datasets/strings{0}Input{1}.csv | powershell -NoProfile select -First {2} | powershell -NoProfile Out-File ../datasets/{2}_strings{0}Input{1}.csv'.format(prefix, dataset, i))