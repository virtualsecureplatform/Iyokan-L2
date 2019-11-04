//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTOUT_HPP
#define IYOKAN_L2_LOGICPORTOUT_HPP

#include "Logic.hpp"

class LogicPortOut : public Logic {
public:
    LogicPortOut(int id) : Logic(id) {
        Type = "OUTPUT";
    }

    void PrepareExecution() {
        if (input.size() == 0) {
            executable = true;
        }
    }

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        if (input.at(0)->res != bootsSymDecrypt(input.at(0)->value, key)) {
            throw new std::runtime_error("value not matched: OUTPUT");
        }
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
        executed = true;
        ReadyQueue->push(this);
    }

    bool NoticeInputReady() {
        executable = true;
        return executable;
    }

    int Get(TFheGateBootstrappingSecretKeySet *key) {
        if (input.size() > 0) {
            return bootsSymDecrypt(input.front()->value, key);
        } else {
            return 0;
        }
    }

    int Get() {
        if (input.size() > 0) {
            return input.front()->res;
        } else {
            return 0;
        }
    }

    void AddInput(Logic *logic) {
        if (input.size() > 0) {
            throw std::runtime_error("Input is already assigned");
        }
        if (input.size() == 0) {
            input.push_back(logic);
        }
    }

    void AddOutput(Logic *logic) {

    }

    bool Tick(const TFheGateBootstrappingCloudKeySet *key) {
        if (input.size() == 0) {
            executable = true;
        } else {
            executable = false;
        }
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICPORTOUT_HPP
