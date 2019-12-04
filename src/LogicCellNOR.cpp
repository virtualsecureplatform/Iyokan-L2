#include "LogicCellNOR.hpp"

LogicCellNOR::LogicCellNOR(
        int id,
        int pri,
        tbb::concurrent_queue<Logic *> *queue,
        const TFheGateBootstrappingCloudKeySet *ck
) :Logic(id, pri, queue, ck){
    Type = "NOR";
}

void LogicCellNOR::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    if(cipher){
        value = new_gate_bootstrapping_ciphertext(key->params);
        bootsCONSTANT(value, 0, key);
    }else{
        res = 0;
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellNOR::Execute() {
    if(cipher) {
        bootsNOR(value, input.at(0)->value, input.at(1)->value, key);
    }else{
        res = (~(input.at(0)->res | input.at(1)->res)) & 0x1;
    }
    executed = true;
    executedQueue->push(this);
}

bool LogicCellNOR::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[NOR] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellNOR::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellNOR::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellNOR::Tick(bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
