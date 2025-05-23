PLATFORM = $(shell uname -s)
OBJS = $(patsubst src/%.cpp,obj/%.o, $(filter-out src/core.cpp,$(wildcard src/*.cpp)))

CXX = clang++
GLOBAL_LIBS =
GLOBAL_FLAGS = -w -O3 -stdlib=libc++ -std=gnu++11 -g -march=native -Wall -Wextra
GLOBAL_INCLUDES = -Isrc/
PLATFORM_FLAGS = 
PLATFORM_LIBS = 
PLATFORM_INCLUDES =

ifeq ($(PLATFORM), Darwin)
	CXX = clang++
	PLATFORM_FLAGS = -mmacosx-version-min=10.7 -D__MAC__
	PLATFORM_LIBS = -framework GLUT -framework OpenGL
	PLATFORM_INCLUDES = -I/opt/Homebrew/Cellar/eigen/3.4.0_1/include/eigen3/ -I/opt/Homebrew/Cellar/freeglut/3.6.0/include/
endif

LIBS = $(PLATFORM_LIBS) $(GLOBAL_LIBS)
FLAGS = $(PLATFORM_FLAGS) $(GLOBAL_FLAGS)
INCLUDES = $(PLATFORM_INCLUDES) $(GLOBAL_INCLUDES)

EXEC = main.out
CORE_LIB = main_core.dylib

$(EXEC): $(OBJS)
	$(CXX) $(FLAGS) $(INCLUDES) $(LIBS) src/core.cpp $(OBJS) -o $(EXEC)

obj/%.o: src/%.cpp obj
	$(CXX) $(FLAGS) $(INCLUDES) -c -MMD -MP $< -o $@

obj:
	mkdir -p obj

clean:
	rm -rf $(EXEC) $(CORE_LIB) $(EXEC).dSYM obj

# TODO: Make the dependency for this be the platform the user is on
run: $(EXEC)
	./$(EXEC)
