// DumpAll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment(lib, "psapi")
#pragma comment(lib, "comctl32")


void mym(void *dst, unsigned char val, unsigned int size_m){
	__asm	pushad
	__asm	mov	edi, dst
	__asm	mov ecx, size_m
	__asm	mov	al, val
	__asm	cld
	__asm	rep	stosb
	__asm	popad
}

HWND GlobalList;

INT_PTR CALLBACK DlgProc(HWND hdlg, UINT wmsg, WPARAM wpara, LPARAM lparam);

int main(int argc, char *argv[])
{
	HANDLE htoken;
	::OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &htoken);
	SetPrivilege(htoken, SE_DEBUG_NAME, TRUE);
	::CloseHandle(htoken);

	::InitCommonControlsEx(NULL);
	::DialogBoxParamA(GetModuleHandleA(0), MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc, 0);
	::ExitProcess(0);
}

bool dump_all(DWORD pid, char *dir_path)
{
	
	HANDLE phandle;
	MEMORY_BASIC_INFORMATION mbi;
	DWORD	address, dummy, oldProt;
	HANDLE  fhandle, shandle;
	PVOID	mhandle;
	char fileName[256];
	
	phandle = ::OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (!phandle)
		return false;

	address = 0;

	while (address < 0x80000000){
		if (VirtualQueryEx(phandle, (PVOID)address, &mbi, sizeof(MEMORY_BASIC_INFORMATION))){
			if (mbi.State == MEM_COMMIT && !(mbi.Type & MEM_IMAGE)){
				//generate file name...
				::wsprintfA(fileName, "%s\\r%.08X.dmp", dir_path,mbi.BaseAddress);
				fhandle = ::CreateFileA(fileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0,0);
				if (fhandle == INVALID_HANDLE_VALUE) continue;
				shandle = ::CreateFileMappingA(fhandle, 0, PAGE_READWRITE, 0, mbi.RegionSize, 0);
				if (!shandle) { CloseHandle(fhandle); continue; }
				mhandle = ::MapViewOfFile(shandle, FILE_MAP_ALL_ACCESS, 0,0, mbi.RegionSize);
				if (!mhandle) { CloseHandle(shandle); CloseHandle(fhandle); continue; }
				if (mbi.Protect & PAGE_GUARD){
					::VirtualProtectEx(phandle, mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldProt);
					::ReadProcessMemory(phandle, mbi.BaseAddress, mhandle, mbi.RegionSize, &dummy);
					::VirtualProtectEx(phandle, mbi.BaseAddress, mbi.RegionSize, oldProt, &oldProt);
				}else
					::ReadProcessMemory(phandle, mbi.BaseAddress, mhandle, mbi.RegionSize, &dummy);

				::UnmapViewOfFile(mhandle);
				::CloseHandle(shandle);
				::CloseHandle(fhandle);
			}
			address += mbi.RegionSize;
		}else
			address += 0x1000;
	}
	return 0;
}

DWORD pids[4096];
DWORD needed;
DWORD num_of_processes;
HMODULE modules[4096];
char buffer[4096];
char proc_name[4096];

INT_PTR CALLBACK DlgProc(HWND hdlg, UINT wmsg, WPARAM wparam, LPARAM lparam){
	HANDLE phandle;
	ULONG index;
	OPENFILENAME ofn;
	HICON icon;

	switch (wmsg){
		case WM_INITDIALOG:
			GlobalList = GetDlgItem(hdlg, IDC_LIST1);
			icon = ::LoadIconA(GetModuleHandleA(0), (LPSTR)IDI_ICON1);
			SendMessageA(hdlg, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			SendMessageA(GlobalList, LB_RESETCONTENT, 0,0);
			::EnumProcesses(pids, 4096 * sizeof(DWORD), &needed);
			num_of_processes = needed / sizeof(DWORD);
			for (DWORD i = 0; i < num_of_processes; i++){
				phandle = ::OpenProcess(PROCESS_ALL_ACCESS, false, pids[i]);
				if (!phandle){
					wsprintf(buffer, "%.04X - process can't be opened", pids[i]);
					SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
					continue;
				}
				
				if (!EnumProcessModules(phandle, modules, 4096 * sizeof(HMODULE), &needed)){
					::CloseHandle(phandle);
					wsprintfA(buffer, "%.04X - can't obtain name", pids[i]);
					SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
					continue;
				}

				if (!::GetModuleFileNameExA(phandle, modules[0], proc_name, 4096)){
					::CloseHandle(phandle);
					wsprintfA(buffer, "%.04X - can't obtain name", pids[i]);
					SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
					continue;
				}

				wsprintfA(buffer, "%.04X - %s", pids[i], proc_name);
				SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
				::CloseHandle(phandle);
			}
			return 1;
		case WM_CLOSE:
			EndDialog(hdlg, 0);
			return 1;
		case WM_COMMAND:
			if (wparam == ID_DUMP){
				index = SendMessageA(GlobalList, LB_GETCURSEL, 0,0);
				if (index == LB_ERR){
					MessageBoxA(hdlg, "select process for dumping...", 0, 0);
					return 1;
				}
				mym(&ofn, 0, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.lpstrFilter = "Anyfile\0*.*\0\0";
				ofn.lpstrFile = (LPSTR)x_alloc(MAX_PATH);
				ofn.nMaxFile = MAX_PATH;
				ofn.hwndOwner = hdlg;
				if (!GetSaveFileNameA(&ofn)){
					MessageBoxA(hdlg, "select some file...", 0, 0);
					x_free(ofn.lpstrFile);
					return 1;
				}

				char *dir_name = ofn.lpstrFile + ::lstrlenA(ofn.lpstrFile) + 1;
				while (*dir_name != '\\')
					dir_name--;
				
				dir_name[0] = 0;
				dump_all(pids[index], ofn.lpstrFile);
				MessageBoxA(hdlg, "duming done...", "success", 0);
				x_free(ofn.lpstrFile);
				return 1;
			}
			
			if (wparam == ID_REFRESH){
				SendMessageA(GlobalList, LB_RESETCONTENT, 0,0);
				::EnumProcesses(pids, 4096 * sizeof(DWORD), &needed);
				num_of_processes = needed / sizeof(DWORD);
				for (DWORD i = 0; i < num_of_processes; i++){
					phandle = ::OpenProcess(PROCESS_ALL_ACCESS, false, pids[i]);
					if (!phandle){
						wsprintf(buffer, "%.04X - process can't be opened", pids[i]);
						SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
						continue;
					}
				
					if (!EnumProcessModules(phandle, modules, 4096 * sizeof(HMODULE), &needed)){
						::CloseHandle(phandle);
						wsprintfA(buffer, "%.04X - can't obtain name", pids[i]);
						SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
						continue;
					}

					if (!::GetModuleFileNameExA(phandle, modules[0], proc_name, 4096)){
						::CloseHandle(phandle);
						wsprintfA(buffer, "%.04X - can't obtain name", pids[i]);
						SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
						continue;
					}

					wsprintfA(buffer, "%.04X - %s", pids[i], proc_name);
					SendMessageA(GlobalList, LB_ADDSTRING, 0, (LPARAM)buffer);
					::CloseHandle(phandle);
				}
				return 1;
			}
		default:
			return 0;
	}

	return 0;
}