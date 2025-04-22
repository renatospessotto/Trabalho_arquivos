CC = gcc
CFLAGS = -Wall -g
OBJ = main.o record.o header.o utils.o binary_operations.o
TARGET = programaTrab

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c record.h header.h utils.h binary_operations.h
	$(CC) $(CFLAGS) -c main.c

record.o: record.c record.h
	$(CC) $(CFLAGS) -c record.c

header.o: header.c header.h
	$(CC) $(CFLAGS) -c header.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

binary_operations.o: binary_operations.c binary_operations.h
	$(CC) $(CFLAGS) -c binary_operations.c

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	chmod +x $(TARGET) # Ensure the executable has the correct permissions
	./$(TARGET)
