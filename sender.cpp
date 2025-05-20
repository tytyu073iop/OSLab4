#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <format>

int one() {
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "wrong start. Need {filename} {number}";
        return one();
    }
    
    std::string name = argv[1];
    std::ofstream file(name, std::ios::ate);
    if (!file) {
        std::cout << "Eror opening file\n";
        return one();
    }
    
    std::string order = argv[2];
    std::string eventName = "SenderReady";
    
    HANDLE event = OpenEvent(EVENT_MODIFY_STATE, FALSE, (eventName + order).c_str());
    if (event == NULL) {
        std::cout << "Error creating event: " << GetLastError() << '\n';
        return one();
    }
    
    HANDLE eventBoxIsNotFull = OpenEvent(EVENT_ALL_ACCESS, FALSE, "BoxIsNotFull");
    if (event == NULL) {
        std::cout << "Error creating event BoxIsFull: " << GetLastError() << '\n';
        return one();
    }
    
    std::string mutexName = "output";
    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName.c_str());
    if (mutex == NULL) {
        std::cout << "Cannot open mutex: " << GetLastError() << '\n';
        return one();
    }
    
    HANDLE avaliableMails = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, "boxSemaphore");
    if (avaliableMails == NULL) {
        std::cerr << "Cannot open semaphore: " << GetLastError() << '\n';
        return one();
    }
    
    SetEvent(event);
    
    while (true)
    {
        std::string whatToDo;
        std::cout << "Send message or exit(s/e)";
        std::cin >> whatToDo;
        if (whatToDo == "e") {
            return 0;
        }
        else if (whatToDo == "s") {
            
            std::cout << "enter message: ";
            std::string message;
            std::cin.ignore();
            if (!std::getline(std::cin, message)) {
                std::cerr << "Error getting line\n";
            } else if (message.size() >= 20) {
                std::cerr << "Wrong message size\n";
            } else {
                bool needToWait = false;
                if (!ReleaseSemaphore(avaliableMails, 1, NULL)) {
                    if (GetLastError() == ERROR_TOO_MANY_POSTS) {
                        if(!ResetEvent(eventBoxIsNotFull)) {
                            std::cerr << "Cannot reset eventBoxIsNotFull: " << GetLastError() << '\n';
                        }
                        std::cout << "Box is full. Message will be sent and program will continue after message box will be avaliable\n";
                        needToWait = true;
                    }
                }
                
                DWORD w;
                if (needToWait) {
                    HANDLE* arr = new HANDLE[]{mutex, eventBoxIsNotFull};
                    w = WaitForMultipleObjects(2, arr, TRUE, INFINITE);
                    if (!ReleaseSemaphore(avaliableMails, 1, NULL)) {
                        std::cerr << "cannot release semaphore inside need to wait: " << GetLastError() << '\n';
                    }
                    delete[] arr;
                } else {
                    w = WaitForSingleObject(mutex, INFINITE);
                }
                file.seekp(0, std::ios::end);
                file << message << '\n';
                file.flush();
                auto r = ReleaseMutex(mutex);
                if (!r) {
                    std::cout << "Cannot release mutex: " << GetLastError() << '\n';
                    return one();
                }
            }
            
        }
        else {
            std::cout << "wrong request. Repeating.\n";
        }
    }
    
}