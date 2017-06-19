SRC_DIR = ../
BUILD_DIR = ./build
BIN_DIR = ./bin
CATCH_DIR = ./catch
LIB_DIRS = /usr/local/lib

DRIVER_DIR = ../../drivers

INCDIRS = -I/usr/local/include -I$(LIB_DIRS) -I./$(CATCH_DIR) -I$(SRC_DIR) -I$(DRIVER_DIR)

DRIVER_OBJECTS = $(BUILD_DIR)/SerialReader.o
NODE_OBJECTS =	$(BUILD_DIR)/Gps.o
TEST_OBJECTS =  $(BUILD_DIR)/Gps_test.o


LDFLAGS = -L${LIB_DIRS}
LIBS += -lpthread -lrt

all:

gps-host-sil: CC = gcc
gps-host-sil: CXX = g++
gps-host-sil: DEFINES = -DSIL -DUNIT_TEST
gps-host-sil: CFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS)
gps-host-sil: XFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS) 
gps-host-sil: gps gps_test  
	$(CXX)  -o  $(BIN_DIR)/gps-host-sil_test $(DRIVER_OBJECTS) $(NODE_OBJECTS)  $(TEST_OBJECTS)  $(LIBS) -static

gps-target-sil: CC = arm-linux-gnueabihf-gcc
gps-target-sil: CXX = arm-linux-gnueabihf-g++
gps-target-sil: DEFINES = -DSIL -DUNIT_TEST
gps-target-sil: CFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS)
gps-target-sil: XFLAGS = $(DEFINES) -v -ggdb -O0 -Wall -Werror -std=c++11 $(INCDIRS) 
gps-target-sil: gps gps_test  
	$(CXX)  -o  $(BIN_DIR)/gps-target-sil_test $(DRIVER_OBJECTS) $(NODE_OBJECTS)  $(TEST_OBJECTS)  $(LIBS) -static

# *********** TESTS *****************************

gps_test: gps
	$(CXX) -c $(XFLAGS) ./Gps_test.cpp  -o $(BUILD_DIR)/Gps_test.o

# *********** TARGETS ***************************
gps: serialreader
	$(CXX) $(XFLAGS) -S -fverbose-asm -o $(BUILD_DIR)/Gps.s $(SRC_DIR)/Gps.cpp
	$(CXX) -c $(XFLAGS) $(SRC_DIR)/Gps.cpp -o $(BUILD_DIR)/Gps.o
	
# *********** DRIVERS ****************************
serialreader:
	$(CXX) $(XFLAGS) -S -fverbose-asm -o $(BUILD_DIR)/SerialReader.s $(DRIVER_DIR)/SerialReader.cpp
	$(CXX) -c $(XFLAGS) $(DRIVER_DIR)/SerialReader.cpp  -o $(BUILD_DIR)/SerialReader.o

.PHONY : clean

clean:
	rm -f $(BUILD_DIR)/*