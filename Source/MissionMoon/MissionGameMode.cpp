#include "AntigravGameMode.h"
#include "AntigravCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAntigravGameMode::AAntigravGameMode()
{
	DefaultPawnClass = AAntigravCharacter::StaticClass();
}
