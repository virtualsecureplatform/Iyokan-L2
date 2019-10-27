#include <iostream>
#include <queue>
#include <vector>
#include <typeinfo>
#include "Logic.hpp"
#include "LogicPortIn.hpp"
#include "LogicPortOut.hpp"
#include "LogicCellAND.hpp"

int main() {
    LogicPortIn portIn1(0);
    LogicPortIn portIn2(1);

    LogicCellAND cellAND(2);

    LogicPortOut portOut(3);

    std::vector < Logic * > Logics;
    Logics.push_back(&portIn1);
    Logics.push_back(&portIn2);
    Logics.push_back(&cellAND);
    Logics.push_back(&portOut);

    portIn1.AddOutput(&cellAND);
    portIn2.AddOutput(&cellAND);

    cellAND.AddInput(&portIn1);
    cellAND.AddInput(&portIn2);
    cellAND.AddOutput(&portOut);

    portOut.AddInput(&cellAND);

    portIn1.Set(1);
    portIn2.Set(1);
    std::queue < Logic * > ReadyQueue;
    for (Logic *logic : Logics) {
        logic->PrepareExecution();
        if (logic->executable) {
            ReadyQueue.push(logic);
        }
    }
    while (ReadyQueue.size() > 0) {
        ReadyQueue.front()->Execute(&ReadyQueue);
        ReadyQueue.pop();
        //std::cout << "ReadQueueSize: " << ReadyQueue.size() << std::endl;
    }
    std::cout << "PortOutValue:" << portOut.Get() << std::endl;
}
