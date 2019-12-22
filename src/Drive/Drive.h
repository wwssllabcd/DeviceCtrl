#pragma once
#include <string>
using namespace std;

#include "windows.h"
#include "DefineFiles/DefineFile.h" // for estring

#define	 BUSY			 0
#define	 IDEL			 1

class Drive
{
public:
	//--------------- property ------------
	HANDLE address;
	estring name;
	int testNo;
	int status;

	// -------------- function ------------
	Drive(void);
	~Drive(void);

	bool operator ==(const Drive& rhs) const;
};
