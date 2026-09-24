using UnrealBuildTool;
using System.Collections.Generic;

public class AntigravityShowcaseTarget : TargetRules
{
	public AntigravityShowcaseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("AntigravityShowcase");
	}
}
