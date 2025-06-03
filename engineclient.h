#pragma once

class INetChannel {
public:
	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	enum {
		FLOW_OUTGOING = 0,
		FLOW_INCOMING = 1,
		MAX_FLOWS     = 2
	};

	enum indices : size_t {
		GETLATENCY    = 9,
		GETAVGLATENCY = 10,
		PROCESSPACKET = 41,
		SENDNETMSG    = 42,
		SENDDATAGRAM  = 48,
	};

public:
	__forceinline float GetLatency( int flow ) {
		return util::get_method< float( __thiscall* )( decltype( this ), int ) >( this, GETLATENCY )( this, flow );
	}

	__forceinline float GetAvgLatency( int flow ) {
		return util::get_method< float( __thiscall* )( decltype( this ), int ) >( this, GETAVGLATENCY )( this, flow );
	}

	int SendDatagram(uintptr_t* datagram = nullptr) {
		return util::get_method< bool(__thiscall*)(decltype(this), uintptr_t*) >(this, 48)(this, datagram);
	}

	__forceinline bool SendNetMsg( void* msg, bool rel = false, bool voice = false ) {
		return util::get_method< bool( __thiscall* )( decltype( this ), void*, bool, bool ) >( this, SENDNETMSG )( this, msg, rel, voice );
	}

private:
	PAD( 0x14 );

public:
	bool m_processing_messages;		// 0x0014
	bool m_should_delete;			// 0x0015

private:
	PAD( 0x2 );

public:
	int m_out_seq;					// 0x0018 last send outgoing sequence number
	int m_in_seq;					// 0x001C last received incoming sequnec number
	int m_out_seq_ack;				// 0x0020 last received acknowledge outgoing sequnce number
	int m_out_rel_state;			// 0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int m_in_rel_state;				// 0x0028 state of incoming reliable data
	int m_choked_packets;			// 0x002C number of choked packets

private:
	PAD( 0x414 );					// 0x0030
};

struct player_info_t {
	uint64_t      m_data_map;
	union {
		int64_t   m_xuid;
		struct {
			int   m_xuid_low;
			int   m_xuid_high;
		};
	};
	char          m_name[ 128 ];
	int           m_user_id;
	char          m_guid[ 33 ];
	uint32_t      m_friends_id;
	char          m_friends_name[ 128 ];
	bool          m_fake_player;
	bool          m_is_hltv;
	uint32_t      m_custom_files[ 4 ];
	uint8_t       m_files_downloaded;
};

class CEventInfo {
public:
	enum {
		EVENT_INDEX_BITS    = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA      = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use
	short					m_class_id;
	float					m_fire_delay;
	const void*				m_send_table;
	const ClientClass*		m_client_class;
	int						m_bits;
	uint8_t*				m_data;
	int						m_flags;
	PAD( 0x18 );
	CEventInfo*				m_next;
};

class CClientState {
public:
	class c_clock_drift_manager
	{
	public:
		float clock_offsets[0x10]{ };
		uint32_t cur_clock_offset{ };
		uint32_t server_tick{ };
		uint32_t client_tick{ };
	};

	PAD(0x9C);

	INetChannel* net_channel_ptr{ };
	int challenge_nr{ };

	PAD(0x4);

	double connect_time{ };
	int retry_number{ };

	PAD(0x54);

	int signon_state{ };

	PAD(0x4);

	double next_cmd_time{ };
	int server_count{ };
	int current_sequence{ };

	PAD(0x8);

	c_clock_drift_manager clock_drift_mgr{ };

	int delta_tick{ };

	PAD(19104);

	int max_clients{ };

	PAD(140);

	float frame_time{ };
	int last_outgoing_command{ };
	int choked_commands{ };
	int last_command_ack{ };

	PAD(308);

	CEventInfo* events{ };

public:
	enum indices : size_t {
		PACKETSTART = 5,
		PACKETEND = 6,
		TEMPENTITIES = 36,
	};
};

class CGlobalState {
public:
	uintptr_t*    m_manager;
	CClientState* m_client_state;
};

class IVEngineClient {
public:
	enum indices : size_t {
		GETSCREENSIZE            = 5,
		GETPLAYERINFO            = 8,
		GETPLAYERFORUSERID       = 9,
		GETLOCALPLAYER           = 12,
		GETLASTTIMESTAMP         = 14,
		GETVIEWANGLES            = 18,
		SETVIEWANGLES            = 19,
		//GETMAXCLIENTS          = 20,
		ISINGAME                 = 26,
		ISCONNECTED              = 27,
		WORLDTOSCREENMATRIX      = 37,
		FIREEVENTS               = 59,
		GETNETCHANNELINFO        = 78,
		ISPAUSED                 = 90,
		//ISTAKINGSCREENSHOT     = 92,
		ISHLTV                   = 93,
		//SETOCCLUSIONPARAMETERS = 96,
		EXECUTECLIENTCMD         = 108,
	};

public:
	__forceinline void GetScreenSize( int& width, int& height ) {
		return util::get_method< void( __thiscall* )( decltype( this ), int&, int& ) >( this, GETSCREENSIZE )( this, width, height );
	}

	__forceinline bool GetPlayerInfo( int index, player_info_t* info ) {
		return util::get_method< bool( __thiscall* )( decltype( this ), int, player_info_t* ) >( this, GETPLAYERINFO )( this, index, info );
	}

	__forceinline int GetPlayerForUserID( int uid ) {
		return util::get_method< int( __thiscall* )( decltype( this ), int ) >( this, GETPLAYERFORUSERID )( this, uid );
	}

	__forceinline int GetLocalPlayer( ) {
		return util::get_method< int( __thiscall* )( decltype( this ) ) >( this, GETLOCALPLAYER )( this );
	}

	__forceinline float GetLastTimestamp( ) {
		return util::get_method< float( __thiscall* )( decltype( this ) ) >( this, GETLASTTIMESTAMP )( this );
	}

	__forceinline void GetViewAngles( ang_t& angle ) {
		return util::get_method< void( __thiscall* )( decltype( this ), ang_t& ) >( this, GETVIEWANGLES )( this, angle );
	}

	__forceinline void SetViewAngles( ang_t& angle ) {
		return util::get_method< void( __thiscall* )( decltype( this ), ang_t& ) >( this, SETVIEWANGLES )( this, angle );
	}

	__forceinline bool IsInGame( ) {
		return util::get_method< bool( __thiscall* )( decltype( this ) ) >( this, ISINGAME )( this );
	}

	__forceinline const VMatrix& WorldToScreenMatrix( ) {
		return util::get_method< const VMatrix&( __thiscall* )( decltype( this ) ) >( this, WORLDTOSCREENMATRIX )( this );
	}

	__forceinline void FireEvents( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, FIREEVENTS )( this );
	}

	__forceinline std::uintptr_t* bsp_tree_query() {
		using fn_t = std::uintptr_t* (__thiscall*)(decltype(this));

		return (*reinterpret_cast<fn_t**> (this))[43u](this);
	}

	__forceinline INetChannel* GetNetChannelInfo( ) {
		return util::get_method< INetChannel*( __thiscall* )( decltype( this ) ) >( this, GETNETCHANNELINFO )( this );
	}

	__forceinline bool IsPaused( ) {
		return util::get_method< bool( __thiscall* )( decltype( this ) ) >( this, ISPAUSED )( this );
	}

	__forceinline void ExecuteClientCmd( const char* cmd ) {
		return util::get_method< void( __thiscall* )( decltype( this ), const char* )>( this, EXECUTECLIENTCMD )( this, cmd );
	}

	//__forceinline void SetOcclusionParameters( float min, float max ) {
	//	float arr[] = { min, max };
	//	return util::get_method< void( __thiscall* )( decltype( this ), float* ) >( this, SETOCCLUSIONPARAMETERS )( this, arr );
	//}
};