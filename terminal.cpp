extern int TTTermType;
class CTTY;
struct	_tType {
	LPCTSTR m_Name;
	CTTY* m_Termcap;
};
extern _tType TTypes[];

BOOL TerminalUpdateTitle();

#include "TTY.cpp"
#include "VT100.cpp"

enum	_TTSB	{
	ttsbSend = 1,
	ttsbIs = 0
};

_tType TTypes[] = {
	{"VT102",&TT_VT100},
	{"DEC-VT102",&TT_VT100},
	{"VT100",&TT_VT100},
	{"DEC-VT100",&TT_VT100},
	{"ANSI",&TT_VT100},
	{"TTY",&TT_TTY},
	{"UNKNOWN",&TT_TTY},
	{"DUMB",&TT_TTY}
};
int	TType = -1;
CTTY* TTTerm = NULL;
int TTTermType = -1;

BOOL TerminalPullType(LPCTSTR ttype,LPCTSTR newName=NULL)
{
	for(int tmp=0;tmp<(sizeof(TTypes)/sizeof(*TTypes));tmp++){
		if(!stricmp(ttype,TTypes[tmp].m_Name)){
			if(tmp){
			_tType t;
				memmove(&t,&TTypes[tmp],sizeof(t));
				memmove(&TTypes[1],&TTypes[0],sizeof(*TTypes)*tmp);
				memmove(&TTypes[0],&t,sizeof(TTypes[0]));
			}
			if(newName)
				TTypes[0].m_Name=newName;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL TerminalPrintTypes()
{
	for(int tmp=0;tmp<(sizeof(TTypes)/sizeof(*TTypes));tmp++)
		printf(" %s",TTypes[tmp].m_Name);
	return TRUE;
}

BOOL TerminalUpdateTitle()
{
CHAR consoleTitle[1024];
	sprintf(consoleTitle,DAMN_KIN_NAME " %s:%s - %s",remoteHost,remoteProt,TTTerm->GetTitle());
	return SetConsoleTitle(consoleTitle);
}

BOOL SelectTermType(int tt)
{
	if(tt<0)
		tt=0;
	if(tt>=(sizeof(TTypes)/sizeof(*TTypes)))
		tt = sizeof(TTypes)/sizeof(*TTypes)-1;
	TTTerm = TTypes[tt].m_Termcap;
	TTTermType=tt;
	VERIFY(TTTerm->Init());
	return TerminalUpdateTitle();
}
BOOL TerminalOut(BYTE c)
{
	ASSERT(TTTerm);
	return TTTerm->Output(c);
}
BOOL TerminalIn(KEY_EVENT_RECORD k)
{
	ASSERT(TTTerm);
	if(k.uChar.AsciiChar){	// Maybe fall for V-Processing if ENHANCED_KEY
		// Workaround for ENTER's \r/\n dilemma.. ***********
		// Maybe should be moved to termtype-dependant code
	CHAR c = k.uChar.AsciiChar;
		if(k.wVirtualKeyCode==VK_RETURN){
			if(c=='\r')
				return TTTerm->Input('\r') && TTTerm->Input('\n');
			else if(c=='\n')
				return TTTerm->Input('\n');
		}
		return TTTerm->Input(c);
	}else
		return TTTerm->VInput(k.wVirtualKeyCode,k.dwControlKeyState);
}
BOOL TerminalIn(LPCTSTR str)
{
	ASSERT(TTTerm);
	while(*str)
		TTTerm->Input(*(str++));
	return TRUE;
}
BOOL TerminalPreO()
{
	ASSERT(TTTerm);
	return TTTerm->PreOutput();
}
BOOL TerminalPostO()
{
	ASSERT(TTTerm);
	return TTTerm->PostOutput();
}

BOOL terminaltypeOnInit()
{
	VERIFY(SetConsoleMode(hConsoleInput,0));
	TType = -1;
	return SelectTermType(0);
}
BOOL terminaltypeOnDo()
{
	TRACE0("DO TERMINAL-TYPE\n");
	return TRUE;
}
BOOL terminaltypeOnSB(LPBYTE data,UINT size)
{
	if(!size)
		return FALSE;
	if(data[0]!=ttsbSend)
		return FALSE;
	TRACE0("SB TERMINAL-TYPE SEND\n");
	TType++;
	if(TType>(sizeof(TTypes)/sizeof(*TTypes)))
		TType=0;
int tt = TType;
	if(tt>=(sizeof(TTypes)/sizeof(*TTypes)))
		tt = sizeof(TTypes)/sizeof(*TTypes)-1;
	VERIFY(SelectTermType(tt));
int ds = strlen(TTypes[tt].m_Name)+1;
LPBYTE lpb = new BYTE[ds];
	lpb[0] = ttsbIs;
	memmove(&lpb[1],TTypes[tt].m_Name,ds-1);
	VERIFY(SubNegotiate(toTerminalType,lpb,ds));
	delete lpb;
	return TRUE;
}
