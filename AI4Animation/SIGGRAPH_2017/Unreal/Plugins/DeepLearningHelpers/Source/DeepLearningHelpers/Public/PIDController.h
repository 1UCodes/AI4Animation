#pragma once

struct PIDController
{
private:
	float m_Error;
	float m_Integrator;
	float m_Differentiator;
	float m_LastError;

public:
	float PGain;
	float IGain;
	float DGain;
	float Value;

	PIDController(float P, float I, float D)
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
		m_Error = target - current;

		m_Integrator += m_Error * update;

		m_Differentiator = (m_Error - m_LastError) / update;

		m_LastError = m_Error;

		Value = m_Error * PGain + m_Integrator * IGain + m_Differentiator * DGain;
		return Value;
	}

	void Reset(float error = 0.f, float integrator = 0.f, float differentiator = 0.f, float lastError = 0.f)
	{
		m_Error = error;
		m_Integrator = integrator;
		m_Differentiator = differentiator;
		m_LastError = lastError;
	}
};
