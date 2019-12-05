#include "LogicCellORNOT.hpp"

LogicCellORNOT::LogicCellORNOT(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *ck) : Logic(id, pri, queue, ck) {
    Type = "ORNOT";
}

LogicCellORNOT::LogicCellORNOT(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue) : Logic(id, pri, queue) {
    Type = "ORNOT";
}

void LogicCellORNOT::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned ORNOT");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    if (cipher) {
        value = new_gate_bootstrapping_ciphertext(key->params);
        bootsCONSTANT(value, 0, key);
    } else {
        res = 0;
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellORNOT::Execute() {
    if (cipher) {
        bootsORYN(value, input.at(0)->value, input.at(1)->value, key);
    } else {
        res = (input.at(0)->res | (~input.at(1)->res)) & 0x1;
    }
    executed = true;
    executedQueue->push(this);
}

bool LogicCellORNOT::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[ORNOT] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellORNOT::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellORNOT::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellORNOT::Tick(bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
