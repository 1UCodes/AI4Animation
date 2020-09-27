// juaxix - 2020 - xixgames

#pragma once

#include "Modules/ModuleManager.h"

class FDeepLearningHelpersModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
