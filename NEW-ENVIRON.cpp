
enum	_NESB {
	nesbSend = 1,
	nesbIs = 0,
	nesbInfo = 2,

	nesbVar = 0,
	nesbValue = 1,
	nesbESC = 2,
	nesbUserVar = 3,

	nesbNothing = 256
};

struct	_envariable	{
	BYTE	type;
	char*	name;
	char*	value;
	_envariable() : type(0), name(NULL), value(NULL) {}
	~_envariable() { ItIsNot(); }
	void ItIsNot() {
		if(name) delete name;
		if(value) delete value;
	}
	BOOL ItIs(BYTE t,LPCTSTR n,LPCTSTR v) {
		ItIsNot();
		name = new CHAR[strlen(n)+1]; strcpy(name,n);
		value = new CHAR[strlen(v)+1]; strcpy(value,v);
		type=t;
		return TRUE;
	}
	int	TransmitSize() {
	char* v = value;
		for(int rv=0;*v;v++,rv++){
			switch(*v){
			case nesbVar:
			case nesbValue:
			case nesbUserVar:
			case nesbESC:
				rv++;
				break;
			}
		}
		for(v=name;*v;v++,rv++){
			switch(*v){
			case nesbVar:
			case nesbValue:
			case nesbUserVar:
			case nesbESC:
				rv++;
				break;
			}
		}
		return rv+2;	// VAR/VAL
	}
	int Paste(CHAR* data) {
	int rv=0;
		ASSERT(type==nesbVar || type==nesbUserVar);
		data[rv++]=type;
		for(CHAR* v=name;*v;v++){
			switch(*v){
			case nesbVar:
			case nesbValue:
			case nesbUserVar:
			case nesbESC:
				data[rv++]=nesbESC;
			default:
				data[rv++]=*v;
			}
		}
		data[rv++]=nesbValue;
		for(v=value;*v;v++){
			switch(*v){
			case nesbVar:
			case nesbValue:
			case nesbUserVar:
			case nesbESC:
				data[rv++]=nesbESC;
			default:
				data[rv++]=*v;
			}
		}
		return rv;
	}
	BOOL IsIt(CHAR* n,int l) {
	BOOL bESC = FALSE;
	CHAR* nPtr = name;
		for(int tmp=0;tmp<l;tmp++){
			if(!*nPtr)
				return FALSE;
			if(bESC)
				bESC=FALSE;
			else if(n[tmp]==nesbESC)
				bESC=TRUE;
			else if(n[tmp]!=*(nPtr++))
				return FALSE;
		}
		return (*nPtr)?FALSE:TRUE;
	}
}	*Envars = NULL;
int	nEnvars = 0;
int nEnvarsAllocated = 0;

_envariable* FindEnvar(BYTE type,CHAR* n,int l)
{
	ASSERT(Envars && nEnvars);
	for(int tmp=0;tmp<nEnvars;tmp++){
		if(Envars[tmp].type!=type)
			continue;
		if(Envars[tmp].IsIt(n,l))
			return &Envars[tmp];
	}
	return NULL;
}

#define	ENVGROW	8

BOOL AddEnvar(BYTE type,LPTSTR name,LPTSTR value)
{
	if(Envars && nEnvars){
		if(FindEnvar(type,name,strlen(name)))	// ** Maybe replace
			return FALSE;
	}
	if(nEnvars>=nEnvarsAllocated){
		ASSERT(nEnvars==nEnvarsAllocated);
	_envariable* ne = new _envariable[nEnvarsAllocated+ENVGROW];
		ASSERT(ne);
		if(nEnvarsAllocated){
			memmove(ne,Envars,sizeof(_envariable)*nEnvarsAllocated);
			delete Envars;
		}
		Envars=ne;
		nEnvarsAllocated+=ENVGROW;
	}
	return Envars[nEnvars++].ItIs(type,name,value);
}
BOOL CleanEnvars()
{
	if(Envars){
		ASSERT(nEnvarsAllocated);
		delete[] Envars;
	}
	return TRUE;
}

BOOL newenvironOnDont()
{
	TRACE0("DONT NEW-ENVIRON\n");
	return TRUE;
}
BOOL newenvironOnDo()
{
	TRACE0("DO NEW-ENVIRON\n");
	return (Envars && nEnvars)?TRUE:FALSE;
}
BOOL newenvironOnSB(LPBYTE data,UINT size)
{
	if(!size)
		return FALSE;
	if(data[0]!=nesbSend)
		return FALSE;
	if(!(Envars && nEnvars)){
	BYTE no = nesbIs;
		return SubNegotiate(toNewEnviron,&no,1);
	}
UINT totalSize = 1;	// IS byte
	if(size>1 ){
	int type = 0;
	int name = 0;
	BOOL bESC = FALSE;
		for(UINT tmp=1;tmp<=size;tmp++){
			switch((tmp==size)?nesbNothing:(UINT)data[tmp]){
			case nesbESC:
				bESC=TRUE;
				break;
			case nesbVar:
			case nesbUserVar:
			case nesbNothing:
				if(!bESC){
					if(type){
						if(name){
						_envariable* ev = FindEnvar(data[type],(CHAR*)&data[name],tmp-name);
							if(ev){
								totalSize+=ev->TransmitSize();
							}
						}else{
							for(int tmp=0;tmp<nEnvars;tmp++){
								if(Envars[tmp].type==data[type]){
									totalSize+=Envars[tmp].TransmitSize();
								}
							}
						}
						type=tmp;
						name=0;
					}else{
						type=tmp;
						ASSERT(!name);
					}
					break;
				}
				// Fall through if ESCaped
			default:
				bESC=FALSE;
				if(!name)
					name=tmp;
				break;
			}
		}
		TRACE1("Total Size: %u\n",totalSize);
	CHAR* d = new CHAR[totalSize];
	UINT ptr = 0;
		d[ptr++]=nesbIs;
		for(tmp=1;tmp<=size;tmp++){
			switch((tmp==size)?nesbNothing:(UINT)data[tmp]){
			case nesbESC:
				bESC=TRUE;
				break;
			case nesbVar:
			case nesbUserVar:
			case nesbNothing:
				if(!bESC){
					if(type){
						if(name){
						_envariable* ev = FindEnvar(data[type],(CHAR*)&data[name],tmp-name);
							if(ev){
								ptr+=ev->Paste(&d[ptr]);
							}
						}else{
							for(int tmp=0;tmp<nEnvars;tmp++){
								if(Envars[tmp].type==data[type]){
									ptr+=Envars[tmp].Paste(&d[ptr]);
								}
							}
						}
						ASSERT(ptr<=totalSize);
						type=tmp;
						name=0;
					}else{
						type=tmp;
						ASSERT(!name);
					}
					break;
				}
				// Fall through if ESCaped
			default:
				bESC=FALSE;
				if(!name)
					name=tmp;
				break;
			}
		}
		ASSERT(ptr==totalSize);
		VERIFY(SubNegotiate(toNewEnviron,(BYTE*)d,totalSize));
		delete d;
	}else{
		for(int tmp=0;tmp<nEnvars;tmp++){
			totalSize+=Envars[tmp].TransmitSize();
		}
		TRACE1("Total Size: %u\n",totalSize);
	CHAR* d = new CHAR[totalSize];
	UINT ptr = 0;
		d[ptr++]=nesbIs;
		for(tmp=0;tmp<nEnvars;tmp++){
			ptr+=Envars[tmp].Paste(&d[ptr]);
			ASSERT(ptr<=totalSize);
		}
		ASSERT(ptr==totalSize);
		VERIFY(SubNegotiate(toNewEnviron,(BYTE*)d,totalSize));
		delete d;
	}
	return TRUE;
}