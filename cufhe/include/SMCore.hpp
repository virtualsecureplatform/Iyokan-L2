//
// Created by naoki on 19/12/12.
//

#ifndef IYOKAN_L2_SMCORE_HPP
#define IYOKAN_L2_SMCORE_HPP

#include "cufhe_gpu.cuh"
#include "cufhe.h"
#include "Logic.hpp"

class SMCore {
public:
    Logic* execLogic = nullptr;

    SMCore(int stream_id, std::priority_queue<Logic *, std::vector<Logic *>, compare_f> *queue, bool isCipher){
        cipher = isCipher;
        if(cipher){
            stream = new cufhe::Stream();
            stream->Create();
        }
        readyQueue = queue;
    }

    bool DependencyUpdate(bool reset){
        if(execLogic == nullptr) {
            if(readyQueue->size() != 0) {
                execLogic = readyQueue->top();
                readyQueue->pop();
                if (cipher) {
                    execLogic->Execute(*stream, reset);
                } else {
                    execLogic->Execute(reset);
                }
                return false;
            }else{
                execLogic = nullptr;
                return false;
            }
        }

        if(isFinished()){
            for (Logic *outlogic : execLogic->output) {
                if (outlogic->NoticeInputReady()) {
                    readyQueue->push(outlogic);
                }
            }
            if(readyQueue->size() != 0){
                execLogic = readyQueue->top();
                readyQueue->pop();
                if(cipher){
                    execLogic->Execute(*stream, reset);
                }else{
                    execLogic->Execute(reset);
                }
            }else{
                execLogic = nullptr;
            }
            return true;
        }
        return false;
    }


private:
    cufhe::Stream* stream = nullptr;
    std::priority_queue<Logic *, std::vector<Logic*>, compare_f> *readyQueue;
    bool cipher = false;

    bool isFinished(){
        if(cipher){
            return cufhe::StreamQuery(*stream);
        }else{
            return true;
        }
    }

};
#endif  //IYOKAN_L2_SMCORE_HPP
