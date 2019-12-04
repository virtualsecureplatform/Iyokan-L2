//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLNAND_HPP
#define IYOKAN_L2_LOGICCELLNAND_HPP

#include "Logic.hpp"

class LogicCellNAND : public Logic {
public:
    LogicCellNAND(int id);

    void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk);

    void PrepareExecution();

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue);

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue);

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset);

};

#endif //IYOKAN_L2_LOGICCELLNAND_HPP