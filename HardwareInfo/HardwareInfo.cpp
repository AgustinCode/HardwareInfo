#include <iostream>
#include "Cpu.h"
#include "Gpu.h"
#include "Disk.h"
#include "Memory.h"
#include "Mboard.h"

int show_menu()
{
    int option;
    while (true)
    {
        std::cout << "--System Manager--\n" << std::endl;
        std::cout << "1 - CPU information \n" << std::endl;
        std::cout << "2 - GPU information \n" << std::endl;
        std::cout << "3 - Memory information \n" << std::endl;
        std::cout << "4 - Disk information \n" << std::endl;
        std::cout << "5 - Motherboard information \n" << std::endl;
        std::cout << "6 - Exit\n" << std::endl;
        std::cout << "Enter an option: "; std::cin >> option;

        if (option <= 6 && option > 0) {
            return option;
        }
        else {
            system("cls");
            std::cout << "Choose a valid option! \n" << std::endl;
        }
    }
}

int main()
{
    Cpu myCpu;
    Gpu myGpu;
    Disk myDisk;
    Memory myMem;
    Mboard myMboard;

    while (true) {
        int op = show_menu();

        if (op == 6) {
            break;
        }

        switch (op) {
        case 1:
            myCpu.CollectInfo();
            myCpu.printInfo();
            break;

        case 2:
            myGpu.CollectInfo();
            myGpu.printInfo();
            break;

        case 3:
            myMem.CollectInfo();
            myMem.printInfo();
            break;

        case 4:
            myDisk.CollectInfo();
            myDisk.printInfo();
            break;

        case 5:
            myMboard.CollectInfo();
            myMboard.printInfo();
            break;

        default:
            std::cout << "Invalid Option!" << std::endl;
            break;
        }
        system("pause");
        system("cls");
    }
    return 0;
}
