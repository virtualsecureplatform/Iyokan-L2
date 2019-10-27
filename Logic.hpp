#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <vector>
#include <queue>
#include <iostream>
#include <exception>
#include <stdexcept>

class Logic {
public:
    int id;
    bool executable;
    int res;

    Logic(int _id) {
        id = _id;
        executable = false;
        executed = false;
    }

    virtual void PrepareExecution() = 0;

    virtual void Execute(std::queue<Logic *> *ReadyQueue) = 0;

    virtual bool NoticeInputReady() = 0;

    void DependencyUpdate(std::queue<Logic *> *ReadyQueue) {
        if(!executed){
            throw std::runtime_error("this logic is not executed");
        }
        for (Logic *logic : output) {
            if (logic->NoticeInputReady()) {
                ReadyQueue->push(logic);
            }
        }
    }

protected:
    bool executed;
    int InputCount;
    int ReadyInputCount;

    std::vector<Logic *> input{};
    std::vector<Logic *> output{};
};

#endif
