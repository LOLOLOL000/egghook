#pragma once

typedef void* file_name_handle_t;
struct snd_info_t {
	int			m_guid;
	file_name_handle_t m_handle;
	int			m_source;
	int			m_channel;
	int			m_speaker_ent;
	float		m_volume;
	float		m_last_spatialized_volume;
	float		m_radius;
	int			m_pitch;
	vec3_t* m_p_origin;
	vec3_t* m_p_direction;
	bool		m_update_pos;
	bool		m_is_sentence;
	bool		m_dry_mix;
	bool		m_speaker;
	bool		m_from_server;
};

class IEngineSound {
public:
	// indexes for virtuals and hooks.
	enum indices : size_t {
		EMITAMBIENTSOUND = 12,
		EMITSOUND = 5,
	};

	enum {
		PITCH_NORM = 100,	// non-pitch shifted
		PITCH_LOW = 95,		// other values are possible - 0-255, where 255 is very high
		PITCH_HIGH = 120
	};

	__forceinline void EmitAmbientSound( const char* sample, float volume, int pitch = PITCH_NORM, int flags = 0, float time = 0.f ) {
		return util::get_method< void( __thiscall* )( void*, const char*, float, int, int, float ) >( this, EMITAMBIENTSOUND )( this, sample, volume, pitch, flags, time );
	}

	__forceinline void get_act_sounds(CUtlVector < snd_info_t >& snd_list) {
		using fn_t = void(__thiscall*)(decltype(this), CUtlVector < snd_info_t >&);

		return (*reinterpret_cast<fn_t**> (this))[19u](this, snd_list);
	};
};