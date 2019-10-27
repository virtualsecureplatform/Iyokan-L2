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
    netList.PrepareExecution();
    netList.Execute();

    std::cout << "Result:" << netList.Get(2) << std::endl;
}
