#pragma once

#include <optional>

// pre-declare.
class LagRecord;

class BackupRecord {
public:
	BoneArray m_bones[128];
	int        m_bone_count;
	vec3_t     m_origin, m_abs_origin;
	vec3_t     m_mins;
	vec3_t     m_maxs;
	ang_t      m_abs_ang;


public:
	__forceinline void store(Player* player) {
		// get bone cache ptr.
		CBoneCache* cache = &player->m_BoneCache();

		// store bone data.
		std::memcpy(m_bones, cache->m_pCachedBones, sizeof(matrix3x4_t) * 128);

		m_bone_count = cache->m_CachedBoneCount;
		m_origin = player->m_vecOrigin();
		m_mins = player->m_vecMins();
		m_maxs = player->m_vecMaxs();
		m_abs_origin = player->GetAbsOrigin();
		m_abs_ang = player->GetAbsAngles();
	}

	__forceinline void restore(Player* player) {
		// get bone cache ptr.
		CBoneCache* cache = &player->m_BoneCache();

		std::memcpy(cache->m_pCachedBones, m_bones, sizeof(matrix3x4_t) * m_bone_count);
		cache->m_CachedBoneCount = m_bone_count;

		player->m_vecOrigin() = m_origin;
		player->m_vecMins() = m_mins;
		player->m_vecMaxs() = m_maxs;
		player->SetAbsAngles(m_abs_ang);
		player->SetAbsOrigin(m_origin);
	}
};

class LagRecord {
public:
	// data.
	Player* m_player;
	float   m_immune;
	bool m_resolved;
	int     m_tick;
	int     m_lag;
	bool    m_dormant;

	std::optional< bool >			m_jumped{};

	// netvars.
	float  m_sim_time;
	bool m_valid;
	float  m_old_sim_time;
	bool on_ground, real_on_ground;
	int    m_flags;
	vec3_t m_origin;
	vec3_t m_abs_origin;
	vec3_t m_old_origin;
	float m_on_ground_time;
	vec3_t m_velocity;
	vec3_t m_mins;
	vec3_t m_maxs;
	ang_t  m_eye_angles;
	ang_t  m_abs_ang;
	float  m_body;
	float  m_duck;
	bool m_extrapolating;
	Weapon* m_weapon;
	float m_max_speed;

	// anim stuff.
	float m_foot_yaw, m_prev_foot_yaw, m_move_yaw, m_move_yaw_cur_to_ideal, m_move_yaw_ideal, m_eye_yaw;
	C_AnimationLayer m_layers[13];
	float            m_poses[24];
	vec3_t           m_anim_velocity;
	float m_anim_speed;

	// bone stuff.
	bool m_invalid;
	bool m_setup;
	BoneArray m_bones[128];
	BoneArray m_visual_bones[128];

	// lagfix stuff.
	bool   m_broke_lc;
	vec3_t m_pred_origin;
	vec3_t m_pred_velocity;
	float  m_pred_time;
	int    m_pred_flags;

	// resolver stuff.
	size_t m_mode;
	bool   m_fake_walk;
	bool   m_shot, m_fixing_pitch, m_sim_tick;
	float  m_away;
	float  m_anim_time;

	// other stuff.
	float  m_interp_time;
public:

	// default ctor.
	__forceinline LagRecord() :
		m_setup{ false },
		m_broke_lc{ false },
		m_fake_walk{ false },
		m_shot{ false },
		m_lag{} {}

	// ctor.
	__forceinline LagRecord(Player* player) :
		m_setup{ false },
		m_broke_lc{ false },
		m_fake_walk{ false },
		m_shot{ false },
		m_lag{} {

		store(player);
	}

	__forceinline ~LagRecord() {
		// free heap allocated game mem.
	}

	__forceinline void invalidate() {
		// free heap allocated game mem.

		// mark as not setup.
		m_setup = false;

		// allocate new memory.
	}

	// function: allocates memory for SetupBones and stores relevant data.
	void store(Player* player) {
		// allocate game heap.
		//m_bones = (BoneArray*)g_csgo.m_mem_alloc->Alloc(sizeof(BoneArray) * 128);

		// player data.
		m_valid = true;
		m_player = player;
		m_immune = player->m_fImmuneToGunGameDamageTime();
		m_tick = g_csgo.m_cl->clock_drift_mgr.server_tick;
		m_weapon = player->GetActiveWeapon();
		m_max_speed = m_weapon && m_weapon->GetWpnData() ? m_player->m_bIsScoped() ? m_weapon->GetWpnData()->m_max_player_speed_alt : m_weapon->GetWpnData()->m_max_player_speed : 260.f;

		// netvars.
		m_pred_time = m_sim_time = player->m_flSimulationTime();
		m_old_sim_time = player->m_flOldSimulationTime();
		m_pred_flags = m_flags = player->m_fFlags();
		m_pred_origin = m_origin = player->m_vecOrigin();
		m_abs_origin = player->GetAbsOrigin();
		m_old_origin = player->m_vecOldOrigin();
		m_eye_angles = player->m_angEyeAngles();
		m_abs_ang = player->GetAbsAngles();
		m_body = player->m_flLowerBodyYawTarget();
		m_mins = player->m_vecMins();
		m_maxs = player->m_vecMaxs();
		m_duck = player->m_flDuckAmount();
		m_pred_velocity = m_velocity = player->m_vecVelocity();

		// save networked animlayers.
		player->GetAnimLayers(m_layers);

		// normalize eye angles.
		m_eye_angles.normalize();
		math::clamp(m_eye_angles.x, -90.f, 90.f);

		// get lag.
		m_lag = game::TIME_TO_TICKS(m_sim_time - m_old_sim_time);

		// compute animtime.
		m_anim_time = m_old_sim_time + g_csgo.m_globals->m_interval;
	}

	// function: restores 'predicted' variables to their original.
	__forceinline void predict() {
		m_broke_lc = false;
		m_pred_origin = m_origin;
		m_pred_velocity = m_velocity;
		m_pred_time = m_sim_time;
		m_pred_flags = m_flags;
	}

	// function: writes current record to bone cache.
	__forceinline void cache() {
		// get bone cache ptr.
		CBoneCache* cache = &m_player->m_BoneCache();

		std::memcpy(cache->m_pCachedBones, m_bones, sizeof(matrix3x4_t) * cache->m_CachedBoneCount);

		m_player->m_vecOrigin() = m_pred_origin;
		m_player->m_vecMins() = m_mins;
		m_player->m_vecMaxs() = m_maxs;

		m_player->SetAbsAngles(m_abs_ang);
		m_player->SetAbsOrigin(m_pred_origin);
	}

	__forceinline bool dormant() {
		return m_dormant;
	}

	__forceinline bool immune() {
		return m_immune > 0.f;
	}

	// function: checks if LagRecord obj is hittable if we were to fire at it now.
	bool valid() {
		auto netchan = g_csgo.m_engine->GetNetChannelInfo();
		if (!netchan)
			return false;

		float time = g_cl.m_local->alive() ? game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) : g_csgo.m_globals->m_curtime;

		float correct = 0.f;

		correct += netchan->GetLatency(INetChannel::FLOW_OUTGOING);
		correct += netchan->GetLatency(INetChannel::FLOW_INCOMING);
		correct += g_cl.m_lerp;

		correct = std::clamp< float >(correct, 0.0f, g_csgo.sv_maxunlag->GetFloat());

		float deltaTime = correct - (time - this->m_sim_time);

		return std::fabs(deltaTime) < 0.2f;
	}
};