# Makefile IntroParCo- Hannes + Davide x 2
# Just a test...for now

ifeq ($(OS),Windows_NT)
 baseline = .\baseline.exe
 omp = .\baseline_omp.exe
 lock_s = .\lock_striping.exe
 atomic = .\atomic.exe
 reduction = .\reduction.exe
 lockStriping = .\lockstriping.exe
else
 baseline = ./baseline
 omp = ./baseline_omp
 lock_s = ./lock_striping
 atomic = ./atomic
 reduction = ./reduction
 lockStriping = ./lockstriping
endif	

sha = git rev-parse --short HEAD
	

baseline: Baseline.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o baseline Baseline.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
	$(baseline) ../datasets/stringsInput_n_10_m_1000000.csv ../datasets/stringsTest_n_10_m_1000000.csv 123 6 100000
full_baseline:Baseline.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c 
	gcc -DDEBUG -fopenmp -o baseline Baseline.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
omp: BaselineOpenMP.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o omp BaselineOpenMP.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
	$(omp) ../datasets/stringsInput_n_20_m_1000000.csv ../datasets/stringsTest_n_20_m_1000000.csv 123 6 100000 4
full_omp: BaselineOpenMP.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o omp BaselineOpenMP.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
atomic: Atomic.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o atomic Atomic.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
	$(atomic) ../datasets/stringsInput_n_20_m_1000000.csv ../datasets/stringsTest_n_20_m_1000000.csv 123 6 100000 4 
full_atomic: Atomic.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o atomic Atomic.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
reduction: OMPReduction.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o reduction OMPReduction.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
	$(reduction) ../datasets/stringsInput_n_20_m_1000000.csv ../datasets/stringsTest_n_20_m_1000000.csv 123 6 100000 2
full_reduction: OMPReduction.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o reduction OMPReduction.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
lockStriping: LockStriping.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o lockstriping LockStriping.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm
	$(lockStriping) ../datasets/stringsInput_n_20_m_1000000.csv ../datasets/stringsTest_n_20_m_1000000.csv 123 6 100000 2
full_lockStriping: LockStriping.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c
	gcc -DDEBUG -fopenmp -o lockstriping LockStriping.c bloom/bloom.c murmur3/murmur3.c dataloader/dataloader.c utility/utility.c experiment/experiment.c xoshiro256starstar/xoshiro256starstar.c -lm

check-vars:
ifndef out
	$(error out path variable of the results is undefined (add out=your_result_path to specify it))
endif