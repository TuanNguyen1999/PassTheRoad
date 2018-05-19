//Player_Enemy.cpp -- function for player and enemy structure
//
#include "Declar.h"
// Player struct
Player::Player(short x, short y, int speed,bool st)
{
	coord = { x,y };
	state = st;
	walksp = speed;
	hsp = vsp = 0;
}
// Enemy struct
Enemy::Enemy(short x, short y, int oriential, int speed,int len, Spawner* _where, bool destroy)
{
	hsp = 0;
	coord = { x,y };
	dir = oriential;
	walksp = speed;
	length = len;
	ReadyToDestroy = destroy;
	SpawnFrom = _where;
}
DeadPlayer::DeadPlayer(short x, short y)
{
	coord.X = x;
	coord.Y = y;
}
void ControlPlayer(Player* pThis)
{
	if (pThis->state && !gl_StopReceiveSignal)
	{
		pThis->hsp = 0;
		pThis->vsp = 0;
		if (GetAsyncKeyState(LEFT))
		{
			pThis->hsp = -1 * pThis->walksp;
		}
		else if (GetAsyncKeyState(RIGHT))
		{
			pThis->hsp = 1 * pThis->walksp;
		}
		if (GetAsyncKeyState(UP))
		{
			pThis->vsp = -1 * pThis->walksp;
		}
		else if (GetAsyncKeyState(DOWN))
		{
			pThis->vsp = 1 * pThis->walksp;
		}
		//update player pos
		COORD NowCoord = pThis->coord;
		// delete player previous coord in buffer
		NextBuffer[NowCoord.X][NowCoord.Y] = L' ';
		int NextCoord_X = NowCoord.X + pThis->hsp;
		int NextCoord_Y = NowCoord.Y + pThis->vsp;

		if (NextCoord_X >= m_PLAYFIELD_RIGHT || NextCoord_X <= m_PLAYFIELD_LEFT)
		{
			NextCoord_X = IsPositive(pThis->hsp) >= 0 ? m_PLAYFIELD_RIGHT - 1 : m_PLAYFIELD_LEFT + 1;
			pThis->hsp = 0;
		}
		NowCoord.X = NextCoord_X;
		if (NextCoord_Y >= m_PLAYFIELD_BOTTOM || NextCoord_Y <= m_PLAYFIELD_TOP)
		{
			NextCoord_Y = IsPositive(pThis->vsp) >= 0 ? m_PLAYFIELD_BOTTOM - 1 : m_PLAYFIELD_TOP + 1;
			pThis->vsp = 0;
		}
		NowCoord.Y = NextCoord_Y;
		pThis->coord = NowCoord;
		// check for player has reached finish line
	/*	PlayerCollision(pThis);*/
		PlayerPassLevel(pThis);
	}
	return;
}
void ControlEnemy(Player* pl,Enemy* en)
{
	int save = en->walksp;
	if (en->SpawnFrom->Stop)
	{
		en->walksp = 0;
	}
	else
	{
		en->walksp = save;
	}
	COORD HeadCoord = en->coord;
	COORD TempCoord;
	en->hsp = en->walksp * en->dir;
	bool GoFrom = (en->dir == 1) ? true : false; // true if go from left, false if go from righ
	short i;
	// access to enemy body
	for (i = 0; i < en->length; i++)
	{
		TempCoord = {
			HeadCoord.X + (GoFrom ? -i:i),
			HeadCoord.Y
		};
		// delete last coord
		if (TempCoord.X >= m_PLAYFIELD_LEFT && TempCoord.X <= m_PLAYFIELD_RIGHT)
		{
			NextBuffer[TempCoord.X][TempCoord.Y] = L' ';
		}
	}
	// check for collision with player when traveling
	// TempCoord now is Tail Coordinate
	for (int i = 0; i < en->length + en->walksp;i++)
	{
		//int enemyCoord_X = TempCoord.X + (GoFrom ? i : -i);
		if (TempCoord.X + (GoFrom ? i : -i) == pl->coord.X && TempCoord.Y == pl->coord.Y)
		{
			// set player state to false
			pl->state = false;
			break;
		}
	}
	// check if the enemy tail reach the playfield border
	// after finishing loop TempCoord contains enemy tail coordinate
	if (GoFrom) // enemy goes from left
	{
		if (TempCoord.X + en->hsp >= m_PLAYFIELD_RIGHT)
		{
			// enemy ready to be destroyes if reach out playfield
			en->ReadyToDestroy = true;
		}
	}
	else // enemy goes from right
	{
		if (TempCoord.X + en->hsp <= m_PLAYFIELD_LEFT)
		{
			// enemy ready to be destroyes if reach out playfield
			en->ReadyToDestroy = true;
		}
	}
	HeadCoord.X += en->hsp;
	en->coord = HeadCoord;
	return;
}
void PlayerPassLevel(Player* pThis)
{
	if (pThis->coord.Y == m_PLAYFIELD_TOP + 1)
	{
		gl_SystemSignal |= m_SIGNAL_NEW_LEVEL;
		gl_CurrentLvl++;
	}
}