//SubThread.cpp
//

#include "Declar.h"
void thr_Process(Player* pl, std::vector<DeadPlayer*>* DeadArr,std::vector<Enemy*>* enArr, std::vector<Spawner*>* spArr, Data* data)
{
	int SpawnChance = 10;
	int MaxStopTime = 123;
	int MinStopTime = 20;
	while (gl_SystemSignal & m_SIGNAL_GAME_START)
	{
		if (!(gl_SystemSignal & m_SIGNAL_PAUSE))
		{
			// Process Enemy and Spawner
			// choose an random spawner
			if (rand() % data->StopChance == 0)
			{
				Spawner* randSpawner = *(spArr->begin() + (rand() % spArr->size()));
				randSpawner->Stop = true;
				randSpawner->WaitTime = rand() % (MaxStopTime - MinStopTime) + MinStopTime;
			}
			// Spawning enemy
			for (auto itSpawner = spArr->begin(); itSpawner != spArr->end(); itSpawner++)
			{
				Spawner* ThisSpw = *itSpawner;
				if (ThisSpw->WaitTime <= 0) // ready to spawn
				{
					ThisSpw->Stop = false;
					if (rand() % SpawnChance == 0)
					{
						if (gl_TotalEnemy < data->MaxEnemy)
						{
							enArr->push_back(SpawnEnemy(*itSpawner, data));
							gl_TotalEnemy++;
						}
					}
				}
				else {
					ThisSpw->WaitTime--;
				}
			}
			// destroy if enemy is out of playfield
			OtherMtx.lock();
			for (auto itEnemy = enArr->begin(); itEnemy != enArr->end(); /*itEnemy++*/)
			{
				if ((*itEnemy)->ReadyToDestroy)
				{
					(*itEnemy)->SpawnFrom = NULL; // set to NULL to not delete the spawner
					delete *itEnemy;
					itEnemy = enArr->erase(itEnemy);
					gl_TotalEnemy--;
				}
				else
				{
					ControlEnemy(pl, *itEnemy);
					if (!pl->state)
					{
						// if player is dead, add dead player to DeadPlayer vector and create new player and 
						DeadArr->push_back(new DeadPlayer(pl->coord.X, pl->coord.Y));
						*pl = { rand() % (m_PLAYFIELD_RIGHT - m_PLAYFIELD_LEFT - 2) + m_PLAYFIELD_LEFT + 1, m_PLAYFIELD_BOTTOM - 1, 1 };
						std::wcout << '\a';
					}
					itEnemy++;
				}
			}
			OtherMtx.unlock();
		}
		// Print to screen
		OtherMtx.lock();
		for (const auto& dpl : *DeadArr)
		{
			DrawDeadPlayer(dpl);
		}
		DrawPlayer(pl);
		for (const auto& en : *enArr)
		{
			DrawEnemy(en);
		}
		DrawPlayField();
		DrawTextBox();
		DrawGameMessage();
		PrintToScreen();
		OtherMtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(m_fElapseTime));
	}
	return;
}