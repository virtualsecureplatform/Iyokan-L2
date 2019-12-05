#ifndef IYOKAN_L2_KVSP_PACKET_HPP
#define IYOKAN_L2_KVSP_PACKET_HPP

#include <vector>
#include <string>
#include <iostream>
#include <memory>

class TFheGateBootstrappingCloudKeySet;
class LweSample;

struct KVSPReqPacket {
    std::shared_ptr<TFheGateBootstrappingCloudKeySet> cloudKey;
    std::vector<std::shared_ptr<LweSample>> rom, ram;

    static KVSPReqPacket readFrom(std::istream &is);
};

struct KVSPResPacket {
    std::shared_ptr<TFheGateBootstrappingCloudKeySet> cloudKey;
    std::vector<std::shared_ptr<LweSample>> flags;
    std::vector<std::vector<std::shared_ptr<LweSample>>> regs;
    std::vector<std::shared_ptr<LweSample>> ram;

    void writeTo(std::ostream &os);
};

#endif
