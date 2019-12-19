#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <unistd.h>
#include "ExecManager.hpp"
#include "cufhe_gpu.cuh"
#include "KVSPPacket.hpp"
#include "Utils.hpp"

const std::string usageMsg =
    "Usage:\n\
PlainMode\n\
    ./iyokanl2 -l <logic_file_name> -p <plain_file_name> -o <result_file_name>\n\
CipherMode\n\
    ./iyokanl2 -l <logic_file_name> -c <cycle_num> -t <thread_num> -i <cipher_file_name> -o <result_file_name>\n";

int main(int argc, char *argv[]) {
    int opt;
    opterr = 0;
    bool verbose = true;
    int execCycle = 6;
    std::string logicFile = "";
    std::string cipherFile = "";
    std::string resultFile = "";
    std::string plainFile = "";
    bool plainMode = false;
    while ((opt = getopt(argc, argv, "vp:c:l:i:o:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'p':
                plainMode = true;
                plainFile = optarg;
                break;
            case 'c':
                execCycle = atoi(optarg);
                break;
            case 'l':
                logicFile = optarg;
                break;
            case 'i':
                cipherFile = optarg;
                break;
            case 'o':
                resultFile = optarg;
                break;
            default:
                std::cout << usageMsg << std::endl;
                exit(1);
                break;
        }
    }

    if (resultFile != "") {
        std::cout << "ResultFile:" << resultFile << std::endl;
    } else {
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if (logicFile != "") {
        std::cout << "LogicFile:" << logicFile << std::endl;
    } else {
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(!plainMode) {
        if (execCycle != 0) {
            std::cout << "ExecCycle:" << execCycle << std::endl;
        } else {
            std::cout << usageMsg << std::endl;
            exit(1);
        }

        if (cipherFile != "") {
            std::cout << "CipherFile:" << cipherFile << std::endl;
        } else {
            std::cout << usageMsg << std::endl;
            exit(1);
        }
    }else{
        if(plainFile != ""){
            std::cout << "PlainFile:" << plainFile << std::endl;
        }else{
            std::cout << usageMsg << std::endl;
            exit(1);
        }
    }

    std::shared_ptr<cufhe::PubKey> cloudKey;

    std::shared_ptr<TFheGateBootstrappingCloudKeySet> tfheCloudKey = nullptr;
    ExecManager manager(1, execCycle, verbose, !plainMode);

    NetList netList(logicFile, verbose, false);

    if (!plainMode) {
        std::ifstream ifs{cipherFile, std::ios_base::binary};
        auto packet = KVSPReqPacket::readFrom(ifs);
        tfheCloudKey = packet.cloudKey;

        cudaSetDevice(0);
        cufhe::SetSeed();
        cloudKey = tfhe2cufhe(*tfheCloudKey.get());
        cufhe::Initialize(*cloudKey.get());

        auto cufheRom = tfhe2cufhe(*packet.cloudKey.get(), packet.rom);
        auto cufheRam = tfhe2cufhe(*packet.cloudKey.get(), packet.ram);
        netList = NetList(logicFile, verbose, true);
        netList.SetROMCipherAll(cufheRom);
        netList.SetRAMCipherAll(cufheRam);
    }else{
        std::ifstream ifs{cipherFile, std::ios_base::binary};
        auto packet = KVSPPlainReqPacket::readFrom(ifs);
        netList.SetROMPlainAll(packet.rom);
        netList.SetRAMPlainAll(packet.ram);
    }


    manager.SetNetList(&netList);
    manager.Prepare();

    if(plainMode) {
        int execCnt = manager.ExecUntilFinish();
        printf("Exec count:%d\n", execCnt);
    }else {
        std::chrono::system_clock::time_point start, end;
        start = std::chrono::system_clock::now();
        manager.Start();
        end = std::chrono::system_clock::now();

        double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                          1000.0);
        printf("Execution time %lf[ms]\n", time);
    }
    netList.DebugOutput();
    manager.Stats();

    if (plainMode) {
        std::vector<uint8_t> flags = {(uint8_t)netList.GetPortPlain("io_finishFlag")};
        std::vector<uint16_t> regs =
            {
                (uint16_t)netList.GetPortPlain("io_regOut_x0"),
                (uint16_t)netList.GetPortPlain("io_regOut_x1"),
                (uint16_t)netList.GetPortPlain("io_regOut_x2"),
                (uint16_t)netList.GetPortPlain("io_regOut_x3"),
                (uint16_t)netList.GetPortPlain("io_regOut_x4"),
                (uint16_t)netList.GetPortPlain("io_regOut_x5"),
                (uint16_t)netList.GetPortPlain("io_regOut_x6"),
                (uint16_t)netList.GetPortPlain("io_regOut_x7"),
                (uint16_t)netList.GetPortPlain("io_regOut_x8"),
                (uint16_t)netList.GetPortPlain("io_regOut_x9"),
                (uint16_t)netList.GetPortPlain("io_regOut_x10"),
                (uint16_t)netList.GetPortPlain("io_regOut_x11"),
                (uint16_t)netList.GetPortPlain("io_regOut_x12"),
                (uint16_t)netList.GetPortPlain("io_regOut_x13"),
                (uint16_t)netList.GetPortPlain("io_regOut_x14"),
                (uint16_t)netList.GetPortPlain("io_regOut_x15")};
        std::vector<uint8_t> ram = netList.GetRAMPlainAll();
        std::ofstream ofs{resultFile, std::ios_base::binary};
        KVSPPlainResPacket{flags, regs, ram}.writeTo(ofs);
    } else {
        std::vector<std::shared_ptr<cufhe::Ctxt>> flags = {netList.GetPortCipher("io_finishFlag")};
        std::vector<std::vector<std::shared_ptr<cufhe::Ctxt>>> regs =
            {
                netList.GetPortCipher("io_regOut_x0"),
                netList.GetPortCipher("io_regOut_x1"),
                netList.GetPortCipher("io_regOut_x2"),
                netList.GetPortCipher("io_regOut_x3"),
                netList.GetPortCipher("io_regOut_x4"),
                netList.GetPortCipher("io_regOut_x5"),
                netList.GetPortCipher("io_regOut_x6"),
                netList.GetPortCipher("io_regOut_x7"),
                netList.GetPortCipher("io_regOut_x8"),
                netList.GetPortCipher("io_regOut_x9"),
                netList.GetPortCipher("io_regOut_x10"),
                netList.GetPortCipher("io_regOut_x11"),
                netList.GetPortCipher("io_regOut_x12"),
                netList.GetPortCipher("io_regOut_x13"),
                netList.GetPortCipher("io_regOut_x14"),
                netList.GetPortCipher("io_regOut_x15")};
        std::vector<std::shared_ptr<cufhe::Ctxt>> ram = netList.GetRAMCipherAll();

        auto tfheFlags = cufhe2tfhe(*tfheCloudKey.get(), flags);
        auto tfheRam = cufhe2tfhe(*tfheCloudKey.get(), ram);
        std::vector<std::vector<std::shared_ptr<LweSample>>> tfheRegs;
        for(auto reg : regs){
            auto tfheReg = cufhe2tfhe(*tfheCloudKey.get(), reg);
            tfheRegs.push_back(tfheReg);
        }
        std::ofstream ofs{resultFile, std::ios_base::binary};
        KVSPResPacket{tfheCloudKey, tfheFlags, tfheRegs, tfheRam}.writeTo(ofs);
    }
}
