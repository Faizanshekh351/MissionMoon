#include "MissionGameMode.h"
#include "MissionCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMissionGameMode::AMissionGameMode()
{
	DefaultPawnClass = AMissionCharacter::StaticClass();
}
