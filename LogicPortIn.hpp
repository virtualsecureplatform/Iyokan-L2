//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTIN_HPP
#define IYOKAN_L2_LOGICPORTIN_HPP

#include "Logic.hpp"

class LogicPortIn : public Logic {
public:
    LogicPortIn(int id) : Logic(id) {}

    void PrepareExecution() {
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        executable = true;
    }

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        if(res != bootsSymDecrypt(value, key)){
            throw new std::runtime_error("value not matched: INPUT");
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
        return executable;
    }

    void Set(int val, const TFheGateBootstrappingSecretKeySet* key) {
        res = val;
        bootsSymEncrypt(value, val, key);
    }

    void AddInput(Logic *logic) {

    }

    void AddOutput(Logic *logic) {
        output.push_back(logic);
    }

    bool Tick(const TFheGateBootstrappingCloudKeySet *key) {
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICPORTIN_HPP
