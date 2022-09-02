#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

int main()
{
	DWORD pid;
	LPCVOID addr;
	SIZE_T len;
	
	printf("enter pid, addr, len: ");
	scanf("%d %zx %zd", &pid, &addr, &len);

	char* buf = new char[len];

	HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

	ReadProcessMemory(hproc, (LPCVOID)addr, buf, len, NULL);

	printf(buf);

	getchar();

	return 0;
}