#include "event.h"
ntrevent::ntrevent(byte i, vector<vector<byte>> p)
{
	id = i;
	parameters = p;
}
vector<ntrevent> events::pending;
int events::seconds = 0;
int events::minutes = 0;
int events::hours = 0;
bool events::timeSet = false;
void events::eventLoop()
{
	pending = vector<ntrevent>();
	int newMin = ntime::getMinute();
	int newSec = ntime::getSecond();
	int newHour = ntime::getHour();
	if (newMin != minutes)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeMinutes, vector<vector<byte>>());
		ne.parameters.push_back(bitconverter::toarray(newMin));
		pending.push_back(ne);
		minutes = newMin;
	}
	if (newSec != seconds)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeSeconds, vector<vector<byte>>());
		ne.parameters.push_back(bitconverter::toarray(newSec));
		pending.push_back(ne);
		seconds = newSec;
	}
	if (newHour != hours)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeHours, vector<vector<byte>>());
		ne.parameters.push_back(bitconverter::toarray(newHour));
		pending.push_back(ne);
		hours = newHour;
	}
	if (timeSet)
	{
		ntrevent ne = ntrevent(eventid::TimeSet, vector<vector<byte>>());
		ne.parameters.push_back(bitconverter::toarray(ntime::getTimeS()));
		pending.push_back(ne);
		timeSet = false;
	}

	for (unsigned int i = 0; i < vmmgr::processes.size(); i++)
	{
		for (ntrevent n : pending)
		{
			if (vmmgr::processes[vmmgr::procidx[i]].eventHandlers.count((eventid)n.id) && vmmgr::processes[vmmgr::procidx[i]].eventsenabled)
			{
				for (vector<byte> vb : n.parameters)
				{
					vmmgr::processes[vmmgr::procidx[i]].astack.push(vb);
				}
				int cpc = vmmgr::processes[vmmgr::procidx[i]].pc;
				vmmgr::processes[vmmgr::procidx[i]].branch(vmmgr::processes[vmmgr::procidx[i]].eventHandlers[n.id]);
				vmmgr::processes[vmmgr::procidx[i]].eventsenabled = false;
				while (util::contains(vmmgr::processes[vmmgr::procidx[i]].callstack, cpc))
				{
					vmmgr::processes[vmmgr::procidx[i]].cycle();
				}
				vmmgr::processes[vmmgr::procidx[i]].eventsenabled = true;
			}
		}
		long long lillis = ntime::getMillis();
		for (pair<int, timerevt> p : vmmgr::processes[vmmgr::procidx[i]].timers)
		{
			if (lillis >= p.second.startTime + p.second.interval && p.second.running)
			{
				int cpc = vmmgr::processes[vmmgr::procidx[i]].pc;
				vmmgr::processes[vmmgr::procidx[i]].branch(p.second.handler);
				vmmgr::processes[vmmgr::procidx[i]].eventsenabled = false;
				while (util::contains(vmmgr::processes[vmmgr::procidx[i]].callstack, cpc))
				{
					vmmgr::processes[vmmgr::procidx[i]].cycle();
				}
				vmmgr::processes[vmmgr::procidx[i]].eventsenabled = true;
				vmmgr::processes[vmmgr::procidx[i]].timers[p.first].startTime = lillis;
				if (!p.second.loop)
				{
					vmmgr::processes[vmmgr::procidx[i]].timers[p.first].running = false;
				}
			}
		}
	}
}