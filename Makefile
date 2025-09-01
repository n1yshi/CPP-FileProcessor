CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
INCLUDES = -Iinclude
SRCDIR = src
OBJDIR = obj
TESTDIR = tests
TARGET = file_processor

SOURCES = $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp)
TEST_TARGETS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(TESTDIR)/%)

.PHONY: all clean debug release test

all: release

release: CXXFLAGS += -O2 -DNDEBUG
release: $(TARGET)

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(CXXFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_TARGETS)
	@for test in $(TEST_TARGETS); do \
		echo "Running $$test..."; \
		./$$test; \
	done

$(TESTDIR)/%: $(TESTDIR)/%.cpp $(filter-out $(OBJDIR)/main.o, $(OBJECTS))
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(filter-out $(OBJDIR)/main.o, $(OBJECTS)) -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET) $(TEST_TARGETS)
	rm -f *.log

install: release
	cp $(TARGET) /usr/local/bin/

.SECONDARY: $(OBJECTS)