#include "Mboard.h"
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>

#pragma comment(lib, "wbemuuid.lib")

Mboard::Mboard() : manufacturer(""), model(""), chipset(""), bios_version("") {}

void Mboard::CollectInfo() {
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

    // Consultar la información de la placa base
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_BaseBoard"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "Query for baseboard failed. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;
    }

    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn) {
            break;
        }

        VARIANT vtProp;

        // Obtener el fabricante
        hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
        manufacturer = _bstr_t(vtProp.bstrVal);
        VariantClear(&vtProp);

        // Obtener el modelo
        hr = pclsObj->Get(L"Product", 0, &vtProp, 0, 0);
        model = _bstr_t(vtProp.bstrVal);
        VariantClear(&vtProp);

        // Obtener la versión del BIOS
        hr = pclsObj->Get(L"Version", 0, &vtProp, 0, 0);
        bios_version = _bstr_t(vtProp.bstrVal);
        VariantClear(&vtProp);

        // Obtener el chipset (no siempre disponible directamente)
        hr = pclsObj->Get(L"Description", 0, &vtProp, 0, 0);
        chipset = _bstr_t(vtProp.bstrVal);
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
}

std::string Mboard::getManufacturer() const {
    return manufacturer;
}

std::string Mboard::getModel() const {
    return model;
}

std::string Mboard::getChipset() const {
    return chipset;
}

std::string Mboard::getBIOSVersion() const {
    return bios_version;
}

void Mboard::printInfo() const {
    std::cout << "Manufacturer: " << manufacturer << std::endl;
    std::cout << "Model: " << model << std::endl;
    std::cout << "Chipset: " << chipset << std::endl;
    std::cout << "BIOS Version: " << bios_version << std::endl;
}
