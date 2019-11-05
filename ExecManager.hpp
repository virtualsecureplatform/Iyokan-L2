//
// Created by naoki on 19/11/05.
//

#ifndef IYOKAN_L2_EXECMANAGER_HPP
#define IYOKAN_L2_EXECMANAGER_HPP
#include <vector>
#include "Logic.hpp"
#include "Utils.hpp"
#include "ExecWorker.hpp"
#include "tbb/concurrent_priority_queue.h"
#include "tbb/concurrent_queue.h"
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"
class ExecManager{
public:
    ExecManager(NetList *_netList, int num, int _step, const TFheGateBootstrappingCloudKeySet *cloudKey, bool v){
        netList = _netList;
        step = _step;
        key = cloudKey;
        workerNum = num;
        verbose = v;
        for(int i=0;i<num;i++){
            workers.emplace_back(ExecWorker(key));
        }
    }

    void Prepare(){
        PrepareExecution();
        ClearQueues();
        for(int i=0;i<workers.size();i++){
            workers[i].Start();
        }
        Reset();
    }

    void Start(){
        for(int i=0;i<step;i++){
            Tick();
            while(DepencyUpdate(i+1,step)){
                usleep(100);
            }
        }
        TerminateWorker();
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
private:

    int executionCount = 0;
    NetList *netList;
    const TFheGateBootstrappingCloudKeySet *key;
    std::vector<ExecWorker> workers;

    int readyQueueRoundRobinCouneter = 0;
    int workerNum = 0;
    int step = 0;
    bool verbose = true;
    std::map<std::string, int> ExecCounter;

    void AddReadyQueue(Logic *logic){
        workers[readyQueueRoundRobinCouneter].AddReadyQueue(logic);
        readyQueueRoundRobinCouneter = (readyQueueRoundRobinCouneter+1)%workerNum;
    }

    bool DepencyUpdate(int nowCnt, int maxCnt) {
        Logic *logic;
        for(int i=0;i<workerNum;i++) {
            while (workers[i].GetExecutedQueue(logic)) {
                executionCount++;
                if (!logic->executed) {
                    throw std::runtime_error("this logic is not executed");
                }
                if (executionCount % 1000 == 0 && verbose) {
                    printf("Executed:%d/%lu %d/%d\n", executionCount, netList->Logics.size(), nowCnt, maxCnt);
                }
                ExecCounter[logic->Type]++;
                if (executionCount == netList->Logics.size()) {
                    if(verbose){
                        printf("Executed:%d/%lu %d/%d\n", executionCount, netList->Logics.size(), nowCnt, maxCnt);
                    }
                    return false;
                }
                for (Logic *outlogic : logic->output) {
                    if (outlogic->NoticeInputReady()) {
                        AddReadyQueue(outlogic);
                    }
                }
            }
        }
        return true;
    }

    void Reset() {
        netList->Set("reset", 1);
        Tick();
        while (DepencyUpdate(0, 0));
        netList->Set("reset", 0);
    }

    void PrepareExecution() {
        for (auto logic : netList->Logics) {
            logic.second->PrepareExecution();
            if (logic.second->executable) {
                AddReadyQueue(logic.second);
            }
        }
        executionCount = 0;
    }

    void ClearQueues(){
        for(int i=0;i<workers.size();i++){
            workers[i].ClearQueue();
        }
    }

    void Tick() {
        executionCount = 0;
        for (auto logic : netList->Logics) {
            if (logic.second->Tick(key)) {
                AddReadyQueue(logic.second);
            }
        }
    }

    void TerminateWorker(){
        for(int i=0;i<workers.size();i++){
            workers[i].Terminate();
        }
    }
};
#endif //IYOKAN_L2_EXECMANAGER_HPP
