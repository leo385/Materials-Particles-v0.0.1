#define OLC_PGE_APPLICATION
#include "state.h"

int main(int argv, char* argc[])
{
	state s;
	if(s.Construct(400, 300, 2, 2, false))
		s.Start();

	return 0;
}
