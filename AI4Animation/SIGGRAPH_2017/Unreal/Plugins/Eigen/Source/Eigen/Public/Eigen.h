// juaxix - 2020-2022 - xixgames

#pragma once
#include "Modules/ModuleManager.h"

class FEigenModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


};
