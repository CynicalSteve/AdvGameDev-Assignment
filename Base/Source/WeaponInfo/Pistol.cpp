#include "Pistol.h"


CPistol::CPistol()
{
}


CPistol::~CPistol()
{
}

// Initialise this instance to default values
void CPistol::Init(void)
{
	// Call the parent's Init method
	CWeaponInfo::Init();

	// The number of ammunition in a magazine for this weapon
	magRounds = 100;
	// The maximum number of ammunition for this magazine for this weapon
	maxMagRounds = 100;
	// The current total number of rounds currently carried by this player
	totalRounds = 100;
	// The max total number of rounds currently carried by this player
	maxTotalRounds = 100;

	// The time between shots
	timeBetweenShots = 0.3333;
	// The elapsed time (between shots)
	elapsedTime = 0.0;
	// Boolean flag to indicate if weapon can fire now
	bFire = true;

	setWeaponID("Pistol");
}
