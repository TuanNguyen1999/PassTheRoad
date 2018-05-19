//Signal.cpp

#include "Declar.h"
void ReceiveUserSignal()
{
	if (!gl_StopReceiveSignal)
	{
		// check for key is downed
		if (GetAsyncKeyState(PAUSE))
		{
			//unpause if paused and vice-versa
			if (gl_SystemSignal & m_SIGNAL_PAUSE)
			{
				// currently pausing
				gl_SystemSignal &= ~m_SIGNAL_PAUSE;
			}
			else
			{
				gl_SystemSignal |= m_SIGNAL_PAUSE;
			}
		}
		else if (GetAsyncKeyState(EXIT))
		{
			gl_SystemSignal |= m_SIGNAL_EXIT;
			gl_SystemSignal &= ~(m_SIGNAL_GAME_START);
		}
		else if (GetAsyncKeyState(RESTART))
		{
			gl_SystemSignal |= m_SIGNAL_RESTART;
			gl_SystemSignal &= ~(m_SIGNAL_GAME_START);
		}
		else if (GetAsyncKeyState(LOAD))
		{
			//pause game and load menu
			gl_SystemSignal |= m_SIGNAL_LOAD | m_SIGNAL_PAUSE;
		}
		else if (GetAsyncKeyState(SAVE))
		{
			//pause game and save menu
			gl_SystemSignal |= m_SIGNAL_SAVE | m_SIGNAL_PAUSE;
		}
	}
	return;
}
void ClearUserSignal()
{
	// clear signal by checking but do noting
	if (GetAsyncKeyState(PAUSE));
	if (GetAsyncKeyState(EXIT));
	if (GetAsyncKeyState(RESTART));
	if (GetAsyncKeyState(LOAD));
	if (GetAsyncKeyState(SAVE));
	return;
}
bool RequestUserInput()
{
	using std::wcin;
	using std::wofstream;
	bool flag;
	gl_StopReceiveSignal = true;
	wchar_t msg[] = L"\u00B7 SAVE/LOAD TO FILE \u00B7";
	wchar_t request[] = L"Enter file name (limit 8 characters)";
	wchar_t success[] = L"OPERATION SUCCESS!!";
	wchar_t fail[] = L"OPERATION FAILED!!";
	COORD msgCOORD = { m_TEXTFIELD_LEFT + 14, m_TEXTFIELD_TOP + 15 };
	COORD rqCOORD = { m_TEXTFIELD_LEFT + 8, m_TEXTFIELD_TOP + 16 };
	COORD doneCOORD = { m_TEXTFIELD_LEFT + 15, m_TEXTFIELD_TOP + 17 };
	COORD failCOORD = { m_TEXTFIELD_LEFT + 15, m_TEXTFIELD_TOP + 17 };
	DrawMessage(msg, msgCOORD);
	DrawMessage(request, rqCOORD);
	int LIMIT = 15;
	wchar_t fname[15];
	COORD CurSor = { m_TEXTFIELD_LEFT + 2, m_TEXTFIELD_TOP + 18 };
	m_SetCursor(true);
	GoTo(CurSor);
	ClearKeyBoard();
	wcin.get(fname, LIMIT);
	wcscat(fname, L".txt");
	SMALL_RECT area = { m_TEXTFIELD_LEFT + 2, m_TEXTFIELD_TOP + 15,m_TEXTFIELD_RIGHT - 1,m_TEXTFIELD_BOTTOM - 1 };
	try
	{
		if (gl_SystemSignal & m_SIGNAL_SAVE)
		{
			SaveToFile(fname);
		}
		else if (gl_SystemSignal & m_SIGNAL_LOAD)
		{
			int temp = LoadFromFile(fname);
			gl_CurrentLvl = temp;
		}
		DrawMessage(success, doneCOORD);
		flag = true;
	}
	catch (const std::exception&)
	{
		DrawMessage(fail, failCOORD);
		flag = false;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(350));
	wcin.clear();
	wcin.ignore(INT_MAX, '\n');
	ClearUserSignal();
	ClearArea(area);
	m_SetCursor(false);
	gl_StopReceiveSignal = false;
	return flag;
}
void SaveToFile(const wchar_t* fname)
{
	using std::wofstream;
	wofstream wf(fname);
	wf << gl_CurrentLvl;
	return;
}
int LoadFromFile(const wchar_t* fname)
{
	int level;
	using std::wifstream;
	wifstream rf(fname);
	while(rf.good())
		rf >> level;
	if (rf.eof() && level < m_TOTAL_LVL && level >=0)
		return level;
	throw std::invalid_argument("File not Found or Data is corrupted");
}