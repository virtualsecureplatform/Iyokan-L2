#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <vector>
#include <queue>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "tbb/concurrent_queue.h"

class Logic {
public:
    int id;
    bool executable;
    int res;
    LweSample *value;

    Logic(int _id) {
        id = _id;
        executable = false;
        executed = false;
    }

    virtual void PrepareExecution() = 0;

    void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
        res = 0;
        value = new_gate_bootstrapping_ciphertext(bk->params);
        bootsCONSTANT(value, 0, bk);
    }

    virtual void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) = 0;

    virtual void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) = 0;

    //virtual void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) = 0;

    virtual bool NoticeInputReady() = 0;

    virtual void AddInput(Logic *logic) = 0;

    virtual void AddOutput(Logic *logic) = 0;

    virtual bool Tick(const TFheGateBootstrappingCloudKeySet *key) = 0;

    void DependencyUpdate(tbb::concurrent_queue<Logic *> *ReadyQueue) {
        if (!executed) {
            throw std::runtime_error("this logic is not executed");
        }
        for (Logic *logic : output) {
            if (logic->NoticeInputReady()) {
                ReadyQueue->push(logic);
            }
        }
    }

    void SetExecutable() {
        executable = true;
    }

    bool executed;
    std::vector<Logic *> output{};
protected:
    int InputCount;
    int ReadyInputCount;
    std::vector<Logic *> input{};
};

#endif
