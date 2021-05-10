#define _CRT_SECURE_NO_WARNINGS

#include "include.h"
#include "resource.h"

#define IDC_PATH IDC_EDIT5


void Start(HWND hwnd)
{
	wchar_t dlgItem[MAX_PATH];
	int dlgLen = GetDlgItemText(hwnd, IDC_PATH, dlgItem, MAX_PATH);

	wchar_t path[MAX_PATH] = { 0, };
	int pathLen = GetFullPathName(dlgItem, MAX_PATH, path, NULL);
	if (pathLen == 0)
	{
		MessageBox(hwnd, L"Cannot OPEN File", L"Error", MB_OK);
		return;
	}

	wchar_t memo[MAX_PATH + 3 + 5 + 1] = { 0, };

	_snwprintf(memo, MAX_PATH + 3 + 5, L"memo=%s\0\0", path);

	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	PROCESS_INFORMATION pi;
	int err = CreateProcess(L"file\\FileReader.exe", NULL, NULL, NULL, true, CREATE_UNICODE_ENVIRONMENT, memo, NULL, &si, &pi);

	if (err == 0)
	{
		MessageBox(NULL, L"Cannot Run Program", L"Error", MB_OK);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

INT_PTR CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hwnd, IDC_EDIT4, L"Path");
		SetDlgItemText(hwnd, IDC_BUTTON1, L"Run");


		return true;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			Start(hwnd);
			return true;
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
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, winProc);
	return 0;
}