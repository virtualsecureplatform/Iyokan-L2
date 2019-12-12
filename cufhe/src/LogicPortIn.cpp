#include "LogicPortIn.hpp"

LogicPortIn::LogicPortIn(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *ck) : Logic(id, pri, queue, ck) {
    Type = "INPUT";
}

LogicPortIn::LogicPortIn(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue) : Logic(id, pri, queue) {
    Type = "INPUT";
}

void LogicPortIn::Prepare() {
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
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
}

void LogicPortIn::SetPlain(int val) {
    res = val & 0x1;
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
