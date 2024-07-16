#ifndef GPU_H
#define GPU_H

#include <string>
#include <iostream>

class Gpu {
private:
    std::string brand;
    std::string model;
    int vram;          // en MB
    double clock_speed;

public:
    Gpu(); // Constructor

    // Métodos
    void CollectInfo();
    std::string getBrand() const;
    std::string getModel() const;
    int getVRAM() const;
    std::string getClockSpeed() const;
    void printInfo() const; // Para depuración
};

#endif // GPU_H