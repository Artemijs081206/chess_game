# Makefile для проекта Checkers

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
INCLUDES = -Isrc $(shell sdl2-config --cflags)
LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_net

# Находим все исходные файлы .cpp в каталоге src
SOURCES := $(wildcard src/*.cpp)
# Получаем объектные файлы, заменяя расширение .cpp на .o
OBJECTS := $(SOURCES:.cpp=.o)

TARGET = Checkers

.PHONY: all run clean

# Сборка проекта
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Запуск исполняемого файла после сборки
run: all
	./$(TARGET)

# Очистка проекта
clean:
	rm -f $(OBJECTS) $(TARGET)
