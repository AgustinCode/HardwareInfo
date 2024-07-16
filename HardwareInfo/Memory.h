#ifndef MEMORY_H
#define MEMORY_H

#include <string>

class Memory {
private:
    std::string type;  // DDR3, DDR4, etc.
    int total_size;    //  MB
    int speed;         //  MHz
    int slots_used;

public:
    Memory(); // Constructor

    // Métodos
    void CollectInfo();
    std::string getType() const;
    int getTotalSize() const;
    int getSpeed() const;
    int getSlotsUsed() const;
    void printInfo() const; // For depuration
};

#endif // MEMORY_H
