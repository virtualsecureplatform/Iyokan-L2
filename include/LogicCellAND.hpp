#ifndef IYOKAN_L2_LOGICCELLAND_HPP
#define IYOKAN_L2_LOGICCELLAND_HPP

#include "Logic.hpp"

class LogicCellAND : public Logic {
public:

    LogicCellAND(int id);

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

#endif //IYOKAN_L2_LOGICCELLAND_HPP
