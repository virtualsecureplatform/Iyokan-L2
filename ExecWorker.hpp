//
// Created by naoki on 19/11/05.
//

#ifndef IYOKAN_L2_EXECWORKER_HPP
#define IYOKAN_L2_EXECWORKER_HPP
#include <thread>
#include "Logic.hpp"
#include "Utils.hpp"
#include "tbb/concurrent_priority_queue.h"
#include "tbb/concurrent_queue.h"
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"

class ExecWorker{
public:
    ExecWorker(const TFheGateBootstrappingCloudKeySet *cloudKey){
        key = cloudKey;
    }

    void Start(){
        thread = std::thread(Worker, this);
    }

    void AddReadyQueue(Logic * logic){
        ReadyQueue.push(logic);
    }

    bool GetExecutedQueue(Logic * &logic){
        return ExecutedQueue.try_pop(logic);
    }

    void ClearQueue() {
        ReadyQueue.clear();
        ExecutedQueue.clear();
    }

    void Terminate(){
        terminate = true;
        thread.detach();
    }

    bool terminate = false;
    const TFheGateBootstrappingCloudKeySet *key;
private:


    tbb::concurrent_priority_queue<Logic *, compare_f> ReadyQueue;
    tbb::concurrent_queue<Logic *> ExecutedQueue;
    std::thread thread;

    static void Worker(ExecWorker *worker){
        while(!worker->terminate){
            Logic *logic;
            if (worker->ReadyQueue.try_pop(logic)) {
                //logic->Execute(worker->key, &worker->ExecutedQueue);
                logic->Execute(&worker->ExecutedQueue);
            }else{
                usleep(100);
            }
        }
    }
};
#endif //IYOKAN_L2_EXECWORKER_HPP
