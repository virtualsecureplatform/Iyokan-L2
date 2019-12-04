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
    bool cipher = false;
    const TFheGateBootstrappingCloudKeySet *key;
    std::vector<Logic *> output{};

    Logic(int _id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck) {
        id = _id;
        priority = pri;
        executedQueue = queue;

        cipher = true;
        key = ck;

        executable = false;
        executed = false;
    }

    Logic(int _id, int pri, tbb::concurrent_queue<Logic *> *queue) {
        id = _id;
        priority = pri;
        executedQueue = queue;

        cipher = false;

        executable = false;
        executed = false;
    }


    virtual void Prepare() = 0;

    virtual void Execute() = 0;

    virtual bool NoticeInputReady() = 0;

    virtual void AddInput(Logic *logic) = 0;

    virtual void AddOutput(Logic *logic) = 0;

    virtual bool Tick(bool reset) = 0;

protected:
    tbb::concurrent_queue<Logic *> *executedQueue;
    int InputCount;
    int ReadyInputCount;
    std::vector<Logic *> input{};
};

#endif
