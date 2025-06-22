#pragma once

#include <Windows.h>
#include <string>
#include <memory>

#include <d3d11.h>
#include <d3dx11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
