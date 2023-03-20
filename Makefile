CC = g++
TARGET = Server
OBJECTS = main.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) -o $@ $^

clean :
	rm *.o Server
