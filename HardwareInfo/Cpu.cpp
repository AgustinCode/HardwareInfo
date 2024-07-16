#include "Cpu.h"
#include <iostream>
#include <windows.h>
#include <intrin.h>
#include <vector>

Cpu::Cpu() : brand(""), model(""), cores(0), threads(0), clock_speed(0.0) {}

void Cpu::CollectInfo()
{
    int cpuInfo[4] = { -1 };
    char brandString[0x40];

    // Obtain CPU Brand 
    __cpuid(cpuInfo, 0x80000002);
    memcpy(brandString, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000003);
    memcpy(brandString + 16, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000004);
    memcpy(brandString + 32, cpuInfo, sizeof(cpuInfo));
    brand = brandString;

    // Obtain cores and threads
    DWORD len = 0;
    GetLogicalProcessorInformation(nullptr, &len); // Get the necessary size
    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> info(len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

    if (GetLogicalProcessorInformation(info.data(), &len)) {
        for (const auto& entry : info) {
            if (entry.Relationship == RelationProcessorCore) {
                cores++; // Count the number of cores
                threads += __popcnt64(entry.ProcessorMask); // Count logical threads
            }
        }
    }

    // Obtain clock speed
    LARGE_INTEGER qpf, qpc;
    QueryPerformanceFrequency(&qpf);
    QueryPerformanceCounter(&qpc);
    clock_speed = static_cast<double>(qpf.QuadPart) / 1000000.0;  // MHz
}

std::string Cpu::getBrand() const { return brand; }
std::string Cpu::getModel() const { return model; }
int Cpu::getCores() const { return cores; }
int Cpu::getThreads() const { return threads; }
double Cpu::getClockSpeed() const { return clock_speed; }

void Cpu::printInfo() const {
    std::cout << "CPU: " << brand << std::endl;
    std::cout << "Cores: " << cores << std::endl;
    std::cout << "Threads: " << threads << std::endl;
    std::cout << "Clock speed: " << clock_speed << " MHz" << std::endl;
}
