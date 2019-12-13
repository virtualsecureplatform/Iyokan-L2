#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <vector>
#include <queue>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <functional>

#include "cufhe_gpu.cuh"
#include "cufhe.h"



class Logic;
class compare_f;

using pri_queue = std::priority_queue<Logic*, std::vector<Logic*>, compare_f>;

class Logic {
public:
    int id;
    int priority;
    std::string Type;

    bool executed;
    bool executable;

    int res;
    cufhe::Ctxt *value;

    bool cipher = false;
    std::vector<Logic *> output{};

    Logic(int _id, int pri, bool isCipher) {
        id = _id;
        priority = pri;

        executable = false;
        executed = false;

        if(isCipher){
            cipher = true;
            value = new cufhe::Ctxt();
            cufhe::Synchronize();
        }else{
            cipher = false;
            res = 0;
        }
    }

    virtual void Prepare() = 0;

    virtual void Execute(cufhe::Stream stream, bool reset) = 0;

    virtual void Execute(bool reset) = 0;

    virtual bool NoticeInputReady() = 0;

    virtual void AddInput(Logic *logic) = 0;

    virtual void AddOutput(Logic *logic) = 0;

    virtual bool Tick() = 0;

protected:
    int InputCount;
    int ReadyInputCount;
    std::vector<Logic *> input{};
};

class compare_f{
public:
    bool operator()(Logic *u, Logic*v){
        return u->priority < v->priority;
    }
};

#endif
