# Compiler
CC = g++

# Compiler Flags
CFLAGS = -std=c++11 -Wall

# Executable
EXEC = game

# Main Target
$(EXEC): game.cpp
	$(CC) $(CFLAGS) game.cpp -o $(EXEC) -lncurses

# Clean Target
clean:
	rm -f $(EXEC)