#include <windows.h>
#include <dde.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define DLL_EXPORT __declspec(dllexport)

#define WM_USER_INITIATE (WM_USER + 1)
#define DDE_TIMEOUT 50000
#pragma warning ( disable : 4996 ) // functions like strcpy are now deprecated for security reasons; this disables the warning

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double x, y, z, l, m, n, opd, intensity;
    double Exr, Exi, Eyr, Eyi, Ezr, Ezi;
    int wave, error, vigcode, want_opd;
}DDERAYDATA;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void WaitForData(HWND hwnd);
char *GetString(char *szBuffer, int n, char *szSubString);
void remove_quotes(char *s);
int  PostRequestMessage(char *szItem, char *szBuffer);
int  PostArrayTraceMessage(char *szBuffer, DDERAYDATA *RD);
void CenterWindow(HWND hwnd);
void UserFunction(); /*void UserFunction(char *szCommandLine);*/
void MakeEmptyWindow(int text, char *szAppName, char *szOptions);
void Get_2_5_10(double cmax, double *cscale);
void print_ray_data(DDERAYDATA * pRAD);

//DLL_EXPORT int __stdcall arrayTrace(DDERAYDATA * pRAD, char* tmpfile, int txtflag, int optflag);
DLL_EXPORT int __stdcall arrayTrace(DDERAYDATA * pRAD);

#ifdef __cplusplus
}
#endif
