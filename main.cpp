#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <pthread.h>
#include <tfhe/tfhe.h>
#include <unistd.h>
#include <tfhe/tfhe_io.h>
#include "ExecManager.hpp"

int main(int argc, char* argv[]) {

    int opt;
    opterr = 0;
    bool perfMode = false;
    bool verbose = false;
    int execCycle = 60;
    int threadNum = 4;
    while((opt = getopt(argc, argv, "vpc:t:")) != -1){
        switch(opt){
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
                std::cout << "Usage: [-d] [-v] [-c cycle] [-t thread_num]" << std::endl;
                exit(1);
                break;
        }
    }
    NetList netList("../test.json", verbose);
    ExecManager manager(&netList, threadNum, execCycle, &netList.key->cloud, verbose);

    netList.SetROM(0, 0x0E018835);
    netList.SetROM(1, 0x0);
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

    manager.Prepare();
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    manager.Start();
    end = std::chrono::system_clock::now();


    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                      1000.0);
    if(perfMode){
        printf("%d, %d, %lf, %d\n", threadNum, execCycle, time, manager.GetExecutedLogicNum());
    }else{
        printf("Execution time %lf[ms]\n", time);
        netList.DebugOutput();
        manager.Stats();
    }
}

