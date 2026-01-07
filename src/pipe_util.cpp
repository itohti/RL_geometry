#include "pipe_util.hpp"
HANDLE g_pipe = INVALID_HANDLE_VALUE;


std::thread eli_thread;
HANDLE eli_pipe = INVALID_HANDLE_VALUE;
bool eli_running = true;

std::queue<std::string> eli_messages;
std::mutex eli_mutex;

void eliWorker() {
    while (eli_running) {
        eli_pipe = CreateNamedPipeA(
            "\\\\.\\pipe\\rl_receive",
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1, 1024, 1024, 0, nullptr
        );

        if (eli_pipe == INVALID_HANDLE_VALUE)
            return;

        BOOL connected = ConnectNamedPipe(eli_pipe, nullptr) ||
                         GetLastError() == ERROR_PIPE_CONNECTED;

        if (!connected) {
            CloseHandle(eli_pipe);
            continue;
        }

        while (eli_running) {
            char buffer[1024];
            DWORD bytesRead = 0;

            BOOL ok = ReadFile(
                eli_pipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr
            );

            if (!ok || bytesRead == 0)
                break;

            buffer[bytesRead] = '\0';

            std::lock_guard lock(eli_mutex);
            eli_messages.push(buffer);
        }

        CloseHandle(eli_pipe);
        eli_pipe = INVALID_HANDLE_VALUE;
    }
}
bool initPipe() {
    g_pipe = CreateFileA(
        "\\\\.\\pipe\\rl_send",  // pipe name
        GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (eli_thread.joinable()) {
        return g_pipe != INVALID_HANDLE_VALUE;
    }

    eli_running = true;
    eli_thread = std::thread(eliWorker);

    return g_pipe != INVALID_HANDLE_VALUE;
}

void sendToPython(const std::string& data) {
    if (g_pipe != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(g_pipe, data.c_str(), data.size(), &written, nullptr);
    }
}

bool readFromPython(std::string& outMsg) {
    std::lock_guard lock(eli_mutex);
    if (!eli_messages.empty()) {
        outMsg = eli_messages.front();
        eli_messages.pop();
        return true;
    }
    
    return false;
}

void closeEliPipe() {
    eli_running = false;
    
    if (eli_pipe != INVALID_HANDLE_VALUE) {
        CancelIoEx(eli_pipe, nullptr);
        DisconnectNamedPipe(eli_pipe);
        CloseHandle(eli_pipe);
        eli_pipe = INVALID_HANDLE_VALUE;
    }

    if (eli_thread.joinable()) {
        eli_thread.join();
    }
}

void closePipe() {
    if (g_pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(g_pipe);
        g_pipe = INVALID_HANDLE_VALUE;
    }
}