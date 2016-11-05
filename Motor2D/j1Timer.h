#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "p2Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	//Start time or "reset" it
	void Start();
	//Get the timer value in ms
	uint32 Read() const;
	//Get timer value in seconds
	float ReadSec() const;

private:

	uint32	started_at;
};

#endif //__j1TIMER_H__