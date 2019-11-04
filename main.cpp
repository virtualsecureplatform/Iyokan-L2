#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <pthread.h>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

NetList netList("../test-pri.json");

void *Execute(void *args) {
    while (netList.execute) {
        netList.Execute();
    }
    return NULL;
}

void Reset() {
    netList.Set("reset", 1);
    netList.Tick();
    while (netList.DepencyUpdate(0, 0)) {
        netList.Execute();
    }
    netList.DebugOutput();
    netList.Set("reset", 0);
}

void *ExecuteAndManage(void *args) {
    Reset();
    for (int i = 0; i < 60; i++) {
        netList.Tick();
        while (netList.DepencyUpdate(i, 59)) {
            netList.Execute();
        }
    }
    netList.execute = false;
    std::cout << "\n" << std::endl;
    return NULL;
}

int main() {

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
    netList.PrepareExecution();
    netList.ClearQueue();

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    netList.execute = true;
    pthread_t threads[64];
    pthread_create(&threads[0], NULL, ExecuteAndManage, NULL);
    for (int i = 0; i < 63; i++) {
        pthread_create(&threads[i + 1], NULL, Execute, NULL);
    }
    pthread_join(threads[0], NULL);
    end = std::chrono::system_clock::now();

    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                      1000.0);
    printf("Execution time %lf[ms]\n", time);
    netList.DebugOutput();
    netList.Stats();
}

