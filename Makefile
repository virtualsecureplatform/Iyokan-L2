main: main.cpp Logic.hpp LogicPortIn.hpp LogicPortOut.hpp LogicCellAND.hpp NetList.hpp
	g++ -o main main.cpp
clean:
	rm main
