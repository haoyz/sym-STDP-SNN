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
            sed -i "s/#define tc_theta .*/#define tc_theta 1e7*$tc_theta/" src/model.h
            sed -i "s/#define theta_plus_e .*/#define theta_plus_e 0.05*$theta_plus_e/" src/model.h
            sed -i "s/#define _NORMAL .*/#define _NORMAL $normal/" src/model.h
            # ./run.sh 1>>output_file 2>>error_file
            ./run.sh
            cd ..
        done
    done
done
