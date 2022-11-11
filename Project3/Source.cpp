#include"Header.h"

class NtSystemCall Nt;

int main() {
	// L"" = ETC: steam.exe
	if (Nt.NtGetApplicationId(pSystem->ProcId, L"")) {
		Nt.DbgPrint(L"[+] Found Process..");
		Nt.NtGetPeImage(L"", L""); 
		
		std::this_thread::sleep_for(std::chrono::milliseconds(420));
		if (Nt.NtInjectProc(pSystem->ProcId, L""))
			Nt.DbgPrint(L"[+] Successfully Injected..");
	}
	else {
		Nt.DbgPrint(L"[-] Failed To Locate Process..");
		std::this_thread::sleep_for(std::chrono::milliseconds(420));
		exit(EXIT_FAILURE);
	}
	NULL;
}