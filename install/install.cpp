#include "resource.h"
#include "../shared-code/install.h"

#define	KINAME	"KINSole 1.1"
#define	SKINAME	"KINSole"

BOOL Install(void)
{
STRING tPath = strFETCH_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group",SKINAME "Path");
STRING kPath = strFETCH_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group","KINPath");
LPCSTR qPath = ((LPCSTR)tPath)?(LPCSTR)tPath:(((LPCSTR)kPath)?(LPSTR)kPath:"C:\\Program Files\\Klever\\Nothings");
STRING path = REQUESTPATH("  " KINAME,"Note: KINSole program file will be installed into your windows directory so that you can rely on it's presence in your search path\nEnter destination path:",qPath);
	if(!path)
		return NULL;
STRING winDir(_MAX_PATH);

	GetWindowsDirectory(winDir,_MAX_PATH);
	INSTALLFILE("KINSole.ex_",winDir,"KINSole.exe");
	MAKE_PATH(path);
STRING shortPath = GET_SHORT_PATH(path);
	if(!shortPath){
		MessageBox(NULL,"Failed to install " KINAME " in specified directory",NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	if(!(
		INSTALLFILE("KINSole.hl_",path,"KINSole.hlp") && 
		INSTALLFILE("KINSole.cn_",path,"KINSole.cnt")
	)){
		MessageBox(NULL,"Failed to install " KINAME " in specified directory",NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	ADDMENU("Klever Group","KINSole Help",path,SKINAME ".hlp");
	strSET_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group", SKINAME "Path",path);
	strSET_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group","KINPath",path);
FILE* inf=CREATE_INF_FILE(path, SKINAME ".INF");
	if(!inf){
		MessageBox(NULL,"Failed to install " KINAME,NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	INF_FILE_HEADER(inf);
	INF_FILE_SECTION(inf,"Uninstall");
	fprintf(inf,"AddReg=kFiles\nDelReg=kReg\nUpdateInis=kMenu\n");

	INF_FILE_SECTION(inf,"kFiles");
	INF_REMOVE_ROOT(inf,SKINAME "Files",shortPath);
	INF_REMOVE_HELP_FILE(inf,SKINAME "Files",SKINAME);
	INF_REMOVE_FILE(inf,SKINAME "Files",SKINAME ".inf");

	INF_REMOVE_ROOT(inf,SKINAME "WFiles",winDir);
	INF_REMOVE_FILE(inf,SKINAME "WFiles",SKINAME ".exe");

	INF_FILE_SECTION(inf,"kReg");
	INF_UNINSTALL_REG(inf,SKINAME);

	INF_FILE_SECTION(inf,"kMenu");
	INF_MENU_GROUP(inf,1,"Klever Group");
	INF_MENU_ITEM(inf,1,"KINSole Help");
	fclose(inf);

	REG_UNINSTALL_COMMAND(SKINAME,"Klever " KINAME,shortPath,SKINAME ".INF","Uninstall");

	MessageBox(NULL,KINAME " installed successfully, you may now run it from command line, read documentation in 'Programs/Klever Group' menu or simply remove it using Control Panel Add/Remove Programs applet."," Rejoice!",MB_ICONINFORMATION|MB_OK);

	return TRUE;
}
