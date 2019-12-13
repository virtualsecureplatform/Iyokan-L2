#include "LogicPortOut.hpp"

LogicPortOut::LogicPortOut(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "OUTPUT";
}

void LogicPortOut::Prepare() {
    if (input.size() == 0) {
        executable = true;
    }
}

void LogicPortOut::Execute(cufhe::Stream stream, bool reset) {
    executed = true;
}

void LogicPortOut::Execute(bool reset) {
    executed = true;
}

bool LogicPortOut::NoticeInputReady() {
    executable = true;
    return executable;
}

cufhe::Ctxt *LogicPortOut::GetCipher() {
    if (input.size() > 0) {
        cufhe::GetFromGPU(*input.front()->value);
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

bool LogicPortOut::Tick() {
    if (input.size() == 0) {
        executable = true;
    } else {
        executable = false;
    }
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
