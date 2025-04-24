# Platform identifier
MACHINE = $(shell uname -s)
# Our source, so we can include relative to src/
INCLUDES = -Isrc/ -I/opt/Homebrew/Cellar/eigen/3.4.0_1/include/eigen3/ -I/opt/Homebrew/Cellar/freeglut/3.6.0/include/ -I/opt/Homebrew/Cellar/glew/2.2.0_1/include/
# Directories inside src that we should look in for compilation
DIRS = 
# Object files, by keeping the directory structure from src/ in obj/
OBJS = $(patsubst src/core/%.cpp,obj/core/%.o, $(wildcard src/core/*.cpp) $(foreach d, $(DIRS), $(wildcard src/core/$(d)/*.cpp)))
# Dependency files, used to automatically recompile a source if a header it depends on changes
DEPS = $(OBJS:.o=.d)

# Our compiler
CXX = clang++
# initialize $PLATFORM_LIBS to blank so the platform-specfic section can fill it in later
PLATFORM_LIBS = 
# Warnings
WARNINGS = -Wall -Wextra
# compiler features
FEATURES =
CFLAGS = -w -O3 -stdlib=libc++ -std=gnu++11 -g -march=native $(WARNINGS) $(FEATURES)

# Handle platform-specific setup
ifeq ($(MACHINE), Darwin)
	CXX = clang++
	PLATFORM_FLAGS = -mmacosx-version-min=10.7 -D__MAC__
	PLATFORM_LIBS = -framework GLUT -framework OpenGL
endif

# The libs we build with should be our platform libs plus whatver platform-independent libs we want
LIBS = -lm $(PLATFORM_LIBS) -lGLEW
# Same with the flags
FLAGS = $(PLATFORM_FLAGS) $(CFLAGS)

# The name of our executable
EXEC = main.out
CORE_LIB = main_core.dylib

# Link into an executable
# UNIX FORM:
unix: dirs $(OBJS)
	$(CXX) $(FLAGS) $(INCLUDES) -o $(EXEC) $(LIBS) src/core.cpp $(OBJS)

# Compile for each thing
obj/%.o: src/%.cpp
	$(CXX) $(FLAGS) $(INCLUDES) -c -MMD -MP $< -o $@

# Create empty dirs if they don't exist
dirs:
	@test -d obj || mkdir obj
	@test -d obj/core || mkdir obj/core
	@for DIRECTORY in $(DIRS) ; do \
		test -d obj/core/$$DIRECTORY || mkdir obj/core/$$DIRECTORY; \
	done

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS) $(CORE_LIB) *.out*
	rm -rf obj

# Run
# TODO: Make the dependency for this be the platform the user is on
run: unix
	./$(EXEC)

-include $(DEPS)
