
enum	_TOption {
	toBinaryTransmission = 0,
	toEcho = 1,						// Done.
	toReconnection = 2,
	toSuppressGA = 3,				// Eaten
	toApproxMsgSizeNegotiation = 4,
	toStatus = 5,					// Half-implemented
	toTimingMark = 6,
	toRemoteControlledTransAndEcho = 7,
	toOutputLineWidth = 8,
	toOutputPageSize = 9,
	toOutputCRDisposition = 10,
	toOutputHTabstops = 11,
	toOutputHTabDisposition = 12,
	toOutputFFDisposition = 13,
	toOutputVTabstops = 14,
	toOutputVTabDisposition = 15,
	toOutputLFDisposition = 16,
	toExtendedASCII = 17,
	toLogout = 18,
	toByteMacro = 19,
	toDET = 20,
	toSUPDUP = 22,	// ???
	toSUPDUPOutput = 22,
	toSendLocation = 23,
	toTerminalType =24,				// Done.
	toEndOfRecord = 25,
	toTACACSUserId = 26,
	toOutputMarking = 27,
	toTerminalLocationNumber = 28,
	toTelnet3270Regime = 29,
	toX3PAD = 30,
	toNAWS = 31,					// Done.
	toTerminalSpeed = 32,			// No need to implement, really.
	toRemoteFlowControl = 33,		// !!!
	toLineMode = 34,
	toXDisplayLocation = 35,		// No need to implement until we
									// do our own X-Server.
	toEnvironmentOption = 36,
	toAuthenticationOption = 37,
	toEncryptionOption = 38,
	toNewEnviron = 39,				// Need to implement
	toTN3270E = 40,
	toExtendedOptionsList = 255
};

struct	TOption	{
	enum _state {
		stateNone=0, stateNo, stateYes, stateWantNo, stateWantYes
	};
	UINT m_StateU:3;	// State - US
	UINT m_StateH:3;	// State - HIM
	int m_Q:1;
	int m_Supported:1;
	UINT m_SBCluster;
	UINT m_AllocatedSB;
	UINT m_StoredSB;
	LPBYTE m_SB;

	BOOL (*m_OnDo)();
	BOOL (*m_OnDont)();
	BOOL (*m_OnWill)();
	BOOL (*m_OnWont)();
	BOOL (*m_OnSB)(LPBYTE data,UINT size);
	BOOL (*m_OnInit)();

	BOOL OnDo() { return m_OnDo?(*m_OnDo)():FALSE; }
	BOOL OnDont() { return m_OnDont?(*m_OnDont)():FALSE; }
	BOOL OnWill() { return m_OnWill?(*m_OnWill)():FALSE; }
	BOOL OnWont() { return m_OnWont?(*m_OnWont)():FALSE; }
	BOOL OnSB(LPBYTE data,UINT size) { return m_OnSB?(*m_OnSB)(data,size):FALSE; }
	BOOL OnInit() { return m_OnInit?(*m_OnInit)():FALSE; }

	BOOL StoreSByte(BYTE c) {
		if(m_AllocatedSB>=m_StoredSB){
			ASSERT(m_SBCluster);
		LPBYTE nsb = new BYTE[m_AllocatedSB+m_SBCluster];
			ASSERT(nsb);
			if(m_StoredSB){
				ASSERT(m_SB);
				memmove(nsb,m_SB,m_StoredSB);
			}
			if(m_SB)
				delete m_SB;
			m_SB = nsb;
		}
		ASSERT(m_SB);
		m_SB[m_StoredSB++]=c;
		return TRUE;
	}
	BOOL CleanSB() {
		if(m_SB)
			delete m_SB;
		m_SB = NULL;
		m_AllocatedSB=m_StoredSB=0;
		return TRUE;
	}

}	Options[256];


BOOL AskWill(BYTE o);
BOOL AskWont(BYTE o);
BOOL AskDo(BYTE o);
BOOL AskDont(BYTE o);

#include "NAWS.cpp"
#include "terminal.cpp"
#include "status.cpp"
#include "NEW-ENVIRON.cpp"
#include "TIMING-MARK.cpp"

BOOL On_ACK()	{ return TRUE; }


void InitOptionsTable()
{
	memset(Options,0,sizeof(Options));
	// Echo
	Options[toEcho].m_OnDo = On_ACK;
	Options[toEcho].m_OnDont = On_ACK;
	Options[toEcho].m_OnWill = On_ACK;
	Options[toEcho].m_OnWont = On_ACK;
	// Suppress Go-Ahead
	Options[toSuppressGA].m_OnWill = On_ACK;
	Options[toSuppressGA].m_OnDo = On_ACK;
	// Status
	Options[toStatus].m_OnWill = statusOnWill;
	Options[toStatus].m_OnDo = statusOnDo;
	Options[toStatus].m_OnSB = statusOnSB;
	// Terminal Type;
	Options[toTerminalType].m_OnDo = terminaltypeOnDo;
	Options[toTerminalType].m_OnSB = terminaltypeOnSB;
	Options[toTerminalType].m_OnInit = terminaltypeOnInit;
	Options[toTerminalType].m_SBCluster = 32;
	// NAWS
	Options[toNAWS].m_OnDo = nawsOnDo;
	Options[toNAWS].m_SBCluster = 16;
	// New Environment
	Options[toNewEnviron].m_OnDo = newenvironOnDo;
	Options[toNewEnviron].m_OnDont = newenvironOnDont;
	Options[toNewEnviron].m_OnSB = newenvironOnSB;
	Options[toNewEnviron].m_SBCluster = 32;
	// Timing Mark
	Options[toTimingMark].m_OnDo = tmOnDo;
	Options[toTimingMark].m_OnWill = tmOnWill;

	for(int tmp=0;tmp<(sizeof(Options)/sizeof(*Options));tmp++){
	TOption& to = Options[tmp];
		to.OnInit();
		if(to.m_OnDo || to.m_OnDont || to.m_OnWill || to.m_OnWont
		   || to.m_OnSB || to.m_OnInit)
			to.m_Supported=TRUE;
		if(!to.m_SBCluster)
			to.m_SBCluster=128;
	}
}

void ProcessDO(BYTE c)
{
TOption& to = Options[c];
	// Ignore this junk if we're already in desired mode of operation
	if(to.m_StateU==TOption::stateYes){
		connState = cstateNone;
		return;
	}
	TRACE1("He want us to DO %d\n",(WORD)c);
	if(!to.m_Supported){
		TRACE1("Unsupported option = %d\n",(WORD)c);
		// Option is not suported - send WONT and switch to stateNo unless
		// we've already denied this option.
		if(to.m_StateU == TOption::stateNone){
			VERIFY(ShowUnwill(c));
			to.m_StateU = TOption::stateNo;
		}
	}else{
		// Okay, if we do -  we do and tell him so, unless we asked for it.
		// Otherwise - Tell him we're not about to.
		if(to.OnDo()){
			if(to.m_StateU!=TOption::stateWantYes)
				VERIFY(ShowWill(c));
			to.m_StateU = TOption::stateYes;
		}else{
			if(to.m_StateU!=TOption::stateWantNo)
				VERIFY(ShowUnwill(c));
			to.m_StateU = TOption::stateNo;
		}
	}
	connState = cstateNone;
}

void ProcessWILL(BYTE c)
{
TOption& to = Options[c];
	// Ignore this junk if we consider him already in this mode
	if(to.m_StateH==TOption::stateYes){
		connState = cstateNone;
		return;
	}
	TRACE1("He WILL %d\n",(WORD)c);
	if(!to.m_Supported){
		TRACE1("Unsupported option = %d\n",(WORD)c);
		// Since we don't expect remote end to use this option - beg him
		// not to go for it unless we think we already did.
		if(to.m_StateH == TOption::stateNone){
			VERIFY(BegDont(c));
			to.m_StateH = TOption::stateNo;
		}
	}else{
		if(to.OnWill()){
			// We've accepted this - tell him to go ahead if we thought he's
			// in opposite state and adjust our vision/
			if(to.m_StateH!=TOption::stateWantYes)
				VERIFY(BegDo(c));
			to.m_StateH = TOption::stateYes;
		}else{
			if(to.m_StateH!=TOption::stateWantNo)
				VERIFY(BegDont(c));
			to.m_StateH = TOption::stateNo;
		}
	}
	connState = cstateNone;
}

void ProcessWONT(BYTE c)
{
TOption& to = Options[c];
	// Ignore this junk if we consider him already in this mode
	if(to.m_StateH==TOption::stateNo){
		connState = cstateNone;
		return;
	}
	TRACE1("He WONT %d\n",(WORD)c);
	if(!to.m_Supported){
		TRACE1("Unsupported option = %d\n",(WORD)c);
		// Since we don't expect remote end to use this option - beg him
		// not to go for it unless we think we already did.
		if(to.m_StateH == TOption::stateNone){
			VERIFY(BegDont(c));
			to.m_StateH = TOption::stateNo;
		}
	}else{
		if(to.OnWont()){
			// We've accepted this - tell him to go ahead if we thought he's
			// in opposite state and adjust our vision/
			if(to.m_StateH!=TOption::stateWantNo)
				VERIFY(BegDont(c));
			to.m_StateH = TOption::stateNo;
		}else{
			if(to.m_StateH!=TOption::stateWantYes)
				VERIFY(BegDo(c));
			to.m_StateH = TOption::stateYes;
		}
	}
	connState = cstateNone;
}

void ProcessDONT(BYTE c)
{
TOption& to = Options[c];
	// Ignore this junk if we consider him already in this mode
	if(to.m_StateU==TOption::stateNo){
		connState = cstateNone;
		return;
	}
	TRACE1("He want us to DONT %d\n",(WORD)c);
	if(!to.m_Supported){
		TRACE1("DONT for unsupported option = %d\n",(WORD)c);
		// Since we don't expect remote end to use this option - beg him
		// not to go for it unless we think we already did.
		if(to.m_StateU == TOption::stateNone){
			VERIFY(ShowUnwill(c));
			to.m_StateU = TOption::stateNo;
		}
	}else{
		if(to.OnDont()){
			if(to.m_StateU!=TOption::stateWantNo)
				VERIFY(ShowUnwill(c));
			to.m_StateU = TOption::stateNo;
		}else{
			if(to.m_StateU!=TOption::stateWantYes)
				VERIFY(ShowWill(c));
			to.m_StateU = TOption::stateYes;
		}
	}
	connState = cstateNone;
}

void ProcessSBData(BYTE c)
{
	switch(connState){
	case cstateSBData:
		if(c==tnIAC){
			connState=cstateSBDataIAC;
			return;
		}
		break;
	case cstateSBDataIAC:
		if(c==tnSE){
			// Just let option handler process the data
		TOption& to = Options[negOption];
			to.OnSB(to.m_SB,to.m_StoredSB);
			to.CleanSB();
			connState = cstateNone;
			return;
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
TOption& to = Options[negOption];
	to.StoreSByte(c);
}

BOOL AskWill(BYTE o)
{
TOption& to = Options[o];
	if(to.m_StateU==TOption::stateYes || to.m_StateU==TOption::stateWantYes)
		return TRUE;
#ifdef _DEBUG
	if(to.m_StateU==TOption::stateWantNo)
		TRACE1("NEED TO QUEUE %d\n",(WORD)o);
#endif
	VERIFY(ShowWill(o));
	to.m_StateU=TOption::stateWantYes;
	return TRUE;
}

BOOL AskUnwill(BYTE o)
{
TOption& to = Options[o];
	if(to.m_StateU==TOption::stateNo || to.m_StateU==TOption::stateWantNo)
		return TRUE;
#ifdef _DEBUG
	if(to.m_StateU==TOption::stateWantNo)
		TRACE1("NEED TO QUEUE %d\n",(WORD)o);
#endif
	VERIFY(ShowUnwill(o));
	to.m_StateU=TOption::stateWantNo;
	return TRUE;
}

BOOL AskDo(BYTE o)
{
TOption& to = Options[o];
	if(to.m_StateH==TOption::stateYes || to.m_StateH==TOption::stateWantYes)
		return TRUE;
#ifdef _DEBUG
	if(to.m_StateH==TOption::stateWantNo)
		TRACE1("NEED TO QUEUE %d\n",(WORD)o);
#endif
	VERIFY(BegDo(o));
	to.m_StateH=TOption::stateWantYes;
	return TRUE;
}

BOOL AskDont(BYTE o)
{
TOption& to = Options[o];
	if(to.m_StateH==TOption::stateNo || to.m_StateH==TOption::stateWantNo)
		return TRUE;
#ifdef _DEBUG
	if(to.m_StateH==TOption::stateWantYes)
		TRACE1("NEED TO QUEUE %d\n",(WORD)o);
#endif
	VERIFY(BegDont(o));
	to.m_StateH=TOption::stateWantNo;
	return TRUE;
}
