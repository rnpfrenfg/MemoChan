#define WINDOW_WIDTH 1280		
#define WINDOW_HEIGHT 500		
#define WINDOWNAME TEXT("WINDOW")		
#define CLASSNAME TEXT("CLASS")		

#define _CRT_SECURE_NO_WARNINGS

#include "include.h"
#include "resource.h"

#define EDIT_Output IDC_EDIT3

int nowIndex = 1;
int maxIndex;

HANDLE file;
HANDLE fileMap;
HANDLE nowFileView;
wchar_t* nowFileStr = NULL;
bool isUnicode = true;

int sysAllocGranu;

int fileLen;

struct IntLowHigh
{
	DWORD low;
	DWORD high;
};
union BigInteger
{
	__int64 big;
	IntLowHigh offset;
};

void CUpdateDialog(HWND hwnd)
{
	SetDlgItemInt(hwnd, IDC_EDIT1, nowIndex, FALSE);

	if (isUnicode)
	{
		SetDlgItemTextW(hwnd, EDIT_Output, nowFileStr);
	}
	else
	{
		SetDlgItemTextA(hwnd, EDIT_Output, (LPCSTR)nowFileStr);
	}
}

void ReadFile(int index)
{
	if (index > maxIndex)
		return;
	if (index <= 0)
		return;

	if (nowFileStr != NULL)
	{
		UnmapViewOfFile(nowFileStr);
	}

	BigInteger loc;
	loc.big = index;
	loc.big -= 1;
	loc.big *= sysAllocGranu;

	int readSize = sysAllocGranu;
	if (index == maxIndex)
	{
		readSize = fileLen % sysAllocGranu;
	}

	nowFileStr = (wchar_t*) MapViewOfFile(fileMap, FILE_MAP_READ | FILE_MAP_WRITE, loc.offset.high, loc.offset.low, readSize);

	if (nowFileStr == NULL)
	{
		MessageBox(NULL, L"Cannot Open Index", L"Error", MB_OK);
		return;
	}

	nowIndex = index;
	isUnicode = IsTextUnicode(nowFileStr, readSize, NULL);
}

INT_PTR CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwnd, IDC_EDIT1, L"1");
		SetDlgItemInt(hwnd, IDC_EDIT2, maxIndex, FALSE);

		SetDlgItemText(hwnd, EDIT_Output, L"");

		SetDlgItemText(hwnd, IDC_BUTTON4, L"goto");

		SetDlgItemText(hwnd, IDC_BUTTON2, L"<-");
		SetDlgItemText(hwnd, IDC_BUTTON3, L"->");

		CUpdateDialog(hwnd);

		return true;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON2:
		{
			nowIndex--;
			if (nowIndex == 0)
				nowIndex++;
			else
			{
				ReadFile(nowIndex);
				CUpdateDialog(hwnd);
			}
			return true;
		}

		case IDC_BUTTON3:
		{
			nowIndex++;
			if (nowIndex == maxIndex + 1)
				nowIndex--;
			else
			{
				ReadFile(nowIndex);
				CUpdateDialog(hwnd);
			}
			return true;
		}

		case IDC_BUTTON4:
		{
			BOOL suc;
			nowIndex = GetDlgItemInt(hwnd, IDC_EDIT1, &suc, FALSE);
			if (!suc)
			{
				return true;//
			}
			ReadFile(nowIndex);
			CUpdateDialog(hwnd);
		}
		}
		return false;

	case WM_CLOSE:
		EndDialog(hwnd, IDOK);
		return true;
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	const int strLen = 41;
	wchar_t filePath[MAX_PATH];
	ExpandEnvironmentStrings(_T("%memo%"), filePath, strLen);

	file = CreateFile(filePath, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, L"Cannot read File", L"Error", MB_OK);
		return 0;
	}

	fileLen = GetFileSize(file, NULL);


	fileMap = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, fileLen, NULL);

	if (fileMap == NULL)
	{
		MessageBox(NULL, L"Cannot read file", L"Error", MB_OK);

		CloseHandle(file);
		return 0;
	}

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	sysAllocGranu = sysInfo.dwAllocationGranularity;

	maxIndex = fileLen / sysAllocGranu;
	if ((fileLen % sysAllocGranu) != 0)
	{
		maxIndex += 1;
	}

	ReadFile(nowIndex);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, winProc);

	return 0;
}