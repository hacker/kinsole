class CTTY	{
public:
	virtual BOOL Input(UINT c) {
		VERIFY(SendLiteral(c));
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
			PostOutput();
		}
		return TRUE;
	}
	virtual BOOL VInput(WORD vk,DWORD cks) {
		return TRUE;
	}
	virtual	BOOL PreOutput() {
		return TRUE;
	}
	virtual BOOL Output(UINT c) {
		if(!c)
			return TRUE;
	static DWORD dummyWritten = 0;
		VERIFY(::WriteConsole(hConsoleOutput,&c,1,&dummyWritten,NULL));
		return TRUE;
	}
	virtual BOOL PostOutput() {
		return TRUE;
	}
	virtual BOOL Init() {
		return TRUE;
	}
	virtual LPCTSTR GetTitle() {
		if(TTTermType<0)
			return "Unknown Terminal Type";
		return TTypes[TTTermType].m_Name;
	}
}
	TT_TTY;