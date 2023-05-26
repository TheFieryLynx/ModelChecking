SOURCE=bdd.cpp formula.cpp test.cpp
TARGET=test

CFLAGS = -I. -Wall

all: $(TARGET)

$(TARGET): $(SOURCE)
	g++ -std=c++17 $(CFLAGS) -o $@ -I. $^

run: all
	./$(TARGET)

clean:
	rm -rf $(TARGET)

