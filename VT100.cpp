class CVT100 : public CTTY	{
	static	DWORD	dummyWritten;
	static	CHAR	m_ctGraphs[256];
	CHAR	sfCrap[256];
public:
	BYTE m_Sequence[1024];		// Buffered ESC[ sequence
	UINT m_Sequenced;
	CHAR_INFO m_Buffer[132*2];	// Deferred output buffer
	UINT m_Buffered;
	UINT m_Parms[16];			// ESC[ Parameters
	UINT m_nParms;
	UINT m_ParmPtrs[16];
	INT m_TABs[132];			// Tabulations
	CHAR* m_CT;					// Codes table for various charsets
	CHAR* m_ctG0;
	CHAR* m_ctG1;
	int m_ctG;
	CHAR* m_savedCTG0;			// Saved for ESC#7/ESC#8
	CHAR* m_savedCTG1;
	CHAR* m_savedCT;
	COORD m_savedCP;
	int m_savedCTG;
	int m_savedAttrs;
	BOOL m_bRight;				// At right margin
	BOOL m_bKPN;				// Keypad is in numeric mode
	BOOL m_bCKM;				// Cursor Keys Mode (DECCKM)
	BOOL m_bAWM;				// Autowrap mode
	BOOL m_bOM;					// Origin Mode
	BOOL m_bLNM;				// New Line Mode
	BOOL m_bIM;					// Insert Mode
	BOOL m_bVT52;				// VT52 Mode (as opposed to ANSI which is default)
	BOOL m_bTR;					// Total Reverse of the whole screen background/foreground
	BOOL m_bEBFailure;			// ESC[ Failure flag
	int	m_vt52Y1, m_vt52Y2;		// ESCY in VT52 mode parameters

	CHAR_INFO* m_tmpScrollBuffer;// Buffer used for temporary storage when doing scrolling.
	UINT m_tmpScrollBufferSize;

	void AllocateTmpBuffer() {
	UINT newSize = m_CSBI.dwSize.X*m_CSBI.dwSize.Y;
		if(newSize>m_tmpScrollBufferSize || !m_tmpScrollBuffer){
			if(m_tmpScrollBuffer) delete m_tmpScrollBuffer;
			VERIFY(m_tmpScrollBuffer=new CHAR_INFO[newSize]);
			m_tmpScrollBufferSize=newSize;
		}
	}

	CVT100() : m_tmpScrollBuffer(NULL) { }
	virtual ~CVT100() { if(m_tmpScrollBuffer) delete m_tmpScrollBuffer; }

	enum _state {
		stateNone,stateESeq,stateEBSeq, stateEPOSeq, stateEPCSeq, stateEPSSeq,
		stateEY52Seq1, stateEY52Seq2
	}	state;

	enum _attrs {
		attrNone = 0, attrBold = 1, attrUnderline = 2, attrBlink = 4, attrReverse = 8
	};
	int attrs;					// Screen attributes from _attrs enumeration
	int sr0,sr1;				// Scroll Region
	int m_Attrs;				// Screen attributes in console terms
	int m_bgAttr;				// Background color
	int m_fgAttr;				// Foreground color

	CONSOLE_SCREEN_BUFFER_INFO
		m_CSBI;					// Tracking of SB information

	void FillScreen(int x,int y,int n,TCHAR c,WORD a) {
	COORD tmp = {x,y};
		VERIFY(FillConsoleOutputCharacter(hConsoleOutput,c,n,tmp,&dummyWritten));
		VERIFY(FillConsoleOutputAttribute(hConsoleOutput,a,n,tmp,&dummyWritten));
	}
	void HScroll(int x,int y,int n,int x1=-1) {
		if(!n)
			return;
		if(x1<0)
			x1=m_CSBI.dwSize.X-1;
		else
			x1=max(x,min(x1,m_CSBI.dwSize.X-1));
	static
	COORD zero = {0,0};
		if(n>0){
		int r = max(0,min(n,x1-x+1));
			if(!r)
				return;
			if((x+r)<=x1){
				ASSERT(m_tmpScrollBuffer);
			SMALL_RECT sr = { x,y,x1-r,y };
				VERIFY(ReadConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
				sr.Left+=r;
				sr.Right+=r;
				VERIFY(WriteConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
			}
			FillScreen(x,y,r,' ',m_Attrs);
		}else{
		int r = max(0,min(-n,x1-x+1));
			if(!r)
				return;
			if((x+r)<=x1){
				ASSERT(m_tmpScrollBuffer);
			SMALL_RECT sr = {x+r,y,x1,y };
				VERIFY(ReadConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
				sr.Left-=r;
				sr.Right-=r;
				VERIFY(WriteConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
			}
			FillScreen(x1-r+1,y,r,' ',m_Attrs);
		}
	}

	void VScroll(int y,int n,int y1=-1) {
	static
	COORD zero = {0,0};
		if(!n)
			return;
		if(y1<0)
			y1 = m_CSBI.dwSize.Y;
		else
			y1 = max(y,min(y1,m_CSBI.dwSize.Y-1));
		if(n>0){
		int r = max(0,min(n,y1-y+1));
			if(!r)
				return;
			if((y+r)<=y1){
				ASSERT(m_tmpScrollBuffer);
			SMALL_RECT sr = { 0,y,m_CSBI.dwSize.X-1,y1-r };
				VERIFY(ReadConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
				sr.Top+=r;
				sr.Bottom+=r;
				VERIFY(WriteConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
			}
			FillScreen(0,y,r*m_CSBI.dwSize.X,' ',m_Attrs);
		}else{
		int r = max(0,min(-n,y1-y+1));
			if(!r)
				return;
			if((y+r)<=y1){
				ASSERT(m_tmpScrollBuffer);
			SMALL_RECT sr = {0,y+r,m_CSBI.dwSize.X-1,y1 };
				VERIFY(ReadConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
				sr.Top-=r;
				sr.Bottom-=r;
				VERIFY(WriteConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&sr));
			}
			FillScreen(0,y1-r+1,r*m_CSBI.dwSize.X,' ',m_Attrs);
		}
	}

	BOOL DoNAWS() {
		VERIFY(GetConsoleScreenBufferInfo(hConsoleOutput,&m_CSBI));
		AllocateTmpBuffer();
		VERIFY(TerminalUpdateTitle());
		if(Options[toNAWS].m_StateU==TOption::stateYes)
			SendNAWS();
		else
			AskWill(toNAWS);
		return TRUE;
	}

	BOOL ResizeWindow(COORD ns) {
		if(m_CSBI.dwCursorPosition.X>=ns.X)
			m_CSBI.dwCursorPosition.X=0;
		if(m_CSBI.dwCursorPosition.Y>=ns.Y){
			VScroll(0,-(m_CSBI.dwCursorPosition.Y-ns.Y+1),m_CSBI.dwCursorPosition.Y);
			m_CSBI.dwCursorPosition.Y=ns.Y-1;
		}
		if(!SetConsoleCursorPosition(hConsoleOutput,m_CSBI.dwCursorPosition))
			return FALSE;
		if(ns.X<m_CSBI.dwSize.X || ns.Y<m_CSBI.dwSize.Y){
		SMALL_RECT wi = { 0,0,min(ns.X-1,m_CSBI.dwMaximumWindowSize.X-1),
			min(ns.Y-1,m_CSBI.dwMaximumWindowSize.Y-1)
		};
			if(!SetConsoleWindowInfo(hConsoleOutput,TRUE,&wi))
				return FALSE;
		}
		if(!SetConsoleScreenBufferSize(hConsoleOutput,ns))
			TRACE2("Failed to set buffer size to %dx%d\n",ns.X,ns.Y);
		else{
			DoNAWS();
			if(sr0>=m_CSBI.dwSize.Y){
				sr0=0; sr1=m_CSBI.dwSize.Y-1;
			}else if(sr1>=m_CSBI.dwSize.Y){
				sr1=m_CSBI.dwSize.Y-1;
				if(sr0>=sr1)
					sr0=0;
			}
		}
		return TRUE;
	}

	WORD ReverseAttribute(WORD a) {
	WORD rv = 0;
		if(a&FOREGROUND_RED)
			rv|=BACKGROUND_RED;
		if(a&FOREGROUND_GREEN)
			rv|=BACKGROUND_GREEN;
		if(a&FOREGROUND_BLUE)
			rv|=BACKGROUND_BLUE;
		if(a&FOREGROUND_INTENSITY)
			rv|=BACKGROUND_INTENSITY;
		if(a&BACKGROUND_RED)
			rv|=FOREGROUND_RED;
		if(a&BACKGROUND_GREEN)
			rv|=FOREGROUND_GREEN;
		if(a&BACKGROUND_BLUE)
			rv|=FOREGROUND_BLUE;
		if(a&BACKGROUND_INTENSITY)
			rv|=FOREGROUND_INTENSITY;
		return rv;
	}

	void ReverseScreen() {
	static
	COORD zero = {0,0};
	SMALL_RECT whole = {0,0,m_CSBI.dwSize.X-1,m_CSBI.dwSize.Y-1};
		ASSERT(m_tmpScrollBuffer);
		VERIFY(ReadConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&whole));
		for(int tmp=0;tmp<(m_CSBI.dwSize.X*m_CSBI.dwSize.Y);tmp++)
			m_tmpScrollBuffer[tmp].Attributes = ReverseAttribute(m_tmpScrollBuffer[tmp].Attributes);
		VERIFY(WriteConsoleOutput(hConsoleOutput,m_tmpScrollBuffer,m_CSBI.dwSize,zero,&whole));
	}

	BOOL SetAttrs() {
	WORD a = (attrs&attrReverse)?BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE:
		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
		if(m_fgAttr>=0){
			a&=~(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
			a|=m_fgAttr|FOREGROUND_INTENSITY;
		}
		if(m_bgAttr>=0){
			a&=~(BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE);
			a|=m_bgAttr;
		}
		if(attrs&attrBold)
			a|=FOREGROUND_INTENSITY;
		if((attrs&attrUnderline) && (m_fgAttr<0)){
			if(attrs&attrReverse)	// Is it right way to underline? - we make foreground blue.
				a|=FOREGROUND_BLUE;
			else
				a&=~(FOREGROUND_RED|FOREGROUND_GREEN);
		}
		if(attrs&attrBlink)
			a|=BACKGROUND_INTENSITY;
		if(m_bTR)
			m_Attrs=ReverseAttribute(a);
		else
			m_Attrs = a;
		return TRUE;
	}

	virtual BOOL Input(UINT c) {
		VERIFY(SendLiteral(c));
		if(c=='\r' && m_bLNM)
			VERIFY(SendLiteral('\n'));
		if(
			(
				Options[toEcho].m_StateU==TOption::stateYes 
				|| Options[toEcho].m_StateU==TOption::stateNone
			) && (
				Options[toEcho].m_StateH!=TOption::stateYes
				|| Options[toEcho].m_StateH==TOption::stateNone
				)
		){
			PreOutput();
			Output(c);
			if(c=='\r' && m_bLNM)
				Output('\n');
			PostOutput();
		}
		return TRUE;
	}
	virtual BOOL VInput(WORD vk,DWORD cks) {
#define	AROKEY(c,n)	( \
	m_bVT52 \
	? "\33" c \
	:( \
		(cks&ENHANCED_KEY) \
		? (m_bCKM?"\33O" c:"\33[" c) \
		: (m_bKPN \
			?"\33O" n \
			:(m_bCKM?"\33O" c:"\33[" c) \
		) \
	) \
)
#define FUNKEY(c,s) ( \
	(cks&SHIFT_PRESSED) \
	?( \
		m_bVT52 \
		? NULL \
		: "\33" s \
	) \
	:( \
		m_bVT52 \
		? "\33" c \
		: "\33O" c \
	) \
)
		if(cks&(LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED)){
		COORD ns = {m_CSBI.dwSize.X,m_CSBI.dwSize.Y};
			switch(vk){
			case VK_UP:
				if(ns.Y<2)
					return TRUE;
				ns.Y--;
				break;
			case VK_DOWN:
				ns.Y++;
				break;
			case VK_LEFT:
				if(ns.X<2)
					return TRUE;
				ns.X--;
				break;
			case VK_RIGHT:
				ns.X++;
				break;
			default:
				return TRUE;
			}
			ResizeWindow(ns);
			return TRUE;
		}
		switch(vk){
		case VK_F1:		return VIn(FUNKEY("P","[23~"));
		case VK_F2:		return VIn(FUNKEY("Q","[24~"));
		case VK_F3:		return VIn(FUNKEY("R","[25~"));
		case VK_F4:		return VIn(FUNKEY("S","[26~"));
		case VK_F5:		return VIn(FUNKEY("t","[28~"));
		case VK_F6:		return VIn(FUNKEY("u","[29~"));
		case VK_F7:		return VIn(FUNKEY("v","[31~"));
		case VK_F8:		return VIn(FUNKEY("l","[32~"));
		case VK_F9:		return VIn(FUNKEY("w","[33~"));
		case VK_F10:	return VIn(FUNKEY("x","[34~")); // terminfo says kf10 "y"
		case VK_F11:	return VIn("\33[23~");
		case VK_F12:	return VIn("\33[24~");
		case VK_UP:
		case VK_NUMPAD8:	return VIn(AROKEY("A","x"));
		case VK_DOWN:
		case VK_NUMPAD2:	return VIn(AROKEY("B","r"));
		case VK_LEFT:
		case VK_NUMPAD4:	return VIn(AROKEY("D","t"));
		case VK_RIGHT:
		case VK_NUMPAD6:	return VIn(AROKEY("C","v"));
		case VK_HOME:
		case VK_NUMPAD7:	return VIn((cks&ENHANCED_KEY)
								? (NULL)
								: (m_bKPN?"\33Ow":"\33[1~")
							);
		case VK_END:
		case VK_NUMPAD1:	return VIn((cks&ENHANCED_KEY)
								? (NULL)
								: (m_bKPN?"\33Oq":"\33[4~")
							);
		case VK_PRIOR:
		case VK_NUMPAD9:	return VIn((cks&ENHANCED_KEY)
								? (NULL)
								: (m_bKPN?"\33Oy":"\33[5~")
							);
		case VK_INSERT:
		case VK_NUMPAD0:	return VIn((cks&ENHANCED_KEY)
								? ("\33Op")
								: (m_bKPN?"\33[3~":"\33Op")
							);
		case VK_DELETE:
		case VK_DECIMAL:	return VIn((cks&ENHANCED_KEY)
								? ("\33On")
								: (m_bKPN?"\33[3~":"\33On")
							);
		}

		return FALSE;
#undef	FUNKEY
#undef	AROKEY
	}
	BOOL VIn(LPCTSTR str) {
		if(!str)
			return TRUE;
	int l = strlen(str);
		VERIFY(SendLiteral(str,l));
		if(
			(
				Options[toEcho].m_StateU==TOption::stateYes 
				|| Options[toEcho].m_StateU==TOption::stateNone
			) && (
				Options[toEcho].m_StateH!=TOption::stateYes
				|| Options[toEcho].m_StateH==TOption::stateNone
				)
		){
			PreOutput();
			for(;l>0;l--)
				Output(*(str++));
			PostOutput();
		}
		return TRUE;
	}

	UINT ParseParms() {
		ASSERT(m_Sequenced>2);
		m_Parms[m_nParms=0]=0;
		m_ParmPtrs[m_nParms]=2;
	BOOL bWas = FALSE;
		for(UINT tmp=2;tmp<(m_Sequenced-1);tmp++){
			if(isdigit(m_Sequence[tmp])){
				m_Parms[m_nParms]*=10;
				m_Parms[m_nParms]+=m_Sequence[tmp]-'0';
				bWas = TRUE;
			}else{
				if((m_nParms+1)>=(sizeof(m_Parms)/sizeof(*m_Parms))){
					TRACE0("Way too many parameters in ESC[ sequence\n");
					return m_nParms=0;
				}
				m_Parms[++m_nParms]=0;
				m_ParmPtrs[m_nParms]=tmp+1;
				bWas = FALSE;
			}
		}
		if(bWas)
			m_nParms++;
		return m_nParms;
	}

	void DoIL(int y,int n) {
		VScroll(y,n,(y<=sr1)?sr1:-1);
	}
	void DoDL(int y,int n) {
		VScroll(y,-n,(y<=sr1)?sr1:-1);
	}


	BOOL DoRI() {
		m_CSBI.dwCursorPosition.Y--;
		if(m_CSBI.dwCursorPosition.Y==(sr0-1)){
			m_CSBI.dwCursorPosition.Y++;
			DoIL(m_CSBI.dwCursorPosition.Y,1);
		}
		return TRUE;
	}
	BOOL DoIND(BOOL bCR=FALSE) {
		m_CSBI.dwCursorPosition.Y++;
		if(m_CSBI.dwCursorPosition.Y==sr1+1){
			m_CSBI.dwCursorPosition.Y--;
			DoDL(sr0,1);
		}
		if(bCR)
			m_CSBI.dwCursorPosition.X=0;
		return TRUE;
	}
	BOOL DoNEL() {
		return DoIND(TRUE);
	}

	BOOL DoEBS() {
		ASSERT(m_Sequenced>2);
		ParseParms();
	CHAR c = m_Sequence[m_Sequenced-1];
		switch(c){
		UINT tmp;
		case '@':	// ICH Insert blank characters (default:1)
					// VT102 Feature
			if(m_nParms==0 || m_nParms==1){
				Flush();
				HScroll(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,m_nParms?m_Parms[0]:1);
			}else
				TRACE0("Invalid number of blanks for ESC[@\n");
			break;
		case 'A':
		case 'F':	// With horizontal homing at the end	?? VT102? rxvt?
			Flush();
			if(m_nParms==0)
				m_CSBI.dwCursorPosition.Y--;
			else
				m_CSBI.dwCursorPosition.Y-=max(1,m_Parms[0]);
			if(m_CSBI.dwCursorPosition.Y<(m_bOM?sr0:0))
				m_CSBI.dwCursorPosition.Y=m_bOM?sr0:0;
			if(c=='F')
				m_CSBI.dwCursorPosition.X=0;
			break;
		case 'B':
		case 'E':	// With horizontal homing at the end	?? VT102? rxvt?
			Flush();
			if(m_nParms==0)
				m_CSBI.dwCursorPosition.Y++;
			else
				m_CSBI.dwCursorPosition.Y+=max(1,m_Parms[0]);
			if(m_CSBI.dwCursorPosition.Y>(m_bOM?sr1:(m_CSBI.dwSize.Y-1)))
				m_CSBI.dwCursorPosition.Y=m_bOM?sr1:(m_CSBI.dwSize.Y-1);
			if(c=='E')
				m_CSBI.dwCursorPosition.X=0;
			break;
		case 'C':
			Flush();
			if(m_nParms==0)
				m_CSBI.dwCursorPosition.X++;
			else
				m_CSBI.dwCursorPosition.X+=max(1,m_Parms[0]);
			if(m_CSBI.dwCursorPosition.X>=m_CSBI.dwSize.X)
				m_CSBI.dwCursorPosition.X=m_CSBI.dwSize.X-1;
			break;
		case 'D':
			Flush();
			if(m_nParms==0)
				m_CSBI.dwCursorPosition.X--;
			else
				m_CSBI.dwCursorPosition.X-=max(1,m_Parms[0]);
			if(m_CSBI.dwCursorPosition.X<0)
				m_CSBI.dwCursorPosition.X=0;
			break;
		case 'G':	// HPA
			if(m_nParms<2){
				Flush();
				m_CSBI.dwCursorPosition.X=min(m_CSBI.dwSize.X-1,m_nParms?max(0,(int)m_Parms[0]-1):0);
			}else
				TRACE0("Invalid args for ESC[G\n");
			break;
		case 'H':	// CUP
		case 'f':	// HVP
		{
			Flush();
			if(m_nParms==0){
				m_CSBI.dwCursorPosition.X = m_CSBI.dwCursorPosition.Y = 0;
			}else if(m_nParms==1){
				m_CSBI.dwCursorPosition.Y = max(
					m_bOM?sr0:0,
					min(
						(m_bOM?sr0:0)+(int)m_Parms[0]-1,
						m_CSBI.dwSize.Y-1
					)
				);
			}else if(m_nParms==2){
				m_CSBI.dwCursorPosition.Y = max(
					m_bOM?sr0:0,
					min(
						(m_bOM?sr0:0)+(int)m_Parms[0]-1,
						m_CSBI.dwSize.Y-1
					)
				);
				m_CSBI.dwCursorPosition.X = max(
					0,
					min(
						(int)m_Parms[1]-1,
						m_CSBI.dwSize.X-1
					)
				);
			}else{
				TRACE0("Invalid arguments for ESC[H\n");
				break;
			}
		}
			break;
		case 'J':
			Flush();
			if(m_nParms==0 || m_Parms[0]==0){
				// Erase from cursor to the end of the screen.
				FillScreen(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,
					(m_CSBI.dwSize.X-m_CSBI.dwCursorPosition.X) +
					(m_CSBI.dwSize.Y-m_CSBI.dwCursorPosition.Y-1)*m_CSBI.dwSize.X,
					' ',m_Attrs);
			}else if(m_Parms[0]==1){
				// Erase from the beggining of the screen up to cursor.
				FillScreen(0,0,m_CSBI.dwSize.X*m_CSBI.dwCursorPosition.Y+m_CSBI.dwCursorPosition.X+1,
					' ',m_Attrs);
			}else if(m_Parms[0]==2){
				// Erase entire screen area
				FillScreen(0,0,m_CSBI.dwSize.X*m_CSBI.dwSize.Y,' ',m_Attrs);
			}else
				TRACE0("Invalid argument for ESC[J\n");
			break;
		case 'K':
			Flush();
			if(m_nParms==0 || m_Parms[0]==0){
				// From cursor to the end of line
				FillScreen(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,
					m_CSBI.dwSize.X-m_CSBI.dwCursorPosition.X,' ',m_Attrs);
			}else if(m_Parms[0]==1){
				// From beginning to cursor
				FillScreen(0,m_CSBI.dwCursorPosition.Y,m_CSBI.dwCursorPosition.X+1,' ',m_Attrs);
			}else if(m_Parms[0]==2){
				// Entire line
				FillScreen(0,m_CSBI.dwCursorPosition.Y,m_CSBI.dwSize.X,' ',m_Attrs);
			}else
				TRACE0("Invalid argument for ESC[K\n");
			break;
		case 'L':	// IL - Insert Line(s) - VT102
			if(m_nParms<2){
				Flush();
				DoIL(m_CSBI.dwCursorPosition.Y,m_nParms?m_Parms[0]:1);
			}else
				TRACE0("Invalid args for ESC[L\n");
			break;
		case 'M':	// DL - Delete Line(s) - VT102
			if(m_nParms<2){
				Flush();
				DoDL(m_CSBI.dwCursorPosition.Y,m_nParms?m_Parms[0]:1);
			}else
				TRACE0("Invalid args for ESC[M\n");
			break;
		case 'P':	// DCH - Delete Character(s) - VT102
			if(m_nParms<2){
				Flush();
				int dlc = m_nParms?m_Parms[0]:1;
				HScroll(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,-dlc);
			}else
				TRACE0("Invalid args for ESC[P\n");
			break;
		case 'X':	// ECH - Erase characters VT102
			if(m_nParms<2){
				Flush();
			int ec = m_nParms?m_Parms[0]:1;
				if((m_CSBI.dwCursorPosition.X+ec)>m_CSBI.dwSize.X)
					ec = m_CSBI.dwSize.X-m_CSBI.dwCursorPosition.X;
				FillScreen(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,ec,' ',m_Attrs);
			}else
				TRACE0("Invalid args for ESC[X\n");
			break;
		case 'c':	// DA - Device attribute
			if(m_nParms==0 || m_Parms[0]==0){
				SendLiteral("\33[?1;3c");	// This is supposed to mean
											// STP & AVO - not sure what
											// is STP, though..
			}else
				TRACE0("Invalid argument for ESC[c\n");
			break;
		case 'd':	// VPA
			if(m_nParms<2){
				Flush();
				m_CSBI.dwCursorPosition.Y=min(m_CSBI.dwSize.Y-1,m_nParms?max(0,(int)m_Parms[0]-1):0);
			}else
				TRACE0("Invalid args for ESC[d\n");
			break;
		case 'g':	// TBC - Tabulation clear
			if(m_nParms==0 || m_Parms[0]==0){
				Flush();
				ClearTAB(m_CSBI.dwCursorPosition.X);
			}else if(m_Parms[0]==3){
				m_TABs[0]=-1;
			}else
				TRACE1("Invalid argument for ESC[g - %d\n",m_nParms?m_Parms[0]:-1);
			break;
		case 'h':	// SM - Set Mode
		{
		BOOL bQ = FALSE;
			for(tmp=0;tmp<m_nParms;tmp++){
				switch(m_Parms[tmp]){
				case 0:	// This may be caused by '?' only
					if(m_Sequence[m_ParmPtrs[tmp]]=='?'){
						bQ=TRUE;
						continue;
					}
					TRACE0("Wrong zeromode in SM/RM command\n");
					break;
				case 1:
					if(bQ){
						m_bCKM=TRUE;
					}
					break;
				case 2:
					m_ctG0=m_ctG1=NULL;	// Designate USASCII for G0-G3?
					break;
				case 3:
					if(bQ){
						Flush();
					COORD ns = { 132,m_CSBI.dwSize.Y };
						ResizeWindow(ns);
					}
					break;
				case 4:
					if(bQ){
						TRACE0("Set smooth scrolling\n");
					}else{
						Flush();
						TRACE0("Insert Mode\n");
						m_bIM = TRUE;
					}
					break;
				case 5:
					if(bQ){
						Flush();
						if(!m_bTR)
							ReverseScreen();
						m_bTR = TRUE;
						SetAttrs();
						TRACE0("Set black on white\n");
					}
					break;
				case 6:
					if(bQ){
						Flush();
						m_bOM = TRUE;
						m_CSBI.dwCursorPosition.X=0;
						m_CSBI.dwCursorPosition.Y=sr0;	// ??? It was +1 ?
					}
					break;
				case 7:
					if(bQ){
						Flush();
						m_bAWM=TRUE;
					}
					break;
				case 8:
					if(bQ){
						TRACE0("Autorepeat On\n");
					}
					break;
				case 20:
					if(!bQ){
						// LNM - New Line Mode
						m_bLNM = TRUE;
					}
					break;
				case 40:
					if(bQ){
						TRACE0("Enable 80/132 switch\n");	// ???
					}
					break;
				case 66:
					if(bQ){
						TRACE0("KP - CS\n");
						m_bKPN = FALSE;
					}
					break;
				default:
					TRACE1("Unsupported mode in SM command: %d\n",m_Parms[tmp]);
					break;
				}
				bQ=FALSE;
			}
		}
			break;
		case 'l':	// RM - Reset Mode
		{
		BOOL bQ = FALSE;
			for(tmp=0;tmp<m_nParms;tmp++){
				switch(m_Parms[tmp]){
				case 0:	// This may be caused by '?' only
					if(m_Sequence[m_ParmPtrs[tmp]]=='?'){
						bQ=TRUE;
						continue;
					}
					TRACE0("Wrong zeromode in SM/RM command\n");
					break;
				case 1:
					if(bQ){
						m_bCKM=FALSE;
					}
					break;
				case 2:
					if(bQ){
						m_bVT52=TRUE;
					}
					break;
				case 3:
					if(bQ){
						Flush();
					COORD ns = { 80,m_CSBI.dwSize.Y };
						ResizeWindow(ns);
					}
					break;
				case 4:
					if(bQ){
						TRACE0("Set jump scrolling\n");
					}else{
						Flush();
						TRACE0("Overtype Mode\n");
						m_bIM = FALSE;
					}
					break;
				case 5:
					if(bQ){
						Flush();
						if(m_bTR)
							ReverseScreen();
						m_bTR = FALSE;
						SetAttrs();
						TRACE0("Set white on black\n");
					}
					break;
				case 6:
					if(bQ){
						Flush();
						m_bOM = FALSE;
						m_CSBI.dwCursorPosition.X = m_CSBI.dwCursorPosition.Y = 0;
					}
					break;
				case 7:
					if(bQ){
						Flush();
						m_bAWM=FALSE;
					}
					break;
				case 8:
					if(bQ){
						TRACE0("Autorepeat Off\n");
					}
					break;
				case 20:
					if(!bQ){
						// LNM - New Line Mode
						m_bLNM = FALSE;
					}
					break;
				case 40:
					if(bQ){
						TRACE0("Disable 80/132 switch\n");	// ???
					}
					break;
				case 66:
					if(bQ){
						TRACE0("KP - Numeric\n");
						m_bKPN = TRUE;
					}
					break;
				default:
					TRACE1("Unsupported mode in RM command: %d\n",m_Parms[tmp]);
					break;
				}
				bQ=FALSE;
			}
		}
			break;
		case 'm':
			if(m_nParms==0){
				attrs=0;
				m_fgAttr=m_bgAttr=-1;
			}else
				for(tmp=0;tmp<m_nParms;tmp++){
					switch(m_Parms[tmp]){
					case 0:
						attrs=0; m_fgAttr=m_bgAttr=-1; break;
					case 1:
						attrs|=attrBold; break;
					case 21:
						attrs&=~attrBold; break;
					case 2:
					case 22:
						break;	// *** LIGHT (as opposed to bold)
					case 4:
						attrs|=attrUnderline; break;
					case 24:
						attrs&=~attrUnderline; break;
					case 5:
						attrs|=attrBlink; break;
					case 25:
						attrs&=~attrBlink; break;
					case 7:
						attrs|=attrReverse; break;
					case 27:
						attrs&=~attrReverse; break;
					case 30:
						m_fgAttr=0; break;
					case 40:
						m_bgAttr=0; break;
					case 31:
						m_fgAttr=FOREGROUND_RED; break;
					case 41:
						m_bgAttr=BACKGROUND_RED; break;
					case 32:
						m_fgAttr=FOREGROUND_GREEN; break;
					case 42:
						m_bgAttr=BACKGROUND_GREEN; break;
					case 33:
						m_fgAttr=FOREGROUND_GREEN|FOREGROUND_RED; break;
					case 43:
						m_bgAttr=BACKGROUND_GREEN|BACKGROUND_RED; break;
					case 34:
						m_fgAttr=FOREGROUND_BLUE; break;
					case 44:
						m_bgAttr=BACKGROUND_BLUE; break;
					case 35:
						m_fgAttr=FOREGROUND_RED|FOREGROUND_BLUE; break;
					case 45:
						m_bgAttr=BACKGROUND_RED|BACKGROUND_BLUE; break;
					case 36:
						m_fgAttr=FOREGROUND_GREEN|FOREGROUND_BLUE; break;
					case 46:
						m_bgAttr=BACKGROUND_GREEN|BACKGROUND_BLUE; break;
					case 37:
						m_fgAttr=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE; break;
					case 47:
						m_bgAttr=BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE; break;
					case 39:
						m_fgAttr=-1; break;
					case 49:
						m_bgAttr=-1; break;
					default:
						TRACE1("Invalid attribute for ESC[m sequence - %u\n",m_Parms[tmp]);
						break;
					}
				}
			SetAttrs();
			break;
		case 'n':	// DSR - Device Status Report
			if(m_nParms==1){
				switch(m_Parms[0]){
				case 5:	// Report Status
					SendLiteral("\33[0n");
					break;
				case 6:	// Report Cursor Position
					Flush();
					sprintf(sfCrap,"\33[%d;%dR",
						(m_bOM?(m_CSBI.dwCursorPosition.Y-sr0):m_CSBI.dwCursorPosition.Y)+1,
						m_CSBI.dwCursorPosition.X+1
					);
					SendLiteral(sfCrap);
					break;
				}
			}else
				TRACE0("Invalid DSR request\n");
			break;
		case 'r':
			Flush();
			if(m_nParms==2){
				sr0=max(0,m_Parms[0]-1);
				if(sr0>=m_CSBI.dwSize.Y)
					sr0=m_CSBI.dwSize.Y-1;
				sr1=max((UINT)sr0,m_Parms[1]-1);
				if(sr1>=m_CSBI.dwSize.Y)
					sr1=m_CSBI.dwSize.Y-1;
				TRACE2("SCROLL{%d,%d}\n",(int)sr0,(int)sr1);
			}else{
				sr0=0; sr1 = m_CSBI.dwSize.Y-1;
			}
			m_CSBI.dwCursorPosition.X=0;
			m_CSBI.dwCursorPosition.Y=sr0;
			break;
		case 'x':	// DECREQTPARM	- Request Terminal Parameters
			// if parameter is zero - we'll send unsolicited reports when we exit SET-UP.
			// if not - we won't. Actually, we won't, anyway.
			// Actually, first value is not made up according to any standard that I know of
			// It's to make vttest happy.
			SendLiteral((m_nParms==0 || m_Parms[0]==0)?"\33[2;1;1;112;112;1;0x":"\33[3;1;1;112;112;1;0x");
			break;
		case 'y':	// Invoke Confidence Test
			if(m_nParms==2 && m_Parms[0]==2){
				if(m_Parms[1]==0){
					Flush();
					Init();
					FillScreen(0,0,m_CSBI.dwSize.X*m_CSBI.dwSize.Y,' ',m_Attrs);
					m_CSBI.dwCursorPosition.X = m_CSBI.dwCursorPosition.Y = 0;
				}else
					TRACE1("Terminal is now invoking tests - %d\n",m_Parms[0]);
			}else
				TRACE0("Invalid args for ESC[y\n");
			break;
		default:
			TRACE2("ESC[ SEQUENCE ENDING WITH '%c' AND %d PARS ",(CHAR)c,(INT)m_nParms);
			for(tmp=0;tmp<m_nParms;tmp++)
				TRACE1("[%d]",m_Parms[tmp]);
			TRACE0("\n");
			break;
		}
		m_Sequenced=0;
		state=stateNone;
		return TRUE;
	}

	BOOL DoPrintable(CHAR c){
		if(m_Buffered>=(sizeof(m_Buffer)/sizeof(*m_Buffer)))
			Flush();
		if(m_CT && m_CT[(BYTE)c])
			c = m_CT[(BYTE)c];
		m_Buffer[m_Buffered].Char.AsciiChar=c;
		m_Buffer[m_Buffered].Attributes=m_Attrs;
		m_Buffered++;
		return TRUE;
	}
	BOOL Flush() {
		for(UINT tmp=0;tmp<m_Buffered;){
			if(m_bRight){
				if(!m_bAWM)
					break;
				DoNEL();
				m_bRight=FALSE;
				continue;
			}
		UINT chars = min(m_Buffered-tmp,(UINT)m_CSBI.dwSize.X-m_CSBI.dwCursorPosition.X);
		COORD bs = {chars,1};
		static
		COORD bc = {0,0};
		SMALL_RECT wr = {m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,
			m_CSBI.dwSize.X-1,m_CSBI.dwCursorPosition.Y};
			if(m_bIM && ((m_CSBI.dwCursorPosition.X+(int)chars)<m_CSBI.dwSize.X))
				// Scroll to make place in insert mode
				HScroll(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,chars);
			VERIFY(::WriteConsoleOutput(hConsoleOutput,&m_Buffer[tmp],bs,bc,&wr));
			m_CSBI.dwCursorPosition.X+=chars;
			tmp+=chars;
			if(m_CSBI.dwCursorPosition.X>=m_CSBI.dwSize.X){
				ASSERT(m_CSBI.dwCursorPosition.X==m_CSBI.dwSize.X);
				m_bRight=TRUE;
				m_CSBI.dwCursorPosition.X--;
			}else{
				m_bRight=FALSE;
			}
		}
		m_Buffered=0;
		//PulseOutput();
		return TRUE;
	}

	BOOL ProcessSingleChar(CHAR c) {
		switch(c){
		case 0:
			break;	// Ignore NULLs
		case '\t':
		{
			Flush();
			for(int tmp=0;tmp<(sizeof(m_TABs)/sizeof(*m_TABs)) && m_TABs[tmp]>=0;tmp++){
				if(m_TABs[tmp]>m_CSBI.dwCursorPosition.X){
					m_CSBI.dwCursorPosition.X=m_TABs[tmp];
					break;
				}
			}
			if(tmp==(sizeof(m_TABs)/sizeof(*m_TABs)) || m_TABs[tmp]<0)
				m_CSBI.dwCursorPosition.X=m_CSBI.dwSize.X-1;
			if(m_CSBI.dwCursorPosition.X>=m_CSBI.dwSize.X)
				m_CSBI.dwCursorPosition.X=m_CSBI.dwSize.X-1;
		}
			break;
		case '\n':	// Handle this manually because of the glitch in windows handling..
		case '\13':	// VT
		case '\14':	// FF
			Flush();
			DoIND();
			break;
		case '\a':	// bell
			Flush();
			MessageBeep(0xFFFFFFFF);
			break;
		case '\b':	// back
			Flush();
			m_CSBI.dwCursorPosition.X--;
			if(m_CSBI.dwCursorPosition.X<0)
				m_CSBI.dwCursorPosition.X=0;
			break;
		case '\r':
			Flush();
			if(m_bLNM)
				DoIND();
			else
				m_CSBI.dwCursorPosition.X=0;
			break;
		case '\16':	// SO - Select G1
			m_CT = m_ctG1;
			m_ctG = 1;
			break;
		case '\17':	// SI - Select G0
			m_CT = m_ctG0;
			m_ctG = 0;
			break;
		case '\5':	// ENQ - Transmit ANSWERBACK message
			return VIn(
				"From kintucky to kinecticut,\n"
				"Kinky kinglet kindly rules,\n"
				"Makin' KINs that're most kinetic, but,\n"
				"Please, admit, they're kinda kool!\n\n"
			);
		default:
			return FALSE;
		}
		return TRUE;
	}


	virtual BOOL Output(UINT c) {
	static
	COORD	zero = {0,0};
		switch(state) {
		case stateEY52Seq1:
			m_vt52Y1=c-037-1;
			state=stateEY52Seq2;
			break;
		case stateEY52Seq2:
			m_vt52Y2=c-037-1;
			Flush();
			m_CSBI.dwCursorPosition.Y = max(
				m_bOM?sr0:0,
				min(
					(m_bOM?sr0:0)+(int)m_vt52Y1,
					m_CSBI.dwSize.Y-1
				)
			);
			m_CSBI.dwCursorPosition.X = max(
				0,
				min(
					m_vt52Y2,
					m_CSBI.dwSize.X-1
				)
			);
			state=stateNone;
			break;
		case stateEPSSeq:	// Pound Sign
			switch(c){
			case '8':	// DECALN - Screen Alignment Display
				Flush();
				FillScreen(0,0,m_CSBI.dwSize.X*m_CSBI.dwSize.Y,'E',FOREGROUND_RED|FOREGROUND_GREEN|
					FOREGROUND_BLUE);
				break;
			default:
				TRACE1("ESC# %c\n",c);
				break;
			}
			state = stateNone;
			break;
		case stateEPOSeq:	// Parenthesis Opening
			switch(c){
			case 'A':
			case 'B':
			case '1':
				m_ctG0 =NULL;
				break;
			case '0':
			case '2':
				m_ctG0 = m_ctGraphs;
				break;
			default:
				TRACE1("Invalide ESC( %c\n",c);
				break;
			}
			if(m_ctG==0)
				m_CT = m_ctG0;
			state = stateNone;
			break;
		case stateEPCSeq:	// Parenthesis Closing
			switch(c){
			case 'A':
			case 'B':
			case '1':
				m_ctG1 =NULL;
				break;
			case '0':
			case '2':
				m_ctG1 = m_ctGraphs;
				break;
			default:
				TRACE1("Invalide ESC( %c\n",c);
				break;
			}
			if(m_ctG)
				m_CT = m_ctG1;
			state = stateNone;
			break;
		case stateEBSeq:	// Bracket
			if(ProcessSingleChar(c))
				break;
			if(m_Sequenced>=sizeof(m_Sequence)){
				m_bEBFailure=TRUE;
				m_Sequenced=2;
			}
			m_Sequence[m_Sequenced++]=c;
			if(isalpha(c) || c=='~' || c=='@'){	// *** Maybe we should reconsider these critera
				if(m_bEBFailure){
					m_Sequenced=0;
					state = stateNone;
				}else
					DoEBS();
			}else if(c==030){ // CAN
				m_Sequenced=0;
				state = stateNone;
				// *** DO WE PUT OUT the checkerboard (error character?)
			}
			break;
		case stateESeq:	// Escape
			if(m_bVT52){
				state=stateNone;
				switch(c){
				case 'A':
					Flush();
					if(m_nParms==0)
						m_CSBI.dwCursorPosition.Y--;
					else
						m_CSBI.dwCursorPosition.Y-=max(1,m_Parms[0]);
					if(m_CSBI.dwCursorPosition.Y<(m_bOM?sr0:0))
						m_CSBI.dwCursorPosition.Y=m_bOM?sr0:0;
					if(c=='F')
						m_CSBI.dwCursorPosition.X=0;
					break;
				case 'B':
					Flush();
					if(m_nParms==0)
						m_CSBI.dwCursorPosition.Y++;
					else
						m_CSBI.dwCursorPosition.Y+=max(1,m_Parms[0]);
					if(m_CSBI.dwCursorPosition.Y>(m_bOM?sr1:(m_CSBI.dwSize.Y-1)))
						m_CSBI.dwCursorPosition.Y=m_bOM?sr1:(m_CSBI.dwSize.Y-1);
					if(c=='E')
						m_CSBI.dwCursorPosition.X=0;
					break;
				case 'C':
					Flush();
					if(m_nParms==0)
						m_CSBI.dwCursorPosition.X++;
					else
						m_CSBI.dwCursorPosition.X+=max(1,m_Parms[0]);
					if(m_CSBI.dwCursorPosition.X>=m_CSBI.dwSize.X)
						m_CSBI.dwCursorPosition.X=m_CSBI.dwSize.X-1;
					break;
				case 'D':
					Flush();
					if(m_nParms==0)
						m_CSBI.dwCursorPosition.X--;
					else
						m_CSBI.dwCursorPosition.X-=max(1,m_Parms[0]);
					if(m_CSBI.dwCursorPosition.X<0)
						m_CSBI.dwCursorPosition.X=0;
					break;
				case 'F':
					m_CT=m_ctG1;
					m_ctG=1;
					break;
				case 'G':
					m_CT=m_ctG0;
					m_ctG=0;
					break;
				case 'H':
					Flush();
					m_CSBI.dwCursorPosition.X = m_CSBI.dwCursorPosition.Y = 0;
					break;
				case 'I':
					Flush();
					DoRI();
					break;
				case 'J':
					Flush();
					// Erase from cursor to the end of the screen.
					FillScreen(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,
						(m_CSBI.dwSize.X-m_CSBI.dwCursorPosition.X) +
						(m_CSBI.dwSize.Y-m_CSBI.dwCursorPosition.Y-1)*m_CSBI.dwSize.X,
						' ',m_Attrs);
					break;
				case 'K':
					Flush();
					FillScreen(m_CSBI.dwCursorPosition.X,m_CSBI.dwCursorPosition.Y,
						m_CSBI.dwSize.X-m_CSBI.dwCursorPosition.X,' ',m_Attrs);
					break;
				case 'Y':
					state=stateEY52Seq1;
					break;
				case 'Z':
					VIn("\33/Z");
					break;
				case '<':
					m_bVT52=FALSE;
					break;
				default:
					TRACE0("Invalid ESC sequence in VT52 mode\n");
					break;
				}
			}else{
				switch(c){
				case '[':
					ASSERT(m_Sequenced==1);
					m_Sequence[m_Sequenced++]=c;
					state = stateEBSeq;
					break;
				case 'H':	// Set TAB at cursor
					InsertTAB(m_CSBI.dwCursorPosition.X);
					state = stateNone;
					break;
				case 'M':	// Arrow Up and scroll if needed.
					Flush();
					DoRI();
					state = stateNone;
					break;
				case 'D':	// Arrow Dn and scroll if needed.
					Flush();
					DoIND();
					state = stateNone;
					break;
				case 'E':	// Next Line - about identical to \r\n
					Flush();
					DoNEL();
					state = stateNone;
					break;
				case 'c':	// RIS - Reset to Initial State
				{
					Flush();
					Init();
					FillScreen(0,0,m_CSBI.dwSize.X*m_CSBI.dwSize.Y,' ',m_Attrs);
					m_CSBI.dwCursorPosition.X = m_CSBI.dwCursorPosition.Y = 0;
				}
					break;
				case '(':
					state = stateEPOSeq;
					break;
				case ')':
					state = stateEPCSeq;
					break;
				case '#':
					state = stateEPSSeq;
					break;
				case '=':	// DECKPAM	- Keypad sends sequences
					TRACE0("KP - CS\n");
					m_bKPN = FALSE;
					state = stateNone;
					break;
				case '>':	// DECKPNM	- Keypad sends numerics
					TRACE0("KP - Numeric\n");
					m_bKPN = TRUE;
					state = stateNone;
					break;
				case '7':	// Save Cursor and charsets
					m_savedCTG0 = m_ctG0; m_savedCTG1 = m_ctG1; m_savedCT = m_CT;
					m_savedCTG=m_ctG;
					memmove(&m_savedCP,&m_CSBI.dwCursorPosition,sizeof(m_savedCP));
					m_savedAttrs = attrs;
					state = stateNone;
					break;
				case '8':	// Restore cursor and charsets
					Flush();
					m_ctG0 = m_savedCTG0; m_ctG1 = m_savedCTG1; m_CT = m_savedCT;
					m_ctG=m_savedCTG;
					memmove(&m_CSBI.dwCursorPosition,&m_savedCP,sizeof(m_CSBI.dwCursorPosition));
					state = stateNone;
					attrs = m_savedAttrs;
					SetAttrs();
					break;
				default:
					TRACE1("ESCNON[ SEQ - '%c'\n",c);
					state = stateNone;
					break;
				}
			}
			break;
		default:
		case stateNone:
			ASSERT(state==stateNone);
			if(isprint(c&0x7F) || (c&0x7F)==0x7F){
				DoPrintable(c);
			}else{
				switch(c){
				case '\33':
					m_Sequenced=0;m_Sequence[m_Sequenced++]=c;
					state = stateESeq;
					break;
				default:
					if(!ProcessSingleChar(c))
						TRACE1("Unrecognized character 0x%02X\n",(WORD)c);
					break;
				}
			}
			m_bRight=FALSE;
			break;
		}
		return TRUE;
	}

	virtual BOOL PreOutput() {
		return TRUE;
	}
	virtual BOOL PostOutput() {
		Flush();
		VERIFY(SetConsoleCursorPosition(hConsoleOutput,m_CSBI.dwCursorPosition));
		return TRUE;
	}
	BOOL PulseOutput() {
		Flush();
		VERIFY(SetConsoleCursorPosition(hConsoleOutput,m_CSBI.dwCursorPosition));
		return TRUE;
	}

	virtual BOOL Init() {
		m_Sequenced=0;
		state = stateNone;
		attrs=0;
		DoNAWS();
		sr0 = 0; sr1 = m_CSBI.dwSize.Y-1;
		m_bRight = FALSE;
		m_Buffered=0;
		m_savedCT = m_CT = NULL;
		m_savedCTG0 = m_ctG0 = NULL;
		m_savedCTG1 = m_ctG1 = m_ctGraphs;
		m_savedCTG = m_ctG;
		m_savedCP.X=m_savedCP.Y=0;
		m_bKPN = TRUE;
		m_bCKM = FALSE;
		m_bAWM = TRUE;
		m_bOM = FALSE;
		m_bLNM = FALSE;
		m_bIM = FALSE;
		m_bVT52 = FALSE;
		m_bTR = FALSE;
		m_bgAttr=-1;
		m_fgAttr=-1;
		SetAttrs();
		InitCT();
		InitTAB();
		SizeWindowToAll();
		return TRUE;
	}

	virtual LPCTSTR GetTitle() {
	static
	CHAR consoleTitle[1024];
		if(TTTermType<0)
			return "WEIRDO";
		sprintf(consoleTitle,"%s [%dx%d]",TTypes[TTTermType].m_Name,m_CSBI.dwSize.X,m_CSBI.dwSize.Y);
		return consoleTitle;
	}

	BOOL SizeWindowToAll() {
	SMALL_RECT wi = {0,0,
			min(m_CSBI.dwSize.X-2,m_CSBI.dwMaximumWindowSize.X-1),
			min(m_CSBI.dwSize.Y-1,m_CSBI.dwMaximumWindowSize.Y-1)
		};
		SetConsoleWindowInfo(hConsoleOutput,TRUE,&wi);
		wi.Right++;
		return SetConsoleWindowInfo(hConsoleOutput,TRUE,&wi);
	}

	BOOL ClearTAB(INT tab) {
		for(int tmp=0;tmp<(sizeof(m_TABs)/sizeof(*m_TABs)) && m_TABs[tmp]>=0;tmp++)
			if(tab==m_TABs[tmp]){
				memmove(&m_TABs[tmp],&m_TABs[tmp+1],sizeof(m_TABs)-sizeof(*m_TABs)*(tmp+1));
				m_TABs[(sizeof(m_TABs)/sizeof(*m_TABs))-1]=-1;
				return TRUE;
			}
		return FALSE;
	}
	BOOL InsertTAB(INT tab) {
		for(int tmp=0;tmp<(sizeof(m_TABs)/sizeof(*m_TABs));tmp++){
			if(tab==m_TABs[tmp])
				return TRUE;
			else if(m_TABs[tmp]>tab || m_TABs[tmp]<0){
				if(tmp!=((sizeof(m_TABs)/sizeof(*m_TABs))-1))
					memmove(&m_TABs[tmp+1],&m_TABs[tmp],sizeof(m_TABs)-sizeof(*m_TABs)*(tmp+1));
				m_TABs[tmp]=tab;
				return TRUE;
			}
		}
		return FALSE;
	}
	void InitTAB() {
		ASSERT(m_CSBI.dwSize.X<=(sizeof(m_TABs)/sizeof(*m_TABs)));
		for(int tmp=0,tab=0;tmp<m_CSBI.dwSize.X;tmp+=8)
			m_TABs[tab++]=tmp;
		m_TABs[tab++]=-1;
	}
	void InitCT() {
		memset(m_ctGraphs,0,sizeof(m_ctGraphs));
		m_ctGraphs['_'] = ' ';	// Blank
		m_ctGraphs['`'] = 4;	//Diamond
		m_ctGraphs['a'] = (CHAR)0xB0;	// Checkerboard
		m_ctGraphs['b'] = (CHAR)0x1D;	// Digigraph: HT
		m_ctGraphs['c'] = (CHAR)0x1F;	// Digigtaph: FF
		m_ctGraphs['d'] = (CHAR)0x11;	// Digigraph: CR
		m_ctGraphs['e'] = (CHAR)0x19; // Digigraph: LF
		m_ctGraphs['f'] = (CHAR)0xF8;	// Degree Symbol
		m_ctGraphs['g'] = (CHAR)0xF1;	// +/i Symbol
		m_ctGraphs['h'] = (CHAR)0x1C;	// Digigraph: NL
		m_ctGraphs['i'] = (CHAR)0x17;	// Digigraph: VT
		m_ctGraphs['j'] = (CHAR)0xD9;	// Lower-Right Corner
		m_ctGraphs['k'] = (CHAR)0xBF;	// Upper-Right Corner
		m_ctGraphs['l'] = (CHAR)0xDA; // Upper-Left Corner
		m_ctGraphs['m'] = (CHAR)0xC0;	// Lower-Left Corner
		m_ctGraphs['n'] = (CHAR)0xC5;	// Crossing Lines
		m_ctGraphs['o'] = (CHAR)0xC4;	// Horiz Line - scan 1
		m_ctGraphs['p'] = (CHAR)0xC4;	// Horiz Line - scan 3
		m_ctGraphs['q'] = (CHAR)0xC4;	// Horiz Line - scan 5
		m_ctGraphs['r'] = (CHAR)0xC4;	// Horiz Line - scan 7
		m_ctGraphs['s'] = (CHAR)0xC4;	// Horiz Line - scan 9
		m_ctGraphs['t'] = (CHAR)0xC3;	// Left 'T' - |-
		m_ctGraphs['u'] = (CHAR)0xB4;	// Right 'T' - -|
		m_ctGraphs['v'] = (CHAR)0xC1;	// Bottom 'T' - _|_
		m_ctGraphs['w'] = (CHAR)0xC2;	// Top 'T' - -,-
		m_ctGraphs['x'] = (CHAR)0xB3;	// Vertical Bar - |
		m_ctGraphs['y'] = (CHAR)0xF3;	// Less or equal <=
		m_ctGraphs['z'] = (CHAR)0xF2; // Greater or equal >=
		m_ctGraphs['{'] = (CHAR)0xE3;	// Pi symbol
		m_ctGraphs['|'] = (CHAR)0xD8;	// Not equal !=
		m_ctGraphs['}'] = (CHAR)0xE0;	// UK pound symbol
		m_ctGraphs['~'] = (CHAR)0xF9;	// Centered dot
	}
}
	TT_VT100;

DWORD CVT100::dummyWritten = 0;
CHAR CVT100::m_ctGraphs[256];
// ?? CHAR CVT100::sfCrap[256];
