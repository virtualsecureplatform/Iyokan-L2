#include "LogicPortIn.hpp"

LogicPortIn::LogicPortIn(int id) : Logic(id) {
    Type = "INPUT";
}

void LogicPortIn::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    if (!created) {
        res = 0;
        value = new_gate_bootstrapping_ciphertext(bk->params);
        bootsCONSTANT(value, 0, bk);
    }
}

void LogicPortIn::PrepareExecution() {
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    executable = true;
}

void LogicPortIn::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: INPUT");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicPortIn::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

void LogicPortIn::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

bool LogicPortIn::NoticeInputReady() {
    return executable;
}

void LogicPortIn::Set(int val, const TFheGateBootstrappingSecretKeySet *key) {
    res = val;
    value = new_gate_bootstrapping_ciphertext(key->params);
    bootsSymEncrypt(value, val, key);
    created = true;
}

void LogicPortIn::AddInput(Logic *logic) {

}

void LogicPortIn::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicPortIn::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
