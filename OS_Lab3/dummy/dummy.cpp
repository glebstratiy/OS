#include <iostream>
#include <Windows.h>

int main()
{
    char buf[] = "Hello, World!";

    printf("pid, addr, len: %d %zx %zd\n", GetCurrentProcessId(), &buf, strlen(buf) + 1);

    getchar();
}