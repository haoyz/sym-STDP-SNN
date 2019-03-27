#$ -cwd
#$ -N GeNN_mnist
#$ -S /bin/bash

nvidia-smi
export PATH=/data1/NLPRMNT/public/gcc485/bin:/data1/NLPRMNT/huangxuhui/yunzhe/GeNN/genn-team-genn-9801652/lib/bin/:/data1/NLPRMNT/huangxuhui/yunzhe/GeNN/genn-team-genn-9801652:$PATH
export GENN_PATH=/data1/NLPRMNT/huangxuhui/yunzhe/GeNN/genn-team-genn-9801652
export LD_LIBRARY_PATH=./libstdc++/:/data1/NLPRMNT/public/cuda-6.5.14/lib64:$LD_LIBRARY_PATH
source ~/.bash_profile
./run.sh
