out: main.o chess.o search.o evaluate.o
	g++ main.o chess.o search.o evaluate.o -Wall -o out 

main.o: main.cpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall main.cpp

chess.o: chess.cpp chess.hpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall chess.cpp

search.o: search.cpp search.hpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall search.cpp

evaluate.o: evaluate.cpp evaluate.hpp
	g++ -c -flto -O3 -msse2 -march=native -std=c++17 -static-libgcc -static-libstdc++ -Wall evaluate.cpp

clean:
	del *.o *.exe