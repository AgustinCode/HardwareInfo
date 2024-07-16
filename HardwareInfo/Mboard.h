#ifndef MBOARD_H
#define MBOARD_H

#include <string>

class Mboard {
private:
    std::string manufacturer;
    std::string model;
    std::string chipset;
    std::string bios_version;

public:
    Mboard(); // Constructor

    void CollectInfo();
    std::string getManufacturer() const;
    std::string getModel() const;
    std::string getChipset() const;
    std::string getBIOSVersion() const;
    void printInfo() const; // For depuration
};

#endif // MBOARD_H
