#ifndef MEMORY_H
#define MEMORY_H

#include <string>

class Memory {
private:
    std::string type;  // DDR3, DDR4, etc.
    int total_size;    // en MB
    int speed;         // en MHz
    int slots_used;

public:
    Memory(); // Constructor

    // Métodos
    void CollectInfo();
    std::string getType() const;
    int getTotalSize() const;
    int getSpeed() const;
    int getSlotsUsed() const;
    void printInfo() const; // Para depuración
};

#endif // MEMORY_H