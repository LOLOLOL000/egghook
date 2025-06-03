#pragma once

class ShotRecord;

class Resolver {
public:
	enum Modes : size_t {
		RESOLVE_NONE = 0,
		RESOLVE_WALK,
		RESOLVE_STAND,
		RESOLVE_STAND1,
		RESOLVE_STAND2,
		RESOLVE_AIR,
		RESOLVE_BODY,
		RESOLVE_STOPPED_MOVING,
		RESOLVE_LBY,
		RESOLVE_DELTA,
		RESOLVE_NO_UPDATE_LBY,
		RESOLVE_AIR_FLICK,
	};

public:
	LagRecord* FindIdealRecord(AimPlayer* data);
	LagRecord* FindLastRecord(AimPlayer* data);

	void OnBodyUpdate( Player* player, float value );
	float GetAwayAngle( LagRecord* record );

	void MatchShot( AimPlayer* data, LagRecord* record );
	void SetMode( LagRecord* record );

	void ResolveAngles( Player* player, LagRecord* record );
	void ResolveWalk( AimPlayer* data, LagRecord* record );
	void ResolveStand( AimPlayer* data, LagRecord* record );
	void StandNS( AimPlayer* data, LagRecord* record );
	void ResolveAir( AimPlayer* data, LagRecord* record );

	void AirNS( AimPlayer* data, LagRecord* record );
	void ResolvePoses( Player* player, LagRecord* record );
	float AntiFreestand(Player* player, LagRecord* record, bool include_base, float base_yaw, float delta);

public:
	std::array< vec3_t, 64 > m_impacts;
};

extern Resolver g_resolver;