#include <stdio.h>

#ifdef	_DEBUG
#include <afxwin.h>
#else
#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)(f))
#define TRACE0(sz)
#define TRACE1(sz, p1)
#define TRACE2(sz, p1, p2)
#define TRACE3(sz, p1, p2, p3)
#endif

#include <winsock.h>
#include "resource.h"
#include "windowsx.h"

#define	DAMN_KIN_NAME		"KINSole"
#define	DAMN_KIN_VERSION	"v1.00 Beta 3"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum {
	WM_USERSOCKET = WM_USER+16,
	WM_USERKEY,
	WM_USERNOP
};
enum {
	tnIAC = 255,
	tnSE = 240,
	tnNOP = 241,
	tnDM = 242,
	tnBreak = 243,
	tnIP = 244,
	tnAO = 245,
	tnAYT = 246,
	tnEC = 247,
	tnEL = 248,
	tnGA = 249,
	tnSB = 250,
	tnWILL = 251,
	tnWONT = 252,
	tnDO = 253,
	tnDONT = 254
};

WSADATA wsaData;
ATOM wsaWC = NULL;
HWND wsaW = NULL;

CHAR remoteHost[256];
CHAR remoteProt[256];
sockaddr_in remoteSIN;


SOCKET telnetSocket;
HANDLE hConsoleInput;
HANDLE hConsoleOutput;
HWND hConsoleWindow;
HANDLE hConsoleThread;
DWORD consoleThreadID;
HANDLE hDispatchThread;
DWORD dispatchThreadID;
BOOL bTelnet,bTermPulled;

enum	_cState {
	cstateNone = 0, cstateIAC, cstateDO, cstateSB, cstateSBData,
	cstateSBDataIAC, cstateWILL, cstateDONT,cstateWONT,
}	connState = cstateNone;
BYTE negOption = 0;


BOOL SelectSocket() {
	return WSAAsyncSelect(telnetSocket,wsaW,WM_USERSOCKET,FD_READ|FD_OOB|FD_CLOSE)!=SOCKET_ERROR;
}

BOOL ShowWill(BYTE o)
{
	TRACE1("We're WILLing to %d\n",(WORD)o);
static
BYTE d[3] = {tnIAC,tnWILL,0};
	d[2] = o;
BOOL rv = send(telnetSocket,(char*)d,sizeof(d),0)==sizeof(d);
	SelectSocket();
	return rv;
}
BOOL ShowUnwill(BYTE o)
{
	TRACE1("We're NOT WILLing to %d\n",(WORD)o);
static
BYTE d[3] = {tnIAC,tnWONT,0};
	d[2] = o;
BOOL rv = send(telnetSocket,(char*)d,sizeof(d),0)==sizeof(d);
	SelectSocket();
	return rv;
}
BOOL BegDo(BYTE o)
{
	TRACE1("We beg to DO %d\n",(WORD)o);
static
BYTE d[3] = {tnIAC,tnDO,0};
	d[2] = o;
BOOL rv = send(telnetSocket,(char*)d,sizeof(d),0)==sizeof(d);
	SelectSocket();
	return rv;
}
BOOL BegDont(BYTE o)
{
	TRACE1("We beg DONT'T %d\n",(WORD)o);
static
BYTE d[3] = {tnIAC,tnDONT,0};
	d[2] = o;
BOOL rv = send(telnetSocket,(char*)d,sizeof(d),0)==sizeof(d);
	SelectSocket();
	return rv;
}
BOOL SubNegotiate(BYTE o,LPBYTE data,UINT size)
{
LPBYTE d = new BYTE[3+size*2+2];
int ds = 0;
	d[ds++]=tnIAC; d[ds++]=tnSB; d[ds++]=o;
	for(UINT tmp=0;tmp<size;tmp++)
		if(data[tmp]!=tnIAC)
			d[ds++]=data[tmp];
		else{
			d[ds++]=tnIAC; d[ds++]=tnIAC;
		}
	d[ds++]=tnIAC;d[ds++]=tnSE;
BOOL rv = send(telnetSocket,(char*)d,ds,0)==ds;
	delete d;
	SelectSocket();
	return rv;
}
BOOL SendLiteral(CHAR c)
{
BYTE d[2] = {tnIAC,0};
BOOL rv = FALSE;
	if(c==tnIAC){
		d[1]=c;
		rv = send(telnetSocket,(char*)d,2,0)==2;
	}else
		rv = send(telnetSocket,&c,1,0)==1;
	return rv;
}
BOOL SendLiteral(LPCTSTR c,UINT size)
{
	for(UINT tmp=0;tmp<size;tmp++)
		SendLiteral(c[tmp]);
	return TRUE;
}
BOOL SendLiteral(LPCTSTR c)
{
	return SendLiteral(c,strlen(c));
}
BOOL SendCommand(BYTE c)
{
	TRACE1("Issuing %d command\n",(WORD)c);
static
BYTE d[2] = {tnIAC,0};
	d[1] = c;
BOOL rv = send(telnetSocket,(char*)d,sizeof(d),0)==sizeof(d);
	SelectSocket();
	return rv;

}

BOOL CALLBACK consoleCtrlHandler(DWORD dwCtrlType)
{
	switch(dwCtrlType){
	case CTRL_BREAK_EVENT:
		SendCommand(tnIP);
		return TRUE;
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		PostMessage(wsaW,WM_QUIT,0,0);
		return TRUE;
	}
	return FALSE;
}

#include "options.cpp"

BOOL ProcessConsoleInput()
{
INPUT_RECORD ir[512];
DWORD got;
	while(GetNumberOfConsoleInputEvents(hConsoleInput,&got) && got){
		VERIFY(ReadConsoleInput(hConsoleInput,ir,(sizeof(ir)/sizeof(*ir)),&got));
		for(DWORD tmp=0;tmp<got;tmp++){
			if(ir[tmp].EventType==KEY_EVENT && ir[tmp].Event.KeyEvent.bKeyDown){
				if((
					ir[tmp].Event.KeyEvent.wVirtualKeyCode=='X' ||
					ir[tmp].Event.KeyEvent.wVirtualKeyCode=='Q'
					) && ir[tmp].Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED)
				){
					PostMessage(wsaW,WM_QUIT,0,0);
					return FALSE;
				}else if(
					ir[tmp].Event.KeyEvent.wVirtualKeyCode=='O'
					&& ir[tmp].Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED)
				){
					SendCommand(tnAO);
				}else if(
					ir[tmp].Event.KeyEvent.wVirtualKeyCode=='Y'
					&& ir[tmp].Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED)
				){
					SendCommand(tnAYT);
				}else if(
					ir[tmp].Event.KeyEvent.wVirtualKeyCode=='T'
					&& ir[tmp].Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED)
				){
					BegDo(toTimingMark);
				}else if(
					ir[tmp].Event.KeyEvent.wVirtualKeyCode==VK_INSERT
					&& ir[tmp].Event.KeyEvent.dwControlKeyState&SHIFT_PRESSED
				){
					if(IsClipboardFormatAvailable(CF_OEMTEXT)){
						ASSERT(wsaW);
						if(OpenClipboard(wsaW)){
						HANDLE h = GetClipboardData(CF_OEMTEXT);
						LPVOID gl = GlobalLock(h);
							TerminalIn((LPCTSTR)gl);
							GlobalUnlock(h);
//							GlobalFree(h);
							CloseClipboard();
						}
					}
				}else if(
					ir[tmp].Event.KeyEvent.wVirtualKeyCode==VK_INSERT
					&& ir[tmp].Event.KeyEvent.dwControlKeyState&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED)
				){
					if(hConsoleWindow)
						SendNotifyMessage(hConsoleWindow,WM_COMMAND,MAKELONG(0xE003,0),NULL);
				}else
					TerminalIn(ir[tmp].Event.KeyEvent);
			}
		}
	}

	return TRUE;
}

ULONG CALLBACK ConsoleThreadProc(LPVOID)
{
	for(;;){
	DWORD eves;
		if(!(GetNumberOfConsoleInputEvents(hConsoleInput,&eves) && eves))
			WaitForSingleObject(hConsoleInput,INFINITE);
		ASSERT(wsaW);
		SendMessage(wsaW,WM_USERKEY,0,0);
	}
	return 0;
}

BOOL SINTelnet(sockaddr_in& sin)
{
protoent* pe = getprotobyname("tcp");
short proto = pe?pe->p_proto:6;
	telnetSocket = socket(sin.sin_family,SOCK_STREAM,proto);
	if(telnetSocket==INVALID_SOCKET){
		printf("Failed to create socket\n");
		return FALSE;
	}
static
BOOL bOOBInline = FALSE;
	if(setsockopt(telnetSocket,SOL_SOCKET,SO_OOBINLINE,(const char*)&bOOBInline,sizeof(bOOBInline))){
		TRACE0("Failed to setsockopt for OOB data\n");
	}
	printf("Trying %s..",inet_ntoa(sin.sin_addr));
	if(connect(telnetSocket,(sockaddr*)&sin,sizeof(sin))){
		switch(WSAGetLastError()){
		case WSAECONNREFUSED: printf("\nConnection refused\n"); break;
		case WSAEHOSTUNREACH: printf("\nNo route to host\n"); break;
		case WSAENETDOWN: printf("\nNetwork is down\n"); break;
		case WSAENETUNREACH: printf("\nNetwork is unreachable\n"); break;
		case WSAETIMEDOUT: printf("\nConnection timed out\n"); break;
		default: printf("\nFailed to connect\n"); break;
		}
		return FALSE;
	}
	printf("\nConnected. Alt-X/Alt-Q - Close telnet connection\n");
//***	hConsoleInput = ::GetStdHandle(STD_INPUT_HANDLE);
//***	hConsoleOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = CreateFile("CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,0,NULL);
	hConsoleOutput = CreateFile("CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,0,NULL);
	ASSERT(hConsoleInput!=INVALID_HANDLE_VALUE &&
		hConsoleOutput!=INVALID_HANDLE_VALUE);
	if(!(bTelnet || bTermPulled))
		TerminalPullType("TTY");
	InitOptionsTable();
	TRACE0("Connected\n");
	VERIFY(SetConsoleCtrlHandler(&consoleCtrlHandler,TRUE));
	hConsoleThread = CreateThread(NULL,0,ConsoleThreadProc,NULL,0,&consoleThreadID);
	ASSERT(hConsoleThread);
	if(bTelnet){
		AskDo(toSuppressGA);
		AskWill(toTerminalType);
		AskWill(toNAWS);
		AskUnwill(toEcho);
		AskDo(toEcho);
		if(Envars && nEnvars)
			AskWill(toNewEnviron);	// *** Or better (what's better?)
	//	AskWill(toLineMode);
		// *** STATUS
	}
MSG msg;
int rvgm;
	PostMessage(wsaW,WM_USERNOP,0,0);
	VERIFY(SelectSocket());
	while(rvgm=GetMessage(&msg,NULL,NULL,NULL)){
		if(rvgm<0)
			break;	// Some wheeping needed
		TranslateMessage(&msg);
		DispatchMessage(&msg);
//	LRESULT CALLBACK WSWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
//		WSWndProc(msg.hwnd,msg.message,msg.wParam,msg.lParam);
		VERIFY(SelectSocket());
	}
	VERIFY(TerminateThread(hConsoleThread,0xFFFFFFFF));
	VERIFY(SetConsoleCtrlHandler(&consoleCtrlHandler,FALSE));
	closesocket(telnetSocket);
	return TRUE;
}

BOOL Telnet(LPCTSTR hostName,UINT port)
{
	memset(&remoteSIN,0,sizeof(remoteSIN));
	remoteSIN.sin_family = AF_INET;
	remoteSIN.sin_port = htons(port);

	remoteSIN.sin_addr.s_addr = inet_addr(hostName);
	if(remoteSIN.sin_addr.s_addr==INADDR_NONE){
	hostent* he = gethostbyname(hostName);
		if(!he){
			printf("Failed to resolve host name\n");
			return FALSE;
		}
		ASSERT(he->h_addrtype==AF_INET);
		ASSERT(he->h_length==sizeof(remoteSIN.sin_addr));
		memmove(&remoteSIN.sin_addr,*he->h_addr_list,sizeof(remoteSIN.sin_addr));
	}
	strcpy(remoteHost,hostName);
	return SINTelnet(remoteSIN);
}


void ProcessIACByte(BYTE c)
{
	connState = cstateNone;
	switch(c){
	case tnIAC:
		TerminalOut(c);
		break;
	case tnSE:
		TRACE0("SE\n");break;
	case tnNOP:
		TRACE0("NOP\n");break;
	case tnDM:
		TRACE0("DM\n");break;
	case tnBreak:
		TRACE0("Break\n");break;
	case tnIP:
		TRACE0("IP\n");break;
	case tnAO:
		TRACE0("AO\n");break;
	case tnAYT:
		TRACE0("AYT\n");break;
	case tnEC:
		TRACE0("EC\n");break;
	case tnEL:
		TRACE0("EL\n");break;
	case tnGA:
		TRACE0("GA\n");break;
	case tnSB:
		connState = cstateSB;
		break;
	case tnWILL:
		connState = cstateWILL;
		break;
	case tnWONT:
		connState = cstateWONT;
		break;
	case tnDO:
		connState = cstateDO;
		break;
	case tnDONT:
		connState = cstateDONT;
		break;
	default:
		TRACE1("Unknown OpCode = %d\n",(WORD)c);
		break;
	}
}

void ProcessNetByte(BYTE c)
{
//	TRACE1("<%d>",connState);
	switch(connState){
	case cstateWONT:
		ProcessWONT(c);
		break;
	case cstateDO:
		ProcessDO(c);
		break;
	case cstateWILL:
		ProcessWILL(c);
		break;
	case cstateDONT:
		ProcessDONT(c);
		break;
	case cstateSB:
		negOption = c;
		connState = cstateSBData;
		break;
	case cstateSBData:
	case cstateSBDataIAC:
		ProcessSBData(c);
		break;
	case cstateIAC:
		ProcessIACByte(c);
		break;
	case cstateNone:
	default:
		ASSERT(connState==cstateNone);
		if(c==tnIAC)
			connState=cstateIAC;
		else
			TerminalOut(c);
		break;
	}
}

LRESULT WSMessage(WPARAM wP,LPARAM lP)
{
	if(WSAGETSELECTERROR(lP)){
		TRACE0("SelectError\n");
		PostMessage(wsaW,WM_QUIT,0,0);
		return 0;
	}
	if(WSAGETSELECTEVENT(lP)&FD_READ){
//??		TRACE0("FD_READ\n");
	BYTE input[80*12];
	int got;
		TerminalPreO();
//??			TRACE0("rv\n");
			got=recv(telnetSocket,(CHAR*)input,sizeof(input),0);
//??			TRACE1("/rv %d\n",got);
			for(int tmp=0;tmp<got;tmp++)
				ProcessNetByte(input[tmp]);
		TerminalPostO();
//??		TRACE0("/FD_READ\n");
		return 0;
	}
	if(WSAGETSELECTEVENT(lP)&FD_OOB){
		TRACE0("OOB\n");
	}
	if(WSAGETSELECTEVENT(lP)&FD_CLOSE){
		TRACE0("CLOSE\n");
		PostMessage(wsaW,WM_QUIT,0,0);
		return 0;
	}
	VERIFY(SelectSocket());
	return 0;
}

LRESULT CALLBACK WSWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg){
	case WM_USERSOCKET:
		return WSMessage(wParam,lParam);
	case WM_USERKEY:
		return ProcessConsoleInput();
	default:
		TRACE0("DEFWINDOWPROC\n");
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}


BOOL InitializeWinsock()
{
	if(WSAStartup(0x101,&wsaData)){
		printf("Failed to initialize winsock services\n");
		return FALSE;
	}
WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc=WSWndProc; wc.hInstance=::GetModuleHandle(NULL);
	wc.lpszClassName = "_WSTFWC_";
	wsaWC = RegisterClass(&wc);
	if(!wsaWC){
		printf("Failed to initialize winsock services - 1\n");
		return FALSE;
	}
	wsaW = ::CreateWindow("_WSTFWC_","KIN Sole Mio",0,0,0,0,0,NULL,NULL,::GetModuleHandle(NULL),NULL);
	if(!wsaW){
		printf("Failed to initialize winsock services\n");
		return FALSE;
	}
	return TRUE;
}
void DeinitializeWinsock()
{
	if(wsaW)
		::DestroyWindow(wsaW);
	wsaW=NULL;
	if(wsaWC)
		::UnregisterClass("_WSTFWC_",::GetModuleHandle(NULL));
	wsaWC=NULL;
	WSACleanup();
}

HWND GetThisConsoleWnd()
{
DWORD pid = GetCurrentProcessId();
CHAR title[512];
CHAR* t = title;
	if(!GetConsoleTitle(title,sizeof(title)))
		t = NULL;
HWND hrv = FindWindowEx(NULL,NULL,"tty",t);
HWND nopro = NULL;
UINT nopros=0;
	do{
	DWORD wpid;
		if(!GetWindowThreadProcessId(hrv,&wpid))
			continue;
		if(wpid==pid)
			return hrv;
		nopro=hrv;
		nopros++;
		hrv = FindWindowEx(NULL,hrv,"tty",t);
	}while(hrv);
	if(nopros==1){
		ASSERT(nopro);
		return nopro;
	}
	return NULL;
}

main(int argc,char*argv[])
{
	if(argc<2){
usagebye:
		printf(
			DAMN_KIN_NAME " " DAMN_KIN_VERSION ", Copyright (c) 1998, 2002 Klever Group (http://www.klever.net/)\n\n"
			"Usage:\t" DAMN_KIN_NAME " [<options> ]<host-name/ip-address>[ <port>]\n\n"
			"Options are:\n"
			"-r## or -##\tSet number of rows in console screenbuffer\n"
			"-c##\t\tSet number of columns in console screenbuffer\n"
				"\tnote: changing console screenbuffer size may not work properly\n"
				"\twhen in full-screen mode\n"
			"-l<user>\tPass username to remote server in environment\n"
			"-e<var>=<val>\tPass environment variable to remote server\n"
			"-v<var>=<val>\tPass user environment variable to remote server\n"
			"-t<termtype>\tChange preferred terminal type\n"
				"\tnote: there are only two different terminal emulations in this\n"
				"\tbeta release - one for dumb terminal and one for vt terminal\n"
		);
		CleanEnvars();
		return 1;
	}
	if(!InitializeWinsock()){
		DeinitializeWinsock();
		return 2;
	}
CONSOLE_SCREEN_BUFFER_INFO csbi;
int ac = 0;
CHAR *ho = NULL, *po = NULL;
HANDLE hConsole = CreateFile("CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,0,NULL);
	ASSERT(hConsole);
// *** GetStdHandle(STD_OUTPUT_HANDLE);	// *?*?* Do something about redirections and not only here.
	VERIFY(GetConsoleScreenBufferInfo(hConsole,&csbi));
BOOL bSized=FALSE;
	bTermPulled=FALSE;
	for(int tmp=1;tmp<argc;tmp++){
	char* v = argv[tmp];
		ASSERT(v && *v);
		if((*v)=='/' || (*v)=='-'){
		int lines = atoi(&v[1]);
			if(lines){
				if(lines<csbi.dwSize.Y){
				SMALL_RECT wi = {0,0,csbi.dwSize.X-1,lines-1};
					VERIFY(SetConsoleWindowInfo(hConsole,TRUE,&wi));
				}
			COORD ns = {csbi.dwSize.X,lines};
				if(SetConsoleScreenBufferSize(hConsole,ns))
					bSized=TRUE;
			}else if(v[1]=='r'){
			int lines = atoi(&v[2]);
				if(lines){
					if(lines<csbi.dwSize.Y){
					SMALL_RECT wi = {0,0,csbi.dwSize.X-1,lines-1};
						VERIFY(SetConsoleWindowInfo(hConsole,TRUE,&wi));
					}
				COORD ns = {csbi.dwSize.X,lines};
					if(SetConsoleScreenBufferSize(hConsole,ns))
						bSized=TRUE;
				}else
					goto usagebye;
			}else if(v[1]=='c'){
			int rows = atoi(&v[2]);
				if(rows){
					if(rows<csbi.dwSize.X){
					SMALL_RECT wi = {0,0,rows-1,csbi.dwSize.Y-1};
						VERIFY(SetConsoleWindowInfo(hConsole,TRUE,&wi));
					}
				COORD ns = {rows,csbi.dwSize.Y};
					if(SetConsoleScreenBufferSize(hConsole,ns))
						bSized=TRUE;
				}else
					goto usagebye;
			}else if(v[1]=='l'){
			CHAR* vv = &v[2];
				VERIFY(AddEnvar(nesbVar,"USER",vv));
			}else if(v[1]=='e'){	// -e<name>=<value>	VAR
			CHAR* n = &v[2];
			CHAR* vv = strchr(&v[2],'=');
				if(!vv)
					goto usagebye;
				*(vv++)=0;
				VERIFY(AddEnvar(nesbVar,n,vv));
			}else if(v[1]=='v'){	// -v<name>=<value>	USERVAR
			CHAR* n = &v[2];
			CHAR* vv = strchr(n,'=');
				if(!vv)
					goto usagebye;
				*(vv++)=0;
				VERIFY(AddEnvar(nesbUserVar,n,vv));
			}else if(v[1]=='t'){	// -t<ttype>  -t<tname>=<ttype>
			CHAR* n = &v[2];
			CHAR* nn = strchr(n,'=');
				if(nn){
					*(nn++)=0;
					if(!*nn)
						nn=NULL;
				}
				if(!TerminalPullType(nn?nn:n,nn?n:NULL)){
					printf("Available terminal types are:");
					TerminalPrintTypes();
					printf("\n");
					goto usagebye;
				}
				bTermPulled=TRUE;
			}else if(v[1]=='#'){
			int cp = atoi(&v[2]);
#ifdef	_DEBUG
				TRACE2("SetCP(%d)=%d\n",cp,SetConsoleCP(cp));
				TRACE2("SetOutCP(%d)=%d\n",cp,SetConsoleOutputCP(cp));
#else
				SetConsoleCP(cp);
				SetConsoleOutputCP(cp);
#endif
				TRACE2("CP,OCP=%d,%d\n",GetConsoleCP(),GetConsoleOutputCP());
			}else
				goto usagebye;
		}else{
			if(ac==0){
				ho = v;
				ac++;
			}else if(ac==1){
				po = v;
				ac++;
			}else
				goto usagebye;
		}
	}
	if(!ho)
		goto usagebye;
servent* se = getservbyname("telnet","tcp");
UINT port = po?atol(po):(se?ntohs(se->s_port):23);
	if(port==23 || (se && port==ntohs(se->s_port)))
		bTelnet = TRUE;
	if(po && !port){
		se = getservbyname(po,"tcp");
		if(!se){
			printf("Failed to resolve tcp-service port name\n");
			DeinitializeWinsock();
			return 2;
		}
		port = ntohs(se->s_port);
		if(!stricmp(po,"telnet"))
			bTelnet = TRUE;
		else
			bTelnet = FALSE;
	}else{
		se = getservbyport(htons(port),"tcp");
		if(se){
			po = se->s_name;
			if(!stricmp(po,"telnet"))
				bTelnet=TRUE;
		}else{
			VERIFY(_itoa(port,remoteProt,10));
			po = NULL;
			bTelnet=FALSE;
		}
	}
	if(po)
		strcpy(remoteProt,po);
HICON hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_IKON));
LONG oldBIcon = NULL, oldSIcon = NULL;
	hConsoleWindow = GetThisConsoleWnd();
	if(hConsoleWindow){
		oldBIcon = SendMessage(hConsoleWindow,WM_SETICON,ICON_BIG,(LPARAM)hIcon);
		oldSIcon = SendMessage(hConsoleWindow,WM_SETICON,ICON_SMALL,(LPARAM)hIcon);
	}
	Telnet(ho,port);
	CleanEnvars();
	if(hConsoleWindow){
		SendMessage(hConsoleWindow,WM_SETICON,ICON_BIG,(LPARAM)oldBIcon);
		SendMessage(hConsoleWindow,WM_SETICON,ICON_SMALL,(LPARAM)oldSIcon);
	}
	Sleep(150);
	if(bSized){
	CONSOLE_SCREEN_BUFFER_INFO CSBI;
		VERIFY(GetConsoleScreenBufferInfo(hConsole,&CSBI));
		if(CSBI.dwSize.Y>csbi.dwSize.Y || CSBI.dwSize.X>csbi.dwSize.X){
		SMALL_RECT wi = {0,0,csbi.dwSize.X-1,csbi.dwSize.Y-1};
			VERIFY(SetConsoleWindowInfo(hConsole,TRUE,&wi));
		}
	COORD ns = {csbi.dwSize.X,csbi.dwSize.Y};
		VERIFY(SetConsoleScreenBufferSize(hConsole,ns));
	}
	Sleep(100);
	DeinitializeWinsock();
	Sleep(100);
	return 0;
}
