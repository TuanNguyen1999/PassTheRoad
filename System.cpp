//System.cpp -- Handling signal, change console window
//

#include "Declar.h"
#include <stdexcept>

int IsPositive(int n) {
	if (n > 0) return 1;
	else if (n < 0) return -1;
	else return 0;
}
void FixConsoleWindow(SHORT width, SHORT height)
{
	HWND cw = GetConsoleWindow();
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (cw == NULL) {
		throw std::runtime_error("Unable To Get Console Handle");
	}
	// console window no longer has maximize button (WS_MAXIMIZEBOX)
	// and cant change size (WS_THICKFRAME)
	LONG style = GetWindowLong(cw, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	if (!SetWindowLong(cw, GWL_STYLE, style))
	{
		throw std::runtime_error("Unable To Set Window Long");

	}
	// change window console size
	// Note: screen buffer size MUST NOT less than the window size
	CONSOLE_SCREEN_BUFFER_INFO consInfo;
	if (!GetConsoleScreenBufferInfo(console, &consInfo))
	{
		throw std::runtime_error("Unable To Get Console Screen Buffer");

	}
	COORD winSize;
	winSize.X = consInfo.srWindow.Right - consInfo.srWindow.Left + 1;
	winSize.Y = consInfo.srWindow.Bottom - consInfo.srWindow.Top + 1;
	// if the given width is less than the window's width
	// or the given height is less than the window's height\
	// set window size to the lower ones
	if (width < winSize.X || height < winSize.Y)
	{
		SMALL_RECT fix = {
			0,													//top
			0,													//left
			(width < winSize.X) ? width - 1 : winSize.X - 1,	//right
			(height < winSize.Y) ? height - 1 : winSize.Y - 1	//bottom
		};
		if (!SetConsoleWindowInfo(console,TRUE, &fix))
		{
			throw std::runtime_error("Unable To Set Console Window");
		}
	}
	// Now the window size will always be less than or equal to given size
	// set screen buffer size
	COORD bs = { width,height };
	if (!SetConsoleScreenBufferSize(console, bs))
	{
		throw std::runtime_error("Unable To Get Console Screen Buffer");

	}
	// now change window size to the given size
	SMALL_RECT ws = { 0,0,width - 1,height - 1}; // top,left,right,bottom
	if (!SetConsoleWindowInfo(console, TRUE, &ws))
	{
		throw std::runtime_error("Unable To Set Console Window");
	}
	return;
}
void ChangeConsoleTitle(const wchar_t* title)
{
	SetConsoleTitleW(title);
	return;
}
void ChangeConsoleFont(const wchar_t* font_name,SHORT width, SHORT height)
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = width;                   // Width of each character
	cfi.dwFontSize.Y = height;  
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_DONTCARE;
	std::wcscpy(cfi.FaceName, font_name);
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &cfi);
	return;
}
void m_SetCursor(bool visible, DWORD size)
{
	HANDLE cw = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ci;
	if (!GetConsoleCursorInfo(cw, &ci))
	{
		throw std::runtime_error("Unable To Get Cursor Info");
	}
	ci.bVisible = visible;
	ci.dwSize = (size == 0) ? ci.dwSize : size;
	if (!SetConsoleCursorInfo(cw, &ci))
	{
		throw std::runtime_error("Unable To Set Cursor Info");
	}
	return;
}
void GoTo(COORD coord)
{
	HANDLE cw = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!SetConsoleCursorPosition(cw, coord))
		throw std::runtime_error("Cannot Set Cursor Coordinate");
	return;
}
void GoTo(short x, short y)
{
	COORD coord = { x,y };
	HANDLE cw = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!SetConsoleCursorPosition(cw, coord))
		throw std::runtime_error("Cannot Set Cursor Coordinate");
	return;
}

//bool StartNewLevel(int level, COORD playerCoord);
Data::Data(int speed, int en_max, int sp_count, int stop_chance)
{
	EnemySpeed = speed;
	MaxEnemy = en_max;
	SpawnerCount = sp_count;
	StopChance = stop_chance;
}
Spawner::Spawner(short x, short y,int dir,bool st)
{
	Location = { x,y };
	direction = dir;
	WaitTime = 0;
	Stop = st;
}
Spawner* CreateSpawner(short x, short y, int dir,bool st)
{
	Spawner* pThis = new Spawner(x, y, dir, st);
	return pThis;
}
Enemy* SpawnEnemy(Spawner* sp, Data* dt)
{
	Enemy* pThis = new Enemy(
		(sp->direction == 1) ? m_PLAYFIELD_LEFT + 1 : m_PLAYFIELD_RIGHT - 1,
		sp->Location.Y,
		sp->direction,
		dt->EnemySpeed,
		rand() % (m_MAX_LENGTH -1) + 2, // enemy's length is >= 2
		sp,
		false
	);
	sp->WaitTime = int(std::ceil(float(pThis->length) / float(pThis->walksp)));
	return pThis;
}
void General(
	COORD WindowSize,
	wchar_t* window_title,
	/*SMALL_RECT PlayFieldCoord,*/
	COORD FontSize,
	wchar_t* font_name,
	bool CursorVisible,
	DWORD CursorSize
)
{
	CurrBuffer = CreateBuffer(WindowSize.X, WindowSize.Y);
	NextBuffer = CreateBuffer(WindowSize.X, WindowSize.Y);
	FixConsoleWindow(WindowSize.X, WindowSize.Y);
	ChangeConsoleTitle(window_title);
	ChangeConsoleFont(font_name); // default if font name in invalid
	m_SetCursor(CursorVisible, CursorSize);
	return;
}
void GetData(int whatLv,Data* dt)
{
	*dt = { 
		EnemySpeed[whatLv],
		MaxEnemy[whatLv],
		SpawnerCount[whatLv],
		StopChance[whatLv]
	};
	return;
}
void GameSetting(Data* dt, std::vector<Spawner*>* spArr, Player* pl)
{
	int margin = int((double(m_PLAYFIELD_BOTTOM - m_PLAYFIELD_TOP - 4)) / double(dt->SpawnerCount) + 0.5);
	if (margin * (dt->SpawnerCount - 1) > m_PLAYFIELD_BOTTOM - m_PLAYFIELD_TOP - 4)
	{
		throw std::runtime_error("Not enough space to create spawner");
	}
	for (int i = 0; i < dt->SpawnerCount; i++)
	{
		spArr->push_back(CreateSpawner(m_PLAYFIELD_LEFT + 1, m_PLAYFIELD_TOP + 2 + i * margin, (rand() % 2 == 0) ? 1 : -1));
	}
	*pl = { (m_PLAYFIELD_RIGHT - m_PLAYFIELD_LEFT) / 2, m_PLAYFIELD_BOTTOM - 1, 1 };
}
void ResetData(std::vector<Spawner*>* spArr, std::vector<Enemy*>* enArr, std::vector<DeadPlayer*>* DeadArr)
{
	for (auto i = spArr->begin(); i != spArr->end();/*i++*/)
	{
		delete *i;
		i = spArr->erase(i);
	}
	for (auto it = enArr->begin(); it != enArr->end();)
	{
		delete *it;
		it = enArr->erase(it);
	}
	for (auto it = DeadArr->begin(); it != DeadArr->end();)
	{
		delete *it;
		it = DeadArr->erase(it);
	}
	return;
}
void CleanData(std::vector<Spawner*>* spArr, std::vector<Enemy*>* enArr, std::vector<DeadPlayer*>* DeadArr)
{
	ResetData(spArr, enArr, DeadArr);
	DestroyBuffer(
		CurrBuffer, m_WINDOW_SIZE_WIDTH,
		m_WINDOW_SIZE_HEIGHT
	);
	DestroyBuffer(
		NextBuffer, m_WINDOW_SIZE_WIDTH,
		m_WINDOW_SIZE_HEIGHT
	);
	return;
}

