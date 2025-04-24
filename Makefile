PLATFORM = $(shell uname -s)
INCLUDES = -Isrc/ -I/opt/Homebrew/Cellar/eigen/3.4.0_1/include/eigen3/ -I/opt/Homebrew/Cellar/freeglut/3.6.0/include/ -I/opt/Homebrew/Cellar/glew/2.2.0_1/include/
OBJS = $(patsubst src/core/%.cpp,obj/core/%.o, $(wildcard src/core/*.cpp))
# Dependency files, used to automatically recompile a source if a header it depends on changes
DEPS = $(OBJS:.o=.d)

CXX = clang++
GLOBAL_LIBS = -lGLEW
GLOBAL_FLAGS = -w -O3 -stdlib=libc++ -std=gnu++11 -g -march=native -Wall -Wextra
PLATFORM_FLAGS = 
PLATFORM_LIBS = 

ifeq ($(PLATFORM), Darwin)
	CXX = clang++
	PLATFORM_FLAGS = -mmacosx-version-min=10.7 -D__MAC__
	PLATFORM_LIBS = -framework GLUT -framework OpenGL
endif

LIBS = $(PLATFORM_LIBS) $(GLOBAL_LIBS)
FLAGS = $(PLATFORM_FLAGS) $(GLOBAL_FLAGS)

EXEC = main.out
CORE_LIB = main_core.dylib

unix: obj/ obj/core/ $(OBJS)
	$(CXX) $(FLAGS) $(INCLUDES) -o $(EXEC) $(LIBS) src/core.cpp $(OBJS)

obj/%.o: src/%.cpp
	$(CXX) $(FLAGS) $(INCLUDES) -c -MMD -MP $< -o $@

obj/:
	mkdir -p obj
obj/core/:
	mkdir -p obj/core

clean:
	rm -rf $(EXEC) $(CORE_LIB) $(EXEC).dSYM obj

# TODO: Make the dependency for this be the platform the user is on
run: unix
	./$(EXEC)
