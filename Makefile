CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = test_seq_brtree
SOURCES = test_seq_brtree.cpp sequential_brtree.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean