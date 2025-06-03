#pragma once

class Movement {
public:
	float  m_speed;
	float  m_ideal;
	float  m_ideal2;
	vec3_t m_mins;
	vec3_t m_maxs;
	vec3_t m_origin;
	float  m_switch_value = 1.f;
	int    m_strafe_index;
	float  m_old_yaw;
	float  m_circle_yaw;
	bool   m_invert;
	int m_ground_ticks;
	float m_max_speed;

public:
	void update_ground_ticks();
	bool on_ground();

	void ViewModelChanger();
	void force_speed(float max_speed);
	void JumpRelated( );
	void Strafe( );
	void extrapolate(Player* player, vec3_t& origin, vec3_t& velocity, int& flags, bool on_ground);
	void DoPrespeed( );
	bool GetClosestPlane( vec3_t& plane );
	bool WillCollide( float time, float step );
	void FixMove( CUserCmd* cmd, const ang_t& old_angles );
	void AutoPeek( );
	void QuickStop( );
	void FastStop();
	void FakeWalk( );
};

extern Movement g_movement;