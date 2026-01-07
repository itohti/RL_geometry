#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
// initializer
bool initPipe();

// mod -> bot
void sendToPython(const std::string& data);
void closePipe();

// bot -> mod
void eliWorker();
void closeEliPipe();
bool readFromPython(std::string& outMsg);
#endif