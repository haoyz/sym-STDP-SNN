GREEN='\033[0;32m'
NC='\033[0m' #No Color

######  STDP training period  ######
echo -e "${GREEN}Cleaning...${NC}"
./rm.sh

echo -e "${GREEN}Generating code...${NC}"
genn-buildmodel.sh src/model.cc

echo -e "${GREEN}Compiling generated code...${NC}"
make

echo -e "${GREEN}Running...${NC}"
./train
