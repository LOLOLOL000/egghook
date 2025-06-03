#pragma once

#include <d3d9.h>

template<class T> struct CUtlReference {
	CUtlReference* m_pNext;
	CUtlReference* m_pPrev;
	T* m_pObject;
};
template<class T> struct CUtlIntrusiveList {
	T* m_pHead;
};
template<class T> struct CUtlIntrusiveDList : public CUtlIntrusiveList<T> {};
template<class T> struct CUtlReferenceList : public CUtlIntrusiveDList< CUtlReference<T> > {};

enum EAttributeDataType {
	ATTRDATATYPE_NONE = -1,
	ATTRDATATYPE_FLOAT = 0,
	ATTRDATATYPE_4V,
	ATTRDATATYPE_INT,
	ATTRDATATYPE_POINTER,

	ATTRDATATYPE_COUNT,
};

#define MAX_PARTICLE_ATTRIBUTES 24

#define DEFPARTICLE_ATTRIBUTE( name, bit, datatype )            \
    const int PARTICLE_ATTRIBUTE_##name##_MASK = (1 << bit);    \
    const int PARTICLE_ATTRIBUTE_##name = bit;                    \
    const EAttributeDataType PARTICLE_ATTRIBUTE_##name##_DATATYPE = datatype;

DEFPARTICLE_ATTRIBUTE(XYZ, 0, ATTRDATATYPE_4V);

// particle lifetime (duration) of particle as a float.
DEFPARTICLE_ATTRIBUTE(LIFE_DURATION, 1, ATTRDATATYPE_FLOAT);

// prev coordinates for verlet integration
DEFPARTICLE_ATTRIBUTE(PREV_XYZ, 2, ATTRDATATYPE_4V);

// radius of particle
DEFPARTICLE_ATTRIBUTE(RADIUS, 3, ATTRDATATYPE_FLOAT);

// rotation angle of particle
DEFPARTICLE_ATTRIBUTE(ROTATION, 4, ATTRDATATYPE_FLOAT);

// rotation speed of particle
DEFPARTICLE_ATTRIBUTE(ROTATION_SPEED, 5, ATTRDATATYPE_FLOAT);

// tint of particle
DEFPARTICLE_ATTRIBUTE(TINT_RGB, 6, ATTRDATATYPE_4V);

// alpha tint of particle
DEFPARTICLE_ATTRIBUTE(ALPHA, 7, ATTRDATATYPE_FLOAT);

// creation time stamp (relative to particle system creation)
DEFPARTICLE_ATTRIBUTE(CREATION_TIME, 8, ATTRDATATYPE_FLOAT);

// sequnece # (which animation sequence number this particle uses )
DEFPARTICLE_ATTRIBUTE(SEQUENCE_NUMBER, 9, ATTRDATATYPE_FLOAT);

// length of the trail
DEFPARTICLE_ATTRIBUTE(TRAIL_LENGTH, 10, ATTRDATATYPE_FLOAT);

// unique particle identifier
DEFPARTICLE_ATTRIBUTE(PARTICLE_ID, 11, ATTRDATATYPE_INT);

// unique rotation around up vector
DEFPARTICLE_ATTRIBUTE(YAW, 12, ATTRDATATYPE_FLOAT);

// second sequnece # (which animation sequence number this particle uses )
DEFPARTICLE_ATTRIBUTE(SEQUENCE_NUMBER1, 13, ATTRDATATYPE_FLOAT);

// hit box index
DEFPARTICLE_ATTRIBUTE(HITBOX_INDEX, 14, ATTRDATATYPE_INT);

DEFPARTICLE_ATTRIBUTE(HITBOX_RELATIVE_XYZ, 15, ATTRDATATYPE_4V);

DEFPARTICLE_ATTRIBUTE(ALPHA2, 16, ATTRDATATYPE_FLOAT);

// particle trace caching fields
DEFPARTICLE_ATTRIBUTE(SCRATCH_VEC, 17, ATTRDATATYPE_4V);        //scratch field used for storing arbitraty vec data
DEFPARTICLE_ATTRIBUTE(SCRATCH_FLOAT, 18, ATTRDATATYPE_4V);    //scratch field used for storing arbitraty float data
DEFPARTICLE_ATTRIBUTE(UNUSED, 19, ATTRDATATYPE_FLOAT);
DEFPARTICLE_ATTRIBUTE(PITCH, 20, ATTRDATATYPE_4V);

DEFPARTICLE_ATTRIBUTE(NORMAL, 21, ATTRDATATYPE_4V);            // 0 0 0 if none

DEFPARTICLE_ATTRIBUTE(GLOW_RGB, 22, ATTRDATATYPE_4V);            // glow color
DEFPARTICLE_ATTRIBUTE(GLOW_ALPHA, 23, ATTRDATATYPE_FLOAT);    // glow alpha

struct CParticleAttributeAddressTable {
	float* m_pAttributes[MAX_PARTICLE_ATTRIBUTES];
	size_t m_nFloatStrides[MAX_PARTICLE_ATTRIBUTES];

	__forceinline float* FloatAttributePtr(int nAttribute, int nParticleNumber) const {
		int block_ofs = nParticleNumber / 4;
		return m_pAttributes[nAttribute] +
			m_nFloatStrides[nAttribute] * block_ofs +
			(nParticleNumber & 3);
	}
};

struct CUtlString_simple {
	char* buffer;
	int capacity;
	int grow_size;
	int length;
};


class CParticleSystemDefinition {
	BYTE pad_0[308];
public:
	CUtlString_simple m_Name;
};

class CParticleCollection {
	BYTE pad_0[48];//0
public:
	int m_nActiveParticles;//48
private:
	BYTE pad_1[12];//52
public:
	CUtlReference<CParticleSystemDefinition> m_pDef;//64
private:
	BYTE pad_2[60];//80
public:
	CParticleCollection* m_pParent;//136
private:
	BYTE pad_3[84];//140
public:
	CParticleAttributeAddressTable m_ParticleAttributes;//224
};

class C_INIT_RandomColor {
	BYTE pad_0[92];
public:
	vec3_t m_normal_color_min;
	vec3_t m_normal_color_max;
};

enum soundlevel_t
{
	SNDLVL_NONE = 0,

	SNDLVL_20dB = 20,			// rustling leaves
	SNDLVL_25dB = 25,			// whispering
	SNDLVL_30dB = 30,			// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,			// refrigerator

	SNDLVL_50dB = 50,	// 3.9	// average home
	SNDLVL_55dB = 55,	// 3.0

	SNDLVL_IDLE = 60,	// 2.0	
	SNDLVL_60dB = 60,	// 2.0	// normal conversation, clothes dryer

	SNDLVL_65dB = 65,	// 1.5	// washing machine, dishwasher
	SNDLVL_STATIC = 66,	// 1.25

	SNDLVL_70dB = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,	// 0.8	// busy traffic

	SNDLVL_80dB = 80,	// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
	SNDLVL_TALKING = 80,	// 0.7
	SNDLVL_85dB = 85,	// 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,	// 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,			// helicopter, power mower
	SNDLVL_110dB = 110,			// snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,			// auto horn, propeller aircraft
	SNDLVL_130dB = 130,			// air raid siren

	SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,	// 0.2

	SNDLVL_150dB = 150,	// 0.2

	SNDLVL_180dB = 180,			// rocket launching

	// NOTE: Valid soundlevel_t values are 0-255.
	//       256-511 are reserved for sounds using goldsrc compatibility attenuation.
};

class SoundInfo_t
{
public:
	char pad_0000[4]; //0x0000
	int32_t entityIndex; //0x0004
	char pad_0008[8]; //0x0008
	vec3_t vOrigin; //0x0010
	char pad_001C[28]; //0x001C
	int32_t nFlags; //0x0038
	char pad_003C[12]; //0x003C
};

namespace min_hooks
{
	void __fastcall build_transformations(Player* ecx, void* edx, CStudioHdr* hdr, int a3, int a4, int a5, int a6, int a7);
	inline decltype(&build_transformations) orig_build_transformations{ };

	void __fastcall modify_eye_position(CCSGOPlayerAnimState* ecx, void* edx, vec3_t& pos);
	inline decltype(&modify_eye_position) orig_modify_eye_position{ };

	void __fastcall add_renderable(void* ecx, void* edx, IClientRenderable* pRenderable, bool bRenderWithViewModels, int nType, int nModelType, int nSplitscreenEnabled);
	inline decltype(&add_renderable) orig_add_renderable{ };

	bool __fastcall want_reticle_shown(void* ecx, void* edx);
	inline decltype(&want_reticle_shown) orig_want_reticle_shown{ };

	void __fastcall physics_simulate(Player* const ecx, const std::uintptr_t edx);
	inline decltype(&physics_simulate) orig_physics_simulate{ };

	void __fastcall check_file_crc_with_server(void* ecx, void* edx);
	inline decltype(&check_file_crc_with_server) orig_check_file_crc_with_server{};

	void __fastcall SetVisualsData(void* ecx, void* edx, char* pCompositingShaderName);
	inline decltype(&SetVisualsData) fn_set_visuals_data{ }; // E8 ? ? ? ? EB 02 33 DB 8B 03 + 1 [relative]

	bool __fastcall setup_bones(const std::uintptr_t ecx, const std::uintptr_t edx, matrix3x4_t* const bones, int max_bones, int mask, float time);
	inline decltype(&setup_bones) orig_setup_bones{ };

	void __fastcall update_client_side_anim(Player* const player, const std::uintptr_t edx);
	inline decltype(&update_client_side_anim) orig_update_client_side_anim{ };

	void __fastcall GetColorModulation(IMaterial* ecx, void* edx, float* r, float* g, float* b);
	inline decltype(&GetColorModulation) OrigGetColorModulation{ };

	void __fastcall GetExposureRange(float* pflAutoExposureMin, float* pflAutoExposureMax);
	inline decltype(&GetExposureRange) OrigGetExposureRange{ };

	bool __stdcall IsUsingStaticPropDebugModes();
	inline decltype(&IsUsingStaticPropDebugModes) OrigIsUsingStaticPropDebugModes{ };

	void __fastcall CL_DispatchSound(SoundInfo_t* info);
	inline decltype(&CL_DispatchSound) OrigCLDispatchSound{ };

	ang_t* __fastcall eye_angles(Player* ecx, void* edx);
	inline decltype(&eye_angles) orig_eye_angles{ };

	int __fastcall list_leaves_in_box(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		const vec3_t& mins, const vec3_t& maxs, const uint16_t* const list, const int max
	);
	inline decltype(&list_leaves_in_box) orig_list_leaves_in_box{ };

	void __fastcall init_new_particles_scalar(C_INIT_RandomColor* ecx, void* edx, CParticleCollection* particles, int start_position, int particles_count, int attribute_write_mask, void* ctx);
	inline decltype(&init_new_particles_scalar) orig_init_new_particles_scalar{ };

	void __fastcall CParticleCollection_Simulate(CParticleCollection* thisPtr, void* edx);
	inline decltype(&CParticleCollection_Simulate) OrigCParticleCollection_Simulate{ };

	void __fastcall render_glow_boxes(CGlowObjectManager* ecx, void* edx, int iPass, void* pRenderContext);
	inline decltype(&render_glow_boxes) orig_render_glow_boxes{ };
}


class Hooks {
public:
	void init( );

	bool m_updating_bones[65];
	bool m_updating_anims[65];

public:
	// forward declarations
	class IRecipientFilter;

	// prototypes.
	using PaintTraverse_t              = void( __thiscall* )( void*, VPANEL, bool, bool );
	using DoPostScreenSpaceEffects_t   = bool( __thiscall* )( void*, CViewSetup* );
	using CreateMove_t                 = bool( __thiscall* )( void*, float, CUserCmd* );
	using LevelInitPostEntity_t        = void( __thiscall* )( void* );
	using LevelShutdown_t              = void( __thiscall* )( void* );
	using IsPaused_t = bool(__thiscall*)(void*);
	using LevelInitPreEntity_t         = void( __thiscall* )( void*, const char* );
	using IN_KeyEvent_t                = int( __thiscall* )( void*, int, int, const char* );
	using FrameStageNotify_t           = void( __thiscall* )( void*, Stage_t );
	using UpdateClientSideAnimation_t  = void( __thiscall* )( void* );
    using GetActiveWeapon_t            = Weapon*( __thiscall * )( void* );
	using DoExtraBoneProcessing_t      = void( __thiscall* )( void*, int, int, int, int, int, int );
	using CalcViewModelView_t          = void( __thiscall* )( void*, vec3_t&, ang_t& );
	using InPrediction_t               = bool( __thiscall* )( void* );
	using OverrideView_t               = void( __thiscall* )( void*, CViewSetup* );
	using LockCursor_t                 = void( __thiscall* )( void* );
	using RunCommand_t                 = void( __thiscall* )( void*, Entity*, CUserCmd*, IMoveHelper* );
	using ProcessPacket_t              = void( __thiscall* )( void*, void*, bool );
	using SendDatagram_t               = int( __thiscall* )( void*, void* );
	// using CanPacket_t                = bool( __thiscall* )( void* );
	using PlaySound_t                  = void( __thiscall* )( void*, const char* );
	using GetScreenSize_t              = void( __thiscall* )( void*, int&, int& );
	using Push3DView_t                 = void( __thiscall* )( void*, CViewSetup&, int, void*, void* );
	using SceneEnd_t                   = void( __thiscall* )( void* );
	using DrawModelExecute_t           = void( __thiscall* )( void*, uintptr_t, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );
	using ComputeShadowDepthTextures_t = void( __thiscall* )( void*, const CViewSetup&, bool );
	using GetInt_t                     = int( __thiscall* )( void* );
	using GetBool_t                    = bool( __thiscall* )( void* );
	using IsConnected_t                = bool( __thiscall* )( void* );
	using IsHLTV_t                     = bool( __thiscall* )( void* );
	using OnEntityCreated_t            = void( __thiscall* )( void*, Entity* );
	using OnEntityDeleted_t            = void( __thiscall* )( void*, Entity* );
	using RenderSmokeOverlay_t         = void( __thiscall* )( void*, bool );
	using ShouldDrawFog_t              = bool( __thiscall* )( void* );
	using ShouldDrawParticles_t        = bool( __thiscall* )( void* );
	using Render2DEffectsPostHUD_t     = void( __thiscall* )( void*, const CViewSetup& );
	using OnRenderStart_t              = void( __thiscall* )( void* );
    using RenderView_t                 = void( __thiscall* )( void*, const CViewSetup &, const CViewSetup &, int, int );
	using GetMatchSession_t            = CMatchSessionOnlineHost*( __thiscall* )( void* );
	using OnScreenSizeChanged_t        = void( __thiscall* )( void*, int, int );
	using OverrideConfig_t             = bool( __thiscall* )( void*, MaterialSystem_Config_t*, bool );
	using PostDataUpdate_t             = void( __thiscall* )( void*, DataUpdateType_t );
	using TempEntities_t               = bool( __thiscall* )( void*, void * );
	using EmitSound_t                  = void( __thiscall* )( void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, float, int, int, int, const vec3_t*, const vec3_t*, void*, bool, float, int );
	// using PreDataUpdate_t            = void( __thiscall* )( void*, DataUpdateType_t );
	using CalcView_t = void(__thiscall*)(void*, vec3_t&, vec3_t&, float&, float&, float&);
	using StandardBlendingRules_t = void(__thiscall*)(void*, int, int, int, int, int);
	using DrawSetColor_t = void(__thiscall*)(void*, Color);
	using PacketStart_t = void(__thiscall*)(void*, int, int);
	using PacketEnd_t = void(__thiscall*)(void*);

public:
	void packet_start(int incoming, int outgoing);
	void packet_end();

	bool                     TempEntities( void *msg );
	void                     PaintTraverse( VPANEL panel, bool repaint, bool force );
	bool                     DoPostScreenSpaceEffects( CViewSetup* setup );
	bool                     CreateMove( float input_sample_time, CUserCmd* cmd );
	void                     CalcView(vec3_t& eye_origin, vec3_t& eye_angles, float& z_near, float& z_far, float& fov);
	void                     LevelInitPostEntity( );
	void                     LevelShutdown( );
	//int                      IN_KeyEvent( int event, int key, const char* bind );
	void                     LevelInitPreEntity( const char* map );
	void                     FrameStageNotify( Stage_t stage );
	void                     StandardBlendingRules(int a2, int a3, int a4, int a5, int a6);
    Weapon*                  GetActiveWeapon( );
	bool                     InPrediction( );
	bool                     ShouldDrawParticles( );
	bool                     ShouldDrawFog( );
	void                     OverrideView( CViewSetup* view );
	void                     LockCursor( );
	void                     PlaySound( const char* name );
	void					 HKDrawSetColor(Color col);
	bool                     IsPaused();
	void                     OnScreenSizeChanged( int oldwidth, int oldheight );
	void                     RunCommand( Entity* ent, CUserCmd* cmd, IMoveHelper* movehelper );
	int                      SendDatagram( void* data );
	void                     ProcessPacket( void* packet, bool header );
	//void                     GetScreenSize( int& w, int& h );
	void                     SceneEnd( );
	void                     DrawModelExecute( uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone );
	void                     ComputeShadowDepthTextures( const CViewSetup& view, bool unk );
	int                      DebugSpreadGetInt( );
	bool                     NetShowFragmentsGetBool( );
	void                     DoExtraBoneProcessing( int a2, int a3, int a4, int a5, int a6, int a7 );
	bool                     IsConnected( );
	bool                     IsHLTV( );
	void                     EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const vec3_t* pOrigin, const vec3_t* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity );
	void                     RenderSmokeOverlay( bool unk );
	void                     OnRenderStart( );
    void                     RenderView( const CViewSetup &view, const CViewSetup &hud_view, int clear_flags, int what_to_draw );
	void                     Render2DEffectsPostHUD( const CViewSetup& setup );
	CMatchSessionOnlineHost* GetMatchSession( );
	bool                     OverrideConfig( MaterialSystem_Config_t* config, bool update );
	void                     PostDataUpdate( DataUpdateType_t type );

	static LRESULT WINAPI WndProc( HWND wnd, uint32_t msg, WPARAM wp, LPARAM lp );

public:
	// vmts.
	VMT m_panel;
	VMT m_client_mode;
	VMT m_client;
	VMT m_client_state;
	VMT m_engine;
	VMT m_engine_sound;
	VMT m_prediction;
	VMT m_surface;
	VMT m_render;
	VMT m_net_channel;
	VMT m_render_view;
	VMT m_model_render;
	VMT m_shadow_mgr;
	VMT m_view_render;
	VMT m_match_framework;
	VMT m_material_system;
	VMT m_fire_bullets;
	VMT m_net_show_fragments;

	// player shit.
	std::array< VMT, 64 > m_player;

	// cvars
	VMT m_debug_spread;

	// wndproc old ptr.
	WNDPROC m_old_wndproc;

	// old player create fn.
	StandardBlendingRules_t     m_StandardBlendingRules;
	DoExtraBoneProcessing_t     m_DoExtraBoneProcessing;
	UpdateClientSideAnimation_t m_UpdateClientSideAnimation;
    GetActiveWeapon_t           m_GetActiveWeapon;
	CalcView_t                  m_1CalcView;

	// netvar proxies.
	RecvVarProxy_t m_Pitch_original;
	RecvVarProxy_t m_Body_original;
    RecvVarProxy_t m_Force_original;
	RecvVarProxy_t m_AbsYaw_original;
};

class Entity;

struct listened_entity_t
{
	Entity* m_entity{ };
	int m_idx{ };
	int m_class_id{ };

	listened_entity_t() = default;

	listened_entity_t(Entity* ent)
	{
		m_entity = ent;
		m_idx = ent->index();
		m_class_id = ent->GetClientClass()->m_ClassID;
	}
};

enum listener_ent_t
{
	ent_invalid = -1,
	ent_player,
	ent_weapon,
	ent_fog,
	ent_tonemap,
	ent_light,
	ent_ragdoll,
	ent_c4,
	ent_max,
};


// note - dex; these are defined in player.cpp.
class CustomEntityListener : public IEntityListener {
private:
	std::array< std::vector< listened_entity_t >, ent_max > ent_lists{ };

public:
	const std::vector< listened_entity_t >& get_entity(int type);

    virtual void OnEntityCreated( Entity *ent );
    virtual void OnEntityDeleted( Entity *ent );

    __forceinline void init( ) {
        g_csgo.AddListenerEntity( this );
    }
};

extern Hooks                g_hooks;
extern CustomEntityListener g_custom_entity_listener;