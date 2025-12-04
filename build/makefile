GPP = g++
FLAGS = -Werror -Wextra -Wall
SRC = ../Prog.cpp ../main.cpp ../B_tree_find.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = ./program

all: $(TARGET)

$(TARGET): $(OBJ)
	$(GPP) $(FLAGS) -o $@ $^
	

%.o: %.cpp
	$(GPP) $(FLAGS) -c $< -o $@
	

clean:
	rm -f $(OBJ) $(TARGET)
