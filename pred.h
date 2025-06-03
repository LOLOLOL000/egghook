#pragma once

class InputPrediction {
public:
	float m_curtime;
	float m_frametime;
	bool m_in_pred;

public:
	void update( );
	void run( );
	void restore( );
	void UpdatePitch(const float& pitch);
};

extern InputPrediction g_inputpred;