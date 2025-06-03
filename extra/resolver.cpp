#include "includes.h"

Resolver g_resolver{};;

float Resolver::AntiFreestand(Player* player, LagRecord* record, bool include_base, float base_yaw, float delta)
{
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

	// constants.
	constexpr float STEP{ 4.f };
	constexpr float RANGE{ 32.f };

	// construct vector of angles to test.
	std::vector< AdaptiveAngle > angles{ };

	angles.emplace_back(base_yaw + delta);
	angles.emplace_back(base_yaw - delta);

	if (include_base)
		angles.emplace_back(base_yaw);

	// start the trace at the enemy shoot pos.
	vec3_t start = g_cl.m_shoot_pos;

	// see if we got any valid result.
	// if this is false the path was not obstructed with anything.
	bool valid{ false };

	// get the enemies shoot pos.
	vec3_t shoot_pos = player->GetShootPosition();

	// iterate vector of angles.
	for (auto it = angles.begin(); it != angles.end(); ++it) {

		// compute the 'rough' estimation of where our head will be.
		vec3_t end{ shoot_pos.x + std::cos(math::deg_to_rad(it->m_yaw)) * RANGE,
			shoot_pos.y + std::sin(math::deg_to_rad(it->m_yaw)) * RANGE,
			shoot_pos.z };

		// draw a line for debugging purposes.
		//g_csgo.m_debug_overlay->AddLineOverlay( start, end, 255, 0, 0, true, 0.1f );

		// compute the direction.
		vec3_t dir = end - start;
		float len = dir.normalize();

		// should never happen.
		if (len <= 0.f)
			continue;

		// step thru the total distance, 4 units per step.
		for (float i{ 0.f }; i < len; i += STEP) {
			// get the current step position.
			vec3_t point = start + (dir * i);

			// get the contents at this point.
			int contents = g_csgo.m_engine_trace->GetPointContents(point, MASK_SHOT_HULL);

			// contains nothing that can stop a bullet.
			if (!(contents & MASK_SHOT_HULL))
				continue;

			float mult = 1.f;

			// over 50% of the total length, prioritize this shit.
			if (i > (len * 0.5f))
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if (i > (len * 0.75f))
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if (i > (len * 0.9f))
				mult = 2.f;

			// append 'penetrated distance'.
			it->m_dist += (STEP * mult);

			// mark that we found anything.
			valid = true;
		}
	}

	if (!valid)
		return base_yaw;

	// put the most distance at the front of the container.
	std::sort(angles.begin(), angles.end(),
		[](const AdaptiveAngle& a, const AdaptiveAngle& b) {
			return a.m_dist > b.m_dist;
		});

	// the best angle should be at the front now.
	return angles.front().m_yaw;
}

LagRecord* Resolver::FindIdealRecord(AimPlayer* data)
{
	LagRecord* first_valid, * current;

	if (data->m_records.empty())
		return nullptr;

	first_valid = nullptr;

	// iterate records.
	for (const auto& it : data->m_records) {
		if (it->dormant() || it->immune() || !it->valid())
			continue;

		// get current record.
		current = it.get();

		// first record that was valid, store it for later.
		if (!first_valid)
			first_valid = current;

		// try to find a record with a shot, lby update, walking or no anti-aim.
		if (it->m_shot || it->m_mode == Modes::RESOLVE_BODY || it->m_mode == Modes::RESOLVE_WALK || it->m_mode == Modes::RESOLVE_NONE)
			return current;
	}

	// none found above, return the first valid record if possible.
	return (first_valid) ? first_valid : nullptr;
}

LagRecord* Resolver::FindLastRecord(AimPlayer* data) {
	LagRecord* current;

	if (data->m_records.empty())
		return nullptr;

	// iterate records in reverse.
	for (auto it = data->m_records.crbegin(); it != data->m_records.crend(); ++it) {
		current = it->get();

		// if this record is valid.
		// we are done since we iterated in reverse.
		if (current->valid() && !current->immune() && !current->dormant())
			return current;
	}

	return nullptr;
}

void Resolver::OnBodyUpdate( Player* player, float value ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	// set data.
	data->m_old_body = data->m_body;
	data->m_body     = value;

	if (player->m_vecVelocity().length_2d() > 0.1f || !(player->m_fFlags() & FL_ONGROUND)) {
		data->m_body_proxy_updated = false;
		data->m_body_proxy_old = value;
		data->m_body_proxy = value;
		return;
	}

	// lol
	if (fabsf(math::angle_diff(value, data->m_body_proxy)) >= 15.f) {
		data->m_body_proxy_old = data->m_body_proxy;
		data->m_body_proxy = value;
		data->m_body_proxy_updated = true;
	}
}

float Resolver::GetAwayAngle( LagRecord* record ) {
	float  delta{ std::numeric_limits< float >::max( ) };
	vec3_t pos;
	ang_t  away;

	// other cheats predict you by their own latency.
	// they do this because, then they can put their away angle to exactly
	// where you are on the server at that moment in time.

	// the idea is that you would need to know where they 'saw' you when they created their user-command.
	// lets say you move on your client right now, this would take half of our latency to arrive at the server.
	// the delay between the server and the target client is compensated by themselves already, that is fortunate for us.

	// we have no historical origins.
	// no choice but to use the most recent one.
	//if( g_cl.m_net_pos.empty( ) ) {
		math::VectorAngles( g_cl.m_local->m_vecOrigin( ) - record->m_pred_origin, away );
		return away.y;
	//}

	// half of our rtt.
	// also known as the one-way delay.
	//float owd = ( g_cl.m_latency / 2.f );

	// since our origins are computed here on the client
	// we have to compensate for the delay between our client and the server
	// therefore the OWD should be subtracted from the target time.
	//float target = record->m_pred_time; //- owd;

	// iterate all.
	//for( const auto &net : g_cl.m_net_pos ) {
		// get the delta between this records time context
		// and the target time.
	//	float dt = std::abs( target - net.m_time );

		// the best origin.
	//	if( dt < delta ) {
	//		delta = dt;
	//		pos   = net.m_pos;
	//	}
	//}

	//vec3_tAngles( pos - record->m_pred_origin, away );
	//return away.y;
}

void Resolver::MatchShot( AimPlayer* data, LagRecord* record ) {
	// do not attempt to do this in nospread mode.
	if( g_menu.main.config.mode.get( ) == 1 )
		return;

	Weapon* wpn = data->m_player->GetActiveWeapon();
	if (!wpn)
		return;

	WeaponInfo* wpn_data = wpn->GetWpnData();

	if (!wpn_data)
		return;

	const auto shot_time = wpn->m_fLastShotTime();
	const auto shot_tick = game::TIME_TO_TICKS(shot_time);
	const auto sim_tick = game::TIME_TO_TICKS(record->m_sim_time);
	const auto anim_tick = game::TIME_TO_TICKS(record->m_anim_time);

	player_info_t info;
	g_csgo.m_engine->GetPlayerInfo(record->m_player->index(), &info);

	// reset
	record->m_sim_tick = record->m_shot = record->m_fixing_pitch = false;

	// player fired this tick
	if (shot_tick == sim_tick && record->m_lag <= 2)
	{
		// dont need to fix pitch here, they should have onshot.
		record->m_fixing_pitch = false;
		record->m_shot = record->m_sim_tick = true;
		//g_cl.print(tfm::format("player fired | lag: %d | shot_tick: %d | sim tick: %f | anim tick: %f \n", record->m_lag, shot_tick, sim_tick, anim_tick));
	}
	else
	{
		if (shot_tick > anim_tick && sim_tick > shot_tick)
		{
			//g_cl.print(tfm::format("player fired 3 | lag: %d | shot_tick: %d | sim tick: %f | anim tick: %f \n", record->m_lag, shot_tick, sim_tick, anim_tick));
			record->m_shot = record->m_fixing_pitch = true;
		}
		else if (shot_tick == anim_tick)
		{
			//g_cl.print(tfm::format("player fired 2 | lag: %d | shot_tick: %d | sim tick: %f | anim tick: %f \n", record->m_lag, shot_tick, sim_tick, anim_tick));

			// dont need to fix pitch here, they should have onshot.
			if (shot_tick + 1 == sim_tick) {
				record->m_shot = true;
				record->m_fixing_pitch = false;
				return;
			}

			// fix it
			record->m_shot = record->m_fixing_pitch = true;
		}
	}

	if (record->m_fixing_pitch) {
		float valid_pitch = 89.f;
		
		for (const auto& it : data->m_records) {
			if (it.get() == record || it->m_dormant)
				continue;

			if (!it->m_fixing_pitch && !record->m_sim_tick) {
				valid_pitch = it->m_eye_angles.x;
				break;
			}
		}

		record->m_eye_angles.x = valid_pitch;
	}
}

void Resolver::SetMode( LagRecord* record ) {
	// the resolver has 3 modes to chose from.
	// these modes will vary more under the hood depending on what data we have about the player
	// and what kind of hack vs. hack we are playing (mm/nospread).

	float speed = record->m_velocity.length( );

	// if on ground, moving, and not fakewalking.
	if( ( record->m_flags & FL_ONGROUND ) && speed > 0.1f && !record->m_fake_walk )
		record->m_mode = Modes::RESOLVE_WALK;

	// if on ground, not moving or fakewalking.
	if( ( record->m_flags & FL_ONGROUND ) && ( speed <= 0.1f || record->m_fake_walk ) )
		record->m_mode = Modes::RESOLVE_STAND;

	// if not on ground.
	else if( !( record->m_flags & FL_ONGROUND ) )
		record->m_mode = Modes::RESOLVE_AIR;
}

void Resolver::ResolveAngles( Player* player, LagRecord* record ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	// mark this record if it contains a shot.
	MatchShot( data, record );

	// next up mark this record with a resolver mode that will be used.
	SetMode( record );

	// if we are in nospread mode, force all players pitches to down.
	// TODO; we should check thei actual pitch and up too, since those are the other 2 possible angles.
	// this should be somehow combined into some iteration that matches with the air angle iteration.
	if( g_menu.main.config.mode.get( ) == 1 )
		record->m_eye_angles.x = 90.f;

	// we arrived here we can do the acutal resolve.
	if( record->m_mode == Modes::RESOLVE_WALK ) 
		ResolveWalk( data, record );

	else if( record->m_mode == Modes::RESOLVE_STAND )
		ResolveStand( data, record );

	else if( record->m_mode == Modes::RESOLVE_AIR )
		ResolveAir( data, record );

	// normalize the eye angles, doesn't really matter but its clean.
	math::NormalizeAngle( record->m_eye_angles.y );
}

void Resolver::ResolveWalk( AimPlayer* data, LagRecord* record ) {
	// apply lby to eyeangles.
	record->m_eye_angles.y = record->m_body;

	// delay body update.
	data->m_body_update = record->m_anim_time + 0.22f;

	// reset stand and body index.
	data->m_stand_index  = 0;
	data->m_moved = false;
	data->m_has_body_updated = false;
	data->m_flick_angle_missed = 0.f;
	data->m_air_flick_miss = 0;
	data->m_stand_index2 = 0;
	data->m_no_flick_index = 0;
	data->m_flicks = 0;
	data->m_no_flicks = 0;
	data->m_body_flick = FLT_MAX;
	data->m_body_index   = 0;
	data->m_lby_index = 0;
	data->m_bruteforce_idx = 0;
	data->m_updated_lby = record->m_body;

	// copy the last record that this player was walking
	// we need it later on because it gives us crucial data.
	std::memcpy( &data->m_walk_record, record, sizeof( LagRecord ) );
}

void Resolver::ResolveStand( AimPlayer* data, LagRecord* record ) {
	// for no-spread call a seperate resolver.
	if( g_menu.main.config.mode.get( ) == 1 ) {
		StandNS( data, record );
		return;
	}

	// get predicted away angle for the player.
	float away = GetAwayAngle( record );
	LagRecord* previous = data->m_records.size() >= 2 ? data->m_records[1].get() : nullptr;

	// pointer for easy access.
	LagRecord* move = &data->m_walk_record;

	// we have a valid moving record.
	if( move->m_sim_time > 0.f ) {
		vec3_t delta = move->m_origin - record->m_origin;

		// check if moving record is close.
		if( delta.length( ) <= 60.f ) {
			// indicate that we are using the moving lby.
			data->m_moved = true;
		}
	}

	// body updated.
	if (data->m_old_body != record->m_body) {
		// update old body.
		data->m_old_body = record->m_body;

		// set angles to current LBY.
		record->m_eye_angles.y = record->m_body;

		// set the resolve mode.
		record->m_mode = Modes::RESOLVE_BODY;

		// delay body update.
		data->m_body_update = record->m_anim_time + 1.1f;

		// we've seen them update.
		data->m_has_body_updated = true;

		// we updated, our flick is no more valid.
		data->m_flick_angle_missed = 0.f;

		// exit out of the resolver, thats it.
		return;
	}

	// lby should have updated here.
	if (data->m_has_body_updated && record->m_anim_time >= data->m_body_update && data->m_body_index <= 3) {
		// set angles to current LBY.
		record->m_eye_angles.y = record->m_body;

		// predict next body update.
		data->m_body_update = record->m_anim_time + 1.1f;
		data->m_updated_lby = record->m_body;

		if (previous && std::fabs(math::AngleDiff(record->m_body, previous->m_body)) > 35.f)
			++data->m_flicks;
		else
			++data->m_no_flicks;

		// set the resolve mode.
		record->m_mode = Modes::RESOLVE_BODY;

		// exit out of the resolver, thats it.
		return;
	}

	const float back = away + 180.f;
	const float forward_body_diff = math::AngleDiff(away, record->m_body);

	// reset overlap delta amount
	data->m_overlap_offset = 0.f;

	const float back_delta = math::AngleDiff(record->m_body, back);

	if (std::fabs(back_delta) >= 25.f) {
		if (back_delta < 0.f)
			data->m_overlap_offset = std::clamp(-(std::fabs(back_delta) / 2.f), -60.f, 60.f);
		else
			data->m_overlap_offset = std::clamp((std::fabs(back_delta) / 2.f), -60.f, 60.f);
	}

	if (!data->m_body_proxy_updated && data->m_lby_index <= 0)
	{
		record->m_mode = Modes::RESOLVE_LBY;
		record->m_eye_angles.y = record->m_body;
		return;
	}

	if ((data->m_flicks || data->m_no_flicks) && data->m_body_proxy_updated
		&& data->m_flicks < 2
		&& data->m_no_flick_index < 2)
	{
		record->m_mode = Modes::RESOLVE_NO_UPDATE_LBY;
		record->m_eye_angles.y = record->m_body;
		data->m_body_flick = record->m_body;
		return;
	}

	// a valid moving context was found
	if( data->m_moved ) {
		float move_lby_diff = std::fabs(math::AngleDiff(record->m_body, move->m_body));
		float old_lby_diff = std::fabs(math::AngleDiff(data->m_updated_lby, record->m_body));

		record->m_mode = Modes::RESOLVE_DELTA;

		switch (data->m_bruteforce_idx % 6)
		{
		case 0:
			if (std::fabs(math::AngleDiff(data->m_updated_lby, move->m_body)) <= 35.f)
			{
				record->m_eye_angles.y = move->m_body + move_lby_diff;
			}
			else
			{
				record->m_eye_angles.y = AntiFreestand(record->m_player, record, true, record->m_body, 90.f);
			}
			break;
		case 1:
			record->m_eye_angles.y = move->m_body + move_lby_diff;
			break;
		case 2:
			record->m_eye_angles.y = record->m_body + old_lby_diff;
			break;
		case 3:
			record->m_eye_angles.y = back + data->m_overlap_offset;
			break;
		case 4:
			record->m_eye_angles.y = away - 70.f;
			break;
		case 5:
			record->m_eye_angles.y = away + 70.f;
			break;
		default:
			break;
		}
	}
	else
	{
		// stand2 -> no known last move.
		record->m_mode = Modes::RESOLVE_STAND2;

		switch (data->m_stand_index2 % 4) {

		case 0:
			record->m_eye_angles.y = AntiFreestand(record->m_player, record, true, record->m_body, 90.f);
			break;

		case 1:
			record->m_eye_angles.y = back + data->m_overlap_offset;
			break;

		case 2:
			record->m_eye_angles.y = AntiFreestand(record->m_player, record, false, record->m_body, 90.f);
			break;

		case 3:
			record->m_eye_angles.y = AntiFreestand(record->m_player, record, true, away, 90.f);
			break;

		default:
			break;
		}
	}
}

void Resolver::StandNS( AimPlayer* data, LagRecord* record ) {
	// get away angles.
	float away = GetAwayAngle( record );

	switch( data->m_shots % 8 ) {
	case 0:
		record->m_eye_angles.y = away + 180.f;
		break;

	case 1:
		record->m_eye_angles.y = away + 90.f;
		break;
	case 2:
		record->m_eye_angles.y = away - 90.f;
		break;

	case 3:
		record->m_eye_angles.y = away + 45.f;
		break;
	case 4:
		record->m_eye_angles.y = away - 45.f;
		break;

	case 5:
		record->m_eye_angles.y = away + 135.f;
		break;
	case 6:
		record->m_eye_angles.y = away - 135.f;
		break;

	case 7:
		record->m_eye_angles.y = away + 0.f;
		break;

	default:
		break;
	}

	// force LBY to not fuck any pose and do a true bruteforce.
	record->m_body = record->m_eye_angles.y;
}

void Resolver::ResolveAir( AimPlayer* data, LagRecord* record ) {
	// for no-spread call a seperate resolver.
	if( g_menu.main.config.mode.get( ) == 1 ) {
		AirNS( data, record );
		return;
	}

	if (data->m_air_flick_miss > 0) {
		if (math::AngleDiff(data->m_flick_angle_missed, data->m_body_flick) > 35.f) { // he broke lby + changed his angle
			data->m_air_flick_miss = 0; // reset counter.
			data->m_flick_angle_missed = 0.f;
		}
	}
	else {
		if (math::AngleDiff(data->m_body_flick, record->m_body) <= 35.f) {
			record->m_mode = Resolver::RESOLVE_AIR_FLICK;
			record->m_eye_angles.y = data->m_body_flick; // body-flick
			return;
		}
	}

	// try to predict the direction of the player based on his velocity direction.
	// this should be a rough estimation of where he is looking.
	float velyaw = math::rad_to_deg( std::atan2( record->m_velocity.y, record->m_velocity.x ) );

	switch( data->m_shots % 3 ) {
	case 0:
		record->m_eye_angles.y = record->m_body;
		break;

	case 1:
		record->m_eye_angles.y = velyaw + 180.f;
		break;

	case 2:
		record->m_eye_angles.y = velyaw + 90.f;
		break;
	}
}

void Resolver::AirNS( AimPlayer* data, LagRecord* record ) {
	// get away angles.
	float away = GetAwayAngle( record );

	switch( data->m_shots % 9 ) {
	case 0:
		record->m_eye_angles.y = away + 180.f;
		break;

	case 1:
		record->m_eye_angles.y = away + 150.f;
		break;
	case 2:
		record->m_eye_angles.y = away - 150.f;
		break;

	case 3:
		record->m_eye_angles.y = away + 165.f;
		break;
	case 4:
		record->m_eye_angles.y = away - 165.f;
		break;

	case 5:
		record->m_eye_angles.y = away + 135.f;
		break;
	case 6:
		record->m_eye_angles.y = away - 135.f;
		break;

	case 7:
		record->m_eye_angles.y = away + 90.f;
		break;
	case 8:
		record->m_eye_angles.y = away - 90.f;
		break;

	default:
		break;
	}
}

void Resolver::ResolvePoses( Player* player, LagRecord* record ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	// only do this bs when in air.
	if( record->m_mode == Modes::RESOLVE_AIR ) {
		// ang = pose min + pose val x ( pose range )

		// lean_yaw
		player->m_flPoseParameter( )[ 2 ]  = g_csgo.RandomInt( 0, 4 ) * 0.25f;   

		// body_yaw
		player->m_flPoseParameter( )[ 11 ] = g_csgo.RandomInt( 1, 3 ) * 0.25f;
	}
}