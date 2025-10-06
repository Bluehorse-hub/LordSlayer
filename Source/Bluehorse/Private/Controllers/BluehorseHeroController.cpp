// Shungen All Rights Reserved.


#include "Controllers/BluehorseHeroController.h"

ABluehorseHeroController::ABluehorseHeroController()
{
	HeroTeamId = FGenericTeamId(0);
}

FGenericTeamId ABluehorseHeroController::GetGenericTeamId() const
{
	return HeroTeamId;
}
