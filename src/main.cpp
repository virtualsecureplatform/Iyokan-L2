#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <unistd.h>
#include <tfhe/tfhe_io.h>
#include "ExecManager.hpp"
#include "KVSPPacket.hpp"

int main(int argc, char *argv[]) {
    int opt;
    opterr = 0;
    bool perfMode = false;
    bool verbose = true;
    int execCycle = 20;
    int threadNum = 4;
    while ((opt = getopt(argc, argv, "vpc:t:")) != -1) {
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
            default:
                std::cout << "Usage: [-p] [-v] [-c cycle] [-t thread_num]" << std::endl;
                exit(1);
                break;
        }
    }
    std::ifstream ifs{"../../test.enc", std::ios_base::binary};
    auto packet = KVSPReqPacket::readFrom(ifs);
    ExecManager manager(threadNum, execCycle, verbose);
    NetList netList("../../vsp-core.json", &manager.ExecutedQueue, packet.cloudKey.get(), verbose);
    manager.SetNetList(&netList);
    manager.Prepare();
    netList.SetROMCipherAll(packet.rom);


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
        //netList.DebugOutput();
        manager.Stats();
    }
    std::vector<std::shared_ptr<LweSample>> flags = {packet.rom[0]};
    std::vector<std::vector<std::shared_ptr<LweSample>>> regs =
        {
            netList.GetPortCipher("io_testRegx8"),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
            std::vector<std::shared_ptr<LweSample>>(packet.ram.begin() + 0, packet.ram.begin() + 16),
        };
    std::vector<std::shared_ptr<LweSample>> ram = packet.ram;
    std::ofstream ofs{"result.enc", std::ios_base::binary};
    KVSPResPacket{packet.cloudKey, flags, regs, ram}.writeTo(ofs);
}
