using UnrealBuildTool;
using System.Collections.Generic;

public class MissionMoonEditorTarget : TargetRules
{
	public MissionMoonEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("MissionMoon");
	}
}
