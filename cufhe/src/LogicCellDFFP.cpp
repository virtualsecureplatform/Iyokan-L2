#include "LogicCellDFFP.hpp"

LogicCellDFFP::LogicCellDFFP(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *ck) : Logic(id, pri, queue, ck) {
    Type = "DFFP";
}

LogicCellDFFP::LogicCellDFFP(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue) : Logic(id, pri, queue) {
    Type = "DFFP";
}

void LogicCellDFFP::Prepare() {
    if (input.size() != 1) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    executable = true;
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellDFFP::Execute() {
    executed = true;
    executedQueue->push(this);
}

bool LogicCellDFFP::NoticeInputReady() {
    return false;
}

void LogicCellDFFP::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellDFFP::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellDFFP::Tick(bool reset) {
    if (cipher) {
        bootsCOPY(value, input.at(0)->value, key);
    } else {
        res = input.at(0)->res;
    }
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
