#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <unistd.h>
#include <tfhe/tfhe_io.h>
#include "ExecManager.hpp"
#include "KVSPPacket.hpp"

const std::string usageMsg =  "Usage: [-p] [-v] -c <cycle> -t <thread_num> -l <logic_file_name> -i <cipher_file_name> -o <result_file_name> [-s <secretKeyFile>]";
int main(int argc, char *argv[]) {
    int opt;
    opterr = 0;
    bool perfMode = false;
    bool verbose = false;
    int execCycle = 0;
    int threadNum = 0;
    std::string logicFile = "";
    std::string cipherFile = "";
    std::string resultFile = "";
    std::string secretKeyFile = "";
    bool testMode = false;
    while ((opt = getopt(argc, argv, "vpc:t:l:i:o:s:")) != -1) {
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
            case 's':
                secretKeyFile = optarg;
                testMode = true;
                break;
            default:
                std::cout << usageMsg << std::endl;
                exit(1);
                break;
        }
    }
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

    std::ifstream ifs{cipherFile, std::ios_base::binary};
    auto packet = KVSPReqPacket::readFrom(ifs);

    ExecManager manager(threadNum, execCycle, verbose);

    NetList netList(logicFile, &manager.ExecutedQueue, packet.cloudKey.get(), verbose);
    std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey = nullptr;
    if (testMode) {
        std::ifstream ifs_secret(secretKeyFile, std::ios_base::binary);
        secretKey = std::shared_ptr<TFheGateBootstrappingSecretKeySet> {
            new_tfheGateBootstrappingSecretKeySet_fromStream(ifs_secret),
            delete_gate_bootstrapping_secret_keyset};

        netList = NetList(logicFile, &manager.ExecutedQueue, verbose);
        netList.SetROMDecryptCipherAll(packet.rom, secretKey);
        netList.SetRAMDecryptCipherAll(packet.ram, secretKey);
    }else{
        netList.SetROMCipherAll(packet.rom);
        netList.SetRAMCipherAll(packet.ram);
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
        printf("Execution time %lf[ms]\n", time);
        netList.DebugOutput();
        manager.Stats();
    }

    if (testMode) {
        std::vector<std::shared_ptr<LweSample>> flags = {netList.GetPortEncryptPlain("io_finishFlag", 1, secretKey)};
        std::vector<std::vector<std::shared_ptr<LweSample>>> regs =
            {
                netList.GetPortEncryptPlain("io_regOut_x0", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x1", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x2", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x3", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x4", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x5", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x6", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x7", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x8", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x9", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x10", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x11", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x12", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x13", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x14", 16, secretKey),
                netList.GetPortEncryptPlain("io_regOut_x15", 16, secretKey)};
        std::vector<std::shared_ptr<LweSample>> ram = netList.GetRAMEncryptPlainAll(secretKey);
        std::ofstream ofs{resultFile, std::ios_base::binary};
        KVSPResPacket{packet.cloudKey, flags, regs, ram}.writeTo(ofs);
    } else {
        std::vector<std::shared_ptr<LweSample>> flags = {netList.GetPortCipher("io_finishFlag")};
        std::vector<std::vector<std::shared_ptr<LweSample>>> regs =
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
        std::vector<std::shared_ptr<LweSample>> ram = netList.GetRAMCipherAll();
        std::ofstream ofs{resultFile, std::ios_base::binary};
        KVSPResPacket{packet.cloudKey, flags, regs, ram}.writeTo(ofs);
    }
}
