/*	
	Jo�o Ricardo Louzada
	joaotande@yahoo.com.br
	Kansas City - MO, UMKC 2014
	Aqui � Brasil PORRA! heuaheuhua
*/

//ABOUT THE CODE.
//This code was constructed with different pieces of others codes that I found in the internet.
//It is a example about how read and write in space of memory from another process.
//In this example I am reading a space of memory from notepad and I can change what is written there.
//The adress that I used is a dinamic adress. So you need use the CheatEngine to find another valid adress to make this code work.
//I tried find a pointer to do not need use the CheatEngine all the times that I want to use this code. But I did not find it =/.
//This Code was modified from the original one because that was not working in my X64 windows 8.1. So if you want to use my code you must compile it as a X64 code.

#include <windows.h>
#include <iostream>
#include <string.h>

using namespace std;



int main()
{
	
	DWORDLONG address = 0x0EA0BEA8A0;// Here you need put a andress of memory that you wanna read or write.
	DWORDLONG value = 0x00;
	DWORD pid;
	HWND hwnd;
	

	cout << hex << address << "\n";
	hwnd = FindWindow(NULL, L"Untitled - Notepad");// You need put here the title of the main windows of the process that you wanna read and write on.
	if (!hwnd)
	{
		cout << "Window not found!\n";
		cin.get();
	}
	else
	{
		GetWindowThreadProcessId(hwnd, &pid);
		//PROCESS_VM_OPERATION |	PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD; // This are the kinds os autorizations to manipulate memory from onther process. 
		HANDLE phandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD, 0, pid);// This line give the autorizations to use the space of memory from another process.
		if (!phandle)
		{
			cout << "Could not get handle!\n";
			cin.get();
		}
		else
		{
			while (1)
			{
				ReadProcessMemory(phandle, (LPVOID)address, &value, sizeof(value), 0);//Read a specific position from memory.
				cout << hex << value << "\n";
				Sleep(1000);
				value++;
				
				if (WriteProcessMemory(phandle, (LPVOID)address, &value, sizeof(value), 0))//Write in a specific position of memory.
				{

					cout << "It is working!"<< endl;
				}
				else
				{

					cout << "It is not working" << GetLastError() << endl;//	
				}
				
				PostMessage(hwnd, WM_ACTIVATE/*WM_CLOSE*/, 0, 0);// this line just send a message to windows message queu. So you can simulate the press of any button or any another other thing. Try use the spy of Visual studio to get what mesage you wanna use. 
			}
			return 0;
		}
	}
}

// Infomations extract from http://reverseengineering.stackexchange.com/questions/3482/does-code-injected-into-process-memory-always-belong-to-a-page-with-rwx-access

/*The assumption does not hold true, as it is possible to alter page protection flags after you've allocated memory.

The usual mechanism for code injection on Windows is as follows:

    Call OpenProcess for the target process, to get a handle that has appropriate access privileges.
    Use VirtualAllocEx to allocate a buffer in the target process, with a set of memory page access flags.
    Use WriteProcessMemory to copy the memory to the target.
    Either patch existing code to jump to the new code block, or use CreateRemoteThread to execute within the process via a new thread.

Now, there are two options here. The first is that you can specify PAGE_EXECUTE_READWRITE as a flag to VirtualAllocEx, so that you have the right to use WriteProcessMemory on that page, and also the right to execute that memory when you get to step 4. This is the "lazy" way that leads to having RWX buffers hanging around. The alternative way is to pass PAGE_READWRITE when allocating the block, then write the code, and call VirtualProtectEx to swap the flag over to PAGE_EXECUTE_READ before step 4. This gives you an RW buffer when copying data, then an RX buffer when executing.

Pseudocode:*/

/*rights = PROCESS_VM_OPERATION |
         PROCESS_VM_READ | PROCESS_VM_WRITE | 
         PROCESS_QUERY_INFORMATION | 
         PROCESS_CREATE_THREAD;

handle = OpenProcess(rights, false, pid);

targetAddr = VirtualAllocEx(handle, NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

buffer = "Hello, world!\0";
bytesWritten = 0;
WriteProcessMemory(handle, targetAddr, buffer, 14, &bytesWritten);

oldProtect = 0;
VirtualProtectEx(handle, targetAddr, 4096, PAGE_EXECUTE_READ, &oldProtect);

threadId = 0;
CreateRemoteThread(handle, NULL, 0, targetAddr, NULL, 0, &threadId);*/