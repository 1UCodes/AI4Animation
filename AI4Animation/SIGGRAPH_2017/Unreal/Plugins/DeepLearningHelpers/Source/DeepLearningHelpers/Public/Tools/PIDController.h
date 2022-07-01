// juaxix - 2020-2022 - xixgames

#pragma once

namespace DeepLearningHelpers
{
	struct DEEPLEARNINGHELPERS_API PIDController
	{
		float PGain;
		float IGain;
		float DGain;

		float Value;

		float Error;
		float Integrator;
		float Differentiator;
		float LastError;

		PIDController() = default;

		explicit PIDController(float P, float I, float D)
		{
			SetParameters(P, I, D);
		}

		void SetParameters(float P, float I, float D)
		{
			PGain = P;
			IGain = I;
			DGain = D;
		}

		float Update(float target, float current, float update)
		{
			Error = target-current;
				
			Integrator += Error*update;

			Differentiator = (Error-LastError)/update;

			LastError = Error;

			Value = Error*PGain + Integrator*IGain + Differentiator*DGain;
			return Value;
		}

		void Reset(float error = 0.f, float integrator = 0.f, float differentiator = 0.f, float lastError = 0.f)
		{
			Error = error;
			Integrator = integrator;
			Differentiator = differentiator;
			LastError = lastError;
		}
	};
}
