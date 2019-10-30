main: main.cpp Logic.hpp LogicPortIn.hpp LogicPortOut.hpp LogicCellAND.hpp NetList.hpp
	g++ -o main main.cpp -ltbb -lpthread -ltfhe-spqlios-avx
clean:
	rm main
