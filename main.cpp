#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <pthread.h>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

NetList netList("../test.json");

void *Execute(void *args) {
    while (netList.execute) {
        netList.Execute();
    }
    return NULL;
}

void *ExecuteAndManage(void *args) {
    for(int i=0;i<5;i++){
        netList.Tick();
        while (netList.DepencyUpdate()) {
            netList.Execute();
        }
    }
    netList.execute = false;
    std::cout << "\n" << std::endl;
    return NULL;
}

int main() {

    netList.Set("reset", 0);
    //netList.Set("io_in_jump", 0);
    //netList.Set("io_enable", 1);
    //netList.Set("io_in_romData", 0x0E018835);
    //netList.Set("io_address", 0);
    netList.PrepareExecution();
    netList.ClearQueue();

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    netList.execute = true;
    pthread_t thread_1, thread_2, thread_3, thread_4, thread_5, thread_6, thread_7, thread_8;
    pthread_create(&thread_1, NULL, ExecuteAndManage, NULL);
    pthread_create(&thread_2, NULL, Execute, NULL);
    pthread_create(&thread_3, NULL, Execute, NULL);
    pthread_create(&thread_4, NULL, Execute, NULL);
    pthread_create(&thread_5, NULL, Execute, NULL);
    pthread_create(&thread_6, NULL, Execute, NULL);
    pthread_create(&thread_7, NULL, Execute, NULL);
    pthread_create(&thread_8, NULL, Execute, NULL);
    pthread_join(thread_1, NULL);
    end = std::chrono::system_clock::now();

    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                      1000.0);
    printf("Execution time %lf[ms]\n", time);
    netList.DebugOutput();
    netList.Stats();
}

