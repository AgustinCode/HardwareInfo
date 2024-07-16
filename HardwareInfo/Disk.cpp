#include "Disk.h"
#include <windows.h>
#include <vector>
#include <iostream>

Disk::Disk() : type(""), model(""), capacity(0.0), free_space(0.0) {}

void Disk::CollectInfo() {
    // Obtener la información de todos los discos lógicos
    DWORD drives = GetLogicalDrives();
    if (drives == 0) {
        std::cerr << "Error obteniendo la información de los discos" << std::endl;
        return;
    }

    // Iterar sobre todas las unidades posibles (A: - Z:)
    for (char drive = 'A'; drive <= 'Z'; ++drive) {
        if (drives & (1 << (drive - 'A'))) {
            std::string drivePath = std::string(1, drive) + ":\\";
            UINT driveType = GetDriveTypeA(drivePath.c_str());

            // Determinar el tipo de disco
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

            // Obtener la capacidad y espacio libre
            ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
            if (GetDiskFreeSpaceExA(drivePath.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
                capacity = static_cast<double>(totalBytes.QuadPart) / (1024 * 1024 * 1024); // Convertir a GB
                free_space = static_cast<double>(totalFreeBytes.QuadPart) / (1024 * 1024 * 1024); // Convertir a GB
            }

            // Obtener el modelo del disco (esto no es directo y puede necesitar información adicional)
            // Aquí se asigna un valor por defecto o se puede implementar una forma más avanzada para obtener esta información.
            model = "Unknown Model";

            break; // Procesar solo la primera unidad encontrada
        }
    }
}

std::string Disk::getType() const {
    return type;
}

std::string Disk::getModel() const {
    return model;
}

double Disk::getCapacity() const {
    return capacity;
}

double Disk::getFreeSpace() const {
    return free_space;
}

void Disk::printInfo() const {
    std::cout << "Type: " << type << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Capacity: " << capacity << " GB" << std::endl;
    std::cout << "Free Space: " << free_space << " GB" << std::endl;
}
