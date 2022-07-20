// juaxix - 2020 - xixgames

using UnrealBuildTool;
using System.Collections.Generic;

public class AI4AnimationTarget : TargetRules
{
	public AI4AnimationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "AI4Animation" } );
	}
}
