BOOL SendNAWS()
{
CONSOLE_SCREEN_BUFFER_INFO csbi;
	VERIFY(::GetConsoleScreenBufferInfo(hConsoleOutput,&csbi));
BYTE sbd[4];
	sbd[0]=csbi.dwSize.X>>8;
	sbd[1]=csbi.dwSize.X&0xFF;
	sbd[2]=csbi.dwSize.Y>>8;
	sbd[3]=csbi.dwSize.Y&0xFF;
	return SubNegotiate(toNAWS,sbd,sizeof(sbd));
}

BOOL nawsOnDo()
{
	TRACE0("DO NAWS\n");
	AskWill(toNAWS);
	return SendNAWS();
}
