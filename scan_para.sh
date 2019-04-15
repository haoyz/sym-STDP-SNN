#!/bin/bash

source ~/.bashrc
source ~/.bash_profile
for tc_theta in $(seq 0.2 0.2 1.0); do
    for theta_plus_e in $(seq 0.2 0.2 1.0); do
        for normal in $(seq 0.05 0.05 0.25); do
            new_dir="sym-stdp_"$tc_theta"_"$theta_plus_e"_"$normal
            rm -r $new_dir
            cp -r sym-stdp $new_dir
            cd $new_dir
            sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 0/" global.h
            sed -i "s/#define tc_theta .*/#define tc_theta 1e7*$tc_theta/" src/model.h
            sed -i "s/#define theta_plus_e .*/#define theta_plus_e 0.05*$theta_plus_e/" src/model.h
            sed -i "s/#define NORMAL .*/#define NORMAL 784*$normal/" src/model.h
            echo file: $new_dir para: $tc_theta"_"$theta_plus_e"_"$normal
            ./run.sh
            cd ..
        done
    done
done

# #!/bin/bash

# source ~/.bashrc
# source ~/.bash_profile
# (
#     for tc_theta in $(seq 0.2 0.2 1.0); do
#         new_dir="sym-stdp_"$tc_theta
#         rm -r $new_dir
#         cp -r sym-stdp $new_dir
#         cd $new_dir
#         sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 0/" global.h
#         sed -i "s/#define tc_theta .*/#define tc_theta 1e7*$tc_theta/" src/model.h
#         nohup ./run.sh 1>../no_N=400_$new_dir 2>&1
#         cd ..
#     done
# ) &
# sleep 30s
# (
#     for tc_theta in $(seq 1.2 0.2 2.0); do
#         new_dir="sym-stdp_"$tc_theta
#         rm -r $new_dir
#         cp -r sym-stdp $new_dir
#         cd $new_dir
#         sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 1/" global.h
#         sed -i "s/#define tc_theta .*/#define tc_theta 1e7*$tc_theta/" src/model.h
#         nohup ./run.sh 1>../no_N=400_$new_dir 2>&1
#         cd ..
#     done
# ) &
# sleep 30s
# (
#     for theta_plus_e in $(seq 0.2 0.2 1.0); do
#         new_dir="sym-stdp_400_"$theta_plus_e
#         rm -r $new_dir
#         cp -r sym-stdp $new_dir
#         cd $new_dir
#         sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 2/" global.h
#         sed -i "s/#define theta_plus_e .*/#define theta_plus_e 0.05*$theta_plus_e/" src/model.h
#         nohup ./run.sh 1>../no_N=400_$new_dir 2>&1
#         cd ..
#     done
# ) &
# sleep 30s
# (
#     for theta_plus_e in $(seq 1.0 0.2 2.0); do
#         new_dir="sym-stdp_400_"$theta_plus_e
#         rm -r $new_dir
#         cp -r sym-stdp $new_dir
#         cd $new_dir
#         sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 3/" global.h
#         sed -i "s/#define theta_plus_e .*/#define theta_plus_e 0.05*$theta_plus_e/" src/model.h
#         nohup ./run.sh 1>../no_N=400_$new_dir 2>&1
#         cd ..
#     done
# ) &
# sleep 30s
# (
#     for normal in $(seq 0.1 0.05 0.25); do
#         new_dir="sym-stdp_400_"$normal
#         rm -r $new_dir
#         cp -r sym-stdp $new_dir
#         cd $new_dir
#         sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 4/" global.h
#         sed -i "s/#define tc_theta .*/#define tc_theta 1e7*0.6/" src/model.h
#         sed -i "s/#define theta_plus_e .*/#define theta_plus_e 0.05*0.7/" src/model.h
#         sed -i "s/#define NORMAL .*/#define NORMAL 784*$normal/" src/model.h
#         nohup ./run.sh 1>../no_N=400_$new_dir 2>&1
#     done
# ) &
# sleep 30s
# (
#     for normal in $(seq 0.30 0.05 0.5); do
#         new_dir="sym-stdp_400_"$normal
#         rm -r $new_dir
#         cp -r sym-stdp $new_dir
#         cd $new_dir
#         sed -i "s/#define ID_DEVICE .*/#define ID_DEVICE 5/" global.h
#         sed -i "s/#define tc_theta .*/#define tc_theta 1e7*0.6/" src/model.h
#         sed -i "s/#define theta_plus_e .*/#define theta_plus_e 0.05*0.7/" src/model.h
#         sed -i "s/#define NORMAL .*/#define NORMAL 784*$normal/" src/model.h
#         nohup ./run.sh 1>../no_N=400_$new_dir 2>&1
#         cd ..
#     done
# ) &
