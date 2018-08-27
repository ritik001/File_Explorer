CC = g++
CFLAGS = -Wall -std=c++1z
DEPS = File_Exlporer_Main.h
OBJ = normalmode.o fileList.o openFileList.o
all: next
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

next: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

