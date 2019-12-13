#include "LogicCellROM.hpp"

LogicCellROM::LogicCellROM(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "ROM";
}

void LogicCellROM::Prepare() {
    if (input.size() != 0) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    executable = true;
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellROM::Execute(cufhe::Stream stream, bool reset) {
    executed = true;
}

void LogicCellROM::Execute(bool reset) {
    executed = true;
}

bool LogicCellROM::NoticeInputReady() {
    return false;
}

void LogicCellROM::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellROM::AddOutput(Logic *logic) {
    output.push_back(logic);
}

/*
void LogicCellROM::SetCipher(std::shared_ptr<LweSample> val) {
    bootsCOPY(value, val.get(), key);
}
*/

void LogicCellROM::SetPlain(int val) {
    res = val & 0x1;
}

bool LogicCellROM::Tick() {
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
