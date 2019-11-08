#include "LogicPortOut.hpp"

LogicPortOut::LogicPortOut(int id) : Logic(id) {
    Type = "OUTPUT";
}

void LogicPortOut::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    res = 0;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, 0, bk);
}

void LogicPortOut::PrepareExecution() {
    if (input.size() == 0) {
        executable = true;
    }
}

void LogicPortOut::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    if (input.at(0)->res != bootsSymDecrypt(input.at(0)->value, key)) {
        throw new std::runtime_error("value not matched: OUTPUT");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicPortOut::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

void LogicPortOut::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

bool LogicPortOut::NoticeInputReady() {
    executable = true;
    return executable;
}

int LogicPortOut::Get(TFheGateBootstrappingSecretKeySet *key) {
    if (input.size() > 0) {
        return bootsSymDecrypt(input.front()->value, key);
    } else {
        return 0;
    }
}

int LogicPortOut::Get() {
    if (input.size() > 0) {
        return input.front()->res;
    } else {
        return 0;
    }
}

void LogicPortOut::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    if (input.size() == 0) {
        input.push_back(logic);
    }
}

void LogicPortOut::AddOutput(Logic *logic) {

}

bool LogicPortOut::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    if (input.size() == 0) {
        executable = true;
    } else {
        executable = false;
    }
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
