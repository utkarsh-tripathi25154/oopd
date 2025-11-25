CXX = g++
CXXFLAGS_DEBUG = -g -O0 -std=c++17 -DDEBUG -fexceptions -Wall -Wextra -I./include
CXXFLAGS_RELEASE = -O3 -std=c++17 -DNDEBUG -fexceptions -Wall -Wextra -I./include
CXXFLAGS_RELEASE = -O3 -std=c++17 -DNDEBUG -fexceptions -Wall -Wextra -I./include -Wno-stringop-overflow
CXXFLAGS_RELEASE = -O3 -std=c++17 -DNDEBUG -fexceptions -Wall -Wextra -I./include -Wno-stringop-overflow -Wno-array-bounds

OBJDIR = build
BINDIR = bin

all: debug release

debug: $(BINDIR)/cellsim-debug
release: $(BINDIR)/cellsim-release

$(BINDIR)/cellsim-debug: $(OBJDIR)/debug/main.o $(OBJDIR)/debug/basicIO.o $(OBJDIR)/debug/syscall.o
	@mkdir -p $(BINDIR)
	$(CXX) -o $@ $^

$(BINDIR)/cellsim-release: $(OBJDIR)/release/main.o $(OBJDIR)/release/basicIO.o $(OBJDIR)/release/syscall.o
	@mkdir -p $(BINDIR)
	$(CXX) -o $@ $^

$(OBJDIR)/debug/main.o: src/main.cpp
	@mkdir -p $(OBJDIR)/debug
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(OBJDIR)/debug/basicIO.o: basicIO.cpp
	@mkdir -p $(OBJDIR)/debug
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(OBJDIR)/debug/syscall.o: syscall.S
	@mkdir -p $(OBJDIR)/debug
	$(CXX) -c $< -o $@

$(OBJDIR)/release/main.o: src/main.cpp
	@mkdir -p $(OBJDIR)/release
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(OBJDIR)/release/basicIO.o: basicIO.cpp
	@mkdir -p $(OBJDIR)/release
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(OBJDIR)/release/syscall.o: syscall.S
	@mkdir -p $(OBJDIR)/release
	$(CXX) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all debug release clean
