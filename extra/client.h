#pragma once

class Sequence {
public:
	float m_time;
	int   m_state;
	int   m_seq;

public:
	__forceinline Sequence( ) : m_time{}, m_state{}, m_seq{} {};
	__forceinline Sequence( float time, int state, int seq ) : m_time{ time }, m_state{ state }, m_seq{ seq } {};
};

class NetPos {
public:
	float  m_time;
	vec3_t m_pos;
	ang_t  m_view_angle;

public:
	__forceinline NetPos() : m_time{}, m_pos{}, m_view_angle{} {};
	__forceinline NetPos( float time, vec3_t pos ) : m_time{ time }, m_pos{ pos } {};
	__forceinline NetPos(float time, vec3_t pos, ang_t ang) : m_time{ time }, m_pos{ pos }, m_view_angle{ang} {};
};

class Client {
public:
	// hack thread.
	static ulong_t __stdcall init( void* arg );

	bool is_able_to_shoot(bool revolver = false);
	bool is_firing();

	void StartMove( CUserCmd* cmd );
	void EndMove( CUserCmd* cmd );
	void BackupPlayers( bool restore );
	void DoMove( );
	void DrawHUD( );
	void update_viewmodel();
	void UpdateInterpLocal();
	void UpdateLocalMatrix();
	void UpdateLocal();
	void UpdateAnimations( );
	void UpdateInformation();
	void KillFeed( );

	void OnPaint( );
	void OnMapload( );
	void OnTick( CUserCmd* cmd );

	// debugprint function.
	void print( const std::string text, ... );

	// check if we are able to fire this tick.
	bool CanFireWeapon( );
	void UpdateRevolverCock( );
	void UpdateIncomingSequences( );

public:
	// local player variables.
	bool m_pressing_move;
	Player*          m_local;
	bool	         m_processing;
	float velocity_modifier;
	int	             m_flags;
	vec3_t	         m_shoot_pos;
	bool	         m_player_fire;
	bool	         m_shot;
	bool	         m_old_shot;
	float            m_abs_yaw;
	CCSGOPlayerAnimState* fake_anim_state;
	int m_rate;
	C_AnimationLayer m_layers[13];
	float            m_poses[ 24 ];
	matrix3x4_t m_bones[128];
	matrix3x4_t m_bones_chams[128];

	// active weapon variables.
	Weapon*     m_weapon;
	int         m_weapon_id;
	WeaponInfo* m_weapon_info;
	int         m_weapon_type;
	bool        m_weapon_fire;

	// revolver variables.
	int	 m_revolver_cock;
	int	 m_revolver_query;
	bool m_revolver_fire;

	// general game varaibles.
	bool     m_round_end;
	Stage_t	 m_stage;
	int	     m_max_lag;
	int      m_lag;
	int	     m_old_lag;
	bool*    m_packet;
	bool*    m_final_packet;
	int sent_tick_count;
	bool	 m_old_packet;
	float	 m_lerp;
	float    m_latency;
	int      m_latency_ticks;
	int      m_server_tick;
	int      m_arrival_tick;
	int      m_width, m_height;

	// usercommand variables.
	CUserCmd* m_cmd;
	int	      m_tick;
	int	      m_buttons;
	int       m_old_buttons;
	float m_spawn;
	ang_t     m_view_angles;
	ang_t	  m_strafe_angles;
	ang_t m_orig_ang;
	vec3_t	  m_forward_dir;

    penetration::PenetrationOutput_t m_pen_data;

	std::deque< Sequence > m_sequences;
	std::deque< NetPos >   m_net_pos;
	std::deque< NetPos >   m_anim_pos;

	// animation variables.
	ang_t  m_angle;
	ang_t  m_hidden_angle;
	ang_t  m_interp_angle;
	ang_t  m_rotation;
	ang_t  m_radar;
	float  m_body;
	float  m_body_pred;
	float  m_speed;
	float  m_anim_time;
	float  m_anim_frame;
	bool   m_ground;
	bool m_console_open;
	bool   m_lagcomp;
	bool m_updating_paintkit;

	// hack username.
	std::string m_user;
};

extern Client g_cl;