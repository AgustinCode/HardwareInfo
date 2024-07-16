#include "Gpu.h"                    // Include custom header file Gpu.h
#include <windows.h>                // Include Windows header file
#include <d3d11.h>                  // Include Direct3D 11 header file
#include <dxgi.h>                   // Include DXGI header file
#include <comdef.h>                 // Include COM definitions header file
#include <Wbemidl.h>                // Include WMI (Windows Management Instrumentation) header file
#include <iostream>                 // Include standard input/output stream
#include <vector>                   // Include vector header file (though not used in this code)

#pragma comment(lib, "d3d11.lib")   // Link with Direct3D 11 library
#pragma comment(lib, "dxgi.lib")    // Link with DXGI library
#pragma comment(lib, "wbemuuid.lib")// Link with WMI UUID library

Gpu::Gpu() : brand(""), model(""), vram(0), clock_speed(0.0) {}  // Constructor initializes member variables

void Gpu::CollectInfo() {
    HRESULT hres;

    // Initialize COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);   // Initialize COM with multithreaded concurrency model
    if (FAILED(hres)) {
        std::cerr << "Failed to initialize COM library. Error code = 0x"
            << std::hex << hres << std::endl;
        return;
    }

    // Set general security levels
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

    // Obtain WMI service locator
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

    // Connect to WMI namespace
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

    // Set security levels for the connection
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

    // Query GPU information
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

        // Get GPU brand
        hr = pclsObj->Get(L"AdapterCompatibility", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR)) {
            char buffer[256];
            size_t convertedChars = 0;
            wcstombs_s(&convertedChars, buffer, sizeof(buffer), vtProp.bstrVal, _TRUNCATE);
            brand = buffer;
        }
        VariantClear(&vtProp);

        // Get GPU name (usually includes model)
        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR)) {
            char buffer[256];
            size_t convertedChars = 0;
            wcstombs_s(&convertedChars, buffer, sizeof(buffer), vtProp.bstrVal, _TRUNCATE);
            model = buffer;
        }
        VariantClear(&vtProp);

        // Get VRAM size
        hr = pclsObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4)) {
            vram = vtProp.uintVal / (1024 * 1024); // Convert from bytes to MB
        }
        VariantClear(&vtProp);

        // Get clock speed
        hr = pclsObj->Get(L"CurrentClockSpeed", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4)) {
            clock_speed = vtProp.uintVal; // in MHz
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
