BOOL tmOnDo()
{
	ShowWill(toTimingMark);
	Options[toTimingMark].m_StateU=TOption::stateWantNo;
	return FALSE;
}

BOOL tmOnWill()
{
	//
	if(hConsoleWindow){
		for(int tmp=0;tmp<3;tmp++){
			FlashWindow(hConsoleWindow,TRUE);
			Sleep(300);
			FlashWindow(hConsoleWindow,FALSE);
			Sleep(300);
		}
	}else
		Beep(880,100);
	Options[toTimingMark].m_StateH=TOption::stateWantNo;
	return FALSE;
}
