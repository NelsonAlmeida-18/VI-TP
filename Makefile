OPENCV_INCLUDE_PATH = /usr/include/opencv4
OPENCV_LIB_PATH = /usr/lib

CXX      := g++ 
CXXFLAGS := -std=c++11 -O3 -fopenmp  -I$(OPENCV_INCLUDE_PATH)
LDFLAGS  := -ljpeg -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
SHELL    := /bin/bash
ARGS     := 

TARGET   := VI-RT

TINYEXR_PATH := ./VI-RT/Image/tinyexr-1.0.8/tinyexr.cc
MINIZ_O_PATH := ./VI-RT/Image/tinyexr-1.0.8/deps/miniz/miniz.o

INCLUDE  := -IVI-RT/Camera/ -IVI-RT/Image -IVI-RT/Light -IVI-RT/Primitive -IVI-RT/Primitive/BRDF -IVI-RT/Primitive/Geometry -IVI-RT/Rays -IVI-RT/Renderer -IVI-RT/Scene -IVI-RT/Shader -IVI-RT/utils -IVI-RT/Image/mozjpeg/ -IVI-RT/Image/miniz/ -IVI-RT/Image/tinyexr-1.0.8/  -IVI-RT/Image/tinyexr-1.0.8/examples/common/  -IVI-RT/Scene/tinyobjloader/ -I/usr/include/opencv4/opencv -I/usr/include/opencv4

SRC      :=                      \
   $(wildcard VI-RT/*.cpp) \
   $(wildcard VI-RT/Camera/*.cpp)         \
   $(wildcard VI-RT/Image/*.cpp)         \
   $(wildcard VI-RT/Primitive/BRDF/*.cpp)         \
   $(wildcard VI-RT/Primitive/Geometry/*.cpp)         \
   $(wildcard VI-RT/Renderer/*.cpp)         \
   $(wildcard VI-RT/Scene/*.cpp)         \
   $(wildcard VI-RT/Shader/*.cpp)         \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
# Add tinyexr.o to OBJECTS
#OBJECTS  += $(OBJ_DIR)/tinyexr.o

DEPENDENCIES \
			:= $(OBJECTS:.o=.d)

all:	build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

# Rule to compile tinyexr.cc into tinyexr.o
$(OBJ_DIR)/tinyexr.o: $(TINYEXR_PATH)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
    # Include MINIZ_O_PATH in the linking step
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(MINIZ_O_PATH) $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean 

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

run:
	$(APP_DIR)/$(TARGET) $(ARGS)