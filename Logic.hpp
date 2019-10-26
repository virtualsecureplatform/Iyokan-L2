#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <vector>
#include <queue>
#include <iostream>

class Logic {
public:
    int id;
    bool executable;
    bool executed;
    int res;
    std::vector<Logic *> input{};
    std::vector<Logic *> output{};
    int InputCount;
    int ReadyInputCount;

    Logic(int _id) {
        id = _id;
    }

    void PrepareExecution() {
        InputCount = input.size();
        ReadyInputCount = 0;
        std::cout << "Logic" << id << " InputCount:" << InputCount << std::endl;

        if (input.size() == 0) {
            res = id;
            executable = true;
        } else {
            executable = false;
        }
        executed = false;
    }

    void Execute(std::queue<Logic *> *ReadyQueue) {
        if (!executed) {
            std::cout << "Executed:Logic" << id << std::endl;
            for (Logic *logic : output) {
                logic->ReadyInputCount++;
                //std::cout <<"Logic"<< logic->id <<" ReadyInputCount:" << logic->ReadyInputCount << std::endl;
                if (logic->ReadyInputCount == logic->InputCount) {
                    logic->executable = true;
                    //std::cout <<"Executable:Logic"<< logic->id << std::endl;
                    ReadyQueue->push(logic);
                }
            }
            executed = true;
        }
    }

private:
};

#endif
