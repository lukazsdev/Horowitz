ifeq ($(OS), Windows_NT)
    uname_S := Windows
else
    uname_S := $(shell uname -s)
endif
ifeq ($(uname_S), Darwin)
    TARGET = -mcpu=apple-a14
else
    TARGET = -march=native
endif

ifeq ($(build), release)
    CFLAGS = -static-libgcc -static-libstdc++ -static
endif

horowitz.o: main.o chess.o search.o see.o evaluate.o uci.o timemanager.o zobrist.o tt.o psqt.o
	g++ main.o chess.o search.o see.o evaluate.o uci.o timemanager.o zobrist.o tt.o psqt.o $(CFLAGS) -o horowitz -lpthread

main.o: main.cpp 
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall main.cpp

chess.o: chess.cpp chess.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall chess.cpp

search.o: search.cpp search.h 
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall search.cpp

see.o: see.cpp see.h 
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall see.cpp

evaluate.o: evaluate.cpp evaluate.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall evaluate.cpp

uci.o: uci.cpp uci.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall uci.cpp

timemanager.o: timemanager.cpp timemanager.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall timemanager.cpp

zobrist.o: zobrist.cpp zobrist.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall zobrist.cpp

tt.o: tt.cpp tt.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall tt.cpp

psqt.o: psqt.cpp psqt.h
	g++ -c -flto -O3 $(TARGET) -std=c++17 -static -Wall psqt.cpp

clean:
	del *.o *.exe