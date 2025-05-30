#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <format>

// edit max amount of messages, wait if empty and filled

std::wstring string_to_wstring(const std::string& str) {
	if (str.empty()) return L"";
	int size_needed = MultiByteToWideChar(
		CP_ACP,                  // Используем системную кодовую страницу
		0,
		str.c_str(),
		(int)str.size(),
		NULL,
		0
	);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(
		CP_ACP,
		0,
		str.c_str(),
		(int)str.size(),
		&wstr[0],
		size_needed
	);
	return wstr;
}

int main() {
	std::cout << "Enter name of the bin file: ";
	std::string name;
	std::cin >> name;
	
	name += ".bin";
	std::wstring wname = string_to_wstring(name);
	
	std::ofstream MyFile(name);
	if (!MyFile.is_open()) {
		std::cerr << "Error opening file!\n";
		return 1;
	}
	MyFile.close();
	
	size_t amountOfSenders;
	std::cout << "How much senders: ";
	std::cin >> amountOfSenders;
	
	size_t boxSize = 0;
	std::cout << "How many mails can file handle: ";
	std::cin >> boxSize;
	while (!std::cin.good() || boxSize == 0) {
		std::cerr << "Cannot read boxSize, data should be positive number. Please reenter:";
		std::cin >> boxSize;
	}
	
	HANDLE* events = new HANDLE[amountOfSenders];
	PROCESS_INFORMATION* processes = new PROCESS_INFORMATION[amountOfSenders];
	STARTUPINFO* sis = new STARTUPINFO[amountOfSenders];
	std::string eventName = "SenderReady";
	std::string mutexName = "output";
	HANDLE mutex = CreateMutex(NULL, FALSE, mutexName.c_str());
	if (mutex == NULL) {
		std::cerr << "Error creating mutex: " << GetLastError() << '\n';
		exit(1);
	}
	HANDLE avaliableMails = CreateSemaphore(NULL, 0, boxSize, "boxSemaphore");
	if (avaliableMails == NULL) {
		std::cerr << "Error creating avaliableMails: " << GetLastError() << '\n';
		exit(1);
	}
	
	auto eventBoxIsNotFull = CreateEvent(NULL, TRUE, TRUE, "BoxIsNotFull");
	if (eventBoxIsNotFull == NULL) {
		std::cout << "ERror creating event: " << GetLastError() << '\n';
	}
	
	for (size_t i = 0; i < amountOfSenders; i++)
	{
		ZeroMemory(&sis[i], sizeof(STARTUPINFO));
		sis[i].cb = sizeof(STARTUPINFO);
		LPSTR program = _strdup(("Sender.exe " + name + " " + std::to_string(i)).c_str());
		if (program == NULL) {
			std::cout << "ERROR cannot create process name string\n";
			exit(1);
		}
		auto h = CreateProcess(NULL, program, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &sis[i], &processes[i]);
		if (h == FALSE) {
			std::cout << "ERRor creating process: " << GetLastError() << '\n';
		}
		auto q = CreateEvent(NULL, FALSE, FALSE, ("SenderReady" + std::to_string(i)).c_str());
		if (q == NULL) {
			std::cout << "ERror creating event: " << GetLastError() << '\n';
		}
		events[i] = q;
		
		
	}
	
	WaitForMultipleObjects(amountOfSenders, events, TRUE, INFINITE);
	std::ifstream file(name);
	if (!file.is_open()) {
		std::cout << "Error opening file\n";
		exit(0);
	}
	
	while (true) {
		std::string whatToDo;
		std::cout << "Read the message or exit application(r/e): ";
		std::cin >> whatToDo;
		if (whatToDo == "e") {
			break;
		}
		else if (whatToDo == "r") {
			file.clear();
			HANDLE* arr = new HANDLE[]{mutex, avaliableMails};
			auto w = WaitForMultipleObjects(2, arr, TRUE, INFINITE);
			std::string line;
			if (!std::getline(file, line)) {
				std::cerr << "CANNOT read message\n";
			} else {
				std::cout << "message: " << line << '\n';
			}
			auto r = ReleaseMutex(mutex);
			if(!SetEvent(eventBoxIsNotFull)) {
				std::cerr << "cannot set eventBoxIsNotFull\n";
			}
			if (!r) {
				std::cout << "Cannot release mutex: " << GetLastError() << '\n';
				return 1;
			}
		}
		else {
			std::cout << "wrong message. Repeating.\n";
		}
	}
	
	// do process exiting
	for (size_t i = 0; i < amountOfSenders; i++)
	{
		if (!TerminateProcess(processes[i].hProcess, 0)) {
			std::cout << "Couldn't stop process, error: " << i << ' ' << GetLastError() << '\n';
		}
		if (!CloseHandle(processes[i].hThread)) {
			std::cout << "Couldn't close thread handle, error: " << i << ' ' << GetLastError() << '\n';
		}
		if (!CloseHandle(processes[i].hProcess)) {
			std::cout << "Couldn't close process handle, error: " << i << ' ' << GetLastError() << '\n';
		}
		// there should be cleanup
	}
	
}