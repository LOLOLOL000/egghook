#pragma once

struct OffScreenDamageData_t {
    float m_time, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t( ) : m_time{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t( float time, float m_color_step, Color color ) : m_time{ time }, m_color{ color } {}
};

struct shot_mdl_t {
    int                         m_player_index{ };
    unsigned int                m_hash{ };
    float						m_time{ }, m_alpha{ 1.f }, m_is_death{ };
    matrix3x4_t				m_bones[128];
    matrix3x4_t					m_world_matrix{ };


    ModelRenderInfo_t	m_info{ };
    DrawModelState_t	m_state{ };
};

struct dormant_data_t {
    float m_alpha{ };
    bool m_was_unseen{ };
    vec3_t m_origin{ };
    float m_receive_time{ };
    int m_weapon_id{ };
    int m_weapon_type{ -1 };
    bool m_use_shared{ };
    float m_last_shared_time{ };
};


struct InfernoData {
    InfernoData(Entity* inferno) noexcept;

    std::vector<vec3_t> points;
};


struct impact_info {
    float x, y, z;
    float time;
};

struct hitmarker_info {
    impact_info impact;
    int alpha;
    float time;
    int damage;
    Player* player;
};

struct client_hit_verify_t {
    vec3_t pos;
    float time;
    float expires;
};

class Visuals {
public:
    std::array<dormant_data_t, 65> m_dormant_data{ };
    std::vector< shot_mdl_t >		m_shot_mdls{ };
	std::array< bool, 64 >                  m_draw;
	std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
	vec2_t                                  m_crosshair;
    std::vector<impact_info>                impacts;
    std::vector<hitmarker_info>             hitmarkers;
	bool                                    m_thirdperson;
	float					                m_hit_start, m_hit_end, m_hit_duration;

    // info about planted c4.
    bool        m_c4_planted;
    Entity      *m_planted_c4;
    float       m_planted_c4_explode_time;
    vec3_t      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    std::string m_last_bombsite;

	IMaterial* smoke1;
	IMaterial* smoke2;
	IMaterial* smoke3;
	IMaterial* smoke4;
    IMaterial* molotov1;
    IMaterial* molotov2;
    IMaterial* molotov3;
    std::vector<InfernoData> infernoData;

    std::unordered_map< int, char > m_weapon_icons = {
    	{ DEAGLE, 'F' },
    	{ ELITE, 'S' },
    	{ FIVESEVEN, 'U' },
    	{ GLOCK, 'C' },
    	{ AK47, 'B' },
    	{ AUG, 'E' },
    	{ AWP, 'R' },
    	{ FAMAS, 'T' },
    	{ G3SG1, 'I' },
    	{ GALIL, 'V' },
    	{ M249, 'Z' },
    	{ M4A4, 'W' },
    	{ MAC10, 'L' },
    	{ P90, 'M' },
    	{ UMP45, 'Q' },
    	{ XM1014, ']' },
    	{ BIZON, 'D' },
    	{ MAG7, 'K' },
    	{ NEGEV, 'Z' },
    	{ SAWEDOFF, 'K' },
    	{ TEC9, 'C' },
    	{ ZEUS, 'Y' },
    	{ P2000, 'Y' },
    	{ MP7, 'X' },
    	{ MP9, 'D' },
    	{ NOVA, 'K' },
    	{ P250, 'Y' },
    	{ SCAR20, 'I' },
    	{ SG553, '[' },
    	{ SSG08, 'N' },
    	{ KNIFE_CT, 'J' },
    	{ FLASHBANG, 'G' },
    	{ HEGRENADE, 'H' },
    	{ SMOKE, 'P' },
    	{ MOLOTOV, 'H' },
    	{ DECOY, 'G' },
    	{ FIREBOMB, 'H' },
    	{ C4, '\\' },
    	{ KNIFE_T, 'J' },
    	{ M4A1S, 'W' },
    	{ USPS, 'Y' },
    	{ CZ75A, 'Y' },
    	{ REVOLVER, 'F' },
    	{ KNIFE_BAYONET, 'J' },
    	{ KNIFE_FLIP, 'J' },
    	{ KNIFE_GUT, 'J' },
    	{ KNIFE_KARAMBIT, 'J' },
    	{ KNIFE_M9_BAYONET, 'J' },
    	{ KNIFE_HUNTSMAN, 'J' },
    	{ KNIFE_FALCHION, 'J' },
    	{ KNIFE_BOWIE, 'J' },
    	{ KNIFE_BUTTERFLY, 'J' },
    	{ KNIFE_SHADOW_DAGGERS, 'J' },
    };

public:
   static void ModulateWorld();
	void ThirdpersonThink( );
	void Hitmarker( );
    void Hitmarker3D();
	void NoSmoke( );
	void think( );
    void ImpactData();
	void Spectators( );
	void StatusIndicators( );
	void SpreadCrosshair( );
    void PenetrationCrosshair( );
    void DrawPlantedC4();
	void draw( Entity* ent );
	void DrawProjectile( Weapon* ent );
	void DrawItem( Weapon* item );
	void OffScreen( Player* player, int alpha );
	void DrawPlayer( Player* player );
    void Update();
    void drawMolotovHull();
	bool GetPlayerBoxRect( Player* player, Rect& box );
	void DrawHistorySkeleton( Player* player, int opacity );
    void ManualAntiAim();
	void DrawSkeleton( Player* player, int opacity );
	void RenderGlow( );
	void DrawHitboxMatrix( LagRecord* record, Color col, float time );
    void DrawBeams( );
};

extern Visuals g_visuals;