using UnrealBuildTool;
using System.Collections.Generic;

public class AntigravityShowcaseEditorTarget : TargetRules
{
	public AntigravityShowcaseEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("AntigravityShowcase");
	}
}
