#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdio.h>

#include "stdafx.h"
#include "enumser.h"

#elif __linux__

#else
#error "Unsupported platform"
#endif

int main()
{
#ifdef _WIN32
	printf("Windows detected, listing COM ports\n");

	CEnumerateSerial::CPortsArray ports;
	CEnumerateSerial::UsingGetCommPorts(ports);

	printf("Found %d COM ports\n", ports.size());
#endif

	return 0;
}