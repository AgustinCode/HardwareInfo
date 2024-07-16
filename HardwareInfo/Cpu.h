#ifndef CPU_H
#define CPU_H

#include <string>

class Cpu {

private:
    std::string brand;
    std::string model;
    int cores;
    int threads;
    double clock_speed;

public:
    Cpu(); // Constructor

    // Métodos
    void CollectInfo();
    std::string getBrand() const;
    std::string getModel() const;
    int getCores() const;
    int getThreads() const;
    double getClockSpeed() const;

    void printInfo() const; // For depuration

};

#endif // CPU_H
