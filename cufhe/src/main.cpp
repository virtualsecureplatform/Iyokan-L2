#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <unistd.h>
#include "ExecManager.hpp"
#include "cufhe_gpu.cuh"
#include "KVSPPacket.hpp"
#include "Utils.hpp"

const std::string usageMsg = "Usage: [-p] [-v] -c <cycle> -t <thread_num> -l <logic_file_name> -i <cipher_file_name> -o <result_file_name> [-s <secretKeyFile>]";
int main(int argc, char *argv[]) {
    int opt;
    opterr = 0;
    bool perfMode = false;
    bool verbose = true;
    int execCycle = 6;
    int threadNum = 0;
    std::string logicFile = "../../vsp-core.json";
    std::string cipherFile = "../../lb_test.enc";
    std::string resultFile = "../../result.enc";
    bool plainMode = false;
    while ((opt = getopt(argc, argv, "vpc:t:l:i:o:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'p':
                perfMode = true;
                break;
            case 'c':
                execCycle = atoi(optarg);
                break;
            case 't':
                threadNum = atoi(optarg);
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
    /*
    if(execCycle != 0){
        std::cout << "ExecCycle:" << execCycle << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(threadNum != 0){
        std::cout << "ThreadNum:" << threadNum << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(logicFile != ""){
        std::cout << "LogicFile:" << logicFile << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(cipherFile != ""){
        std::cout << "CipherFile:" << cipherFile << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(resultFile != ""){
        std::cout << "ResultFile:" << resultFile << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(secretKeyFile != ""){
        std::cout << "SecretKeyFile:" << secretKeyFile << std::endl;
    }

    if(testMode){
        std::cout << "Running on TestMode" << std::endl;
    }
     */
    std::ifstream ifs{cipherFile, std::ios_base::binary};
    auto packet = KVSPReqPacket::readFrom(ifs);
    std::shared_ptr<cufhe::PubKey> cloudKey;

    if (!plainMode) {
        cudaSetDevice(0);
        cufhe::SetSeed();
        cloudKey = tfhe2cufhe(*packet.cloudKey.get());
        cufhe::Initialize(*cloudKey.get());
    }

    ExecManager manager(threadNum, execCycle, verbose, !plainMode);

    NetList netList(logicFile, verbose, !plainMode);

    if (!plainMode) {
        auto cufheRom = tfhe2cufhe(*packet.cloudKey.get(), packet.rom);
        auto cufheRam = tfhe2cufhe(*packet.cloudKey.get(), packet.ram);
        netList.SetROMCipherAll(cufheRom);
        netList.SetRAMCipherAll(cufheRam);
        /*
        netList.SetROMEncryptPlain(0, 0x15040035, secretKey);
        netList.SetROMEncryptPlain(1, 0x000E0208, secretKey);
        netList.SetRAMEncryptPlain(6, 0x2A, secretKey);
        netList.SetRAMEncryptPlain(7, 0x2B, secretKey);
         */
    } else {
        netList.SetROMPlain(0, 0x15040035);
        netList.SetROMPlain(1, 0x000E0208);
        netList.SetRAMPlain(6, 0x2A);
        netList.SetRAMPlain(7, 0x2B);
    }

    manager.SetNetList(&netList);
    manager.Prepare();

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    manager.Start();
    end = std::chrono::system_clock::now();

    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                      1000.0);
    if (perfMode) {
        printf("%d, %d, %lf, %d\n", threadNum, execCycle, time, manager.GetExecutedLogicNum());
    } else {
        /*
        if (!testMode) {
            cufhe::Synchronize();
            uint32_t res = netList.GetROMDecryptCipher(0, secretKey);
            std::printf("ROM[0x01]: 0x%X\n", res);
            int x8 = netList.GetPortDecryptCipher("io_regOut_x8", secretKey);
            std::printf("x8:%d\n", x8);
        }
         */
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

        auto tfheFlags = cufhe2tfhe(*packet.cloudKey.get(), flags);
        auto tfheRam = cufhe2tfhe(*packet.cloudKey.get(), ram);
        std::vector<std::vector<std::shared_ptr<LweSample>>> tfheRegs;
        for(auto reg : regs){
            auto tfheReg = cufhe2tfhe(*packet.cloudKey.get(), reg);
            tfheRegs.push_back(tfheReg);
        }
        std::ofstream ofs{resultFile, std::ios_base::binary};
        KVSPResPacket{packet.cloudKey, tfheFlags, tfheRegs, tfheRam}.writeTo(ofs);
        printf("Execution time %lf[ms]\n", time);
        netList.DebugOutput();
        manager.Stats();
    }
}
