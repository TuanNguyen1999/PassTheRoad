﻿//main.cpp -- executing file
//
#include "Declar.h"

wchar_t **CurrBuffer;
wchar_t **NextBuffer;
short gl_SystemSignal = m_SIGNAL_START_MENU;
int gl_TotalEnemy = 0;
int gl_CurrentLvl = 0;
bool gl_StopReceiveSignal = false;
std::mutex OtherMtx;
std::mutex BufferMtx;
const int EnemySpeed[m_TOTAL_LVL]{ 1,2,3};
const int MaxEnemy[m_TOTAL_LVL]{ 43,70,100 };
const int SpawnerCount[m_TOTAL_LVL]{ 15,15,17 };
const int StopChance[m_TOTAL_LVL]{ 110,125,141 };
int main() {
	_setmode(_fileno(stdout), _O_WTEXT);
	srand(time(NULL));
	using std::cout;
	using std::cin;
	using std::endl;
	using std::vector;

	// General Setting
	COORD WindowSize = { m_WINDOW_SIZE_WIDTH,m_WINDOW_SIZE_HEIGHT };
	//SMALL_RECT PlayFieldCOORD = { m_PLAYFIELD_LEFT,m_PLAYFIELD_TOP,m_PLAYFIELD_RIGHT,m_PLAYFIELD_BOTTOM };
	wchar_t title[] = L"PASSING THE VIETNAMESE ROAD";
	wchar_t FontName[] = L"Consolas";
	COORD FontSize = { 0,16 };
	bool Visible = false;
	DWORD CursorSize = 0;
	General(
		WindowSize,
		title,
		/*PlayFieldCOORD,*/
		FontSize,
		FontName,
		Visible,
		CursorSize
	);

	// Game Setting
	vector<Spawner*> SpArr;
	vector<Enemy*> EnArr;
	vector<DeadPlayer*> DeadArr;
	Player pl;
	Data LvInfo;
	GetData(gl_CurrentLvl, &LvInfo);
	GameSetting(&LvInfo, &SpArr, &pl);
	//Subthread
	std::thread ProcessThread;
	// Game-Play
	while (!(gl_SystemSignal & m_SIGNAL_EXIT))
	{
		// m_SIGNAL_GAME_START is currently off
		if (gl_SystemSignal & m_SIGNAL_START_MENU)
		{
			// this function will turnoff m_SIGNAL_MENU
			// and turn on m_SIGNAL_GAME_START if user want to play
			DrawStartScreen();
		}
		if (gl_SystemSignal & m_SIGNAL_GAME_START)
		{
			// Initialize data
			gl_TotalEnemy = 0;
			gl_CurrentLvl = 0;
			ResetData(&SpArr, &EnArr, &DeadArr);
			GetData(gl_CurrentLvl, &LvInfo);
			GameSetting(&LvInfo, &SpArr, &pl);
			// start SubThread
			ProcessThread = std::thread(thr_Process, &pl, &DeadArr, &EnArr, &SpArr, &LvInfo);
		}
		while (gl_SystemSignal & m_SIGNAL_GAME_START)
		{
			ReceiveUserSignal();
			if (!(gl_SystemSignal & m_SIGNAL_PAUSE)) // check
			{
				OtherMtx.lock();
				ControlPlayer(&pl);
				OtherMtx.unlock();
			}
			if (gl_SystemSignal & m_SIGNAL_SAVE)
			{
				RequestUserInput();
				gl_SystemSignal &= ~(m_SIGNAL_SAVE) & ~(m_SIGNAL_PAUSE); //turn off
			}
			if (gl_SystemSignal & m_SIGNAL_LOAD)
			{
				if (RequestUserInput())
				{
					gl_SystemSignal |= m_SIGNAL_NEW_LEVEL;
				}
				gl_SystemSignal &= ~(m_SIGNAL_LOAD) & ~(m_SIGNAL_PAUSE); //turn off
			}
			if (gl_SystemSignal & m_SIGNAL_RESTART) // checked
			{
				// wait for Thread to finish
				ProcessThread.join();
				// reset signal and level
				gl_CurrentLvl = 0;
				gl_SystemSignal = m_SIGNAL_START_MENU;
			}
			if (gl_SystemSignal & m_SIGNAL_NEW_LEVEL) // checked
			{
				// wait for Thread to finish (for safely resetting data)
				gl_SystemSignal &= ~(m_SIGNAL_GAME_START);
				ProcessThread.join();
				if (gl_CurrentLvl >= m_TOTAL_LVL)
				{
					// turn off m_SIGNAL_GAME_START to exit gameplay
					gl_SystemSignal &= ~(m_SIGNAL_GAME_START);
					// turn on m_SIGNAL_MENU to return to menu screen
					gl_SystemSignal |= m_SIGNAL_END_MENU;
				}
				else
				{
					// initialize data for new level
					gl_TotalEnemy = 0;
					ResetData(&SpArr, &EnArr, &DeadArr);
					GetData(gl_CurrentLvl, &LvInfo);
					GameSetting(&LvInfo, &SpArr, &pl);
					// turn on m_SIGNAL_GAME_START again
					gl_SystemSignal |= m_SIGNAL_GAME_START;
					// start SubThread
					ProcessThread = std::thread(thr_Process, &pl, &DeadArr, &EnArr, &SpArr, &LvInfo);
				}
				// clear playfield
				ClearPlayField();
				// turn off signal
				gl_SystemSignal &= ~(m_SIGNAL_NEW_LEVEL);
			}
			if (gl_SystemSignal & m_SIGNAL_END_MENU)
			{
				DrawEndScreen();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(m_fElapseTime));
		}
	}
	if(ProcessThread.joinable())
		ProcessThread.join();
	CleanData(&SpArr, &EnArr, &DeadArr);
	return 0;
}
