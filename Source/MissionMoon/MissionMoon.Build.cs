using UnrealBuildTool;

public class MissionMoon : ModuleRules
{
	public MissionMoon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"Niagara",
			"CableComponent",
			"CinematicCamera"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"MovieRenderQueueCore"
		});
	}
}
