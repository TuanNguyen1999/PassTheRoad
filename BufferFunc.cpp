//BufferFunc.cpp
//

#include "Declar.h"
void DrawPlayer(const Player* pThis)
{
	COORD coord = pThis->coord;
	NextBuffer[coord.X][coord.Y] = wchar_t(PLAYER_ALIVE_CHAR);
	return;
}
void DrawDeadPlayer(const DeadPlayer* pThis)
{
	COORD coord = pThis->coord; 
	NextBuffer[coord.X][coord.Y] = wchar_t(PLAYER_DEAD_CHAR);
	return;
}
void DrawEnemy(const Enemy* pThis)
{
	COORD HeadCoord = pThis->coord;
	COORD BufferCoord;
	// This is the Coordinate in console screen
	//subtract it to playfield's top left corner coordinate to get buffer coordinate
	bool GoFrom = (pThis->dir == 1) ? true : false; // true if go from left, false if go from righ
	for (short i = 0; i < pThis->length; i++)
	{
		BufferCoord = {
			HeadCoord.X + (GoFrom ? -i : i),
			HeadCoord.Y
		};
		if (BufferCoord.X < m_PLAYFIELD_RIGHT && BufferCoord.X > m_PLAYFIELD_LEFT)
		{
			NextBuffer[BufferCoord.X][BufferCoord.Y] = (GoFrom)? ENEMY_RIGHT_CHAR : ENEMY_LEFT_CHAR;
		}
	}
	return;
}
void DrawArea(SMALL_RECT area,wchar_t ch, bool filled)
{
	if (area.Left < 0 || area.Top < 0 || area.Bottom < 0 || area.Right < 0)
		throw std::runtime_error("Invalid Coordinate");
	if (area.Right < area.Left || area.Bottom < area.Top)
		throw std::runtime_error("Invalid Coordinate");
	if (area.Right - area.Left + 1 > m_WINDOW_SIZE_WIDTH || area.Bottom < 0 - area.Right > m_WINDOW_SIZE_HEIGHT)
		throw std::runtime_error("Invalid Coordinate");
	if (filled)
	{
		for (int i = area.Left; i <= area.Right; i++)
			for (int k = area.Top; k <= area.Bottom; k++)
				NextBuffer[i][k] = ch;
	}
	else
	{
		for (int i = area.Left; i <= area.Right; i++)
		{
			NextBuffer[i][area.Top] = ch;
			NextBuffer[i][area.Bottom] = ch;
		}
		for (int i = area.Top; i <= area.Bottom; i++)
		{
			NextBuffer[area.Left][i] = ch;
			NextBuffer[area.Right][i] = ch;
		}
	}
	return;
}
void DrawPlayField()
{
	SMALL_RECT area = { m_PLAYFIELD_LEFT ,m_PLAYFIELD_TOP ,m_PLAYFIELD_RIGHT,m_PLAYFIELD_BOTTOM };
	wchar_t ch = wchar_t(WALL_CHAR);
	DrawArea(area, ch, false);
	return;
}
void DrawTextBox()
{
	SMALL_RECT area = { m_TEXTFIELD_LEFT ,m_TEXTFIELD_TOP ,m_TEXTFIELD_RIGHT,m_TEXTFIELD_BOTTOM };
	wchar_t ch = wchar_t(WALL_CHAR);
	DrawArea(area, ch, false);
	return;
}
void ClearArea(SMALL_RECT area)
{
	HANDLE cw = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD CurSor = { area.Left  ,area.Top };
	DWORD written;
	for (int i = area.Top; i <= area.Bottom; i++)
	{
		CurSor.Y = i;
		for (int k = area.Left; k <= area.Right; k++)
		{
			CurSor.X = k;
			NextBuffer[k][i] = L' ';
			CurrBuffer[k][i] = L' ';
			FillConsoleOutputCharacterW(cw, NextBuffer[k][i], DWORD(1), CurSor, &written);
		}
	}
	return;
}
void ClearScreen()
{
	SMALL_RECT area = { 0 ,0,m_WINDOW_SIZE_WIDTH - 1,m_WINDOW_SIZE_HEIGHT - 1 };
	ClearArea(area);
	return;
}
void ClearPlayField()
{
	SMALL_RECT area = { m_PLAYFIELD_LEFT ,m_PLAYFIELD_TOP,m_PLAYFIELD_RIGHT,m_PLAYFIELD_BOTTOM };
	ClearArea(area);
	return;
}
void ClearTextField()
{
	SMALL_RECT area = { m_TEXTFIELD_LEFT ,m_TEXTFIELD_TOP,m_TEXTFIELD_RIGHT,m_TEXTFIELD_BOTTOM };
	ClearArea(area);
	return;
}
void PrintToScreen()
{
	HANDLE cw = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD CurSor = { 0  ,0 };
	DWORD written;
	for (int i = 0; i < m_WINDOW_SIZE_HEIGHT; i++)
	{
		CurSor.Y = i;
		for (int k = 0; k < m_WINDOW_SIZE_WIDTH; k++)
		{
			CurSor.X = k;
			if (CurrBuffer[k][i] != NextBuffer[k][i])
			{
				FillConsoleOutputCharacterW(cw, NextBuffer[k][i], DWORD(1), CurSor, &written);
				CurrBuffer[k][i] = NextBuffer[k][i];
			}
		}
	}
	return;
}
void DrawMessage(const wchar_t* msg, COORD _where)
{
	COORD CurSor = { _where.X  ,_where.Y };
	int i = 0;
	while (msg[i])
	{
		if (msg[i] != L'\n')
		{
			NextBuffer[CurSor.X][CurSor.Y] = msg[i];
			CurSor.X++;
		}
		else
		{
			// newline
			CurSor.X = m_TEXTFIELD_LEFT + 2;
			CurSor.Y++;
		}
		if (CurSor.X >= m_TEXTFIELD_RIGHT-1)
		{
			CurSor.Y++;
			CurSor.X = m_TEXTFIELD_LEFT + 2;
		}
		i++;
	}
}
void DrawMessage(const std::wstring& msg, COORD _where)
{
	const wchar_t *cwstr = msg.c_str();
	DrawMessage(cwstr, _where);
	return;
}
void DrawGameMessage()
{
	wchar_t title[] = L"→→ PASSING THE VIETNAMESE ROAD ←←";
	wchar_t Maker[] = L"ĐƯỢC LÀM BỞI NGUYỄN MẠNH TUẤN";
	wchar_t MSSV[] = L"MSSV : 1712875";
	wchar_t command[] = 
		L"\u00B7 Use ←,↑,→,↓ to move\n"
		"\u00B7 Press L to load game from file\n"
		"\u00B7 Press S to save game\n"
		"\u00B7 Press P to pause/resume game\n"
		"\u00B7 Press R to restart game\n"
		"\u00B7 Press Q to quit game\n";
	COORD titleCOORD = { m_TEXTFIELD_LEFT + 10,m_TEXTFIELD_TOP + 3 };
	COORD MakerCOORD = { m_TEXTFIELD_LEFT + 12,m_TEXTFIELD_TOP + 4 };
	COORD MSSVCOORD = { m_TEXTFIELD_LEFT + 20,m_TEXTFIELD_TOP + 5 };
	COORD commandCOORD = { m_TEXTFIELD_LEFT + 2,m_TEXTFIELD_TOP + 7 };
	DrawMessage(title, titleCOORD);
	DrawMessage(Maker, MakerCOORD);
	DrawMessage(MSSV, MSSVCOORD);
	DrawMessage(command, commandCOORD);
	return;
}
void DrawStartScreen()
{
	using std::wcout;
	using std::wcin;
	using std::endl;
	ClearScreen();
	GoTo(0, 0);
	wcout << L"          ___   ___    ____ ____ ____ _  __ _____  ______ __ __ ____  ___   ____   ___    ___ \n";
	wcout << L"         / _ \ / _ |  / __// __//  _// |/ // ___/ /_  __// // // __/ / _ \ / __ \ / _ |  / _ \ \n";
	wcout << L"        / ___\/\/ __ | _\ \ _\ \ _/ / /    \/\/ (_ /   / /  / _  \/\/ _/  / , _\/\/ /_/ \/\/ __ | / \/\/ /\n";
	wcout << L"       /_/   /_/ |_|/___//___//___//_/|_/ \___/   /_/  /_//_//___/ /_/|_| \____//_/ |_|/____/ \n";
	wcout << endl;
	wcout << endl;
	wcout << endl;
	wcout << L"      #####  #######    #    ######  #######     #####     #    #     # #######    #     # ####### #     # \n";
	wcout << L"     #     #    #      # #   #     #    #       #     #   # #   ##   ## #          ##    # #     # #  #  # \n";
	wcout << L"     #          #     #   #  #     #    #       #        #   #  # # # # #          # #   # #     # #  #  # \n";
	wcout << L"      #####     #    #     # ######     #       #  #### #     # #  #  # #####      #  #  # #     # #  #  # \n";
	wcout << L"     #    #    ####### #   #      #       #     # ####### #     # #          #   # # #     # #  #  # \n";
	wcout << L"     #     #    #    #     # #    #     #       #     # #     # #     # #          #    ## #     # #  #  # \n";
	wcout << L"      #####     #    #     # #     #    #        #####  #     # #     # #######    #     # #######  ## ##  \n";
	wcout << endl;
	wcout << endl;
	wcout << endl;
	wcout << L"Enter your choice(y/n): ";
	wchar_t ch;
	ClearKeyBoard();
	wcin >> ch;
	if (ch == L'Y' || ch == L'y')
	{
		ClearScreen();
		gl_SystemSignal |= m_SIGNAL_GAME_START;
		gl_SystemSignal &= ~(m_SIGNAL_START_MENU);
	}
	else
	{
		gl_SystemSignal |= m_SIGNAL_EXIT;
	}
	wcin.clear();
	wcin.ignore(INT_MAX,L'\n');
	wcout.clear();
	ClearUserSignal();
	return;
}
void DrawEndScreen()
{
	using std::wcout;
	using std::wcin;
	using std::endl;
	ClearScreen();
	GoTo(0, 0);
	wcout << L"          ___   ___    ____ ____ ____ _  __ _____  ______ __ __ ____  ___   ____   ___    ___ \n";
	wcout << L"         / _ \ / _ |  / __// __//  _// |/ // ___/ /_  __// // // __/ / _ \ / __ \ / _ |  / _ \ \n";
	wcout << L"        / ___\/\/ __ | _\ \ _\ \ _/ / /    \/\/ (_ /   / /  / _  \/\/ _/  / , _\/\/ /_/ \/\/ __ | / \/\/ /\n";
	wcout << L"       /_/   /_/ |_|/___//___//___//_/|_/ \___/   /_/  /_//_//___/ /_/|_| \____//_/ |_|/____/ \n";
	wcout << endl;
	wcout << endl;
	wcout << endl;
	wcout << L" #####     #    #     # #######    ####### #     # ######  \n";
	wcout << L"#     #   # #   ##   ## #          #       ##    # #     # \n";
	wcout << L"#        #   #  # # # # #          #       # #   # #     # \n";
	wcout << L"#  #### #     # #  #  # #####      #####   #  #  # #     # \n";
	wcout << L"#     # ####### #     # #          #       #   # # #     # \n";
	wcout << L"#     # #     # #     # #          #       #    ## #     # \n";
	wcout << L" #####  #     # #     # #######    ####### #     # ######  \n";
	wcout << endl;
	wcout << endl;
	wcout << endl;
	wcout << L"WOW YOU ARE SUCH A GOOD NGƯỜI QUA ĐƯỜNG\n"
		"Continue Playing?\n"
		"Enter your choice(y/n): ";
	wchar_t ch;
	ClearKeyBoard();
	wcin >> ch;
	if (ch == L'Y' || ch == L'y')
	{
		ClearScreen();
		gl_SystemSignal |= m_SIGNAL_START_MENU;
		gl_SystemSignal &= ~(m_SIGNAL_END_MENU);
	}
	else
	{
		gl_SystemSignal |= m_SIGNAL_EXIT;
	}
	gl_SystemSignal &= ~(m_SIGNAL_GAME_START);
	wcin.clear();
	wcin.ignore(INT_MAX, L'\n');
	wcout.clear();
	ClearUserSignal();
	return;
}
wchar_t** CreateBuffer(int x, int y)
{
	if (x <= 0 || y <= 0)
	{
		throw std::runtime_error("Size must be > 0");
	}
	wchar_t** temp = new wchar_t*[x];
	for (int i = 0; i < x; i++)
	{
		temp[i] = new wchar_t[y];
		for (int k = 0; k < y; k++)
		{
			temp[i][k] = ' ';
		}
	}
	return temp;
}
void DestroyBuffer(wchar_t** buff, int x, int y)
{
	if (x <= 0 || y <= 0)
	{
		throw std::runtime_error("Size must be > 0");
	}
	for (int i = 0; i < x; i++)
		delete[] buff[i];
	delete[] buff;
}
void ClearKeyBoard()
{
	while (_kbhit()) _getch();
	return;
}