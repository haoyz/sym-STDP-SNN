EXECUTABLE      :=train
SOURCES         :=src/train.cu src/model.cc src/reader/mnist/mnist.cc src/reader/cifar10/cifar10.cc src/reader/caltech/caltech.cc src/gnuplot.cc src/plot.cc src/evaluation.cc src/file_read.cc
SIM_CODE        :=model_CODE

# LIBRARIES += opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_videoio
INCLUDE_FLAGS    += `pkg-config --libs --cflags opencv`
LINK_FLAGS       += `pkg-config --libs --cflags opencv`

include $(GENN_PATH)/userproject/include/makefile_common_gnu.mk

