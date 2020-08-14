#include "event.h"
ntrevent::ntrevent()
{
	id = eventid::NoEvent;
	parameters = Array<Array<byte>>();
}
ntrevent::ntrevent(byte i, Array<Array<byte>> p)
{
	id = i;
	parameters = p;
}
Array<ntrevent> events::pending;
int events::seconds = 0;
int events::minutes = 0;
int events::hours = 0;
bool events::timeSet = false;
void events::eventLoop()
{
	pending = Array<ntrevent>();
	int newMin = ntime::getMinute();
	int newSec = ntime::getSecond();
	int newHour = ntime::getHour();
	if (newMin != minutes)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeMinutes, Array<Array<byte>>());
		ne.parameters.push(bitconverter::toArray(newMin));
		pending.push(ne);
		minutes = newMin;
	}
	if (newSec != seconds)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeSeconds, Array<Array<byte>>());
		ne.parameters.push(bitconverter::toArray(newSec));
		pending.push(ne);
		seconds = newSec;
	}
	if (newHour != hours)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeHours, Array<Array<byte>>());
		ne.parameters.push(bitconverter::toArray(newHour));
		pending.push(ne);
		hours = newHour;
	}
	if (timeSet)
	{
		ntrevent ne = ntrevent(eventid::TimeSet, Array<Array<byte>>());
		ne.parameters.push(bitconverter::toArray(ntime::getTimeS()));
		pending.push(ne);
		timeSet = false;
	}
	byte eid = 0;
	for (unsigned int i = 0; i < vmmgr::processes.size(); i++)
	{
		for (int j = 0; j < pending.size; j++)
		{
			eid = pending[j].id;
			if (vmmgr::processes[vmmgr::procidx[i]]->eventHandlers.count(eid && vmmgr::processes[vmmgr::procidx[i]]->eventsenabled))
			{
				for (int k = 0; k < pending[j].parameters.size; k++)
				{
					vmmgr::processes[vmmgr::procidx[i]]->astack.push(pending[j].parameters[k]);
				}
				int cpc = vmmgr::processes[vmmgr::procidx[i]]->pc;
				vmmgr::processes[vmmgr::procidx[i]]->branch(vmmgr::processes[vmmgr::procidx[i]]->eventHandlers[pending[j].id]);
				vmmgr::processes[vmmgr::procidx[i]]->eventsenabled = false;
				while (vmmgr::processes[vmmgr::procidx[i]]->callstack.contains(cpc))
				{
					vmmgr::processes[vmmgr::procidx[i]]->cycle();
				}
				vmmgr::processes[vmmgr::procidx[i]]->eventsenabled = true;
			}
		}
		long long lillis = ntime::getMillis();
		for (pair<int, timerevt> p : vmmgr::processes[vmmgr::procidx[i]]->timers)
		{
			if (lillis >= p.second.startTime + p.second.interval && p.second.running)
			{
				int cpc = vmmgr::processes[vmmgr::procidx[i]]->pc;
				vmmgr::processes[vmmgr::procidx[i]]->branch(p.second.handler);
				vmmgr::processes[vmmgr::procidx[i]]->eventsenabled = false;
				while (vmmgr::processes[vmmgr::procidx[i]]->callstack.contains(cpc))
				{
					vmmgr::processes[vmmgr::procidx[i]]->cycle();
				}
				vmmgr::processes[vmmgr::procidx[i]]->eventsenabled = true;
				vmmgr::processes[vmmgr::procidx[i]]->timers[p.first].startTime = lillis;
				if (!p.second.loop)
				{
					vmmgr::processes[vmmgr::procidx[i]]->timers[p.first].running = false;
				}
			}
		}
	}
}