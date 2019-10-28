#include <iostream>
#include <queue>
#include <vector>
#include <typeinfo>

#include "NetList.hpp"

int main() {
    NetList netList("../test.json");
    netList.ConvertJson();
    netList.Set(0, 0);
    netList.Set(73, 0);
    netList.Set(74, 1);
    netList.Set(130, 1);
    netList.Set(131, 0);

    //netList.SetExecutable(1390);
    netList.PrepareExecution();
    netList.Tick();
    std::cout << "Result:" << netList.Get(159) << netList.Get(158) << std::endl;
}
