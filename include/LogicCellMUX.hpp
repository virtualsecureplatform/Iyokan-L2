//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLMUX_HPP
#define IYOKAN_L2_LOGICCELLMUX_HPP

#include "Logic.hpp"

class LogicCellMUX : public Logic {
public:
    LogicCellMUX(int id);

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

#endif //IYOKAN_L2_LOGICCELLMUX_HPP
