//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTIN_HPP
#define IYOKAN_L2_LOGICPORTIN_HPP

#include "Logic.hpp"

class LogicPortIn : public Logic {
public:
    LogicPortIn(int id) : Logic(id) {
        Type = "INPUT";
    }

    void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
        if(!created){
            res = 0;
            value = new_gate_bootstrapping_ciphertext(bk->params);
            bootsCONSTANT(value, 0, bk);
        }
    }
    void PrepareExecution() {
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        executable = true;
    }

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        if (res != bootsSymDecrypt(value, key)) {
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

    void Set(int val, const TFheGateBootstrappingSecretKeySet *key) {
        res = val;
        value = new_gate_bootstrapping_ciphertext(key->params);
        bootsSymEncrypt(value, val, key);
        created = true;
    }

    void AddInput(Logic *logic) {

    }

    void AddOutput(Logic *logic) {
        output.push_back(logic);
    }

    bool Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
private:
    bool created = false;
};

#endif //IYOKAN_L2_LOGICPORTIN_HPP
