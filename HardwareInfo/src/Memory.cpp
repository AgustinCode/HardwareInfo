#include "../include/Memory.h"
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>

#pragma comment(lib, "wbemuuid.lib")

Memory::Memory() : type(""), total_size(0), speed(0), slots_used(0) {}

void Memory::CollectInfo() {
    HRESULT hres;

    // Inicializar COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        std::cerr << "Failed to initialize COM library. Error code = 0x"
            << std::hex << hres << std::endl;
        return;
    }

    // Establecer la seguridad general
    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );

    if (FAILED(hres)) {
        std::cerr << "Failed to initialize security. Error code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return;
    }

    // Obtener el manejador del servicio WMI
    IWbemLocator* pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres)) {
        std::cerr << "Failed to create IWbemLocator object. Error code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return;
    }

    IWbemServices* pSvc = NULL;

    // Conectar al namespace WMI
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Namespace
        NULL,                    // User
        NULL,                    // Password
        0,                       // Locale
        NULL,                    // Security flags                 
        0,                       // Authority       
        0,                       // Context object
        &pSvc                    // IWbemServices proxy
    );

    if (FAILED(hres)) {
        std::cerr << "Could not connect. Error code = 0x"
            << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return;
    }

    // Establecer los niveles de seguridad para la conexión
    hres = CoSetProxyBlanket(
        pSvc,                        // the proxy to set
        RPC_C_AUTHN_WINNT,           // authentication service
        RPC_C_AUTHZ_NONE,            // authorization service
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // authentication level
        RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation level
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities     
    );

    if (FAILED(hres)) {
        std::cerr << "Could not set proxy blanket. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;
    }

    // Consultar la información de la memoria
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_PhysicalMemory"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "Query for memory failed. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;
    }

    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    int totalCapacity = 0;
    int slotCount = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn) {
            break;
        }

        VARIANT vtProp;

        // Obtener el tipo de memoria
        hr = pclsObj->Get(L"MemoryType", 0, &vtProp, 0, 0);
        int memType = vtProp.intVal;
        type = (memType == 20) ? "DDR" :
            (memType == 21) ? "DDR2" :
            (memType == 22) ? "DDR2 FB-DIMM" :
            (memType == 24) ? "DDR3" :
            (memType == 26) ? "DDR4" : "Unknown";
        VariantClear(&vtProp);

        // Obtener la capacidad total
        hr = pclsObj->Get(L"Capacity", 0, &vtProp, 0, 0);
        totalCapacity += _wtoi64(vtProp.bstrVal) / (1024 * 1024); // Convertir de bytes a MB
        VariantClear(&vtProp);

        // Obtener la velocidad de la memoria
        hr = pclsObj->Get(L"Speed", 0, &vtProp, 0, 0);
        speed = vtProp.intVal;
        VariantClear(&vtProp);

        slotCount++;
        pclsObj->Release();
    }

    total_size = totalCapacity;
    slots_used = slotCount;

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
}

std::string Memory::getType() const {
    return type;
}

int Memory::getTotalSize() const {
    return total_size;
}

int Memory::getSpeed() const {
    return speed;
}

int Memory::getSlotsUsed() const {
    return slots_used;
}

void Memory::printInfo() const {
    std::cout << "Type: " << type << std::endl;
    std::cout << "Total Size: " << total_size << " MB" << std::endl;
    std::cout << "Speed: " << speed << " MHz" << std::endl;
    std::cout << "Slots Used: " << slots_used << std::endl;
}
