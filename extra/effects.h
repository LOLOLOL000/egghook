#pragma once

class CTeslaInfo
{
public:
	vec3_t			m_vPos;
	ang_t			m_vAngles;
	int				m_nEntIndex;
	const char* m_pszSpriteName;
	float			m_flBeamWidth;
	int				m_nBeams;
	vec3_t			m_vColor;
	float			m_flTimeVisible;
	float			m_flRadius;
};

class IEffects
{
public:
	virtual ~IEffects() {};

	virtual void Beam(const vec3_t& Start, const vec3_t& End, int nModelIndex,
		int nHaloIndex, unsigned char frameStart, unsigned char frameRate,
		float flLife, unsigned char width, unsigned char endWidth, unsigned char fadeLength,
		unsigned char noise, unsigned char red, unsigned char green,
		unsigned char blue, unsigned char brightness, unsigned char speed) = 0;

	//-----------------------------------------------------------------------------
	// Purpose: Emits smoke sprites.
	// Input  : origin - Where to emit the sprites.
	//			scale - Sprite scale * 10.
	//			framerate - Framerate at which to animate the smoke sprites.
	//-----------------------------------------------------------------------------
	virtual void Smoke(const vec3_t& origin, int modelIndex, float scale, float framerate) = 0;

	virtual void Sparks(const vec3_t& position, int nMagnitude = 1, int nTrailLength = 1, const vec3_t* pvecDir = NULL) = 0;

	virtual void Dust(const vec3_t& pos, const vec3_t& dir, float size, float speed) = 0;

	virtual void MuzzleFlash(const vec3_t& vecOrigin, const ang_t& vecAngles, float flScale, int iType) = 0;

	// like ricochet, but no sound
	virtual void MetalSparks(const vec3_t& position, const vec3_t& direction) = 0;

	virtual void EnergySplash(const vec3_t& position, const vec3_t& direction, bool bExplosive = false) = 0;

	virtual void Ricochet(const vec3_t& position, const vec3_t& direction) = 0;

	// FIXME: Should these methods remain in this interface? Or go in some 
	// other client-server neutral interface?
	virtual float Time() = 0;
	virtual bool IsServer() = 0;

	// Used by the playback system to suppress sounds
	virtual void SuppressEffectsSounds(bool bSuppress) = 0;
};