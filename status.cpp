
enum	_STSB	{
	stsbSend = 1,
	stsbIs = 0
};

BOOL statusOnDo()
{
	TRACE0("DO STATUS\n");
	return TRUE;
}
BOOL statusOnWill()
{
	TRACE0("WILL STATUS\n");
	return TRUE;
}
BOOL statusOnSB(LPBYTE data,UINT size)
{
	if(!size)
		return FALSE;
	switch(data[0]){
	case stsbSend:
		TRACE0("SB STATUS SEND\n");
		break;
	case stsbIs:
		TRACE0("SB STATUS IS\n");
		break;
	}
	return TRUE;
}