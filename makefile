OPENNI_INCLUDE_PATH=/usr/include/ni #You may need to adapt me
CXXFLAGS=$(shell pkg-config opencv --cflags) -I$(OPENNI_INCLUDE_PATH) -Wall -Wextra
LDFLAGS=$(shell pkg-config opencv --libs) -lOpenNI
 
SRC_FILES=$(shell find . -iname "*.cpp")
HDR_FILES=$(shell find . -iname "*.h")
OBJ_FILES=$(SRC_FILES:%.cpp=%.o)
 
EXENAME=00-kinect-demo
 
all: $(EXENAME)
 
$(EXENAME): $(OBJ_FILES) $(HDR_FILES)
	$(CXX) -std=c++11 -o $@ $(OBJ_FILES) $(LDFLAGS) 
 
clean:
	$(RM) *.o
	$(RM) $(EXENAME)
 
run:
	./$(EXENAME)
 
.PHONY: all clean run