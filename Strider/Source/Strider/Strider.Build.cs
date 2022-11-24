// Copyright 2020 Kenneth Claassen, All Rights Reserved.

using UnrealBuildTool;

public class Strider : ModuleRules
{
	public Strider(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Strider/Public",
				"Strider/Public/Objects",
				"Strider/Public/Objects/AnimNotifies",
				"Strider/Public/Components",

				"Strider/Private",
				"Strider/Private/Objects",
				"Strider/Private/Objects/AnimNotifies",
				"Strider/Private/Components",
				// ... add other private include paths required here ...
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{

				"CoreUObject",
				"Engine",
                "AnimationCore",
                "AnimGraphRuntime",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
