CXX = g++

BLDDIR = build

# Library Path Definitions
INCDIR = include
OBJDIR = obj
SRCDIR = src
SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))
TARGET = $(BLDDIR)/lightstreamer.a

# Library Compiler Definitinos
CXXFLAGS = -std=c++17 -Wall -Wextra -I$(INCDIR) -g
LIBS = -lcrypto -lssl -lpthread

# Example Path Definitions
EXAMPLE_SRCDIR = examples
EXAMPLE_BLDDIR = build/examples
EXAMPLE_SRCFILES = $(wildcard $(EXAMPLE_SRCDIR)/*.cpp)
EXAMPLE_BINFILES = $(patsubst $(EXAMPLE_SRCDIR)/%.cpp,$(EXAMPLE_BLDDIR)/%,$(EXAMPLE_SRCFILES))

# Test Path Definitions
TEST_INCDIR = test/include
TEST_OBJDIR = test/obj
TEST_SRCDIR = test/src
TEST_SRCFILES = $(wildcard $(TEST_SRCDIR)/*.cpp)
TEST_OBJFILES = $(patsubst $(TEST_SRCDIR)/%.cpp,$(TEST_OBJDIR)/%.o,$(TEST_SRCFILES))
TEST_TARGET = $(BLDDIR)/test
TEST_TARGETSRC = test/test.cpp

# Test Compiler Definitions
TEST_CXXFLAGS = $(CXXFLAGS) -I$(TEST_INCDIR)

# Library Build Instructions
all: $(TARGET) test examples
cleanall: clean all

$(TARGET): $(TARGETSRC) $(OBJFILES)
	$(CXX) $(CXXFLAGS) -shared -fPIC $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@ 

.PHONY: clean

clean: testclean
	rm -f $(OBJDIR)/*.o $(TARGET)

# Example Build Instructions
examples: $(EXAMPLE_BINFILES)

$(EXAMPLE_BLDDIR)/%: $(EXAMPLE_SRCDIR)/%.cpp $(TARGET)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

# Test Build Instructions
test: $(TEST_TARGET)
testcleantest: testclean test

$(TEST_TARGET): $(TEST_TARGETSRC) $(TEST_OBJFILES) $(TARGET)
	$(CXX) $(TEST_CXXFLAGS) $^ $(LIBS) -o $@

$(TEST_OBJDIR)/%.o: $(TEST_SRCDIR)/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< $(LIBS) -o $@ 

testclean:
	rm -f $(TEST_OBJDIR)/*.o $(TEST_TARGET)
