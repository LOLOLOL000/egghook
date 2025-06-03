#include "includes.h"

bool Hooks::IsConnected( ) {
	Stack stack;

	static Address IsLoadoutAllowed{ pattern::find( g_csgo.m_client_dll, XOR( "84 C0 75 04 B0 01 5F" ) ) };

	if( g_menu.main.misc.unlock.get( ) && stack.ReturnAddress( ) == IsLoadoutAllowed )
		return false;

	return g_hooks.m_engine.GetOldMethod< IsConnected_t >( IVEngineClient::ISCONNECTED )( this );
}

bool Hooks::IsPaused() {
	if (!this || !g_csgo.m_engine || !g_csgo.m_engine->IsInGame())
		return g_hooks.m_engine.GetOldMethod< IsPaused_t >(IVEngineClient::ISPAUSED)(this);

	static DWORD* return_to_extrapolation = (DWORD*)(pattern::find(g_csgo.m_client_dll,
		XOR("FF D0 A1 ?? ?? ?? ?? B9 ?? ?? ?? ?? D9 1D ?? ?? ?? ?? FF 50 34 85 C0 74 22 8B 0D ?? ?? ?? ??")).add(0x29).as<DWORD*>());

	if (_ReturnAddress() == (void*)return_to_extrapolation)
		return true;

	return g_hooks.m_engine.GetOldMethod< IsPaused_t >(IVEngineClient::ISPAUSED)(this);
}


bool Hooks::IsHLTV( ) {
	Stack stack;

	static Address SetupVelocity{ pattern::find(g_csgo.m_client_dll, XOR("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80")) };
	static Address AccumulateLayers{ pattern::find(g_csgo.m_client_dll, XOR("84 C0 75 0D F6 87")) };

	// AccumulateLayers
	if( g_bones.m_running )
		return true;

	// fix for animstate velocity.
	if (stack.ReturnAddress() == SetupVelocity)
		return true;

	if (stack.ReturnAddress() == AccumulateLayers)
		return true;

	return g_hooks.m_engine.GetOldMethod< IsHLTV_t >( IVEngineClient::ISHLTV )( this );
}

void Hooks::EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const vec3_t* pOrigin, const vec3_t* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity ) {

	// fix double land/footstep sounds
	/*if (iEntIndex == g_cl.m_local->index())
		if (g_csgo.m_prediction->m_in_prediction && !g_csgo.m_prediction->m_first_time_predicted)
			if (strstr(pSoundEntry, "land"))
				flVolume = 0.f;*/

	g_hooks.m_engine_sound.GetOldMethod<EmitSound_t>( IEngineSound::EMITSOUND )( this, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity );
}