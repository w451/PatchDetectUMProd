#include <iostream>
#include <windows.h>
#include <intrin.h>
#include "utils.hpp"
#include "comm.hpp"



using namespace std;

bool isProto(MMPFN* m) {
	return (m->Info & (1ull << 63)) != 0;
}


int main()
{
	DWORD pid = 0;
	do {
		pid = Utils::getPidByName(L"notepad.exe");
	} while (pid == 0);

	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (phandle == NULL) {
		cout << "Failed to open process" << endl;
		cout << GetLastError() << endl;
		getchar();
		return 0;
	}

	cout << "Opened notepad" << endl;

	HANDLE handle = Comm::openDriver();
	if (handle == INVALID_HANDLE_VALUE) {
		cout << "Failed to open driver" << endl;
		cout << GetLastError() << endl;
		getchar();
		return 0;
	}

	cout << "Opened driver" << endl;
	int flag = 0;

	cout << hex;

	ULONG64 region = 0;
	for (MEMORY_BASIC_INFORMATION info = { 0 }; VirtualQueryEx(phandle, (PVOID)region, &info, sizeof(info)); region = ((ULONG64)info.BaseAddress + info.RegionSize)) {
		for (ULONG64 page = (ULONG64)info.BaseAddress; page < ((ULONG64)info.BaseAddress + info.RegionSize); page+=0x1000) {

			if ((info.AllocationProtect & PAGE_EXECUTE_WRITECOPY) != 0) {
				
				PTE pte = { 0 };
				if (Comm::queryVirtualPage(handle, pid, page, &pte)) {
					if (pte.valid) {

						if (pte.page_write) { //If this executable page can be written to then someone forgot to reprotect
							flag++;
							cout << hex << page << " is writable" << endl;
						}
						if (!pte.nx && pte.dirty) { //If this is X and has been written to
							flag++;
							cout << hex << page << " is X and dirty" << endl;
						}
						MMPFN pfn = { 0 };
						if (Comm::queryPfnDatabase(handle, pte.pfn, &pfn)) {
							if (!pte.nx && !isProto(&pfn) && pfn.ShareCount == 1 && pte.accessed) { //getChargeToSize... I found this as a temporary workaround to stop falses on the nvidia overlay dll. should probaby be replaced by something that exempts a region, such as the nvidia overlay dll
								cout << hex << page << " is not the original page" << endl;
								flag += 2;
								
							}
						} else {
							cout << "Couldn't read PFN of " << hex << page << endl;
						}
						//cout << pte.pfn << " " << page << " " << pte.nx << " " << pte.dirty << " " << pte.page_write << " " << pte.accessed << " " << pfn.ShareCount << " " << isProto(&pfn) << endl;
					}
				} else {
					cout << "Couldn't get pte of " << page<< " "<<GetLastError() << endl;
				}
			}
		}
	}
	
	if (flag == 0) {
		cout << "No detections" << endl;
	} else {
		cout << "Some detections" << endl;
	}

	CloseHandle(handle);
	CloseHandle(phandle);

	getchar();
	return 0;
}