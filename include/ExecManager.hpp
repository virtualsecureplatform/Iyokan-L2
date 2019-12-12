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
#include "NetList.hpp"

class ExecManager {
public:
    tbb::concurrent_queue<Logic *> ExecutedQueue;

    ExecManager(int num, int _step, bool v);

    void SetNetList(NetList *_netList);

    void Prepare();

    void Start();

    void Stats();

    int GetExecutedLogicNum();

    bool terminate = false;

private:
    int executionCount = 0;
    NetList *netList;
    tbb::concurrent_priority_queue<Logic *, compare_f> ReadyQueue;

    int workerNum = 0;
    int step = 0;
    bool verbose = true;
    std::map<std::string, int> ExecCounter;
    std::vector<std::thread> threads;

    void ClearQueue();

    bool DepencyUpdate(int nowCnt, int maxCnt);

    void Reset();

    void PrepareExecution();

    void Tick(bool reset);

    void TerminateWorkers();

    static void Worker(ExecManager *worker) {
        while (!worker->terminate) {
            Logic *logic;
            if (worker->ReadyQueue.try_pop(logic)) {
                logic->Execute();
            } else {
                usleep(100);
            }
        }
    }
};

#endif  //IYOKAN_L2_EXECMANAGER_HPP
