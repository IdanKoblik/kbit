CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2

TARGET := kbit
TEST_TARGET := test_runner

ALL_SRCS := $(wildcard *.cpp)

APP_SRCS := $(filter-out decoder.cpp,$(ALL_SRCS))
APP_OBJS := $(APP_SRCS:.cpp=.o)
DECODER_OBJ := decoder.o

TEST_SRCS := $(wildcard test/*.cpp)
TEST_OBJS := $(TEST_SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(APP_OBJS) $(DECODER_OBJ)
	$(CXX) $^ -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(DECODER_OBJ) $(TEST_OBJS)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o test/*.o $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
