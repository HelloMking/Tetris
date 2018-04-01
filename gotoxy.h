void clrscr(void);
void clreol(void);
void clreoscr(void);
void gotoxy(int ,int );

// --- Funktionsimplementierung

/*********************************************************************************/
void clrscr(void)   //clearscreen: gesamten Bildschirm leeren
{
     CONSOLE_SCREEN_BUFFER_INFO     csbiInfo;                             //variablendklaration
     COORD     Home = {0,0};
     DWORD     dummy;

     GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbiInfo);

     FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE),' ',csbiInfo.dwSize.X * csbiInfo.dwSize.Y,Home,&dummy);     //bis cursorposition leerzeichen ausgeben
     csbiInfo.dwCursorPosition.X = 0;                                     //cursorposition X koordinate festlegen
     csbiInfo.dwCursorPosition.Y = 0;                                     //cursorposition Y koordinate festlegen
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),csbiInfo.dwCursorPosition);     //den cursor an die festgelegte koordinate setzen
}


/*********************************************************************************/

void clreol(void)   //clear end of line: den rest der Zeile nach dem cursor l?schen
{
     CONSOLE_SCREEN_BUFFER_INFO         csbiInfo;                             //variablendklaration
     COORD     Home,pos;
     DWORD     dummy;

     GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbiInfo);

     Home = csbiInfo.dwCursorPosition;
     pos.X = 80 - csbiInfo.dwCursorPosition.X;

     FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE),' ',pos.X,Home,&dummy);
}

/*********************************************************************************/

void clreoscr(void)   //clear end of screen: alles nach dem cursor l?schen
{
     CONSOLE_SCREEN_BUFFER_INFO         csbiInfo;                             //variablendklaration
     COORD     Home;
     DWORD     dummy;

     GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbiInfo);

     Home=csbiInfo.dwCursorPosition;
     FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE),' ',csbiInfo.dwSize.X * csbiInfo.dwSize.Y,Home,&dummy);
}

/*********************************************************************************/

void gotoxy(int x,int y)   //cursor an gewuenschte position auf dem bildschirm setzen
{
     CONSOLE_SCREEN_BUFFER_INFO         csbiInfo;                             //variablendklaration

     GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbiInfo);

     csbiInfo.dwCursorPosition.X = x;                                     //cursorposition X koordinate festlegen
     csbiInfo.dwCursorPosition.Y = y;                                     //cursorposition Y koordinate festlegen
     SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),csbiInfo.dwCursorPosition);     //den cursor an die festgelegte koordinate setzen
}


int setcolor(WORD Forg, WORD Back)
{
	Forg=(Forg&0x0f)+((Back<<4)&0xf0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Forg);
	return Forg;
}