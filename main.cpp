#include <iostream>
#include <queue>
#include <vector>
#include <typeinfo>

#include "NetList.hpp"

int main() {
    NetList netList("test.json");
    netList.ConvertJson();
    netList.Set(0, 1);
    netList.Set(1, 0);
    netList.Set(2, 1);
    netList.Set(3, 0);
    netList.Set(4, 1);
    netList.Set(5, 1);
    netList.Set(6, 0);
    netList.Set(7, 0);
    netList.PrepareExecution();
    netList.Execute();
    std::cout << "Result:" << netList.Get(11) << netList.Get(10) << netList.Get(9) << netList.Get(8) << std::endl;
    netList.Tick();
    netList.Set(0, 1);
    netList.Set(1, 0);
    netList.Set(2, 0);
    netList.Set(3, 0);
    netList.Set(4, 0);
    netList.Set(5, 0);
    netList.Set(6, 0);
    netList.Set(7, 1);
    netList.Execute();
    std::cout << "Result:" << netList.Get(11) << netList.Get(10) << netList.Get(9) << netList.Get(8) << std::endl;
}
