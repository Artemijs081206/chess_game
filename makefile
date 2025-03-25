CC = g++
CFLAGS = -Wall -Wextra -O2
LIBS = -lSDL2 -lSDL2_net -lSDL2_image

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = chess_game

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
