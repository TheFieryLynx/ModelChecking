SOURCE=main.cpp
TARGET=main

CFLAGS = -I. -Wall -std=c++17 -O3 -Wpedantic -Wall -Wextra -flto -march=native

all: $(TARGET)

$(TARGET): $(SOURCE)
	g++ $(CFLAGS) -o $@ -I. $^

run: all
	./$(TARGET)

clean:
	rm -rf $(TARGET)
