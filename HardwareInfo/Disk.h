#ifndef DISK_H
#define DISK_H

#include <string>

class Disk {
private:

    std::string type;  // HDD, SSD, etc.
    std::string model;
    double capacity;   // en GB
    double free_space; // en GB

public:

    Disk(); // Constructor

    // M�todos
    void CollectInfo();
    std::string getType() const;
    std::string getModel() const;
    double getCapacity() const;
    double getFreeSpace() const;
    void printInfo() const; // Para depuraci�n
};

#endif // DISK_H