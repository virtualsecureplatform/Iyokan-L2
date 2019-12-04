#include "LogicPortOut.hpp"

LogicPortOut::LogicPortOut(
        int id,
        int pri,
        tbb::concurrent_queue<Logic *> *queue,
        const TFheGateBootstrappingCloudKeySet *ck
) : Logic(id, pri, queue, ck) {
    Type = "OUTPUT";
}

LogicPortOut::LogicPortOut(
        int id,
        int pri,
        tbb::concurrent_queue<Logic *> *queue
) : Logic(id, pri, queue) {
    Type = "OUTPUT";
}

void LogicPortOut::Prepare() {
    if (input.size() == 0) {
        executable = true;
    }
    if (cipher) {
        value = new_gate_bootstrapping_ciphertext(key->params);
        bootsCONSTANT(value, 0, key);
    } else {
        res = 0;
    }
}

void LogicPortOut::Execute() {
    executed = true;
    executedQueue->push(this);
}

bool LogicPortOut::NoticeInputReady() {
    executable = true;
    return executable;
}

LweSample *LogicPortOut::GetCipher() {
    if (input.size() > 0) {
        return input.front()->value;
    } else {
        return nullptr;
    }
}

int LogicPortOut::GetPlain() {
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

bool LogicPortOut::Tick(bool reset) {
    if (input.size() == 0) {
        executable = true;
    } else {
        executable = false;
    }
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
