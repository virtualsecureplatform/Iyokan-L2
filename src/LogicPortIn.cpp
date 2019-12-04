#include "LogicPortIn.hpp"

LogicPortIn::LogicPortIn(
        int id,
        int pri,
        tbb::concurrent_queue<Logic *> *queue,
        const TFheGateBootstrappingCloudKeySet *ck
) : Logic(id, pri, queue, ck) {
    Type = "INPUT";
}

void LogicPortIn::Prepare() {
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    if (!created) {
        if (cipher) {
            value = new_gate_bootstrapping_ciphertext(key->params);
            bootsCONSTANT(value, 0, key);
        } else {
            res = 0;
        }
    }
    executable = true;
}

void LogicPortIn::Execute() {
    executed = true;
    executedQueue->push(this);
}

bool LogicPortIn::NoticeInputReady() {
    return executable;
}

void LogicPortIn::SetCipher(LweSample *val) {
    bootsCOPY(value, val, key);
    created = true;
}

void LogicPortIn::SetPlain(int val) {
    res = val & 0x1;
    created = true;
}

void LogicPortIn::AddInput(Logic *logic) {

}

void LogicPortIn::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicPortIn::Tick(bool reset) {
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
