GPP = g++
FLAGS = -Werror -Wextra -Wall
SRC = Prog.cpp readFile.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = build/program

all: $(TARGET)

$(TARGET): $(OBJ)
	$(GPP) $(FLAGS) -o $@ $^

%.o: %.cpp
	$(GPP) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
