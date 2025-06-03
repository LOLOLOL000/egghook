#include "autowall.h"

c_auto_wall g_autowall;

bool c_auto_wall::trace_to_exit(const vec3_t& src, const vec3_t& dir, const CGameTrace& enter_trace, CGameTrace& exit_trace)
{
    float dist{ };
    int first_contents{ };

    constexpr auto k_step_size = 4.f;
    constexpr auto k_max_dist = 90.f;

    while (dist <= k_max_dist)
    {
        dist += k_step_size;

        const auto out = src + (dir * dist);

        const auto cur_contents = g_csgo.m_engine_trace->GetPointContents(out, CS_MASK_SHOT);

        if (!first_contents)
            first_contents = cur_contents;

        if (cur_contents & MASK_SHOT_HULL && (!(cur_contents & CONTENTS_HITBOX) || cur_contents == first_contents))
            continue;

        g_csgo.m_engine_trace->TraceRay(Ray(out, out - dir * k_step_size), CS_MASK_SHOT, nullptr, &exit_trace);

        if (exit_trace.m_startsolid && exit_trace.m_surface.m_flags & SURF_HITBOX)
        {
           CTraceFilterSimple trace_filter{ exit_trace.m_entity, 0 };

           g_csgo.m_engine_trace->TraceRay({ out, src }, MASK_SHOT_HULL, (ITraceFilter*)(&trace_filter), &exit_trace);

            if (exit_trace.hit() && !exit_trace.m_startsolid)
                return true;

            continue;
        }

        if (!exit_trace.hit() || exit_trace.m_startsolid)
        {
            if (enter_trace.m_entity && enter_trace.m_entity->index() && game::IsBreakable(enter_trace.m_entity))
            {
                exit_trace = enter_trace;
                exit_trace.m_endpos = src + dir;

                return true;
            }

            continue;
        }

        if (exit_trace.m_surface.m_flags & SURF_NODRAW)
        {
            if (game::IsBreakable(exit_trace.m_entity) && game::IsBreakable(enter_trace.m_entity))
                return true;

            if (!(enter_trace.m_surface.m_flags & SURF_NODRAW))
                continue;
        }

        if (exit_trace.m_plane.m_normal.dot(dir) <= 1.f)
            return true;
    }

    return false;
}

void c_auto_wall::clip_trace_to_player(const vec3_t& src, const vec3_t& dst, CGameTrace& trace, Player* const player)
{
    vec3_t mins = player->m_vecMins(), maxs = player->m_vecMaxs();

    vec3_t dir(dst - src);
    dir = dir.normalized();

    vec3_t center = (maxs + mins) / 2, pos(center + player->m_vecOrigin());

    vec3_t to = pos - src;
    float range_along = dir.dot(to);

    float range;

    if (range_along < 0.f)
        range = -to.length(false);
    else if (range_along > dir.length(false))
        range = -(pos - dst).length(false);
    else
    {
        auto ray(pos - (dir * range_along + src));
        range = ray.length(false);
    }

    if (range <= 60.f)
    {
        CGameTrace new_trace{ };
        g_csgo.m_engine_trace->ClipRayToEntity(Ray(src, dst), CS_MASK_SHOT, player, &new_trace);

        if (trace.m_fraction > new_trace.m_fraction)
            trace = new_trace;
    }
}

bool c_auto_wall::handle_bullet_penetration(Player* const shooter, const WeaponInfo* const wpn_data, const CGameTrace& enter_trace, vec3_t& src, const vec3_t& dir, int& pen_count, float& cur_dmg, const float pen_modifier)
{
    if (pen_count <= 0 || wpn_data->m_penetration <= 0.f)
        return false;

    CGameTrace exit_trace{ };

    if (!this->trace_to_exit(enter_trace.m_endpos, dir, enter_trace, exit_trace) && !(g_csgo.m_engine_trace->GetPointContents(enter_trace.m_endpos, MASK_SHOT_HULL) & MASK_SHOT_HULL))
        return false;

    auto final_dmg_modifier = 0.16f;
    float combined_pen_modifier{ };

    const auto exit_surface_data = g_csgo.m_phys_props->GetSurfaceData(exit_trace.m_surface.m_surface_props);
    const auto enter_surface_data = g_csgo.m_phys_props->GetSurfaceData(enter_trace.m_surface.m_surface_props);

    if (enter_surface_data->m_game.m_material == 'G' || enter_surface_data->m_game.m_material == 'Y')
    {
        final_dmg_modifier = 0.05f;
        combined_pen_modifier = 3.f;
    }
    else if (enter_trace.m_contents & CONTENTS_GRATE || enter_trace.m_surface.m_flags & SURF_NODRAW)
    {
        final_dmg_modifier = 0.16f;
        combined_pen_modifier = 1.f;
    }
    else if (enter_trace.m_entity && g_csgo.m_cvar->FindVar(HASH("ff_damage_reduction_bullets"))->GetFloat() == 0.f &&
        enter_surface_data->m_game.m_material == 'F' && ((Player*)enter_trace.m_entity)->m_iTeamNum() == shooter->m_iTeamNum())
    {
        const auto dmg_bullet_pen = g_csgo.m_cvar->FindVar(HASH("ff_damage_bullet_penetration"))->GetFloat();
        if (dmg_bullet_pen == 0.f)
            return false;

        combined_pen_modifier = dmg_bullet_pen;
        final_dmg_modifier = 0.16f;
    }
    else
    {
        combined_pen_modifier = (enter_surface_data->m_game.m_penetration_modifier + exit_surface_data->m_game.m_penetration_modifier) * 0.5f;

        final_dmg_modifier = 0.16f;
    }

    if (enter_surface_data->m_game.m_material == exit_surface_data->m_game.m_material)
    {
        if (exit_surface_data->m_game.m_material == 'U' || exit_surface_data->m_game.m_material == 'W')
            combined_pen_modifier = 3.f;
        else if (exit_surface_data->m_game.m_material == 'L')
            combined_pen_modifier = 2.f;
    }

    const auto modifier = std::max(1.f / combined_pen_modifier, 0.f);
    const auto pen_dist = (exit_trace.m_endpos - enter_trace.m_endpos).length_sqr();

    const auto lost_dmg = cur_dmg * final_dmg_modifier + pen_modifier * (modifier * 3.f) + (pen_dist * modifier) / 24.f;

    if (lost_dmg > cur_dmg)
        return false;

    if (lost_dmg > 0.f)
        cur_dmg -= lost_dmg;

    if (cur_dmg < 1.f)
        return false;

    --pen_count;

    src = exit_trace.m_endpos;

    return true;
}

bool c_auto_wall::can_hit_point(Player* entity, const vec3_t& point, const vec3_t& source, int min_damage, Player* shooter, int* out)
{
    auto weapon = shooter->GetActiveWeapon();
    if (!weapon)
        return false;

    auto weapon_info = weapon->GetWpnData();
    if (!weapon_info)
        return false;

    const auto origin_backup = shooter->GetAbsOrigin();

    shooter->SetAbsOrigin(vec3_t(source.x, source.y, origin_backup.z));

    const auto& data = this->fire_bullet(shooter, entity, weapon_info, weapon->is_taser(), source, point);

    shooter->SetAbsOrigin(origin_backup);

    if (out)
        *out = data.dmg;

    return data.dmg >= min_damage + 1;
}

void c_auto_wall::scale_dmg(Player* const player, float& dmg, const float armor_ratio, const float headshot_mult, const int hitgroup)
{
    const auto has_heavy_armor = player->m_bHasHeavyArmor();

    switch (hitgroup)
    {
    case 1:
        dmg *= 4.f;

        if (has_heavy_armor)
            dmg *= 0.5f;

        break;
    case 3:
        dmg *= 1.25f;
        break;
    case 6:
    case 7:
        dmg *= 0.75f;

        break;
    }

    const auto armor_value = player->m_ArmorValue();
    if (!armor_value || hitgroup < 0 || hitgroup > 5 || (hitgroup == 1 && !player->m_bHasHelmet()))
        return;

    auto heavy_ratio = 1.f, bonus_ratio = 0.5f, ratio = armor_ratio * 0.5f;

    if (has_heavy_armor)
    {
        ratio *= 0.2f;
        heavy_ratio = 0.25f;
        bonus_ratio = 0.33f;
    }

    auto dmg_to_hp = dmg * ratio;

    if (((dmg - dmg_to_hp) * (bonus_ratio * heavy_ratio)) > armor_value)
        dmg -= armor_value / bonus_ratio;
    else
        dmg = dmg_to_hp;
}

pen_data_t c_auto_wall::fire_bullet(Player* const shooter, Player* const target, const WeaponInfo* const wpn_data, const bool is_taser, vec3_t src, const vec3_t& dst, bool ignore_target)
{
    auto pen_modifier = std::max((3.f / wpn_data->m_penetration) * 1.25f, 0.f);

    float cur_dist{ };
    pen_data_t data{ };

    data.remaining_pen = 4;

    auto cur_dmg = static_cast<float>(wpn_data->m_damage);
    auto dir = dst - src;
    dir = dir.normalized();

    CGameTrace trace{ };
    CTraceFilterSkipTwoEntities_game trace_filter{ };
    Player* last_hit_player{ };

    auto max_dist = wpn_data->m_range;

    while (cur_dmg > 0.f)
    {
        max_dist -= cur_dist;

        const auto cur_dst = src + dir * max_dist;

        trace_filter.m_pass_ent1 = shooter;
        trace_filter.m_pass_ent2 = last_hit_player;

        g_csgo.m_engine_trace->TraceRay(Ray(src, cur_dst), CS_MASK_SHOT, (ITraceFilter*)(&trace_filter), &trace);

        // interfaces::debug_overlay->add_text_overlay(cur_dst, interfaces::global_vars->interval_per_tick * 2.f, "%s", trace.surface.name);

        if (target)
            this->clip_trace_to_player(src, cur_dst + dir * 40.f, trace, target);

        if (trace.m_fraction == 1.f)
            break;

        cur_dist += trace.m_fraction * max_dist;
        cur_dmg *= std::pow(wpn_data->m_range_modifier, cur_dist / 500.f);

        if (target)
        {
            if (trace.m_entity)
            {
                const auto is_player = trace.m_entity->IsPlayer();
                if ((trace.m_entity == target || (is_player && ((Player*)trace.m_entity)->m_iTeamNum() != shooter->m_iTeamNum()))
                    && ((trace.m_hitgroup >= 1 && trace.m_hitgroup <= 7) || trace.m_hitgroup == 10))
                {
                    data.hit_player = (Player*)trace.m_entity;
                    data.hitbox = trace.m_hitbox;
                    data.hitgroup = trace.m_hitgroup;

                    if (is_taser)
                        data.hitgroup = 0;

                    this->scale_dmg(data.hit_player, cur_dmg, wpn_data->m_armor_ratio, wpn_data->m_crosshair_delta_distance, data.hitgroup);

                    data.dmg = static_cast<int>(cur_dmg);

                    return data;
                }

                last_hit_player = is_player ? (Player*)trace.m_entity : nullptr;
            }
            else
                last_hit_player = nullptr;
        }

        if (is_taser || (cur_dist > 3000.f && wpn_data->m_penetration > 0.f))
            break;

        const auto enter_surface = g_csgo.m_phys_props->GetSurfaceData(trace.m_surface.m_surface_props);
        if (enter_surface->m_game.m_penetration_modifier < 0.1f || !handle_bullet_penetration(shooter, wpn_data, trace, src, dir, data.remaining_pen, cur_dmg, pen_modifier))
            break;

        if (ignore_target)
            data.dmg = static_cast<int>(cur_dmg);
    }

    return data;
}

pen_data_t c_auto_wall::fire_emulated(Player* const shooter, Player* const target, vec3_t src, const vec3_t& dst)
{
    static const auto wpn_data = []()
        {
            WeaponInfo wpn_data{ };

            wpn_data.m_damage = 115;
            wpn_data.m_range = 8192.f;
            wpn_data.m_penetration = 2.5f;
            wpn_data.m_range_modifier = 0.99f;
            wpn_data.m_armor_ratio = 1.95f;

            return wpn_data;
        }();

        const auto pen_modifier = std::max((3.f / wpn_data.m_penetration) * 1.25f, 0.f);

        float cur_dist{ };

        pen_data_t data{ };

        data.remaining_pen = 4;

        auto cur_dmg = static_cast<float>(wpn_data.m_damage);

        auto dir = dst - src;

        const auto max_dist = dir.normalized_float();

        CGameTrace trace{ };
        CTraceFilterSkipTwoEntities_game trace_filter{ };

        Player* last_hit_player{ };

        while (cur_dmg > 0.f)
        {
            const auto dist_remaining = wpn_data.m_range - cur_dist;

            const auto cur_dst = src + dir * dist_remaining;

            trace_filter.m_pass_ent1 = shooter;
            trace_filter.m_pass_ent2 = last_hit_player;

            g_csgo.m_engine_trace->TraceRay(Ray(src, cur_dst), CS_MASK_SHOT, (ITraceFilter*)(&trace_filter), &trace);

            // interfaces::debug_overlay->add_line_overlay(trace.start, trace.end, 255, 255, 255, false, 0.1f);

            if (target)
                this->clip_trace_to_player(src, cur_dst + dir * 40.f, trace, target);

            if (trace.m_fraction == 1.f || (trace.m_endpos - src).length(false) > max_dist)
                break;

            cur_dist += trace.m_fraction * dist_remaining;
            cur_dmg *= std::pow(wpn_data.m_range_modifier, cur_dist / 500.f);

            if (trace.m_entity)
            {
                const auto is_player = trace.m_entity->IsPlayer();
                if (trace.m_entity == target)
                {
                    data.hit_player = static_cast<Player*>(trace.m_entity);
                    data.hitbox = trace.m_hitbox;
                    data.hitgroup = trace.m_hitgroup;
                    data.dmg = static_cast<int>(cur_dmg);

                    return data;
                }

                last_hit_player = is_player ? static_cast<Player*>(trace.m_entity) : nullptr;
            }
            else
                last_hit_player = nullptr;

            if (cur_dist > 3000.f && wpn_data.m_penetration > 0.f)
                break;

            const auto enter_surface = g_csgo.m_phys_props->GetSurfaceData(trace.m_surface.m_surface_props);
            if (enter_surface->m_game.m_penetration_modifier < 0.1f || !handle_bullet_penetration(shooter, &wpn_data, trace, src, dir, data.remaining_pen, cur_dmg, pen_modifier))
                break;
        }

        data.dmg = static_cast<int>(cur_dmg);

        return data;
}

