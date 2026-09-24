using UnrealBuildTool;
using System.Collections.Generic;

public class MissionMoonTarget : TargetRules
{
	public MissionMoonTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MissionMoon");
	}
}
