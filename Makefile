CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = test_seq_brtree test_cur_brtree
SOURCES = test_seq_brtree.cpp test_cur_brtree.cpp
OBJECTS = test_seq_brtree.o test_cur_brtree

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f test_seq_brtree.o

.PHONY: all clean