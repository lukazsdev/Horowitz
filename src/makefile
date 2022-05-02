horowitz.o: main.o chess.o search.o evaluate.o uci.o
	g++ main.o chess.o search.o evaluate.o uci.o -Wall -o horowitz

main.o: main.cpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall main.cpp

chess.o: chess.cpp chess.h
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall chess.cpp

search.o: search.cpp search.h
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall search.cpp

evaluate.o: evaluate.cpp evaluate.h
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall evaluate.cpp

uci.o: uci.cpp uci.h
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall uci.cpp

clean:
	del *.o *.exe