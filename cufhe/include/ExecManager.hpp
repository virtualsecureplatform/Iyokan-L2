//
// Created by naoki on 19/11/05.
//

#ifndef IYOKAN_L2_EXECMANAGER_HPP
#define IYOKAN_L2_EXECMANAGER_HPP

#include <vector>
#include "Logic.hpp"
#include "Utils.hpp"
#include "NetList.hpp"
#include "SMCore.hpp"

class ExecManager {
public:
    std::priority_queue<Logic *, std::vector<Logic*>, compare_f> readyQueue;

    ExecManager(int num, int _step, bool v, bool isCipher);

    void SetNetList(NetList *_netList);

    void Prepare();

    void Start();

    void Stats();

    int GetExecutedLogicNum();

    bool terminate = false;

private:
    int executionCount = 0;
    NetList *netList;
    bool cipher = false;

    int workerNum = 0;
    int step = 0;
    bool verbose = true;
    std::vector<SMCore *> cores;
    std::map<std::string, int> ExecCounter;

    void Reset();

    void Tick();

    void ExecClock(int nowCnt, int maxCnt, bool reset);
};

#endif  //IYOKAN_L2_EXECMANAGER_HPP
