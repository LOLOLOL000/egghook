#pragma once
#include "includes.h"

class c_dormant_esp {
public:
	void start();

	bool adjust_sound(Player* player);
	void setup_adjust(Player* player, snd_info_t& sound);
	bool valid_sound(snd_info_t& sound);

	struct SoundPlayer {
		void reset(bool store_data = false, const vec3_t& origin = { }, int flags = 0)
		{
			if (store_data)
			{
				m_receive_time = g_csgo.m_globals->m_curtime;
				m_origin = origin;
				m_flags = flags;
			}
			else
			{
				m_receive_time = 0.0f;
				m_origin = { };
				m_flags = 0;
			}
		}

		void override(snd_info_t& sound)
		{
			m_receive_time = g_csgo.m_globals->m_curtime;
			m_origin = *sound.m_p_origin;
		}

		float m_receive_time = 0.0f;
		vec3_t m_origin = { };
		int m_flags = { };
	} m_sound_players[65];

	CUtlVector< snd_info_t > m_sound_buffer;
	CUtlVector< snd_info_t > m_sound_list;
};

inline std::unique_ptr < c_dormant_esp > g_dormant_esp = std::make_unique < c_dormant_esp >();