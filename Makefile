main: main.cpp Logic.hpp LogicPortIn.hpp LogicPortOut.hpp LogicCellAND.hpp
	g++ -o main main.cpp
clean:
	rm main
