#include "../include/Gpu.h"
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "wbemuuid.lib")

Gpu::Gpu() : brand(""), model(""), vram(0), clock_speed(0.0) {}

void Gpu::CollectInfo() {
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

    // Consultar la información de la GPU
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_VideoController"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cerr << "Query for GPU failed. Error code = 0x"
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

        // Obtener la marca de la GPU
        hr = pclsObj->Get(L"AdapterCompatibility", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR)) {
            char buffer[256];
            size_t convertedChars = 0;
            wcstombs_s(&convertedChars, buffer, sizeof(buffer), vtProp.bstrVal, _TRUNCATE);
            brand = buffer;
        }
        VariantClear(&vtProp);

        // Obtener el nombre de la GPU (que generalmente incluye el modelo)
        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR)) {
            char buffer[256];
            size_t convertedChars = 0;
            wcstombs_s(&convertedChars, buffer, sizeof(buffer), vtProp.bstrVal, _TRUNCATE);
            model = buffer;
        }
        VariantClear(&vtProp);

        // Obtener la cantidad de VRAM
        hr = pclsObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4)) {
            vram = vtProp.uintVal / (1024 * 1024); // Convertir de bytes a MB
        }
        VariantClear(&vtProp);

        // Obtener la velocidad del reloj
        hr = pclsObj->Get(L"CurrentClockSpeed", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4)) {
            clock_speed = vtProp.uintVal; // en MHz
        }
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
}

std::string Gpu::getBrand() const {
    return brand;
}

std::string Gpu::getModel() const {
    return model;
}

int Gpu::getVRAM() const {
    return vram;
}

std::string Gpu::getClockSpeed() const {
    return std::to_string(clock_speed) + " MHz";
}

void Gpu::printInfo() const {
    std::cout << "GPU Brand: " << brand << std::endl;
    std::cout << "GPU Model: " << model << std::endl;
    std::cout << "GPU VRAM: " << vram << " MB" << std::endl;
    std::cout << "GPU Clock Speed: " << clock_speed << " MHz" << std::endl;
}
