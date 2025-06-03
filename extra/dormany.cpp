#include "dormany.h"

void c_dormant_esp::start()
{
	m_sound_list.RemoveAll();
	g_csgo.m_sound->get_act_sounds(m_sound_list);

	if (!m_sound_list.Count())
		return;

	for (auto i = 0; i < m_sound_list.Count(); i++)
	{
		auto& sound = m_sound_list.Element(i);

		if (sound.m_source < 1 || sound.m_source > 64)
			continue;

		if (sound.m_p_origin->x == 0.f && sound.m_p_origin->y == 0.f && sound.m_p_origin->z == 0.f)
			continue;

		if (!valid_sound(sound))
			continue;

		auto player = g_csgo.m_entlist->GetClientEntity<Player*>(sound.m_source);

		if (!player || !player->alive() || !player->enemy(g_cl.m_local) ||
			player->m_bIsLocalPlayer())
			continue;

		setup_adjust(player, sound);
		m_sound_players[sound.m_source].override(sound);
	}

	m_sound_buffer = m_sound_list;
}

void c_dormant_esp::setup_adjust(Player* player, snd_info_t& sound)
{
	vec3_t src3D, dst3D;
	CGameTrace tr;
	CTraceFilterSimple filter;

	src3D = *sound.m_p_origin + vec3_t(0.0f, 0.0f, 1.0f);
	dst3D = src3D - vec3_t(0.0f, 0.0f, 100.0f);

	filter.m_pass_ent1 = player;
	Ray ray = Ray{ src3D, dst3D };

	g_csgo.m_engine_trace->TraceRay(ray, MASK_PLAYERSOLID, reinterpret_cast <ITraceFilter*> (&filter), &tr);

	if (tr.m_allsolid)
		m_sound_players[sound.m_source].m_receive_time = -1;

	*sound.m_p_origin = tr.m_fraction <= 0.97f ? tr.m_endpos : *sound.m_p_origin;
	m_sound_players[sound.m_source].m_flags = player->m_fFlags();
	m_sound_players[sound.m_source].m_flags |= (tr.m_fraction < 0.50f ? FL_DUCKING : 0) | (tr.m_fraction < 1.0f ? FL_ONGROUND : 0);
	m_sound_players[sound.m_source].m_flags &= (tr.m_fraction >= 0.50f ? ~FL_DUCKING : 0) | (tr.m_fraction >= 1.0f ? ~FL_ONGROUND : 0);
}

bool c_dormant_esp::adjust_sound(Player* entity)
{
	auto i = entity->index();
	auto sound_player = m_sound_players[i];

	//entity->spotted( ) = true;
	entity->m_fFlags() = sound_player.m_flags;
	entity->SetAbsOrigin(sound_player.m_origin);

	return (fabs(g_csgo.m_globals->m_curtime - sound_player.m_receive_time) < 2.5f);
}

bool c_dormant_esp::valid_sound(snd_info_t& sound)
{
	for (auto i = 0; i < m_sound_buffer.Count(); i++)
		if (m_sound_buffer.Element(i).m_guid == sound.m_guid)
			return false;

	return true;
}