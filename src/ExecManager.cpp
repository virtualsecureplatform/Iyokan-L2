#include "ExecManager.hpp"

ExecManager::ExecManager(int num, int _step, bool v) {
    step = _step;
    workerNum = num;
    verbose = v;
}

void ExecManager::SetNetList(NetList *_netList) {
    netList = _netList;
}

void ExecManager::Prepare() {
    PrepareExecution();
    ClearQueue();
    for (int i = 0; i < workerNum; i++) {
        threads.emplace_back(std::thread(Worker, this));
    }
    Reset();
}

void ExecManager::Start() {
    for (int i = 0; i < step; i++) {
        Tick(false);
        while (DepencyUpdate(i + 1, step)) {
            usleep(100);
        }
    }
    Tick(false);
    TerminateWorkers();
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

void ExecManager::ClearQueue() {
    ReadyQueue.clear();
    ExecutedQueue.clear();
}

bool ExecManager::DepencyUpdate(int nowCnt, int maxCnt) {
    Logic *logic;
    for (int i = 0; i < workerNum; i++) {
        while (ExecutedQueue.try_pop(logic)) {
            executionCount++;
            if (!logic->executed) {
                throw std::runtime_error("this logic is not executed");
            }
            if (executionCount % 1000 == 0 && verbose) {
                printf("Executed:%d/%lu %d/%d\n", executionCount, netList->Logics.size(), nowCnt, maxCnt);
            }
            ExecCounter[logic->Type]++;
            if (executionCount == netList->Logics.size()) {
                if (verbose) {
                    printf("Executed:%d/%lu %d/%d\n", executionCount, netList->Logics.size(), nowCnt, maxCnt);
                }
                return false;
            }
            for (Logic *outlogic : logic->output) {
                if (outlogic->NoticeInputReady()) {
                    ReadyQueue.push(outlogic);
                }
            }
        }
    }
    return true;
}

void ExecManager::Reset() {
    netList->SetPortPlain("reset", 1);
    Tick(true);
    while (DepencyUpdate(0, 0))
        ;
    netList->SetPortPlain("reset", 0);
}

void ExecManager::PrepareExecution() {
    for (auto logic : netList->Logics) {
        logic.second->Prepare();
        if (logic.second->executable) {
            ReadyQueue.push(logic.second);
        }
    }
    executionCount = 0;
}

void ExecManager::Tick(bool reset) {
    executionCount = 0;
    for (auto logic : netList->Logics) {
        if (logic.second->Tick(reset)) {
            ReadyQueue.push(logic.second);
        }
    }
}

void ExecManager::TerminateWorkers() {
    terminate = true;
    for (int i = 0; i < threads.size(); i++) {
        threads[i].detach();
    }
}
