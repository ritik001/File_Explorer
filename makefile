CC = g++
CFLAGS = -Wall -std=c++1z
DEPS = File_Exlporer_Main.h
OBJ = File_Explorer_Main.o Print_Dir.o NormalMode.o Cursor_Movements.o
all: Cursor_Movements
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

Cursor_Movements: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

