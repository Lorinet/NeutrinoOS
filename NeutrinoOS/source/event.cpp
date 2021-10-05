#include "event.h"
#include "nvm.h"
#include "scheduler.h"
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

events::events(scheduler* s)
{
	hours = 0;
	minutes = 0;
	seconds = 0;
	timeSet = 0;
	sched = s;
}

void events::eventLoop()
{
	Array<ntrevent> pending = Array<ntrevent>();
	int newMin = ntime::getMinute();
	int newSec = ntime::getSecond();
	int newHour = ntime::getHour();
	if (newMin != minutes)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeMinutes, Array<Array<byte>>());
		pending.push(ne);
		minutes = newMin;
	}
	if (newSec != seconds)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeSeconds, Array<Array<byte>>());
		pending.push(ne);
		seconds = newSec;
	}
	if (newHour != hours)
	{
		ntrevent ne = ntrevent(eventid::TimeChangeHours, Array<Array<byte>>());
		pending.push(ne);
		hours = newHour;
	}
	if (timeSet)
	{
		ntrevent ne = ntrevent(eventid::TimeSet, Array<Array<byte>>());
		pending.push(ne);
		timeSet = false;
	}
	byte eid = 0;
	for (unsigned int i = 0; i < sched->processes.size; i++)
	{
		for (int j = 0; j < pending.size; j++)
		{
			eid = pending[j].id;
			if (sched->processes[i]->eventHandlers.count(eid) && (sched->processes[i]->eventsenabled || sched->processes[i]->inhandler != -1))
			{
				sched->processes[i]->eventQueue.add(ntrevent(pending[j].id, pending[j].parameters));
			}
		}
		long long lillis = ntime::getMillis();
		for (const pair<int, timerevt>& p : sched->processes[i]->timers)
		{
			if (lillis >= p.second.startTime + p.second.interval && p.second.running && sched->processes[i]->eventsenabled)
			{
				sched->processes[i]->timerQueue.add(ntrevent(p.first, NULL));
				sched->processes[i]->timers[p.first].startTime = lillis;
				if (!p.second.loop)
				{
					sched->processes[i]->timers[p.first].running = false;
				}
			}
		}
	}
}