#include <iostream>
#include <queue>
#include <vector>
#include "Logic.hpp"

int main() {
    Logic logic0(0), logic1(1), logic2(2);
    std::vector < Logic * > Logics;
    std::queue < Logic * > ReadyQueue;

    Logics.push_back(&logic0);
    Logics.push_back(&logic1);
    Logics.push_back(&logic2);

    logic0.output.push_back(&logic2);
    logic1.output.push_back(&logic2);
    logic2.input.push_back(&logic0);
    logic2.input.push_back(&logic1);

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
}
