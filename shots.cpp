#include "shots.h"
#include "resolver.h"

Shots g_shots{ };

static int ClipRayToHB(const Ray& ray, mstudiobbox_t* hitbox, matrix3x4_t& matrix, CGameTrace& trace)
{
	static auto fn = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 F3 0F 10 42"));

	if (!fn || !hitbox)
		return -1;

	trace.m_fraction = 1.0f;
	trace.m_startsolid = false;

	return fn.as<int(__fastcall*)(const Ray&, mstudiobbox_t*, matrix3x4_t&, CGameTrace&)>()(ray, hitbox, matrix, trace);
}


void Shots::OnImpact(IGameEvent* evt)
{
	int        attacker;
	vec3_t     pos, dir, start, end;
	float      time;
	CGameTrace trace;

	// screw this.
	if (!evt || !g_cl.m_local)
		return;

	// get attacker, if its not us, screw it.
	attacker = g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (attacker != g_csgo.m_engine->GetLocalPlayer())
		return;

	// decode impact coordinates and convert to vec3.
	pos = {
		evt->GetFloat(XOR("x")),
		evt->GetFloat(XOR("y")),
		evt->GetFloat(XOR("z"))
	};

	// get prediction time at this point.
	time = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

	if (const auto shot = last_unprocessed())
		shot->m_server_info.m_impact_pos = pos;
}

void Shots::OnHurt(IGameEvent* evt)
{
	int         group, hp;
	float       time, damage;
	std::string name;

	if (!evt || !g_cl.m_local)
		return;

	const auto attacker = g_csgo.m_entlist->GetClientEntity(g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("attacker"))->GetInt()));
	if (!attacker)
		return;

	const auto victim = g_csgo.m_entlist->GetClientEntity(g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt()));
	if (!victim)
		return;

	// skip invalid player indexes.
	// should never happen? world entity could be attacker, or a nade that hits you.
	if (attacker->index() < 1 || attacker->index() > 64 || victim->index() < 1 || victim->index() > 64)
		return;

	// we were not the attacker or we hurt ourselves.
	else if (attacker != g_cl.m_local || victim == g_cl.m_local)
		return;

	// get hitgroup.
	// players that get naded ( DMG_BLAST ) or stabbed seem to be put as HITGROUP_GENERIC.
	group = evt->m_keys->FindKey(HASH("hitgroup"))->GetInt();

	// invalid hitgroups ( note - dex; HITGROUP_GEAR isn't really invalid, seems to be set for hands and stuff? ).
	if (group == HITGROUP_GEAR)
		return;

	// get player info.
	player_info_t info;
	if (!g_csgo.m_engine->GetPlayerInfo(victim->index(), &info))
		return;

	// get player name;
	name = std::string(info.m_name).substr(0, 24);

	// get damage reported by the server.
	damage = (float)evt->m_keys->FindKey(HASH("dmg_health"))->GetInt();

	// get remaining hp.
	hp = evt->m_keys->FindKey(HASH("health"))->GetInt();

	// get prediction time at this point.
	time = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

	// hitmarker.
	g_visuals.m_hit_duration = 1.f;
	g_visuals.m_hit_start = g_csgo.m_globals->m_curtime;
	g_visuals.m_hit_end = g_visuals.m_hit_start + g_visuals.m_hit_duration;

	g_csgo.m_sound->EmitAmbientSound(XOR("buttons/arena_switch_press_02.wav"), 1.f);


	// print this shit.
	if (group == HITGROUP_GENERIC)
		return;

	// if we hit a player, mark vis impacts.
	if (!m_vis_impacts.empty())
	{
		for (auto& i : m_vis_impacts)
		{
			if (i.m_tickbase == g_cl.m_local->m_nTickBase())
				i.m_hit_player = true;
		}
	}

	// we killed him with a headshot
	if (int(damage) >= hp && group == HITGROUP_HEAD)
		++m_headtaps;

	if (g_menu.main.misc.notifications.get(1))
	{
		std::string out = tfm::format(XOR("Hit %s in the %s for %i damage (%i health remaining)\n"), name, m_groups[group], (int)damage, hp);
		g_notify.add(out);
	}

	const auto shot = last_unprocessed();
	if (!shot
		|| (shot->m_target && shot->m_target_index != victim->index()))
		return;

	shot->m_server_info.m_hitgroup = group;
	shot->m_server_info.m_dmg = evt->GetInt(XOR("dmg_health"));
	shot->m_server_info.m_hurt_tick = g_csgo.m_cl->clock_drift_mgr.server_tick;
}

void Shots::OnWeaponFire(IGameEvent* evt)
{
	int        attacker;

	// screw this.
	if (!evt || !g_cl.m_local)
		return;

	// get attacker, if its not us, screw it.
	attacker = g_csgo.m_engine->GetPlayerForUserID(evt->m_keys->FindKey(HASH("userid"))->GetInt());
	if (attacker != g_csgo.m_engine->GetLocalPlayer())
		return;

	const auto shot = std::find_if(
		m_shots.begin(), m_shots.end(),
		[](const ShotRecord& shot)
		{
			return shot.m_cmd_number != -1 && !shot.m_server_info.m_fire_tick
				&& std::abs(g_csgo.m_cl->last_command_ack - shot.m_cmd_number) <= 20;
		}
	);

	if (shot == m_shots.end())
		return;

	// this shot was matched.
	shot->m_process_tick = g_csgo.m_globals->m_realtime + 2.5f;
	shot->m_server_info.m_fire_tick = g_csgo.m_cl->clock_drift_mgr.server_tick;
}

void Shots::Think()
{
	if (!g_csgo.m_engine->IsInGame()  || !g_cl.m_local) {
		return m_shots.clear();
	}

	for (auto& shot : m_shots) {
		if (shot.m_processed
			|| g_csgo.m_globals->m_realtime > shot.m_process_tick)
			continue;

		if (shot.m_target
			&& shot.m_record) {

			if (!shot.m_target->alive()) {
			}
			else {
				AimPlayer* data = &g_aimbot.m_players[shot.m_target->index() - 1];
				if (!data)
					return;

				if (!shot.m_server_info.m_hurt_tick) {

					// this record was deleted already.
					if (!shot.m_record->m_bones)
						return;

					auto pModel = shot.m_target->GetModel();
					if (!pModel)
						return;

					auto pHdr = g_csgo.m_model_info->GetStudioModel(pModel);
					if (!pHdr)
						return;

					auto pHitboxSet = pHdr->GetHitboxSet(shot.m_target->m_nHitboxSet());

					if (!pHitboxSet)
						return;

					auto pHitbox = pHitboxSet->GetHitbox(shot.m_hitbox);

					if (!pHitbox)
						return;

					BackupRecord backup;
					backup.store(shot.m_target);

					// cache
					shot.m_record->cache();

					CGameTrace trace{};
					auto bHit = ClipRayToHB(Ray(shot.m_src, shot.m_server_info.m_impact_pos), pHitbox, shot.m_record->m_bones[pHitbox->m_bone], trace) >= 0;

					if (!bHit) {
						g_notify.add(XOR("missed shot due to spread\n"));
					}
					else {

						size_t mode = shot.m_record->m_mode;

						switch (mode)
						{
						case Resolver::Modes::RESOLVE_BODY:
							++data->m_body_index;
							break;
						case Resolver::Modes::RESOLVE_DELTA:
							++data->m_bruteforce_idx;
							break;
						case Resolver::Modes::RESOLVE_LBY:
							++data->m_lby_index;
							break;
						case Resolver::Modes::RESOLVE_STAND2:
							++data->m_stand_index2;
							break;
						case Resolver::Modes::RESOLVE_NO_UPDATE_LBY:
							++data->m_no_flick_index;
							break;
						case Resolver::Modes::RESOLVE_AIR_FLICK:
							// catch flick angle missed on only the first miss.
							if (!data->m_air_flick_miss)
								data->m_flick_angle_missed = data->m_body_flick;

							++data->m_air_flick_miss;
							break;
						}

						++data->m_missed_shots;

						g_notify.add(XOR("missed shot due to fake angles\n"));
					}

					backup.restore(shot.m_record->m_player);
				}
			}
		}

		shot.m_processed = true;
	}

	m_shots.erase(std::remove_if(
		m_shots.begin(), m_shots.end(),
		[&](const ShotRecord& shot)
		{
			if (!shot.m_processed
				&& shot.m_process_tick)
				return false;

			if (std::abs(g_csgo.m_globals->m_realtime - shot.m_shot_time) >= 2.5f)
				return true;

			if (shot.m_processed
				|| shot.m_process_tick
				|| shot.m_target_index == -1
				|| shot.m_cmd_number == -1
				|| std::abs(g_csgo.m_cl->last_command_ack - shot.m_cmd_number) <= 20)
				return false;

			const auto delta = std::abs(g_csgo.m_globals->m_realtime - shot.m_sent_time);
			if (delta > 2.5f)
				return true;

			if (delta <= shot.m_latency)
				return false;

			return true;
		}
	),
		m_shots.end()
	);
}