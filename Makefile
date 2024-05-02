CC = g++

CFLAGS = -Wall -Wextra -pedantic -std=c++11 
LDFLAGS = -lncurses
TARGET = game
SRC = game.cpp game_state.cpp
OBJ = $(SRC:.cpp=.o)

# Main Target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean Target
.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)