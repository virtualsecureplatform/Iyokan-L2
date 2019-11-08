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
    int priority;
    std::string Type;
    bool executed;
    bool executable;
    int res;
    LweSample *value;
    std::vector<Logic *> output{};

    Logic(int _id) {
        id = _id;
        executable = false;
        executed = false;
    }

    virtual void PrepareExecution() = 0;

    virtual void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) = 0;

    virtual void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) = 0;

    virtual void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) = 0;

    //virtual void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) = 0;

    virtual bool NoticeInputReady() = 0;

    virtual void AddInput(Logic *logic) = 0;

    virtual void AddOutput(Logic *logic) = 0;

    virtual bool Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) = 0;

protected:
    int InputCount;
    int ReadyInputCount;
    std::vector<Logic *> input{};
};

#endif
