# Executable name
EXE_NAME = argparse_example

# List of source files
SRCS = example.cpp
OBJS = $(SRCS:%.cpp=%.o)


# Compile and link options
CC = clang++
WARNINGS = -Wall -Werror
CPPFLAGS = -std=c++14 -O3 -I. -I/usr/local/include


# Make targets
all: clean argparse

argparse: $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o ${EXE_NAME}

clean:
	rm -f *.a *.o ${EXE_NAME}


# Compile command
%.o: %.cpp
	$(CC) $(WARNINGS) $(CPPFLAGS) -c $< -o $@


# GNU Make: targets that don't build files
.PHONY: all clean
