#include "Disk.h"
#include <windows.h>
#include <vector>
#include <iostream>

// Constructor to initialize the Disk object
Disk::Disk() : type(""), model(""), capacity(0.0), free_space(0.0) {}

// Function to collect information about the disk
void Disk::CollectInfo() {
    // Get information about all logical drives
    DWORD drives = GetLogicalDrives();
    if (drives == 0) {
        std::cerr << "Error getting disk information" << std::endl;
        return;
    }

    // Iterate over all possible drives (A: - Z:)
    for (char drive = 'A'; drive <= 'Z'; ++drive) {
        if (drives & (1 << (drive - 'A'))) {
            std::string drivePath = std::string(1, drive) + ":\\";
            UINT driveType = GetDriveTypeA(drivePath.c_str());

            // Determine the type of disk
            switch (driveType) {
            case DRIVE_FIXED:
                type = "HDD/SSD";
                break;
            case DRIVE_REMOVABLE:
                type = "Removable";
                break;
            case DRIVE_CDROM:
                type = "CD-ROM";
                break;
            default:
                type = "Unknown";
                break;
            }

            // Get the capacity and free space
            ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
            if (GetDiskFreeSpaceExA(drivePath.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
                capacity = static_cast<double>(totalBytes.QuadPart) / (1024 * 1024 * 1024); // Convert to GB
                free_space = static_cast<double>(totalFreeBytes.QuadPart) / (1024 * 1024 * 1024); // Convert to GB
            }

            // Get the disk model (this is not straightforward and may require additional information)
            // Here, a default value is assigned or a more advanced method can be implemented to get this information.
            model = "Unknown Model";

            break; // Process only the first found drive
        }
    }
}

// Function to get the disk type
std::string Disk::getType() const {
    return type;
}

// Function to get the disk model
std::string Disk::getModel() const {
    return model;
}

// Function to get the disk capacity
double Disk::getCapacity() const {
    return capacity;
}

// Function to get the free space on the disk
double Disk::getFreeSpace() const {
    return free_space;
}

// Function to print disk information
void Disk::printInfo() const {
    std::cout << "Type: " << type << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Capacity: " << capacity << " GB" << std::endl;
    std::cout << "Free Space: " << free_space << " GB" << std::endl;
}
