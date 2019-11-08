#include "LogicCellORNOT.hpp"

LogicCellORNOT::LogicCellORNOT(int id) : Logic(id) {
    Type = "ORNOT";
}

void LogicCellORNOT::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    res = 0;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, 0, bk);
}

void LogicCellORNOT::PrepareExecution() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned ORNOT");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellORNOT::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsORYN(value, input.at(0)->value, input.at(1)->value, &key->cloud);
    res = (input.at(0)->res | (~input.at(1)->res)) & 0x1;
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: ORNOT");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellORNOT::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsORYN(value, input.at(0)->value, input.at(1)->value, key);
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellORNOT::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    res = (input.at(0)->res | (~input.at(1)->res)) & 0x1;
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellORNOT::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("ReadyInputCount is invalid");
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

bool LogicCellORNOT::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
