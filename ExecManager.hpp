//
// Created by naoki on 19/11/05.
//

#ifndef IYOKAN_L2_EXECMANAGER_HPP
#define IYOKAN_L2_EXECMANAGER_HPP

#include <vector>
#include <thread>
#include "Logic.hpp"
#include "Utils.hpp"
#include "tbb/concurrent_priority_queue.h"
#include "tbb/concurrent_queue.h"
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"

class ExecManager {
public:
    ExecManager(NetList *_netList, int num, int _step, const TFheGateBootstrappingCloudKeySet *cloudKey, bool v) {
        netList = _netList;
        step = _step;
        key = cloudKey;
        workerNum = num;
        verbose = v;
    }

    void Prepare() {
        PrepareExecution();
        ClearQueue();
        for (int i = 0; i < workerNum; i++) {
            threads.emplace_back(std::thread(Worker, this));
        }
        Reset();
    }

    void Start() {
        for (int i = 0; i < step; i++) {
            Tick(false);
            while (DepencyUpdate(i + 1, step)) {
                usleep(100);
            }
        }
        Tick(false);
        TerminateWorkers();
    }

    void Stats() {
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

    int GetExecutedLogicNum() {
        int logicCount = 0;
        for (auto item : ExecCounter) {
            if (item.first != "INPUT" && item.first != "OUTPUT" && item.first != "ROM" && item.first != "DFFP") {
                logicCount += item.second;
            }
        }
        return logicCount;
    }

    bool terminate = false;
private:

    int executionCount = 0;
    NetList *netList;
    const TFheGateBootstrappingCloudKeySet *key;
    tbb::concurrent_priority_queue<Logic *, compare_f> ReadyQueue;
    tbb::concurrent_queue<Logic *> ExecutedQueue;

    int workerNum = 0;
    int step = 0;
    bool verbose = true;
    std::map<std::string, int> ExecCounter;
    std::vector<std::thread> threads;

    void ClearQueue() {
        ReadyQueue.clear();
        ExecutedQueue.clear();
    }

    bool DepencyUpdate(int nowCnt, int maxCnt) {
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

    void Reset() {
        netList->Set("reset", 1);
        Tick(true);
        while (DepencyUpdate(0, 0));
        netList->Set("reset", 0);
    }

    void PrepareExecution() {
        for (auto logic : netList->Logics) {
            logic.second->PrepareExecution();
            if (logic.second->executable) {
                ReadyQueue.push(logic.second);
            }
        }
        executionCount = 0;
    }

    void Tick(bool reset) {
        executionCount = 0;
        for (auto logic : netList->Logics) {
            if (logic.second->Tick(key, reset)) {
                ReadyQueue.push(logic.second);
            }
        }
    }

    void TerminateWorkers() {
        terminate = true;
        for (int i = 0; i < threads.size(); i++) {
            threads[i].detach();
        }
    }

    static void Worker(ExecManager *worker) {
        while (!worker->terminate) {
            Logic *logic;
            if (worker->ReadyQueue.try_pop(logic)) {
                //logic->Execute(worker->key, &worker->ExecutedQueue);
                logic->Execute(&worker->ExecutedQueue);
            } else {
                usleep(100);
            }
        }
    }
};

#endif //IYOKAN_L2_EXECMANAGER_HPP
