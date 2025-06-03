#pragma once
#include "includes.h"

struct pen_data_t
{
	Player* hit_player{ };
	int dmg{ }, hitbox{ }, hitgroup{ }, remaining_pen{ };
};

class c_auto_wall
{
private:
	bool trace_to_exit(const vec3_t& src, const vec3_t& dir, const CGameTrace& enter_trace, CGameTrace& exit_trace);
	void clip_trace_to_player(const vec3_t& src, const vec3_t& dst, CGameTrace& trace, Player* const player);

	bool handle_bullet_penetration(Player* const shooter, const WeaponInfo* const wpn_data, const CGameTrace& enter_trace, vec3_t& src, const vec3_t& dir, int& pen_count, float& cur_dmg, const float pen_modifier);

public:
	bool can_hit_point(Player* entity, const vec3_t& point, const vec3_t& source, int min_damage, Player* shooter = g_cl.m_local, int* out = nullptr);

	void scale_dmg(Player* const player, float& dmg, const float armor_ratio, const float headshot_mult, const int hitgroup);

	pen_data_t fire_bullet(Player* const shooter, Player* const target, const WeaponInfo* const wpn_data, const bool is_taser, vec3_t src, const vec3_t& dst, bool ignore_target = false);
	pen_data_t fire_emulated(Player* const shooter, Player* const target, vec3_t src, const vec3_t& dst);
};

extern c_auto_wall g_autowall;