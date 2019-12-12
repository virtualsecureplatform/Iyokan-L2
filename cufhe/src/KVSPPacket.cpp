#include "KVSPPacket.hpp"

#include <exception>

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

namespace {
uint32_t read32le(std::istream &is) {
    uint32_t ret = 0;
    int ch = 0;
    ch = is.get();
    if (ch == EOF) throw std::runtime_error("Invalid input stream");
    ret |= (ch << 0);
    ch = is.get();
    if (ch == EOF) throw std::runtime_error("Invalid input stream");
    ret |= (ch << 8);
    ch = is.get();
    if (ch == EOF) throw std::runtime_error("Invalid input stream");
    ret |= (ch << 16);
    ch = is.get();
    if (ch == EOF) throw std::runtime_error("Invalid input stream");
    ret |= (ch << 24);

    return ret;
}

uint64_t read64le(std::istream &is) {
    uint64_t l = read32le(is);
    uint64_t h = read32le(is);
    return (h << 32) | l;
}

void write16le(std::ostream &os, uint16_t val) {
    char buf[2] = {static_cast<char>(val), static_cast<char>(val >> 8)};
    os.write(buf, sizeof(buf));
}

void write32le(std::ostream &os, uint32_t val) {
    write16le(os, val & 0xffff);
    write16le(os, val >> 16);
}

void write64le(std::ostream &os, uint64_t val) {
    write32le(os, val & 0xffffffff);
    write32le(os, val >> 32);
}

auto make_lwe_sample(const TFheGateBootstrappingParameterSet *params) {
    return std::shared_ptr<LweSample>{
        new_gate_bootstrapping_ciphertext(params),
        delete_gate_bootstrapping_ciphertext};
}
};  // namespace

KVSPReqPacket KVSPReqPacket::readFrom(std::istream &is) {
    if (!is) throw std::runtime_error("Invalid input stream");

    // Check if the signature ('KVSP') is correct.
    char signature[4];
    is.read(signature, sizeof(signature));
    if (signature[0] != 'K' || signature[1] != 'V' || signature[2] != 'S' || signature[3] != 'P')
        throw std::runtime_error("Invalid signature");

    // Check if the version is correct ('0').
    if (read32le(is) != 0)
        throw std::runtime_error("Invalid version");

    // Get sizes of components.
    uint64_t cloudKeySize = read64le(is);
    uint64_t romSize = read64le(is);
    uint64_t ramSize = read64le(is);
    uint64_t headerSize = is.tellg();

    // Read cloud key.
    std::shared_ptr<TFheGateBootstrappingCloudKeySet> cloudKey{
        new_tfheGateBootstrappingCloudKeySet_fromStream(is),
        delete_gate_bootstrapping_cloud_keyset};

    if (!cloudKey || is.tellg() != headerSize + cloudKeySize)
        throw std::runtime_error("Invalid cloud key");

    // Read encrypted ROM binary.
    std::vector<std::shared_ptr<LweSample>> rom;
    for (uint64_t initialPos = is.tellg(); is.tellg() < initialPos + romSize;) {
        auto sample = make_lwe_sample(cloudKey->params);
        import_gate_bootstrapping_ciphertext_fromStream(is, sample.get(), cloudKey->params);
        rom.push_back(sample);
    }

    if (is.tellg() != headerSize + cloudKeySize + romSize)
        throw std::runtime_error("Invalid encrypted ROM data");

    // Read encrypted RAM binary.
    std::vector<std::shared_ptr<LweSample>> ram;
    for (uint64_t initialPos = is.tellg(); is.tellg() < initialPos + ramSize;) {
        auto sample = make_lwe_sample(cloudKey->params);
        import_gate_bootstrapping_ciphertext_fromStream(is, sample.get(), cloudKey->params);
        ram.push_back(sample);
    }

    if (is.tellg() != headerSize + cloudKeySize + romSize + ramSize)
        throw std::runtime_error("Invalid encrypted RAM data");

    return KVSPReqPacket{cloudKey, rom, ram};
}

void KVSPResPacket::writeTo(std::ostream &os) {
    if (!os) throw std::runtime_error("Invalid output stream");

    // Write the signature ('KVSP').
    char signature[4] = {'K', 'V', 'S', 'P'};
    os.write(signature, sizeof(signature));

    // Write the version ('0').
    uint32_t version = 0;
    write32le(os, 0);

    // Write the number of flags and regs.
    write16le(os, flags.size());
    write16le(os, regs.size());

    // Skip components' sizes; we will fill them later.
    os.seekp(24, std::ios_base::cur);
    uint64_t bodyBegin = os.tellp();

    // Write encrypted flags.
    for (auto &&sample : flags)
        export_gate_bootstrapping_ciphertext_toStream(os, sample.get(), cloudKey->params);
    uint64_t flagsSize = static_cast<uint64_t>(os.tellp()) - bodyBegin;

    // Write encrypted regs.
    for (auto &&reg : regs)
        for (auto &&sample : reg)
            export_gate_bootstrapping_ciphertext_toStream(os, sample.get(), cloudKey->params);
    uint64_t regsSize = static_cast<uint64_t>(os.tellp()) - (bodyBegin + flagsSize);

    // Write encrypted RAM.
    for (auto &&sample : ram)
        export_gate_bootstrapping_ciphertext_toStream(os, sample.get(), cloudKey->params);
    uint64_t ramSize = static_cast<uint64_t>(os.tellp()) - (bodyBegin + flagsSize + regsSize);
    auto bodyEnd = os.tellp();

    // Now we place the size of each component in the header.
    os.seekp(bodyBegin - 24);
    write64le(os, flagsSize);
    write64le(os, regsSize);
    write64le(os, ramSize);
    os.seekp(bodyEnd);
}
