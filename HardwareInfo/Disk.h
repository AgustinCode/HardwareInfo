#ifndef DISK_H
#define DISK_H

#include <string>

class Disk {
private:

    std::string type;  // HDD, SSD, etc.
    std::string model;
    double capacity;   //  GB
    double free_space; //  GB

public:

    Disk(); // Constructor

    // Métodos
    void CollectInfo();
    std::string getType() const;
    std::string getModel() const;
    double getCapacity() const;
    double getFreeSpace() const;
    void printInfo() const; // For depuration
};

#endif // DISK_H
