//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLRAM_HPP
#define IYOKAN_L2_LOGICCELLRAM_HPP

#include "Logic.hpp"

class LogicCellRAM : public Logic {
public:
    LogicCellRAM(int id);

    void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk);

    void PrepareExecution();

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue);

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue);

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    void Set(int val, const TFheGateBootstrappingCloudKeySet *bk);

    int Get(TFheGateBootstrappingSecretKeySet *key);

    int Get();

    bool Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset);

private:
    bool created = false;
};

#endif //IYOKAN_L2_LOGICCELLRAM_HPP
