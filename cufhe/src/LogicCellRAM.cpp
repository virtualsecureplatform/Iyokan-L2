#include "LogicCellRAM.hpp"

LogicCellRAM::LogicCellRAM(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "RAM";
}

void LogicCellRAM::Prepare() {
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

void LogicCellRAM::Execute(cufhe::Stream stream, bool reset) {
    if(!reset){
        cufhe::Copy(*value, *input.at(0)->value, stream);
    }
    executed = true;
}

void LogicCellRAM::Execute(bool reset) {
    if(!reset){
        res = input.at(0)->res;
    }
    executed = true;
}

bool LogicCellRAM::NoticeInputReady() {
    return false;
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[DFFP] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellRAM::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellRAM::AddOutput(Logic *logic) {
    output.push_back(logic);
}

/*
void LogicCellRAM::SetCipher(std::shared_ptr<LweSample> val) {
    bootsCOPY(value, val.get(), key);
}
*/

void LogicCellRAM::SetPlain(int val) {
    res = val & 0x1;
}

/*
LweSample *LogicCellRAM::GetCipher() {
    return value;
}
 */

int LogicCellRAM::GetPlain() {
    return res;
}

bool LogicCellRAM::Tick() {
    /*
    for (Logic *outlogic : output) {
        if (outlogic->NoticeInputReady()) {
            queue->push(outlogic);
        }
    }
    */
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
