#include "LogicCellMUX.hpp"

LogicCellMUX::LogicCellMUX(int id) : Logic(id) {
    Type = "MUX";
}

void LogicCellMUX::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    res = 0;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, 0, bk);
}

void LogicCellMUX::PrepareExecution() {
    if (input.size() != 3) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellMUX::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsMUX(value, input.at(2)->value, input.at(1)->value, input.at(0)->value, &key->cloud);
    if (input.at(2)->res == 0) {
        res = input.at(0)->res;
    } else if (input.at(2)->res == 1) {
        res = input.at(1)->res;
    } else {
        throw std::runtime_error("invalid select signal");
    }
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: MUX");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellMUX::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsMUX(value, input.at(2)->value, input.at(1)->value, input.at(0)->value, key);
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellMUX::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    if (input.at(2)->res == 0) {
        res = input.at(0)->res;
    } else if (input.at(2)->res == 1) {
        res = input.at(1)->res;
    } else {
        throw std::runtime_error("invalid select signal");
    }
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellMUX::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellMUX::AddInput(Logic *logic) {
    if (input.size() > 2) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellMUX::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellMUX::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
