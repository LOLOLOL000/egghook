#include "includes.h"
#include "autowall.h"
#include "lagcomp.h"

#include "threading/threading.h"
#include "threading/shared_mutex.h"

static SharedMutex smtx;

Aimbot g_aimbot{ };;

void force_scope()
{
	bool able_to_zoom = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) >= g_cl.m_weapon->m_flNextSecondaryAttack();

	if (able_to_zoom && g_menu.main.aimbot.zoom.get() && g_cl.m_weapon->m_zoomLevel() < 1 && g_movement.on_ground() && g_cl.m_weapon->is_sniper())
		g_cl.m_cmd->m_buttons |= IN_ATTACK2;
}

void FixVelocity(Player* m_player, LagRecord* record, LagRecord* previous, float max_speed)
{
	if (!previous)
	{
		if (record->m_layers[6].m_playback_rate > 0.0f && record->m_layers[6].m_weight != 0.f && record->m_velocity.length() > 0.1f)
		{
			auto v30 = max_speed;

			if (record->m_flags & 6)
				v30 *= 0.34f;
			else if (m_player->m_bIsWalking())
				v30 *= 0.52f;

			auto v35 = record->m_layers[6].m_weight * v30;
			record->m_velocity *= v35 / record->m_velocity.length();
		}
		else
			record->m_velocity.clear();

		if (record->m_flags & 1)
			record->m_velocity.z = 0.f;

		record->m_anim_velocity = record->m_velocity;
		return;
	}

	if ((m_player->m_fEffects() & 8) != 0
		|| m_player->m_ubEFNoInterpParity() != m_player->m_ubEFNoInterpParityOld())
	{
		record->m_velocity.clear();
		record->m_anim_velocity.clear();
		return;
	}

	auto is_jumping = !(record->m_flags & FL_ONGROUND && previous->m_flags & FL_ONGROUND);

	if (record->m_lag > 1)
	{
		record->m_velocity.clear();
		auto origin_delta = (record->m_origin - previous->m_origin);

		if (!(previous->m_flags & FL_ONGROUND || record->m_flags & FL_ONGROUND))// if not previous on ground or on ground
		{
			auto currently_ducking = record->m_flags & FL_DUCKING;
			if ((previous->m_flags & FL_DUCKING) != currently_ducking)
			{
				float duck_modifier = 0.f;

				if (currently_ducking)
					duck_modifier = 9.f;
				else
					duck_modifier = -9.f;

				origin_delta.z -= duck_modifier;
			}
		}

		auto sqrt_delta = origin_delta.length_2d_sqr();

		if (sqrt_delta > 0.f && sqrt_delta < 1000000.f)
			record->m_velocity = origin_delta / game::TICKS_TO_TIME(record->m_lag);

		record->m_velocity.validate_vec();

		if (is_jumping)
		{
			if (record->m_flags & FL_ONGROUND && !g_csgo.sv_enablebunnyhopping->GetInt())
			{

				// 260 x 1.1 = 286 units/s.
				float max = m_player->m_flMaxspeed() * 1.1f;

				// get current velocity.
				float speed = record->m_velocity.length();

				// reset velocity to 286 units/s.
				if (max > 0.f && speed > max)
					record->m_velocity *= (max / speed);
			}

			// assume the player is bunnyhopping here so set the upwards impulse.
			record->m_velocity.z = g_csgo.sv_jump_impulse->GetFloat();
		}
		// we are not on the ground
		// apply gravity and airaccel.
		else if (!(record->m_flags & FL_ONGROUND))
		{
			// apply one tick of gravity.
			record->m_velocity.z -= g_csgo.sv_gravity->GetFloat() * g_csgo.m_globals->m_interval;
		}
	}

	record->m_anim_velocity = record->m_velocity;

	if (!record->m_fake_walk)
	{
		if (record->m_anim_velocity.length_2d() > 0 && (record->m_flags & FL_ONGROUND))
		{
			record->m_anim_speed = 0.f;

			if (!is_jumping
				&& record->m_layers[11].m_weight > 0.0f
				&& record->m_layers[11].m_weight < 1.0f
				&& record->m_layers[11].m_playback_rate == previous->m_layers[11].m_playback_rate)
			{
				// calculate animation speed yielded by anim overlays
				auto flAnimModifier = 0.35f * (1.0f - record->m_layers[11].m_weight);
				if (flAnimModifier > 0.0f && flAnimModifier < 1.0f)
					record->m_anim_speed = max_speed * (flAnimModifier + 0.55f);
			}

			// this velocity is valid ONLY IN ANIMFIX UPDATE TICK!!!
			// so don't store it to record as m_vecVelocity
			// -L3D451R7
			if (record->m_anim_speed > 0.0f)
			{
				record->m_anim_speed /= record->m_anim_velocity.length_2d();
				record->m_anim_velocity.x *= record->m_anim_speed;
				record->m_anim_velocity.y *= record->m_anim_speed;
			}
		}
	}
	else
		record->m_anim_velocity.clear();

	record->m_anim_velocity.validate_vec();
}

void AimPlayer::UpdateAnimations(LagRecord* record)
{
	CCSGOPlayerAnimState* state = m_player->m_PlayerAnimState();
	if (!state)
		return;

	// player respawned.
	if (m_player->m_flSpawnTime() != m_spawn) {
		// reset animation state.
		game::ResetAnimationState(state);

		// note new spawn time.
		m_spawn = m_player->m_flSpawnTime();
	}

	// backup curtime.
	float curtime = g_csgo.m_globals->m_curtime;
	float frametime = g_csgo.m_globals->m_frametime;

	// set curtime to animtime.
	// set frametime to ipt just like on the server during simulation.
	g_csgo.m_globals->m_curtime = record->m_anim_time;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;

	// backup stuff that we do not want to fuck with.
	AnimationBackup_t backup;

	backup.m_origin = m_player->m_vecOrigin();
	backup.m_abs_origin = m_player->GetAbsOrigin();
	backup.m_velocity = m_player->m_vecVelocity();
	backup.m_abs_velocity = m_player->m_vecAbsVelocity();
	backup.m_flags = m_player->m_fFlags();
	backup.m_eflags = m_player->m_iEFlags();
	backup.m_duck = m_player->m_flDuckAmount();
	backup.m_body = m_player->m_flLowerBodyYawTarget();
	m_player->GetPoseParameters(backup.m_poses);
	m_player->GetAnimLayers(backup.m_layers);

	// is player a bot?
	bool bot = game::IsFakePlayer(m_player->index());

	// get pointer to previous record.
	LagRecord* previous = m_records.size() >= 2 ? m_records[1].get() : nullptr;

	// set uninterpolated origin
	m_player->SetAbsOrigin(record->m_origin);

	// reset fakewalk state.
	record->m_fake_walk = false;
	record->m_broke_lc = previous ? (record->m_origin - previous->m_origin).length_sqr() > 4096.f : false;
	record->m_mode = Resolver::Modes::RESOLVE_NONE;

	// fix velocity.
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/client/c_baseplayer.cpp#L659
	if (record->m_lag > 0 && record->m_lag < 16 && previous && !previous->dormant())
		record->m_velocity = (record->m_origin - previous->m_origin) * (1.f / game::TICKS_TO_TIME(record->m_lag));

	// set this fucker, it will get overriden.
	record->m_anim_velocity = record->m_velocity;

	// fix various issues with the game eW91dHViZS5jb20vZHlsYW5ob29r
	// these issues can only occur when a player is choking data.
	if (record->m_lag > 1 && !bot) {
		// detect fakewalk.
		float speed = record->m_velocity.length();

		if (record->m_flags & FL_ONGROUND && record->m_layers[6].m_weight == 0.f)
			record->m_fake_walk = speed > 0.1f;

		if (record->m_fake_walk)
			record->m_anim_velocity = record->m_velocity = { 0.f, 0.f, 0.f };

		// we need atleast 2 updates/records
		// to fix these issues.
		if (m_records.size() >= 2) {
			// get pointer to previous record.
			LagRecord* previous = m_records[1].get();

			if (previous && !previous->dormant()) {
				m_player->m_fFlags() = previous->m_flags;

				// strip the on ground flag.
				m_player->m_fFlags() &= ~FL_ONGROUND;

				// been onground for 2 consecutive ticks? fuck yeah.
				if (record->m_flags & FL_ONGROUND && previous->m_flags & FL_ONGROUND)
					m_player->m_fFlags() |= FL_ONGROUND;

				// fix jump_fall.
				if (record->m_layers[4].m_weight != 1.f && previous->m_layers[4].m_weight == 1.f && record->m_layers[5].m_weight != 0.f)
					m_player->m_fFlags() |= FL_ONGROUND;

				if (record->m_flags & FL_ONGROUND && !(previous->m_flags & FL_ONGROUND))
					m_player->m_fFlags() &= ~FL_ONGROUND;

				// fix crouching players.
				// the duck amount we receive when people choke is of the last simulation.
				// if a player chokes packets the issue here is that we will always receive the last duckamount.
				// but we need the one that was animated.
				// therefore we need to compute what the duckamount was at animtime.

				// delta in duckamt and delta in time..
				float duck = record->m_duck - previous->m_duck;
				float time = record->m_sim_time - previous->m_sim_time;

				// get the duckamt change per tick.
				float change = (duck / time) * g_csgo.m_globals->m_interval;

				// fix crouching players.
				m_player->m_flDuckAmount() = previous->m_duck + change;

				if (!record->m_fake_walk) {
					// fix the velocity till the moment of animation.
					vec3_t velo = record->m_velocity - previous->m_velocity;

					// accel per tick.
					vec3_t accel = (velo / time) * g_csgo.m_globals->m_interval;

					// set the anim velocity to the previous velocity.
					// and predict one tick ahead.
					record->m_anim_velocity = previous->m_velocity + accel;
				}
			}
		}
	}

	bool fake = !bot && g_menu.main.aimbot.correct.get();

	// if using fake angles, correct angles.
	if (fake)
		g_resolver.ResolveAngles(m_player, record);

	// set stuff before animating.
	m_player->m_vecOrigin() = record->m_origin;
	m_player->m_vecVelocity() = m_player->m_vecAbsVelocity() = record->m_anim_velocity;
	m_player->m_flLowerBodyYawTarget() = record->m_body;

	// EFL_DIRTY_ABSVELOCITY
	// skip call to C_BaseEntity::CalcAbsoluteVelocity
	m_player->m_iEFlags() &= ~0x1000;

	// write potentially resolved angles.
	m_player->m_angEyeAngles() = record->m_eye_angles;

	// fix animating in same frame.
	if (state->m_nLastUpdateFrame == g_csgo.m_globals->m_frame)
		state->m_nLastUpdateFrame -= 1;

	if (state->m_flLastUpdateTime == g_csgo.m_globals->m_curtime)
		state->m_flLastUpdateTime -= 1.f;

	// 'm_animating' returns true if being called from SetupVelocity, passes raw velocity to animstate.
	bool old_csa = m_player->m_bClientSideAnimation();
	g_hooks.m_updating_anims[m_player->index()] = true;
	m_player->m_bClientSideAnimation() = true;
	m_player->UpdateClientSideAnimation();
	m_player->m_bClientSideAnimation() = old_csa;
	g_hooks.m_updating_anims[m_player->index()] = false;

	// store updated/animated poses and rotation in lagrecord.
	m_player->SetAnimLayers(record->m_layers);
	m_player->GetPoseParameters(record->m_poses);
	record->m_abs_ang = ang_t{ 0.f, state->m_flFootYaw, 0.f };

	// reset duration in air
	if (state->m_bOnGround)
		state->m_flDurationInAir = 0.f;
	else if (!state->m_bOnLadder) {

		// we're in the air
		if (state->m_bLeftTheGroundThisFrame)
			state->m_flDurationInAir = 0.f;

		// add air time
		state->m_flDurationInAir += state->m_flLastUpdateIncrement;
	}

	record->m_setup = g_bones.BuildEnemyBones(m_player, BONE_USED_BY_ANYTHING, record->m_bones, record->m_anim_time, record); //g_bones.SetupBonesRebuild(m_player, record->m_bones, 128, BONE_USED_BY_ANYTHING, record->m_sim_time, BoneSetupFlags::UseCustomOutput);

	// restore backup data.
	m_player->m_vecOrigin() = backup.m_origin;
	m_player->m_vecVelocity() = backup.m_velocity;
	m_player->m_vecAbsVelocity() = backup.m_abs_velocity;
	m_player->m_fFlags() = backup.m_flags;
	m_player->m_iEFlags() = backup.m_eflags;
	m_player->m_flDuckAmount() = backup.m_duck;
	m_player->m_flLowerBodyYawTarget() = backup.m_body;
	m_player->SetAbsOrigin(backup.m_abs_origin);
	m_player->SetAnimLayers(backup.m_layers);

	// IMPORTANT: do not restore poses here, since we want to preserve them for rendering.
	// also dont restore the render angles which indicate the model rotation.

	// restore globals.
	g_csgo.m_globals->m_curtime = curtime;
	g_csgo.m_globals->m_frametime = frametime;
}

void AimPlayer::OnNetUpdate(Player* player) {
	bool reset = (!g_menu.main.aimbot.enable.get() || player->m_lifeState() == LIFE_DEAD || !player->enemy(g_cl.m_local));

	// if this happens, delete all the lagrecords.
	if (reset) {
		m_records.clear();
		return;
	}

	// update player ptr if required.
	// reset player if changed.
	if (m_player != player)
		m_records.clear();

	// update player ptr.
	m_player = player;

	// indicate that this player has been out of pvs.
	// insert dummy record to separate records
	// to fix stuff like animation and prediction.
	if (player->dormant()) {
		bool insert = true;

		// we have any records already?
		if (!m_records.empty()) {

			LagRecord* front = m_records.front().get();

			// we already have a dormancy separator.
			if (front->dormant())
				insert = false;
		}

		if (insert) {
			// add new record.
			m_records.emplace_front(std::make_shared< LagRecord >(player));

			// get reference to newly added record.
			LagRecord* current = m_records.front().get();

			// mark as dormant.
			current->m_dormant = true;
		}

		return;
	}

	bool update = player->m_flSimulationTime() > player->m_flOldSimulationTime(); // (m_records.empty() || player->m_flSimulationTime() > m_records.front().get()->m_sim_time);

	// this is the first data update we are receving
	// OR we received data with a newer simulation context.
	if (update) {
		// add new record.
		m_records.emplace_front(std::make_shared< LagRecord >(player));

		// get reference to newly added record.
		LagRecord* current = m_records.front().get();

		// mark as non dormant.
		current->m_dormant = false;

		// update animations on current record.
		// call resolver.
		UpdateAnimations(current);
	}

	// no need to store insane amt of data.
	while (m_records.size() > 256)
		m_records.pop_back();
}

void AimPlayer::OnRoundStart(Player* player) {
	m_player = player;
	m_walk_record = LagRecord{ };
	m_shots = 0;
	m_missed_shots = 0;

	// reset stand and body index.
	m_stand_index = 0;
	m_stand_index2 = 0;
	m_body_index = 0;

	// IMPORTANT: DO NOT CLEAR LAST HIT SHIT.
}

std::vector< std::pair< int, int > > AimPlayer::GetHitboxes(LagRecord* record) {
	// reset hitboxes.
	std::vector< std::pair< int, int > > hitboxes{ };

	if (g_cl.m_weapon_id == ZEUS) {
		// hitboxes for the zeus.
		hitboxes.emplace_back(HITBOX_BODY, HitscanMode::NORMAL);
		hitboxes.emplace_back(HITBOX_CHEST, HitscanMode::NORMAL);
		hitboxes.emplace_back(HITBOX_THORAX, HitscanMode::NORMAL);
		return hitboxes;
	}

	std::vector< size_t > hitbox{ g_menu.main.aimbot.hitbox.GetActiveIndices() };
	if (hitbox.empty())
		return hitboxes;

	for (const auto& h : hitbox) {
		// head.
		if (h == 0)
			hitboxes.emplace_back(HITBOX_HEAD, HitscanMode::NORMAL);

		// chest.
		if (h == 1) {
			hitboxes.emplace_back(HITBOX_THORAX, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_CHEST, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_UPPER_CHEST, HitscanMode::NORMAL);
		}

		// stomach.
		if (h == 2) {
			hitboxes.emplace_back(HITBOX_PELVIS, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_BODY, HitscanMode::NORMAL);
		}

		// arms.
		if (h == 3) {
			hitboxes.emplace_back(HITBOX_L_UPPER_ARM, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_R_UPPER_ARM, HitscanMode::NORMAL);
		}

		// legs.
		if (h == 4) {
			hitboxes.emplace_back(HITBOX_L_THIGH, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_R_THIGH, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_L_CALF, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_R_CALF, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_L_FOOT, HitscanMode::NORMAL);
			hitboxes.emplace_back(HITBOX_R_FOOT, HitscanMode::NORMAL);
		}
	}

	return hitboxes;
}

bool AimPlayer::GetBestAimPosition(vec3_t& aim, float& damage, LagRecord* record, int& hitbox)
{
	bool                  done, pen;
	float                 dmg;
	HitscanData_t         scan;

	// get player hp.
	int hp = std::min(100, m_player->m_iHealth());

	if (g_cl.m_weapon_id == ZEUS) {
		dmg = hp;
		pen = false;
	}

	else {
		dmg = g_aimbot.m_damage_toggle ? g_menu.main.aimbot.override_dmg_value.get() : g_menu.main.aimbot.minimal_damage.get();
		if (g_menu.main.aimbot.minimal_damage_hp.get())
			dmg = std::ceil((dmg / 100.f) * hp);

		pen = g_menu.main.aimbot.penetrate.get();
	}

	// write all data of this record l0l.
	record->cache();

	// iterate hitboxes.
	for (const auto& it : this->GetHitboxes(record)) {
		done = false;

		auto pts = this->SetupHitboxPoints(record, record->m_bones, it.first);

		// iterate points on hitbox.
		for (auto& point : pts) {
			penetration::PenetrationInput_t in;

			in.m_damage = dmg;
			in.m_damage_pen = dmg;
			in.m_can_pen = pen;
			in.m_target = m_player;
			in.m_from = g_cl.m_local;
			in.m_pos = point.first;

			penetration::PenetrationOutput_t out;

			// we can hit p!
			if (penetration::run(&in, &out)) {

				// nope we did not hit head..
				if (it.first == HITBOX_HEAD && out.m_hitgroup != HITGROUP_HEAD)
					continue;

				// lethal
				if (out.m_damage >= hp && point.second && out.m_hitgroup >= 2 && out.m_hitgroup <= 3)
					done = true;

				// we did more damage.
				else if (out.m_damage > scan.m_damage) {
					// save new best data.
					scan.m_damage = out.m_damage;
					scan.m_pos = point.first;
					scan.index = out.m_hitbox;
				}

				// we found a preferred / lethal hitbox.
				if (done) {
					// save new best data.
					scan.m_damage = out.m_damage;
					scan.m_pos = point.first;
					scan.index = out.m_hitbox;
					break;
				}
			}
		}

		// ghetto break out of outer loop.
		if (done)
			break;
	}

	// we found something that we can damage.
	// set out vars.
	if (scan.m_damage > 0.f) {
		aim = scan.m_pos;
		damage = scan.m_damage;
		hitbox = scan.index;
		return true;
	}

	return false;
}

void Aimbot::start_stop()
{
	if (!g_cl.m_weapon || !g_cl.m_weapon_info || !stopping)
		return;

	if (this->should_stop())
		force_accuracy = this->AutoStop();
}

bool Aimbot::should_stop(bool shoot_check)
{
	if (!g_cl.m_local || !g_cl.m_local->alive())
		return false;

	if (!g_menu.main.movement.autostop_always_on.get())
		return false;

	if (!g_cl.m_weapon || !g_cl.m_weapon_info)
		return false;

	if (g_cl.m_weapon->IsKnife())
		return false;

	if (!g_movement.on_ground())
		return false;

	if (g_cl.m_local->m_vecVelocity().length(true) < 1.f)
		return false;

	if (g_input.GetKeyState(g_menu.main.movement.fakewalk.get()))
		return false;

	bool able_to_shoot = g_cl.is_able_to_shoot(true);

	if (shoot_check)
	{
		bool between_shots_ = g_menu.main.movement.between_shots.get();
		if (!able_to_shoot)
			return between_shots_;
	}

	return able_to_shoot;
}


void Aimbot::init() {
	// clear old targets.
	m_targets.clear();

	force_accuracy = true;
	stopping = false;

	m_target = nullptr;
	m_aim = vec3_t{ };
	m_angle = ang_t{ };
	m_damage = 0.f;
	m_record = nullptr;
	m_stop = false;

	m_best_dist = std::numeric_limits< float >::max();
	m_best_fov = 180.f + 1.f;
	m_best_damage = 0.f;
	m_best_hp = 100 + 1;
	m_best_lag = std::numeric_limits< float >::max();
	m_best_height = std::numeric_limits< float >::max();
}

bool Aimbot::AutoStop()
{
	if (!g_cl.m_weapon)
		return false;

	auto velocity = g_cl.m_local->m_vecVelocity();
	float raw_speed = velocity.length(true);

	int max_speed = (int)g_movement.m_max_speed * 0.34f;
	int speed = (int)raw_speed;

	if (speed <= max_speed)
	{
		g_movement.force_speed(g_movement.m_max_speed * 0.34f);
		return true;
	}

	g_movement.QuickStop();

	return g_menu.main.movement.force_accuracy.get();
}

void Aimbot::StripAttack() {
	if (g_cl.m_weapon_id == REVOLVER)
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK2;

	else
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK;
}

void Aimbot::think() {
	// do all startup routines.
	init();

	// sanity.
	if (!g_cl.m_weapon || !g_cl.m_weapon_info)
		return;

	// no grenades or bomb.
	if (g_cl.m_weapon_type == WEAPONTYPE_GRENADE || g_cl.m_weapon_type == WEAPONTYPE_C4)
		return;

	// we have no aimbot enabled.
	if (!g_menu.main.aimbot.enable.get())
		return;

	if (!g_cl.m_weapon_fire)
		return;

	// setup bones for all valid targets.
	for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

		if (!IsValidTarget(player))
			continue;

		AimPlayer* data = &m_players[i - 1];
		if (!data)
			continue;

		// store player as potential target this tick.
		m_targets.emplace_back(data);
	}

	// run knifebot.
	if (g_cl.m_weapon_type == WEAPONTYPE_KNIFE && g_cl.m_weapon_id != ZEUS) {

		if (g_menu.main.aimbot.knifebot.get())
			knife();

		return;
	}

	// scan available targets... if we even have any.
	find();

	// finally set data when shooting.
	apply();
}

void Aimbot::apply()
{
	bool attack, attack2;

	// attack states.
	attack = (g_cl.m_cmd->m_buttons & IN_ATTACK);
	attack2 = (g_cl.m_weapon_id == REVOLVER && g_cl.m_cmd->m_buttons & IN_ATTACK2);

	// ensure we're attacking.
	if (attack || attack2)
	{
		// choke every shot.
		if (!g_input.GetKeyState(g_menu.main.movement.fakewalk.get()))
			*g_cl.m_packet = false;

		if (m_target) {
			// make sure to aim at un-interpolated data.
			// do this so BacktrackEntity selects the exact record.
			if (m_record && !m_record->m_broke_lc)
				g_cl.m_cmd->m_tick = game::TIME_TO_TICKS(m_record->m_sim_time + g_cl.m_lerp);

			if (g_menu.main.aimbot.sparks.get())
				g_csgo.m_effects->Sparks(m_target->GetHitboxPosition(m_hitbox).value(), 5, 2);

			// set angles to target.
			g_cl.m_cmd->m_view_angles = m_angle;

			// if not silent aim, apply the viewangles.
			if (!g_menu.main.aimbot.silent.get())
				g_csgo.m_engine->SetViewAngles(m_angle);
		}

		// nospread.
		if (g_menu.main.aimbot.nospread.get() && g_menu.main.config.mode.get() == 1)
			NoSpread();

		// norecoil.
		if (g_menu.main.aimbot.norecoil.get())
			g_cl.m_cmd->m_view_angles -= g_cl.m_local->m_aimPunchAngle() * g_csgo.weapon_recoil_scale->GetFloat();

		// add shot
		if (m_target && m_record)
			g_shots.add(
				g_cl.m_shoot_pos, m_target,
				-1, g_cl.m_cmd->m_command_number, g_csgo.m_globals->m_realtime,
				g_csgo.m_engine->GetNetChannelInfo()->GetLatency(INetChannel::FLOW_INCOMING) +
				g_csgo.m_engine->GetNetChannelInfo()->GetLatency(INetChannel::FLOW_INCOMING),
				m_record, m_hitbox
			);

		// set that we fired.
		g_cl.m_shot = true;
	}
}

void Aimbot::find() {

	LagRecord* previous = nullptr;
	vec3_t       tmp_pos;
	float        tmp_damage;
	int tmp_hitbox;
	BestTarget_t best;
	best.player = nullptr;
	best.damage = -1.f;
	best.pos = vec3_t{ };
	best.record = nullptr;
	best.index = -1;

	if (m_targets.empty())
		return;

	if (g_cl.m_weapon_id == ZEUS && !g_menu.main.aimbot.zeusbot.get())
		return;

	// iterate all targets.
	for (const auto& t : m_targets) {
		if (t->m_records.empty())
			continue;

		if (t->m_records.size() >= 2)
			previous = t->m_records[1].get();

		// this player broke lagcomp.
		// his bones have been resetup by our lagcomp.
		// therfore now only the front record is valid.
		if (g_menu.main.aimbot.lagfix.get() && g_lagcomp.StartPrediction(t, previous)) {
			LagRecord* front = t->m_records.front().get();

			// rip something went wrong..
			if (t->GetBestAimPosition(tmp_pos, tmp_damage, front, tmp_hitbox) && SelectTarget(front, tmp_pos, tmp_damage)) {

				// if we made it so far, set shit.
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = front;
				best.index = tmp_hitbox;
			}
		}

		// player did not break lagcomp.
		// history aim is possible at this point.
		else {
			LagRecord* ideal = g_resolver.FindIdealRecord(t);
			if (!ideal)
				continue;

			// try to select best record as target.
			if (t->GetBestAimPosition(tmp_pos, tmp_damage, ideal, tmp_hitbox) && SelectTarget(ideal, tmp_pos, tmp_damage)) {
				// if we made it so far, set shit.
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = ideal;
				best.index = tmp_hitbox;
			}

			LagRecord* last = g_resolver.FindLastRecord(t);
			if (!last || last == ideal)
				continue;

			// rip something went wrong..
			if (t->GetBestAimPosition(tmp_pos, tmp_damage, last, tmp_hitbox) && SelectTarget(last, tmp_pos, tmp_damage)) {
				// if we made it so far, set shit.
				best.player = t->m_player;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				best.record = last;
				best.index = tmp_hitbox;
			}
		}
	}

	// verify our target and set needed data.
	if (best.player && best.record) {
		// calculate aim angle.
		math::VectorAngles(best.pos - g_cl.m_shoot_pos, m_angle);

		// set member vars.
		m_target = best.player;
		m_aim = best.pos;
		m_damage = best.damage;
		m_record = best.record;
		m_hitbox = best.index;

		// write data, needed for traces / etc.
		m_record->cache();

		// set autostop shit.
		stopping = true;

		bool on = g_menu.main.aimbot.hitchance.get() && g_menu.main.config.mode.get() == 0;
		bool hit = on && is_accuracy_valid(m_target, m_angle) && force_accuracy;

		// if we can scope.
		bool can_scope = !g_cl.m_local->m_bIsScoped() && (g_cl.m_weapon_id == AUG || g_cl.m_weapon_id == SG553 || g_cl.m_weapon_type == WEAPONTYPE_SNIPER_RIFLE);

		if (can_scope) {
			// always.
			if (g_menu.main.aimbot.zoom.get() == 1) {
				g_cl.m_cmd->m_buttons |= IN_ATTACK2;
				return;
			}

			// hitchance fail.
			else if (g_menu.main.aimbot.zoom.get() == 2 && on && !hit) {
				g_cl.m_cmd->m_buttons |= IN_ATTACK2;
				return;
			}
		}

		if (hit || !on) {
			// right click attack.
			if (g_menu.main.config.mode.get() == 1 && g_cl.m_weapon_id == REVOLVER)
				g_cl.m_cmd->m_buttons |= IN_ATTACK2;

			// left click attack.
			else
				g_cl.m_cmd->m_buttons |= IN_ATTACK;
		}
	}
}

constexpr int total_seedsd = 255;

static int ClipRayToHitbox(const Ray& ray, mstudiobbox_t* hitbox, matrix3x4_t& matrix, CGameTrace& trace)
{
	static auto fn = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 F3 0F 10 42"));

	if (!fn || !hitbox)
		return -1;

	trace.m_fraction = 1.0f;
	trace.m_startsolid = false;

	return fn.as<int(__fastcall*)(const Ray&, mstudiobbox_t*, matrix3x4_t&, CGameTrace&)>()(ray, hitbox, matrix, trace);
}

bool can_hit_hitbox_wrap(const vec3_t& start, const vec3_t& end, Player* player, int hitbox, LagRecord* record, matrix3x4_t* matrix)
{
	auto current_bones = matrix ? matrix : record->m_bones;
	auto model = player->GetModel();
	if (!model)
		return false;

	auto studio_model = g_csgo.m_model_info->GetStudioModel(player->GetModel());
	auto set = studio_model->GetHitboxSet(0);

	if (!set)
		return false;

	auto studio_box = set->GetHitbox(hitbox);
	if (!studio_box)
		return false;

	vec3_t min{ }, max{ };

	math::VectorTransform(studio_box->m_mins, current_bones[studio_box->m_bone], min);
	math::VectorTransform(studio_box->m_maxs, current_bones[studio_box->m_bone], max);

	if (studio_box->m_radius != 1.f)
		return math::SegmentToSegment(start, end, min, max) < studio_box->m_radius;

	math::VectorITransform(start, current_bones[studio_box->m_bone], min);
	math::VectorITransform(end, current_bones[studio_box->m_bone], max);
	return math::IntersectLineWithBB(min, max, studio_box->m_mins, studio_box->m_maxs);
}

bool can_hit_hitbox(const vec3_t& start, const vec3_t& end, Player* player, int hitbox, LagRecord* record)
{
	return can_hit_hitbox_wrap(start, end, player, hitbox, record, nullptr);
}

bool Aimbot::is_accuracy_valid(Player* player, const ang_t& angle)
{
	constexpr float HITCHANCE_MAX = 100.f;
	constexpr int SEED_MAX = 255;

	// Precompute constants for performance optimization
	const float maxHitchance = g_menu.main.aimbot.hitchance_amount.get() * SEED_MAX / HITCHANCE_MAX;
	const float weaponRange = g_cl.m_weapon_info->m_range;

	const vec3_t start = g_cl.m_shoot_pos;
	const ang_t punchAngle = g_cl.m_local->m_aimPunchAngle() * 2.f;

	// Calculate directional vectors
	vec3_t fwd, right, up;
	math::AngleVectors(angle + punchAngle, &fwd, &right, &up);

	// Store off inaccuracy / spread (computed once for efficiency)
	const float inaccuracy = g_cl.m_weapon->GetInaccuracy();
	const float spread = g_cl.m_weapon->GetSpread();

	size_t totalHits = 0;
	size_t neededHits = static_cast<size_t>(std::ceil(maxHitchance));

	for (int i = 0; i <= SEED_MAX; ++i) {
		// Calculate spread
		vec3_t wepSpread = g_cl.m_weapon->CalculateSpread(i, inaccuracy, spread);

		// Calculate direction
		vec3_t dir = (fwd + (right * wepSpread.x) + (up * wepSpread.y)).normalized();

		// Calculate trace end
		vec3_t end = start + (dir * weaponRange);

		// Setup ray and trace
		CGameTrace tr;
		g_csgo.m_engine_trace->ClipRayToEntity(Ray(start, end), MASK_SHOT, player, &tr);

		// Check for valid hits and increment total hits
		if (tr.m_entity == player && game::IsValidHitgroup(tr.m_hitgroup))
			++totalHits;

		// Check if sufficient hits achieved
		if (totalHits >= neededHits)
			return true;

		// Check if impossible to reach needed hits anymore
		if ((SEED_MAX - i + totalHits) < neededHits)
			return false;
	}

	return false;
}

std::vector< std::pair< vec3_t, bool > > AimPlayer::SetupHitboxPoints(LagRecord* record, BoneArray* bones, int index) {
	// reset points.
	std::vector< std::pair< vec3_t, bool > > points = { };

	const model_t* model = m_player->GetModel();
	if (!model)
		return points;

	studiohdr_t* hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return points;

	mstudiohitboxset_t* set = hdr->GetHitboxSet(m_player->m_nHitboxSet());
	if (!set)
		return points;

	mstudiobbox_t* bbox = set->GetHitbox(index);
	if (!bbox)
		return points;

	// get hitbox scales.
	float scale = g_menu.main.aimbot.scale.get() / 100.f;
	float bscale = g_menu.main.aimbot.body_scale.get() / 100.f;

	// these indexes represent boxes.
	if (bbox->m_radius <= 0.f) {
		matrix3x4_t rot_matrix = { };
		g_csgo.AngleMatrix(bbox->m_angle, rot_matrix);

		matrix3x4_t mat = { };
		math::ConcatTransforms(record->m_bones[bbox->m_bone], rot_matrix, mat);

		vec3_t origin = mat.GetOrigin();

		vec3_t center = (bbox->m_mins + bbox->m_maxs) * 0.5f;

		if (index == HITBOX_L_FOOT || index == HITBOX_R_FOOT)
			points.emplace_back(center, true);

		if (points.empty())
			return points;

		for (auto& p : points)
		{
			p.first = { p.first.dot(mat[0]), p.first.dot(mat[1]), p.first.dot(mat[2]) };
			p.first += origin;
		}
	}

	// these hitboxes are capsules.
	else {
		vec3_t max = bbox->m_maxs;
		vec3_t min = bbox->m_mins;
		vec3_t center = (bbox->m_mins + bbox->m_maxs) * 0.5f;

		float head_scale = bbox->m_radius * scale;
		float body_scale = bbox->m_radius * bscale;

		constexpr float rotation = 0.70710678f;
		float near_center_scale = bbox->m_radius * (scale / 2.f);

		if (index == HITBOX_HEAD)
		{
			points.emplace_back(center, true);

			vec3_t point{ };
			point = { max.x + rotation * head_scale, max.y - rotation * head_scale, max.z };
			points.emplace_back(point, false);

			point = { max.x, max.y, max.z + head_scale };
			points.emplace_back(point, false);

			point = { max.x, max.y, max.z - head_scale };
			points.emplace_back(point, false);

			point = { center.x, max.y - head_scale, center.z };
			points.emplace_back(point, false);
		}
		else
		{
			if (index == HITBOX_BODY)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t(center.x, center.y, min.z + body_scale), false);
				points.emplace_back(vec3_t(center.x, center.y, max.z - body_scale), false);
				points.emplace_back(vec3_t{ center.x, max.y - body_scale, center.z }, false);
			}
			else if (index == HITBOX_PELVIS || index == HITBOX_UPPER_CHEST)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t(center.x, center.y, max.z + body_scale), false);
				points.emplace_back(vec3_t(center.x, center.y, min.z - body_scale), false);
			}
			else if (index == HITBOX_THORAX || index == HITBOX_CHEST)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t(center.x, center.y, max.z + body_scale), false);
				points.emplace_back(vec3_t(center.x, center.y, min.z - body_scale), false);

				points.emplace_back(vec3_t{ center.x, max.y - body_scale, center.z }, false);
			}
			else if (index == HITBOX_R_CALF || index == HITBOX_L_CALF)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t{ max.x - (bbox->m_radius / 2.f), max.y, max.z }, false);
			}
			else if (index == HITBOX_R_THIGH || index == HITBOX_L_THIGH)
			{
				points.emplace_back(center, true);
			}
			else if (index == HITBOX_L_UPPER_ARM || index == HITBOX_R_UPPER_ARM)
			{
				points.emplace_back(vec3_t{ max.x + bbox->m_radius, center.y, center.z }, false);
			}
		}

		if (points.empty())
			return points;

		for (auto& p : points)
			math::VectorTransform(p.first, record->m_bones[bbox->m_bone], p.first);
	}

	return points;
}

bool Aimbot::SelectTarget(LagRecord* record, const vec3_t& aim, float damage) {
	float dist, fov, height;
	int   hp;

	// fov check.
	if (g_menu.main.aimbot.fov.get()) {
		// if out of fov, retn false.
		if (math::GetFOV(g_cl.m_view_angles, g_cl.m_shoot_pos, aim) > g_menu.main.aimbot.fov_amount.get())
			return false;
	}

	switch (g_menu.main.aimbot.selection.get()) {

		// distance.
	case 0:
		dist = (record->m_pred_origin - g_cl.m_shoot_pos).length();

		if (dist < m_best_dist) {
			m_best_dist = dist;
			return true;
		}

		break;

		// crosshair.
	case 1:
		fov = math::GetFOV(g_cl.m_view_angles, g_cl.m_shoot_pos, aim);

		if (fov < m_best_fov) {
			m_best_fov = fov;
			return true;
		}

		break;

		// damage.
	case 2:
		if (damage > m_best_damage) {
			m_best_damage = damage;
			return true;
		}

		break;

		// lowest hp.
	case 3:
		// fix for retarded servers?
		hp = std::min(100, record->m_player->m_iHealth());

		if (hp < m_best_hp) {
			m_best_hp = hp;
			return true;
		}

		break;

		// least lag.
	case 4:
		if (record->m_lag < m_best_lag) {
			m_best_lag = record->m_lag;
			return true;
		}

		break;

		// height.
	case 5:
		height = record->m_pred_origin.z - g_cl.m_local->m_vecOrigin().z;

		if (height < m_best_height) {
			m_best_height = height;
			return true;
		}

		break;

	default:
		return false;
	}

	return false;
}

void Aimbot::NoSpread() {
	bool    attack2;
	vec3_t  spread, forward, right, up, dir;

	// revolver state.
	attack2 = (g_cl.m_weapon_id == REVOLVER && (g_cl.m_cmd->m_buttons & IN_ATTACK2));

	// get spread.
	spread = g_cl.m_weapon->CalculateSpread(g_cl.m_cmd->m_random_seed, attack2);

	// compensate.
	g_cl.m_cmd->m_view_angles -= { -math::rad_to_deg(std::atan(spread.length_2d())), 0.f, math::rad_to_deg(std::atan2(spread.x, spread.y)) };
}