#include "ExecManager.hpp"

ExecManager::ExecManager(int num, int _step, bool v, bool isCipher) {
    step = _step;
    workerNum = num;
    verbose = v;
    cipher = isCipher;
    for (int i = 0; i < 240; i++) {
        cores.push_back(new SMCore(i, &readyQueue, isCipher));
    }
}

void ExecManager::SetNetList(NetList *_netList) {
    netList = _netList;
}

void ExecManager::Prepare() {
    for (auto logic : netList->Logics) {
        logic.second->Prepare();
    }
    executionCount = 0;
    Reset();
}

void ExecManager::Start() {
    for (int i = 0; i < step; i++) {
        ExecClock(i + 1, step, false);
    }
}

void ExecManager::Stats() {
    std::cout << "---Execution Stats---" << std::endl;
    int logicCount = 0;
    for (auto item : ExecCounter) {
        std::printf("%s : %d\n", item.first.c_str(), item.second);
        if (item.first != "INPUT" && item.first != "OUTPUT" && item.first != "ROM" && item.first != "DFFP") {
            logicCount += item.second;
        }
    }
    std::printf("Logics : %d\n", logicCount);
}

int ExecManager::GetExecutedLogicNum() {
    int logicCount = 0;
    for (auto item : ExecCounter) {
        if (item.first != "INPUT" && item.first != "OUTPUT" && item.first != "ROM" && item.first != "DFFP") {
            logicCount += item.second;
        }
    }
    return logicCount;
}

void ExecManager::Reset() {
    netList->EnableReset();
    ExecClock(0, 0, true);
    netList->DisableReset();
}

void ExecManager::Tick() {
    executionCount = 0;
    for (auto logic : netList->Logics) {
        if (logic.second->Tick()) {
            readyQueue.push(logic.second);
        }
    }
    for (auto core : cores) {
        core->execLogic = nullptr;
    }
}

void ExecManager::ExecClock(int nowCnt, int maxCnt, bool reset) {
    Tick();
    executionCount = 0;
    while (executionCount < netList->Logics.size()) {
        for (auto core : cores) {
            if (core->DependencyUpdate(ExecCounter, reset)) {
                executionCount += 1;
                //std::printf("Core:%d EXEC\n", core->stream_id);
                if (executionCount % 1000 == 0 && verbose) {
                    printf("Executed:%d/%lu %d/%d\n", executionCount, netList->Logics.size(), nowCnt, maxCnt);
                }
                if (executionCount == netList->Logics.size() && verbose) {
                    printf("Executed:%d/%lu %d/%d\n", executionCount, netList->Logics.size(), nowCnt, maxCnt);
                }
            }
        }
    }
}
