using UnrealBuildTool;

public class AntigravityShowcase : ModuleRules
{
	public AntigravityShowcase(ReadOnlyTargetRules Target) : base(Target)
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
