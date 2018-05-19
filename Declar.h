#ifndef DECLAR_H
#define DECLAR_H
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>
#include <vector>
#include <cstring>
#include <mutex>
#include <string>
#include <conio.h>
#include <fcntl.h>
#include <io.h>
#include <fstream>
// console window
#define m_WINDOW_SIZE_WIDTH 140
#define m_WINDOW_SIZE_HEIGHT 40

#define m_PLAYFIELD_LEFT 0
#define m_PLAYFIELD_RIGHT 79
#define m_PLAYFIELD_TOP  1
#define m_PLAYFIELD_BOTTOM 37

#define m_TEXTFIELD_LEFT 88
#define m_TEXTFIELD_RIGHT 138
#define m_TEXTFIELD_TOP 1
#define m_TEXTFIELD_BOTTOM 37
// global
#define m_TOTAL_LVL 3
#define m_fElapseTime 80 // miliseconds
#define m_MAX_LENGTH 20
// signal
#define m_SIGNAL_PAUSE (1 << 0)			// signal for pausing game, pasuing make other threads "stop"
#define m_SIGNAL_EXIT (1 << 1)			// signal for exiting game
#define m_SIGNAL_SAVE (1 << 2)			// signal for saving game
#define m_SIGNAL_LOAD (1 << 3)			// signal for loading game
#define m_SIGNAL_RESTART (1 << 4)		// signal for restarting game
#define m_SIGNAL_NEW_LEVEL (1 << 5)		// signal for new level
#define m_SIGNAL_GAME_START (1 << 6)	// signal for starting game
#define m_SIGNAL_END_MENU (1 << 7)		// signal for end screen
#define m_SIGNAL_START_MENU (1 << 8)	// signal for start screen
enum {
	UP = VK_UP, DOWN = VK_DOWN, LEFT = VK_LEFT, RIGHT = VK_RIGHT,
	GO_TO_LEFT = -1,
	GO_TO_RIGHT = 1,
	PAUSE = 0x0050,				// p
	EXIT = 0x0051,				// q
	RESTART = 0x0052,			// r
	LOAD = 0x004C,				// l
	SAVE = 0x0053,				// s
	WALL_CHAR = 0x0023,			// #
	PLAYER_ALIVE_CHAR = 0x0059,	// Y
	PLAYER_DEAD_CHAR = 0x0058,	// X
	ENEMY_LEFT_CHAR = 0x2190,	// ←
	ENEMY_RIGHT_CHAR= 0x2192	// →
};

extern wchar_t **CurrBuffer; //"virtual" screen buffer
extern wchar_t **NextBuffer; //"virtual" screen buffer
extern short gl_SystemSignal;
extern int gl_TotalEnemy;
extern int gl_CurrentLvl;
extern bool gl_StopReceiveSignal;
extern std::mutex OtherMtx;
extern std::mutex BufferMtx;
//Data per for each level
extern const int EnemySpeed[m_TOTAL_LVL];
extern const int MaxEnemy[m_TOTAL_LVL];
extern const int SpawnerCount[m_TOTAL_LVL];
extern const int StopChance[m_TOTAL_LVL];
//NOTE : to get acutal Coordinate of object A in console screen
// plus the buffer array's row with m_PLAYFIELD_LEFT and col with m_PLAYFIELD_TOP

struct Data // general info: enemy speed, maxium enemy...
{
	Data(int EnemySpeed = 0, int MaxEnemy = 0, int SpawnerCount = 0, int StopChance = 0);
	int EnemySpeed;
	int MaxEnemy; // Maxium enemy can appear in playfield
	int SpawnerCount;
	int StopChance; // change of cars in a road stop moving
};
struct Spawner //where Enemy is spawned
{
	Spawner(short x, short y, int direction,bool = false);
	COORD Location;
	int direction; // right or left
	int WaitTime; // time until able to spawn enemy
	bool Stop;
};
struct Player
{
	Player(short x = 0, short y = 0, int walksp = 0, bool = true );
	COORD coord; // NOTE: This is the Coordinate in the console screen
	bool state; //alive or dead
	int walksp, hsp, vsp;
};
struct DeadPlayer
{
	DeadPlayer(short x = 0, short y = 0);
	COORD coord;
};
struct Enemy
{
	Enemy(short x, short y, int dir, int walksp, int length, Spawner* _where, bool = false);
	COORD coord; // coordinate of car's head
	Spawner* SpawnFrom; // Spawned from what spawner, Note: Spawner is created with new operator
	int length;
	int walksp;
	int dir;
	int hsp;
	bool ReadyToDestroy;
};


//Functions that apply changes to console window
void FixConsoleWindow(SHORT width, SHORT height); // make console window open at maximize and disable maximize button
void ChangeConsoleTitle(const wchar_t*);
void m_SetCursor(bool visible, DWORD size = 0);
void ChangeConsoleFont(const wchar_t* = L"Consolas",
	SHORT width = 8, SHORT height = 16
	); 

// Controling
void ControlPlayer(Player*); // whether player moves left,right,up,down...
void ControlEnemy(Player*,Enemy*); // enemy moves left,right... or stop
Enemy* SpawnEnemy(Spawner*, Data*);
Spawner* CreateSpawner(short x, short y, int dir, bool = false);
int IsPositive(int);
void GoTo(COORD);
void GoTo(short x, short y);

//Handling events
void PlayerCollision(Player*); // check for collision between player and old player
void PlayerPassLevel(Player*);
void SaveToFile(const wchar_t*);
int LoadFromFile(const wchar_t*);

// Buffer
wchar_t** CreateBuffer(int x, int y);
void DestroyBuffer(wchar_t** buff, int x, int y);
void DrawPlayer(const Player*);
void DrawpPassPlayer(std::vector<Player*>*);
void DrawDeadPlayer(const DeadPlayer*);
void DrawEnemy(const Enemy*);
void DrawArea(SMALL_RECT, wchar_t, bool = true);
void DrawPlayField(); // Draw to buffer not the screen
void DrawTextBox();
void DrawMessage(const wchar_t*, COORD _where);
void DrawMessage(const std::wstring& msg, COORD _where);
void DrawGameMessage();
void DrawStartScreen();
void DrawEndScreen();
void PrintToScreen();

// clearing buffer
void ClearArea(SMALL_RECT); // actually clear the console screen and buffer
void ClearPlayField();
void ClearTextField();
void ClearScreen();

//threads
void thr_Process(Player*, std::vector<DeadPlayer*>*, std::vector<Enemy*>*, std::vector<Spawner*>*, Data*);

//Handing Signal
void ReceiveUserSignal();
void ClearUserSignal();
bool RequestUserInput();
void ClearKeyBoard();

// Pre-processing
void General(
	COORD WindowSize,
	wchar_t* window_title,
	/*SMALL_RECT PlayFieldCoord,*/
	COORD FontSize,
	wchar_t* font_name,
	bool CursorVisible,
	DWORD CursorSize
);
void GameSetting(Data*, std::vector<Spawner*>*, Player*);
void GetData(int whatLv, Data*);

// Clean
void CleanData(std::vector<Spawner*>*, std::vector<Enemy*>*,std::vector<DeadPlayer*>*);
void ResetData(std::vector<Spawner*>*, std::vector<Enemy*>*, std::vector<DeadPlayer*>*);


#endif // DECLAR_H

