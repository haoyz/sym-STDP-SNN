GREEN='\033[0;32m'
NC='\033[0m' #No Color

# # # # #####  STDP training period  ######
echo -e "${GREEN}Cleaning...${NC}"
./rm.sh

echo -e "${GREEN}Generating code...${NC}"
genn-buildmodel.sh src/model.cc

echo -e "${GREEN}Compiling generated code...${NC}"
make

echo -e "${GREEN}Running...${NC}"
./train

# # svm training period for 60000 training samples

# # echo -e "${GREEN}Cleaning...${NC}"
# # ./rm.sh
# # rm output/svm*.dat

# # echo -e "${GREEN}Generating code...${NC}"
# # genn-buildmodel.sh src/model.cc

# # echo -e "${GREEN}Compiling generated code...${NC}"
# # make

# # echo -e "${GREEN}Running...${NC}"
# # ./train

# # # TEST_MODE: label and SL and SVM for 10000 testing samples

# # sed -i "s/#define SVM_TRAIN/\/\/ #define SVM_TRAIN/" global.h

# echo -e "${GREEN}Cleaning...${NC}"
# ./rm.sh

# echo -e "${GREEN}Generating code...${NC}"
# genn-buildmodel.sh src/model.cc

# echo -e "${GREEN}Compiling generated code...${NC}"
# make

# echo -e "${GREEN}Running...${NC}"
# ./train

# # # svm-train -t 0 ./output/svm_train.dat svm.model
# # # svm-predict ./output/svm_test.dat svm.model /dev/null
# # # sed -i "s/\/\/ #define SVM_TRAIN/#define SVM_TRAIN/" global.h
