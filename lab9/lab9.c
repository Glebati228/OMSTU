
#include <windows.h>
#include <process.h>
#include <stdio.h>



CRITICAL_SECTION csec;
char buf[12] = "123456789012";

HANDLE writer1, writer2, writer3, reader1, reader2, reader3, hstdout;
unsigned long writerID1, writerID2, writerID3, readerID1, readerID2, readerID3;


HDC hdc;



void Writer(void *arg)
{
    char pat1[12] = "abcdefghijkl";
	char pat2[12] = "ABCDEFGHIJKL";
	char pat3[12] = "-<+_!\%^&*\\|@";
	
    int k;
    for(k = 0; k < 32; k++)
    {
        int i;
        EnterCriticalSection(&csec);
        for(i = 0; i < 6; i++)
        {
			if(arg == 1)
            buf[i] = pat1[i];
			if(arg == 2)
			buf[i] = pat2[i];
			if(arg == 3)
			buf[i] = pat3[i];
        }
        LeaveCriticalSection(&csec);

        Sleep(arg);

        EnterCriticalSection(&csec);
        for(i = 6; i < 12; i++)
        {
            if(arg == 1)
            buf[i] = pat1[i];
			if(arg == 2)
			buf[i] = pat2[i];
			if(arg == 3)
			buf[i] = pat3[i];
        }
        LeaveCriticalSection(&csec);

        Sleep(arg);
    }
    EnterCriticalSection(&csec);
    COORD pos;
    pos.X=100; pos.Y=4 + arg;
    SetConsoleCursorPosition(hstdout,pos);
	SetConsoleTextAttribute(hstdout,0x0f);
    printf("writer%d exit", arg);
    LeaveCriticalSection(&csec);
}



void Reader (void *arg) {   

    COORD pos;
    int j, k = arg;
    for(j = 0; j < 32; j++)
    {
        
        
        pos.X=20 + arg; pos.Y=j+1;
        EnterCriticalSection(&csec);
        SetConsoleCursorPosition(hstdout,pos);
        SetConsoleTextAttribute(hstdout,(arg == 20) ? FOREGROUND_RED : ((arg == 40) ? FOREGROUND_GREEN : FOREGROUND_BLUE));
        int i;
        for(i = 0; i < 12; i++)
        {
            printf("%c", buf[i]);
        }
        LeaveCriticalSection(&csec);
        
        TextOut(hdc, (20*8) + (k*10), j*16, buf, 12);
        
        Sleep(1);
    } 
}

int main()
{
	
    
    HINSTANCE hInstance;
	STARTUPINFO si;
	int nCmdShow;
	MSG msg;
	WNDCLASS wc;
	HWND hwnd;
	 
    LRESULT WINAPI WinProc(HWND hwnd, UINT tmsg, WPARAM wParam, LPARAM lParam)
    {
       switch(tmsg)
        {
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
        }
        return DefWindowProc(hwnd, tmsg, wParam, lParam);
    }
    GetStartupInfo(&si);
    if(si.dwFlags & STARTF_USESHOWWINDOW)
    {
        nCmdShow = si.wShowWindow;
    }else{
        nCmdShow = SW_SHOWDEFAULT;
    }
    hInstance = GetModuleHandle(NULL);
    memset(&wc, 0, sizeof(wc));
    wc.lpszClassName = "MyClass";
    wc.lpfnWndProc = WinProc;
    wc.hCursor=LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.hInstance = hInstance;
   
    if(!RegisterClass(&wc)) return 0;
    hwnd = CreateWindow("MyClass", "Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInstance, NULL);
	if(hwnd == NULL)
    {
         MessageBox(NULL, "Window Creation Failed!", "Error!",
             MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    hdc = GetDC(hwnd);
    ShowWindow(hwnd, nCmdShow);
    
    hstdout=GetStdHandle(STD_OUTPUT_HANDLE);
    system("cls");
    InitializeCriticalSection(&csec);
        writer1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Writer,(void*)1,0,&writerID1);
		writer2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Writer,(void*)2,0,&writerID2);
		writer3 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Writer,(void*)3,0,&writerID3);
		
		reader1 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Reader,(void*)20,0,&readerID1);
		reader2 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Reader,(void*)40,0,&readerID2);
		reader3 = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Reader,(void*)60,0,&readerID3);

    
   while(GetMessage(&msg, 0, 0, 0))
   {
        DispatchMessage(&msg);
   }
   
    getchar();
   
    DeleteCriticalSection(&csec);
    SetConsoleTextAttribute(hstdout, 0x0f);
    CloseHandle(writer1); CloseHandle(writer2); CloseHandle(writer3);
    CloseHandle(reader1); CloseHandle(reader2); CloseHandle(reader3);
} 