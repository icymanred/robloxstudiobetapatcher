// patcher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "gle.hpp"

const unsigned char SIG[] = { 0x80, 0xbf, 0x78, 0x01, 0x00, 0x00, 0x00, 0x74, 0x05, 0xe8 };
const unsigned char PATCH[] = { 0x80, 0xbf, 0x78, 0x01, 0x00, 0x00, 0x00, 0x90, 0x90, 0xe8 };




int main(int argc, char *argv[])
{
    if (argc <= 1) { std::cout << "Not enough inputs" << "\n"; return -1; }
    LARGE_INTEGER fsSize;
    HANDLE fileToScan = CreateFileA(
        argv[1],
        FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (fileToScan == INVALID_HANDLE_VALUE) {
        std::cout << GetLastErrorAsString() << "\n";
        return -1;
    }
    std::cout << "Found file, opening data..." << std::endl;
    BOOL fsret = GetFileSizeEx(fileToScan, &fsSize);
    if (fsret == 0) {
        std::cout << GetLastErrorAsString() << "\n";
        return -2;
    }
    
    printf("File size is:%I64d MB\n", ((fsSize.QuadPart / 1024 ) / 1024) );
    unsigned char* fileBuf = (unsigned char*)malloc(fsSize.QuadPart);
    DWORD bytesRead = 0;
    BOOL filereadresult = ReadFile(fileToScan, fileBuf, fsSize.QuadPart, &bytesRead, NULL);
    if (filereadresult == 0) { std::cout << GetLastErrorAsString() << "\n"; return -3; };
    if (fileBuf == NULL) { return -4; }
    for (UINT64 i = 0; i < fsSize.QuadPart; i++) {
        unsigned char* charloc = (fileBuf + i);
        unsigned char curChar = *charloc;
                if (curChar == 0x80) {
            int foundSig = memcmp((void*)SIG, (void*)charloc, sizeof(unsigned char) * 9);
            if (!foundSig) {
                DWORD bytesFound = 0;
                long offLow = (long)(i);
                long offHigh = (long)(i << 32);
                std::cout << offLow << "\n" << offHigh << "\n";
                std::cout << "\n";
                SetFilePointer(fileToScan, offLow, &offHigh, 0);
                BOOL result = WriteFile(fileToScan,
                    (void*)PATCH,
                    sizeof(unsigned char) * 9,
                    &bytesFound,
                    NULL


                );
                if (result == 0) {
                    std::cout << GetLastErrorAsString() << "\n";
                    return -4;
                }
                std::cout << "done :D" << "\n";
            
            }
        
        }

    }
    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
