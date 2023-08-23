EXEC = benchmarks
CC = gcc
flags = -Wall -Wextra -Werror -g

FR_DIR = ../src/FreeList
P_DIR = ../src/Pool
SS_DIR = ../src/SegregatedStorage
S_DIR = ../src/Stack

# object files needed
OBJ = $(FR_DIR)/free_list.o \
	  $(P_DIR)/pool.o \
	  $(SS_DIR)/segregated.o \
	  $(S_DIR)/stack.o \
	  benchmark.o

# make the executable file
$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(flags)

# make the object files needed

# GAME
$(EXEC).o: $(SRC_DIR)/$(EXEC).c
	$(CC) -c $(SRC_DIR)/$(EXEC).c $(flags)


free_list.o: $(FR_DIR)/free_list.c 
	$(CC) -c free_list.c $(flags)

pool.o: $(P_DIR)/pool.c
	$(CC) -c pool.c $(flags)

segregated.o: $(SS_DIR)/segregated.c
	$(CC) -c segregated.c $(flags)

stack.o: $(S_DIR)/stack.c
	$(CC) -c stack.c $(flags)

benchmark.o: benchmark.c
	$(CC) -c benchmark.c $(flags)

# delete excess object files
clear:
	rm -f $(OBJ) $(EXEC)

# play the game
run: $(EXEC)
	./$(EXEC)

# run valgrind
help: $(EXEC)
	valgrind --leak-check=full -v --show-leak-kinds=all --track-origins=yes ./$(EXEC)
