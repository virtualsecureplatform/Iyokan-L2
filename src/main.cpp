#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <unistd.h>
#include <tfhe/tfhe_io.h>
#include "ExecManager.hpp"

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
    ExecManager manager(threadNum, execCycle, verbose);
    NetList netList("../../vsp-core.json", &manager.ExecutedQueue, verbose);
    manager.SetNetList(&netList);

    /*
    netList.Set("io_address", 3);
    netList.Set("io_in", 3);
    netList.Set("io_writeEnable", 1);
     */
    //LI 0x24
    netList.SetROMPlain(0, 0x0E2A8835);
    netList.SetROMPlain(1, 0x0);
    /*
    netList.SetROM(0, 0x35041135);
    netList.SetROM(1, 0x101C2A00);
    netList.SetROM(2, 0x000E1814);
     */

    /*
    netList.SetROM(0, 0x0A350974);
    netList.SetROM(1, 0x15FBF430);
    netList.SetROM(2, 0xB8980098);
    netList.SetROM(3, 0x0200981D);
    netList.SetROM(4, 0xF69AF729);
    netList.SetROM(5, 0x0000000E);

    netList.SetRAM(0x11, 0x01);
    netList.SetRAM(0x13, 0x01);
    netList.SetRAM(0x14, 0x80);
    netList.SetRAM(0x15, 0x02);
    netList.SetRAM(0x16, 0x80);
    netList.SetRAM(0x17, 0x02);
    netList.SetRAM(0x18, 0x40);
    netList.SetRAM(0x19, 0x04);
    netList.SetRAM(0x1A, 0x40);
    netList.SetRAM(0x1B, 0x04);
    netList.SetRAM(0x1C, 0xA0);
    netList.SetRAM(0x1D, 0x0A);
    netList.SetRAM(0x1E, 0xA0);
    netList.SetRAM(0x1F, 0x0A);
    netList.SetRAM(0x20, 0x10);
    netList.SetRAM(0x21, 0x11);
    netList.SetRAM(0x22, 0x10);
    netList.SetRAM(0x23, 0x11);
    netList.SetRAM(0x24, 0x88);
    netList.SetRAM(0x25, 0x22);
    netList.SetRAM(0x26, 0x88);
    netList.SetRAM(0x27, 0x22);
    netList.SetRAM(0x28, 0x7C);
    netList.SetRAM(0x29, 0x7C);
    */

    /*
    netList.SetROM(0, 0x0335036E);
    netList.SetROM(1, 0x01043500);
    netList.SetROM(2, 0x06320535);
    netList.SetROM(3, 0x37438000);
    netList.SetROM(4, 0x34800A54);
    netList.SetROM(5, 0xCE05541F);
    netList.SetROM(6, 0xFE0006FE);
    netList.SetROM(7, 0xC0FE1EFC);
    netList.SetROM(8, 0x0E000038);
    netList.SetROM(9, 0x0);
     */

    /*
    netList.SetRAM(0, 1);
    netList.SetRAM(1, 2);
    netList.SetRAM(2, 3);
    netList.SetRAM(3, 4);
     */

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
}
