// The code here has been adapted from the C programs zclient.c and ArrayDemo.c, 
// which were originally written by Kenneth Moore, and they are shipped with Zemax.
// zclient.c
// Originally written by Kenneth Moore June 1997
// Copyright 1997-2006 Kenneth Moore
// ArrayDemo.c sample program
// Written by Kenneth Moore March 1999
// The original zclient.c and ArrayDemo.c files are also available in the same 
// directory for reference

#include "arrayTraceClient.h"

/* global variables used by the client code */
char szAppName[] = "ZemaxClient";
int GotData, ngNumRays, ZEMAX_INSTANCE = 0;
char szGlobalBuffer[5000], szCommandLine[260];
HINSTANCE globalhInstance;
HWND hwndServer, hwndClient;
DDERAYDATA *rdpGRD = NULL;
DDERAYDATA *gPtr2RD = NULL;  /* used for passing the ray data array to the user function */


void UserFunction(void)
{
    static char szBuffer[5000], szSub[256], szAppName[] = "Array Demo";

    /* first, update the lens so we have the latest data; and then test to make sure the system is valid */
    PostRequestMessage("GetUpdate", szBuffer);

    if (atoi(GetString(szBuffer, 0, szSub)))
    {
        /* the current lens cannot be traced! */
        /* some features may be able to create data without tracing rays; some can't */
        /* If data cannot be created return "empty" data displays */
        sprintf(szBuffer, "???");
        MakeEmptyWindow(1, szAppName, szBuffer);
        return;
    }
  
    /* Now go get the data .... the data is put back into gPtr2RD in the 
       function PostArrayTraceMessage*/
    PostArrayTraceMessage(szBuffer, gPtr2RD);
    
    gPtr2RD = NULL;
}

void print_ray_data(DDERAYDATA * pRAD)
{
    /*helper function to print ray data, which may be useful
      for code testing visually*/
    int numRays;
    int i;
    printf("\nRay data info @ C side:\n");
    numRays = pRAD[0].error;
    printf("Total number of rays = %d\n", numRays);
    for (i = 0; i <= numRays; i++)
    {
        printf("\npRAD[%d].x = %f", i, pRAD[i].x);
        printf("\npRAD[%d].y = %f", i, pRAD[i].y);
        printf("\npRAD[%d].z = %f", i, pRAD[i].z);
        printf("\npRAD[%d].l = %f", i, pRAD[i].l);
        printf("\npRAD[%d].m = %f", i, pRAD[i].m);
        printf("\npRAD[%d].n = %f", i, pRAD[i].n);
        printf("\npRAD[%d].opd =%f", i, pRAD[i].opd);
        printf("\npRAD[%d].intensity = %f", i, pRAD[i].intensity);
        printf("\npRAD[%d].Exr = %f", i, pRAD[i].Exr);
        printf("\npRAD[%d].Exi = %f", i, pRAD[i].Exi);
        printf("\npRAD[%d].Eyr = %f", i, pRAD[i].Eyr);
        printf("\npRAD[%d].Eyi = %f", i, pRAD[i].Eyi);
        printf("\npRAD[%d].Ezr = %f", i, pRAD[i].Ezr);
        printf("\npRAD[%d].Ezi = %f", i, pRAD[i].Ezi);
        printf("\npRAD[%d].wave = %d", i, pRAD[i].wave);
        printf("\npRAD[%d].error = %d", i, pRAD[i].error);
        printf("\npRAD[%d].vigcode = %d", i, pRAD[i].vigcode);
        printf("\npRAD[%d].want_opd = %d", i, pRAD[i].want_opd);
        printf("\n");
    }
}

int __stdcall arrayTrace(DDERAYDATA * pRAD)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclass;
    HINSTANCE hInstance = NULL;
    HINSTANCE hPrevInstance = NULL;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = NULL;         // hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wndclass);

    globalhInstance = hPrevInstance;

    gPtr2RD = pRAD;  /* point to where the data is so that the userfunction() can access it */

    hwnd = CreateWindow(szAppName, "ZEMAX Client", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    UpdateWindow(hwnd);
    SendMessage(hwnd, WM_USER_INITIATE, 0, 0L);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    ATOM          aApp, aTop, aItem;
    DDEACK        DdeAck;
    DDEDATA      *pDdeData;
    GLOBALHANDLE  hDdeData;
    WORD          wStatus;
    UINT_PTR      uiLow, uiHi;

    switch (iMsg)
    {
    case WM_CREATE:
        hwndServer = 0;
        return 0;

    case WM_USER_INITIATE:
        /* find ZEMAX */

        // code added September 1, 2006 to identify which ZEMAX is calling us, in case more than 1 copy of ZEMAX is running
        // this currently is only supported by user defined operands.
        // aApp = GlobalAddAtom ("ZEMAX");
        if (1)
        {
            char szSub[500];
            GetString(szCommandLine, 5, szSub);
            if (strcmp(szSub, "ZEMAX1") == 0 || strcmp(szSub, "ZEMAX2") == 0)
            {
                if (strcmp(szSub, "ZEMAX1") == 0)
                {
                    aApp = GlobalAddAtom("ZEMAX1");
                    ZEMAX_INSTANCE = 1;
                }
                if (strcmp(szSub, "ZEMAX2") == 0)
                {
                    aApp = GlobalAddAtom("ZEMAX2");
                    ZEMAX_INSTANCE = 2;
                }
            }
            else
            {
                aApp = GlobalAddAtom("ZEMAX");
                ZEMAX_INSTANCE = 0;
            }
        }

        aTop = GlobalAddAtom("RayData");

        SendMessage(HWND_BROADCAST, WM_DDE_INITIATE, (WPARAM)hwnd, MAKELONG(aApp, aTop));

        /* delete the atoms */
        GlobalDeleteAtom(aApp);
        GlobalDeleteAtom(aTop);

        /* If no response, terminate */
        if (hwndServer == NULL)
        {
            MessageBox(hwnd, "Cannot communicate with ZEMAX!", "Hello?", MB_ICONEXCLAMATION | MB_OK);
            DestroyWindow(hwnd);
            return 0;
        }

        hwndClient = hwnd;

        UserFunction();  /* UserFunction(szCommandLine);*/

        /* terminate the DDE connection */
        PostMessage(hwndServer, WM_DDE_TERMINATE, (WPARAM)hwnd, 0L);
        hwndServer = NULL;

        /* now TERMINATE! */
        DestroyWindow(hwnd);
        return 0;

    case WM_DDE_DATA:
        /* here comes the data! */
        // wParam -- sending window handle
        // lParam -- DDEDATA memory handle & item atom
        UnpackDDElParam(WM_DDE_DATA, lParam, &uiLow, &uiHi);
        FreeDDElParam(WM_DDE_DATA, lParam);
        hDdeData = (GLOBALHANDLE)uiLow;
        pDdeData = (DDEDATA *)GlobalLock(hDdeData);
        aItem = (ATOM)uiHi;

        // Initialize DdeAck structure
        DdeAck.bAppReturnCode = 0;
        DdeAck.reserved = 0;
        DdeAck.fBusy = FALSE;
        DdeAck.fAck = FALSE;


        // Check for matching format, put the data in the buffer
        if (pDdeData->cfFormat == CF_TEXT)
        {
            /* get the data back into RD */
            if (rdpGRD) memcpy(rdpGRD, (DDERAYDATA *)pDdeData->Value, (ngNumRays + 1)*sizeof(DDERAYDATA));
            else strcpy(szGlobalBuffer, (char *)pDdeData->Value);
        }

        GotData = 1;
        GlobalDeleteAtom(aItem);

        // Acknowledge if necessary
        if (pDdeData->fAckReq == TRUE)
        {
            wStatus = *((WORD *)&DdeAck);
            if (!PostMessage((HWND)wParam, WM_DDE_ACK, (WPARAM)hwnd, PackDDElParam(WM_DDE_ACK, wStatus, aItem)))
            {
                if (hDdeData)
                {
                    GlobalUnlock(hDdeData);
                    GlobalFree(hDdeData);
                }
                return 0;
            }
        }

        // Clean up
        GlobalUnlock(hDdeData);
        if (pDdeData->fRelease == TRUE || DdeAck.fAck == FALSE) GlobalFree(hDdeData);
        return 0;

    case WM_DDE_ACK:
        /* we are receiving an acknowledgement */
        /* the only one we care about is in response to the WM_DDE_INITIATE; otherwise free just the memory */
        if (hwndServer == NULL)
        {
            uiLow = (UINT_PTR)NULL;
            uiHi = (UINT_PTR)NULL;
            UnpackDDElParam(WM_DDE_ACK, lParam, &uiLow, &uiHi);
            FreeDDElParam(WM_DDE_ACK, lParam);
            hwndServer = (HWND)wParam;
            if (uiLow) GlobalDeleteAtom((ATOM)uiLow);
            if (uiHi) GlobalDeleteAtom((ATOM)uiHi);
        }
        else
        {
            HWND dummy;
            uiLow = (UINT_PTR)NULL;
            uiHi = (UINT_PTR)NULL;
            UnpackDDElParam(WM_DDE_ACK, lParam, &uiLow, &uiHi);
            FreeDDElParam(WM_DDE_ACK, lParam);
            dummy = (HWND)wParam;
            if (uiLow) GlobalDeleteAtom((ATOM)uiLow);
            if (uiHi) GlobalDeleteAtom((ATOM)uiHi);
        }
        return 0;

    case WM_DDE_TERMINATE:
        PostMessage(hwndServer, WM_DDE_TERMINATE, (WPARAM)hwnd, 0L);
        hwndServer = NULL;
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_CLOSE:
        PostMessage(hwndServer, WM_DDE_TERMINATE, (WPARAM)hwnd, 0L);
        break;             // for default processing

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void WaitForData(HWND hwnd)
{
    int sleep_count;
    MSG msg;
    DWORD dwTime;
    dwTime = GetCurrentTime();

    sleep_count = 0;

    while (!GotData)
    {
        while (PeekMessage(&msg, hwnd, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE))
        {
            DispatchMessage(&msg);
        }
        /* Give the server a chance to respond */
        Sleep(0);
        sleep_count++;
        if (sleep_count > 10000)
        {
            if (GetCurrentTime() - dwTime > DDE_TIMEOUT) return;
            sleep_count = 0;
        }
    }
}

char * GetString(char *szBuffer, int n, char *szSubString)
{
    int i, j, k;
    char szTest[5000];

    szSubString[0] = '\0';
    i = 0;
    j = 0;
    k = 0;
    while (szBuffer[i] && (k <= n))
    {
        szTest[j] = szBuffer[i];

        if (szBuffer[i] == '"')
        {

            i++;
            j++;
            szTest[j] = szBuffer[i];

            /* we have a double quote; keep reading until EOF or another double quote */
            while (szBuffer[i] != '"' && szBuffer[i])
            {
                i++;
                j++;
                szTest[j] = szBuffer[i];
            }
        }

        if (szTest[j] == ' ' || szTest[j] == '\n' || szTest[j] == '\r' || szTest[j] == '\0' || szTest[j] == ',')
        {
            szTest[j] = '\0';
            if (k == n)
            {
                strcpy(szSubString, szTest);
                return szSubString;
            }
            k++;
            j = -1;
        }
        i++;
        j++;
    }

    szTest[j] = '\0';
    if (k == n) strcpy(szSubString, szTest);

    return szSubString;
}

int PostRequestMessage(char *szItem, char *szBuffer)
{
    ATOM aItem;

    aItem = GlobalAddAtom(szItem);

    /* clear the buffers */
    szGlobalBuffer[0] = '\0';
    szBuffer[0] = '\0';

    GotData = 0;

    if (!PostMessage(hwndServer, WM_DDE_REQUEST, (WPARAM)hwndClient, PackDDElParam(WM_DDE_REQUEST, CF_TEXT, aItem)))
    {
        MessageBox(hwndClient, "Cannot communicate with ZEMAX!", "Hello?", MB_ICONEXCLAMATION | MB_OK);
        GlobalDeleteAtom(aItem);
        return -1;
    }

    WaitForData(hwndClient);
    strcpy(szBuffer, szGlobalBuffer);

    if (GotData) return 0;
    else return -1;
}

int PostArrayTraceMessage(char *szBuffer, DDERAYDATA *RD)
{
    ATOM aItem;
    HGLOBAL hPokeData;
    DDEPOKE * lpPokeData;
    long numbytes;
    int numrays;


    if (RD[0].opd > 4)
    {
        /* NSC Rays */
        numrays = (int)RD[0].opd - 5;
    }
    else
    {
        /* sequential rays */
        numrays = RD[0].error;
    }

    /* point to where the data is */
    rdpGRD = RD;
    ngNumRays = numrays;

    numbytes = (1 + numrays)*sizeof(DDERAYDATA);
    hPokeData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (LONG) sizeof(DDEPOKE) + numbytes);
    lpPokeData = (DDEPOKE *)GlobalLock(hPokeData);
    lpPokeData->fRelease = TRUE;
    lpPokeData->cfFormat = CF_TEXT;
    memcpy(lpPokeData->Value, RD, numbytes);

    /* clear the buffers */
    szGlobalBuffer[0] = '\0';
    szBuffer[0] = '\0';

    aItem = GlobalAddAtom("RayArrayData");
    GlobalUnlock(hPokeData);

    GotData = 0;

    if (!PostMessage(hwndServer, WM_DDE_POKE, (WPARAM)hwndClient, PackDDElParam(WM_DDE_POKE, (UINT)hPokeData, aItem)))
    {
        MessageBox(hwndClient, "Cannot communicate with ZEMAX!", "Hello?", MB_ICONEXCLAMATION | MB_OK);
        GlobalDeleteAtom(aItem);
        GlobalFree(hPokeData);
        return -1;
    }
    GlobalDeleteAtom(aItem);

    WaitForData(hwndClient);

    strcpy(szBuffer, szGlobalBuffer);

    /* clear the pointer */
    rdpGRD = NULL;

    if (GotData) return 0;
    else return -1;
}

void MakeEmptyWindow(int text, char *szAppName, char *szOptions)
{
    char szOutputFile[260], szModuleName[260], szBuffer[5000];
    FILE *output;

    /* get the output file name */
    GetString(szCommandLine, 2, szOutputFile);

    /* get the module name */
    GetModuleFileName(globalhInstance, szModuleName, 255);

    if ((output = fopen(szOutputFile, "wt")) == NULL)
    {
        /* can't open the file!! */
        return;
    }

    if (text)
    {
        fputs("System is invalid, cannot compute data.\n", output);
        fclose(output);
        /* create a text window. Note we pass back the filename, module name, and activesurf as a single setting parameter. */
        sprintf(szBuffer, "MakeTextWindow,\"%s\",\"%s\",\"%s\",%s", szOutputFile, szModuleName, szAppName, szOptions);
        PostRequestMessage(szBuffer, szBuffer);
    }
    else
    {
        fputs("NOFRAME\n", output);
        fputs("TEXT \"System is invalid, cannot compute data.\" .1 .5\n", output);
        fclose(output);
        sprintf(szBuffer, "MakeGraphicWindow,\"%s\",\"%s\",\"%s\",1,%s", szOutputFile, szModuleName, szAppName, szOptions);
        PostRequestMessage(szBuffer, szBuffer);
    }
}
