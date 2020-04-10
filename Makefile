rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(call rwildcard,$(SRC_DIR),*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
OBJ_DIRS := $(subst /,/,$(sort $(dir $(OBJ_FILES))))


LDFLAGS :=  -lwiringPi -lwiringPiPca9685 -lncurses
CPPFLAGS := -I./src
CXXFLAGS := -g -Wall -c -std=c++2a

all: create_dirs carboy

carboy: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

clean: 
	rm -rf $(OBJ_DIR)

create_dirs:
	mkdir -p $(OBJ_DIRS)


