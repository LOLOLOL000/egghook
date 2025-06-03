#include "includes.h"
#include "ragebot.h"
#include "threading/threading.h"
#include "threading/shared_mutex.h"

static Semaphore dispatchSem;
static SharedMutex smtx;

using ThreadIDFn = int(_cdecl*)();

ThreadIDFn AllocateThreadID;
ThreadIDFn FreeThreadID;

int AllocateThreadIDWrapper() {
	return AllocateThreadID();
}

int FreeThreadIDWrapper() {
	return FreeThreadID();
}

template<typename T, T& Fn>
static void AllThreadsStub(void*) {
	dispatchSem.Post();
	smtx.rlock();
	smtx.runlock();
	Fn();
}

// TODO: Build this into the threading library
template<typename T, T& Fn>
static void DispatchToAllThreads(void* data) {
	smtx.wlock();

	for (size_t i = 0; i < Threading::numThreads; i++)
		Threading::QueueJobRef(AllThreadsStub<T, Fn>, data);

	for (size_t i = 0; i < Threading::numThreads; i++)
		dispatchSem.Wait();

	smtx.wunlock();

	Threading::FinishQueue(false);
}

Client g_cl{ };

// loader will set this fucker.
char username[33] = "\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x90";

// init routine.
ulong_t __stdcall Client::init(void* arg) {
	// if not in interwebz mode, the driver will not set the username.
	g_cl.m_user = XOR("user");

	auto tier0 = GetModuleHandleA(XOR("tier0.dll"));

	AllocateThreadID = (ThreadIDFn)GetProcAddress(tier0, XOR("AllocateThreadID"));
	FreeThreadID = (ThreadIDFn)GetProcAddress(tier0, XOR("FreeThreadID"));

	Threading::InitThreads();

	DispatchToAllThreads<decltype(AllocateThreadIDWrapper), AllocateThreadIDWrapper>(nullptr);

	// let's also allocate our main thread :)
	AllocateThreadID();

	MH_Initialize();

	// stop here if we failed to acquire all the data needed from csgo.
	if (!g_csgo.init())
		return 0;

	MH_EnableHook(MH_ALL_HOOKS);

	// welcome the user.
	g_notify.add(tfm::format(XOR("welcome %s\n"), g_cl.m_user));

	return 1;
}

bool Client::is_able_to_shoot(bool revolver)
{
	if (!g_cl.m_local || !g_cl.m_weapon)
		return false;

	if (g_cl.m_cmd->m_weapon_select != 0)
		return false;

	if (!g_cl.m_weapon_info)
		return false;

	if (g_cl.m_local->m_fFlags() & 0x40)
		return false;

	if (g_cl.m_local->m_bWaitForNoAttack())
		return false;

	if (g_cl.m_local->m_bIsDefusing())
		return false;

	if (g_cl.m_weapon_info->m_weapon_type >= 1 && g_cl.m_weapon_info->m_weapon_type <= 6 && g_cl.m_weapon->m_iClip1() < 1)
		return false;

	if (g_cl.m_local->m_iPlayerState() > 0)
		return false;

	auto idx = g_cl.m_weapon->m_iItemDefinitionIndex();

	if ((idx == GLOCK || idx == FAMAS) && g_cl.m_weapon->m_iBurstShotsRemaining() > 0)
		return game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) >= g_cl.m_weapon->m_fNextBurstShot();

	if (idx == REVOLVER && revolver)
		return g_cl.m_revolver_fire;

	float next_attack = g_cl.m_local->m_flNextAttack();
	float next_primary_attack = g_cl.m_weapon->m_flNextPrimaryAttack();

	return game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) >= next_attack && game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) >= next_primary_attack;
}

bool Client::is_firing()
{
	auto weapon = g_cl.m_weapon;
	if (!weapon)
		return false;

	bool attack = (g_cl.m_cmd->m_buttons & IN_ATTACK) || (g_cl.m_cmd->m_buttons & IN_ATTACK2);
	short idx = weapon->m_iItemDefinitionIndex();

	if (idx == C4)
		return false;

	if ((idx == GLOCK || idx == FAMAS) && weapon->m_iBurstShotsRemaining() > 0)
	{
		return game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) >= weapon->m_fNextBurstShot();
	}

	if (weapon->is_grenade())
		return !weapon->m_bPinPulled() && weapon->m_fThrowTime() > 0.f && weapon->m_fThrowTime() < game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

	if (weapon->IsKnife())
		return attack && this->is_able_to_shoot();

	return (g_cl.m_cmd->m_buttons & IN_ATTACK) && this->is_able_to_shoot(true);
}


void Client::DrawHUD() {
	if (!g_csgo.m_engine->IsInGame())
		return;

	// get time.
	time_t t = std::time(nullptr);
	std::ostringstream time;
	time << std::put_time(std::localtime(&t), ("%H:%M:%S"));

	// get round trip time in milliseconds.
	int ms = std::max(0, (int)std::round(g_cl.m_latency * 1000.f));

	// get tickrate.
	int rate = (int)std::round(1.f / g_csgo.m_globals->m_interval);

	std::string text = tfm::format(XOR("NegusHook | rtt: %ims | rate: %i | %s"), ms, rate, time.str().data());
	render::FontSize_t size = render::hud.size(text);

	// background.
	render::rect_filled(m_width - size.m_width - 20, 10, size.m_width + 10, size.m_height + 2, { 240, 110, 140, 130 });

	// text.
	render::hud.string(m_width - 15, 10, { 240, 160, 180, 250 }, text, render::ALIGN_RIGHT);
}

void Client::UpdateLocalMatrix()
{
	Player* player = m_local;
	if (!player)
		return;
	
	if (!player->alive() || !m_bones)
		return;

	// adjust render matrix pos
	vec3_t render_origin = player->GetRenderOrigin();
	math::change_matrix_position(m_bones, 128, vec3_t(), render_origin);
	
	// force matrix pos, fix jitter attachments, etc
	player->interpolate_moveparent_pos();
	player->set_bone_cache(m_bones);
	player->attachments_helper();

	// restore matrix pos
	math::change_matrix_position(m_bones, 128, render_origin, vec3_t());
}

void Client::update_viewmodel()
{
	using update_all_viewmodel_addons_fn = int(__fastcall*)(void*);
	static auto update_all_viewmodel_addons = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 ? 83 EC ? 53 8B D9 56 57 8B 03 FF 90 ? ? ? ? 8B F8 89 7C 24 ? 85 FF 0F 84 ? ? ? ? 8B 17 8B CF")).as< update_all_viewmodel_addons_fn>();

	auto viewmodel = g_cl.m_local->get_view_model();
	if (viewmodel)
		update_all_viewmodel_addons(viewmodel);
}

void Client::UpdateInterpLocal()
{
	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	float backup_frametime = g_csgo.m_globals->m_frametime;
	float backup_curtime = g_csgo.m_globals->m_curtime;

	const float v3 = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());
	const float v4 = (v3 / g_csgo.m_globals->m_interval) + .5f;

	g_csgo.m_globals->m_curtime = g_cl.m_local->m_nTickBase() * g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;

	math::clamp(m_interp_angle.x, -90.f, 90.f);
	m_interp_angle.normalize();

	update_viewmodel();

	// CCSGOPlayerAnimState::Update, bypass already animated checks.
	if (state->m_nLastUpdateFrame >= v4)
		state->m_nLastUpdateFrame -= 1;

	if (g_csgo.m_globals->m_curtime != state->m_flLastUpdateTime)
	{
		g_hooks.m_updating_anims[g_cl.m_local->index()] = true;
		state->update(vec3_t(m_interp_angle.x, m_interp_angle.y, m_interp_angle.z));
		g_hooks.m_updating_anims[g_cl.m_local->index()] = false;

		m_abs_yaw = state->m_flFootYaw;

		g_cl.m_local->GetAnimLayers(m_layers);
		g_cl.m_local->GetPoseParameters(m_poses);
	}

	auto ApplyLocalPlayerModifications = [&]() -> void
		{
			// havent got updated layers and poses.
			if (!m_layers || !m_poses)
				return;

			// prevent model sway on player.
			if (m_layers)
				m_layers[12].m_weight = 0.f;
		};

	ApplyLocalPlayerModifications();

	g_cl.m_local->SetAnimLayers(m_layers);
	g_cl.m_local->SetPoseParameters(m_poses);

	g_csgo.m_globals->m_curtime = backup_curtime;
	g_csgo.m_globals->m_frametime = backup_frametime;
}

void Client::UpdateLocal()
{
	if (!g_cl.m_cmd || !g_cl.m_local || !g_cl.m_local->alive())
		return;

	auto state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	// reset animation state
	if (m_spawn != g_cl.m_local->m_flSpawnTime())
	{
		// do it.
		game::ResetAnimationState(state);

		// save time
		m_spawn = g_cl.m_local->m_flSpawnTime();
	}

	if (m_lag > 0)
		return;

	// save sent origin and time.
	m_anim_pos.emplace_front(g_csgo.m_globals->m_curtime, vec3_t(0,0,0), m_cmd->m_view_angles);

	float backup_frametime = g_csgo.m_globals->m_frametime;
	float backup_curtime = g_csgo.m_globals->m_curtime;

	const float v3 = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());
	const float v4 = (v3 / g_csgo.m_globals->m_interval) + .5f;

	g_csgo.m_globals->m_curtime = g_cl.m_local->m_nTickBase() * g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;

	// update time.
	m_anim_frame = game::TICKS_TO_TIME(m_local->m_nTickBase()) - m_anim_time;
	m_anim_time = game::TICKS_TO_TIME(m_local->m_nTickBase());

	// save angles if not flicking
	if (m_anim_time < g_cl.m_body_pred)
		m_hidden_angle = g_cl.m_cmd->m_view_angles;

	// current angle will be animated.
	if (g_menu.main.misc.hide_angles.get()) {
		if (m_anim_time >= m_body_pred && g_hvh.m_mode == AntiAimMode::STAND) {
			m_angle = m_hidden_angle;
		}
		else
			m_angle = g_cl.m_cmd->m_view_angles;
	}
	else
		m_angle = g_cl.m_cmd->m_view_angles;

	math::clamp(m_angle.x, -90.f, 90.f);
	m_angle.normalize();

	// yep..
	update_viewmodel();

	// CCSGOPlayerAnimState::Update, bypass already animated checks.
	if (state->m_nLastUpdateFrame == g_csgo.m_globals->m_frame)
		state->m_nLastUpdateFrame -= 1;

	g_hooks.m_updating_anims[g_cl.m_local->index()] = true;
	state->update(vec3_t(m_angle.x, m_angle.y, m_angle.z));
	g_hooks.m_updating_anims[g_cl.m_local->index()] = false;

	g_cl.m_local->GetAnimLayers(m_layers);
	g_cl.m_local->GetPoseParameters(m_poses);
	g_cl.m_abs_yaw = state->m_flFootYaw;

	if (state->m_bOnGround)
	{
		// from csgo src sdk.
		const float CSGO_ANIM_LOWER_REALIGN_DELAY = 1.1f;

		// we are moving.
		if (state->m_flVelocityLengthXY > 0.1f)
		{
			g_cl.m_body_pred = g_cl.m_anim_time + (CSGO_ANIM_LOWER_REALIGN_DELAY * 0.2f);
			g_cl.m_body = m_angle.y;
		}

		// we arent moving.
		else
		{
			// time for an update.
			if (g_cl.m_anim_time > g_cl.m_body_pred)
			{
				g_cl.m_body_pred = g_cl.m_anim_time + CSGO_ANIM_LOWER_REALIGN_DELAY;
				g_cl.m_body = m_angle.y;
			}
		}
	}

	m_ground = state->m_bOnGround;
	m_rotation = g_cl.m_local->m_angAbsRotation();
	m_speed = state->m_flVelocityLengthXY;

	if (state->m_bOnGround && state->m_flVelocityLengthXY <= 0.1f && !state->m_bLanding && state->m_flLastUpdateIncrement > 0.f)
	{
		float delta = math::normalize(std::abs(state->m_flFootYaw - state->m_flFootYawLast));
		if ((delta / state->m_flLastUpdateIncrement) > 120.f)
		{
			m_layers[3].m_cycle = m_layers[3].m_weight = 0.f;
			m_layers[3].m_sequence = m_local->GetSequenceActivity(979);
		}
	}

	std::array< float, 24 > old_render_poses{ };

	g_cl.m_local->GetPoseParameters(old_render_poses.data());

	g_cl.m_local->SetPoseParameters(m_poses);
	g_cl.m_local->SetAnimLayers(m_layers);
	g_cl.m_local->SetAbsAngles({ 0.f, m_abs_yaw, 0.f });

	g_bones.BuildLocalBones(m_local, BONE_USED_BY_ANYTHING, m_bones, m_local->m_flSimulationTime());

	g_cl.m_local->SetPoseParameters(old_render_poses.data());

	vec3_t render_origin = g_cl.m_local->GetRenderOrigin();
	math::change_matrix_position(m_bones, 128, render_origin, vec3_t());

	g_csgo.m_globals->m_curtime = backup_curtime;
	g_csgo.m_globals->m_frametime = backup_frametime;
}

void Client::KillFeed() {
	if (!g_menu.main.misc.killfeed.get())
		return;

	if (!g_csgo.m_engine->IsInGame())
		return;

	// get the addr of the killfeed.
	KillFeed_t* feed = (KillFeed_t*)g_csgo.m_hud->FindElement(HASH("SFHudDeathNoticeAndBotStatus"));
	if (!feed)
		return;

	int size = feed->notices.Count();
	if (!size)
		return;

	for (int i{ }; i < size; ++i) {
		NoticeText_t* notice = &feed->notices[i];

		// this is a local player kill, delay it.
		if (notice->fade == 1.5f)
			notice->fade = FLT_MAX;
	}
}

void Client::OnPaint() {
	// update screen size.
	g_csgo.m_engine->GetScreenSize(m_width, m_height);

	// render stuff.
	g_visuals.think();
	g_grenades.paint();
	g_notify.think();

	DrawHUD();
	KillFeed();

	// menu goes last.
	g_gui.think();
}

void Client::OnMapload() {
	// store class ids.
	g_netvars.SetupClassData();

	// createmove will not have been invoked yet.
	// but at this stage entites have been created.
	// so now we can retrive the pointer to the local player.
	m_local = g_csgo.m_entlist->GetClientEntity< Player* >(g_csgo.m_engine->GetLocalPlayer());

	g_visuals.ModulateWorld();

	// init knife shit.
	g_skins.load();

	m_sequences.clear();

	// if the INetChannelInfo pointer has changed, store it for later.
	g_csgo.m_net = g_csgo.m_engine->GetNetChannelInfo();

	if (g_csgo.m_net) {
		g_hooks.m_net_channel.reset();
		g_hooks.m_net_channel.init(g_csgo.m_net);
		g_hooks.m_net_channel.add(INetChannel::PROCESSPACKET, util::force_cast(&Hooks::ProcessPacket));
		g_hooks.m_net_channel.add(INetChannel::SENDDATAGRAM, util::force_cast(&Hooks::SendDatagram));
	}
}

void Client::StartMove(CUserCmd* cmd) {
	// save some usercmd stuff.
	m_cmd = cmd;
	m_tick = cmd->m_tick;
	m_view_angles = cmd->m_view_angles;
	m_buttons = cmd->m_buttons;

	// get local ptr.
	m_local = g_csgo.m_entlist->GetClientEntity< Player* >(g_csgo.m_engine->GetLocalPlayer());
	if (!m_local)
		return;

	static auto sv_maxusrcmdprocessticks = g_csgo.m_cvar->FindVar(HASH("sv_maxusrcmdprocessticks"));

	// set max choke
	if (sv_maxusrcmdprocessticks)
		g_cl.m_max_lag = std::max(0, sv_maxusrcmdprocessticks->GetInt() - 2);
	else
		g_cl.m_max_lag = 14;

	// set these
	m_rate = 1.f / g_csgo.m_globals->m_interval;
	m_lag = g_csgo.m_cl->choked_commands;
	m_lerp = game::GetClientInterpAmount();
	m_latency = g_csgo.m_net->GetLatency(INetChannel::FLOW_OUTGOING);
	math::clamp(m_latency, 0.f, 1.f);
	m_latency_ticks = game::TIME_TO_TICKS(m_latency);
	m_server_tick = g_csgo.m_cl->clock_drift_mgr.server_tick;
	m_arrival_tick = m_server_tick + m_latency_ticks;
	m_pressing_move = (m_buttons & (IN_LEFT) || m_buttons & (IN_FORWARD) || m_buttons & (IN_BACK) ||
		m_buttons & (IN_RIGHT) || m_buttons & (IN_MOVELEFT) || m_buttons & (IN_MOVERIGHT) ||
		m_buttons & (IN_JUMP));

	// processing indicates that the localplayer is valid and alive.
	m_processing = m_local && m_local->alive();
	if (!m_processing)
		return;

	// make sure prediction has ran on all usercommands.
	// because prediction runs on frames, when we have low fps it might not predict all usercommands.
	// also fix the tick being inaccurate.
	g_inputpred.update();

	// store some stuff about the local player.
	m_flags = m_local->m_fFlags();

	// ...
	m_shot = false;
}


void Client::BackupPlayers(bool restore) {
	if (restore) {
		// restore stuff.
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

			if (!g_aimbot.IsValidTarget(player))
				continue;

			g_aimbot.m_backup[i - 1].restore(player);
		}
	}

	else {
		// backup stuff.
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

			if (!g_aimbot.IsValidTarget(player))
				continue;

			g_aimbot.m_backup[i - 1].store(player);
		}
	}
}

void Client::DoMove() {
	// backup strafe angles (we need them for input prediction)
	m_strafe_angles = m_cmd->m_view_angles;

	// run movement code before input prediction.
	g_movement.JumpRelated();
	g_movement.Strafe();
	g_movement.FakeWalk();
	g_movement.FastStop();
	g_movement.update_ground_ticks();

	// predict input.
	g_inputpred.run();

	// restore original angles after input prediction
	m_cmd->m_view_angles = m_view_angles;

	// convert viewangles to directional forward vector.
	math::AngleVectors(m_view_angles, &m_forward_dir);

	// store stuff after input pred.
	g_inputpred.UpdatePitch(m_orig_ang.x);

	// reset shit.
	m_weapon = nullptr;
	m_weapon_info = nullptr;
	m_weapon_id = -1;
	m_weapon_type = WEAPONTYPE_UNKNOWN;
	m_player_fire = m_weapon_fire = false;

	// store weapon stuff.
	m_weapon = m_local->GetActiveWeapon();

	if (m_weapon) {
		m_weapon_info = m_weapon->GetWpnData();
		m_weapon_id = m_weapon->m_iItemDefinitionIndex();
		m_weapon_type = m_weapon_info->m_weapon_type;
		g_aimbot.start_stop();

		// ensure weapon spread values / etc are up to date.
		if (m_weapon_type != WEAPONTYPE_GRENADE)
			m_weapon->UpdateAccuracyPenalty();

		// can the player fire.
		m_player_fire = g_csgo.m_globals->m_curtime >= m_local->m_flNextAttack() && !g_csgo.m_gamerules->m_bFreezePeriod() && !(g_cl.m_flags & FL_FROZEN);

		g_movement.m_max_speed = g_cl.m_local->m_bIsScoped() ? g_cl.m_weapon_info->m_max_player_speed_alt : g_cl.m_weapon_info->m_max_player_speed;

		UpdateRevolverCock();
		m_weapon_fire = is_able_to_shoot(true);
	}

	// last tick defuse.
	// todo - dex;  figure out the range for CPlantedC4::Use?
	//              add indicator if valid (on ground, still have time, not being defused already, etc).
	//              move this? not sure where we should put it.
	if (g_input.GetKeyState(g_menu.main.misc.last_tick_defuse.get()) && g_visuals.m_c4_planted) {
		float defuse = (m_local->m_bHasDefuser()) ? 5.f : 10.f;
		float remaining = g_visuals.m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;
		float dt = remaining - defuse - (g_cl.m_latency / 2.f);

		m_cmd->m_buttons &= ~IN_USE;
		if (dt <= game::TICKS_TO_TIME(2))
			m_cmd->m_buttons |= IN_USE;
	}

	// grenade prediction.
	g_grenades.think();

	// run fakelag.
	g_hvh.SendPacket();

	// run aimbot.
	g_aimbot.think();

	// run antiaims.
	g_hvh.AntiAim();
}

void Client::EndMove(CUserCmd* cmd) {
	UpdateLocal();

	if (!g_csgo.m_cl->choked_commands)
		m_interp_angle = m_cmd->m_view_angles;

	// if matchmaking mode, anti untrust clamp.
	if (g_menu.main.config.mode.get() == 0)
		m_cmd->m_view_angles.SanitizeAngle();

	// fix our movement.
	g_movement.FixMove(cmd, m_strafe_angles);

	// this packet will be sent.
	if (*m_packet) {
		g_hvh.m_step_switch = (bool)g_csgo.RandomInt(0, 1);

		// we are sending a packet, so this will be reset soon.
		// store the old value.
		m_old_lag = m_lag;

		// get radar angles.
		m_radar = cmd->m_view_angles;
		m_radar.normalize();

		// get current origin.
		vec3_t cur = m_local->m_vecOrigin();

		// get prevoius origin.
		vec3_t prev = m_net_pos.empty() ? cur : m_net_pos.front().m_pos;

		// check if we broke lagcomp.
		m_lagcomp = (cur - prev).length_sqr() > 4096.f;

		// save sent origin and time.
		m_net_pos.emplace_front(g_csgo.m_globals->m_curtime, cur, m_cmd->m_view_angles);

		// for fakelag
		g_cl.sent_tick_count = g_csgo.m_globals->m_tick_count;
	}

	// store some values for next tick.
	m_old_packet = *m_packet;
	m_old_shot = m_shot;
}

void Client::OnTick(CUserCmd* cmd) {
	// TODO; add this to the menu.
	if (g_menu.main.misc.ranks.get() && cmd->m_buttons & IN_SCORE) {
		static CCSUsrMsg_ServerRankRevealAll msg{ };
		g_csgo.ServerRankRevealAll(&msg);
	}

	// store some data and update prediction.
	StartMove(cmd);

	// not much more to do here.
	if (!m_processing) {
		if (g_hvh.commands.size() > 0)
			g_hvh.commands.clear();

		if (g_hvh.choked_ticks.size() > 0)
			g_hvh.choked_ticks.clear();

		if (g_hvh.choked_commands.size() > 0)
			g_hvh.choked_commands.clear();

		return;
	}

	// save the original state of players.
	BackupPlayers(false);

	// run all movement related code.
	DoMove();

	// store stome additonal stuff for next tick
	// sanetize our usercommand if needed and fix our movement.
	EndMove(cmd);

	// restore the players.
	BackupPlayers(true);

	// restore curtime/frametime
	// and prediction seed/player.
	g_inputpred.restore();
}

void Client::UpdateAnimations() {
	if (!g_cl.m_local || !g_cl.m_processing)
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	// update abs yaw with last networked abs yaw.
	g_cl.m_local->SetAbsAngles(ang_t(0.f, g_cl.m_abs_yaw, 0.f));

	if (g_csgo.m_input->CAM_IsThirdPerson())
		*reinterpret_cast<ang_t*>(uintptr_t(g_cl.m_local) + 0x31C4 + 0x4) = m_interp_angle;
}

void Client::UpdateInformation()
{
	if (g_cl.m_lag > 0)
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	// update time.
	m_anim_frame = g_csgo.m_globals->m_curtime - m_anim_time;
	m_anim_time = g_csgo.m_globals->m_curtime;

	// current angle will be animated.
	m_angle = g_cl.m_cmd->m_view_angles;

	math::clamp(m_angle.x, -90.f, 90.f);
	m_angle.normalize();

	// set lby to predicted value.
	//g_cl.m_local->m_flLowerBodyYawTarget() = m_body;

	if (state->m_bOnGround)
	{
		// from csgo src sdk.
		const float CSGO_ANIM_LOWER_REALIGN_DELAY = 1.1f;

		// we are moving.
		if (state->m_flVelocityLengthXY > 0.1f)
		{
			g_cl.m_body_pred = g_cl.m_anim_time + (CSGO_ANIM_LOWER_REALIGN_DELAY * 0.2f);
			g_cl.m_body = m_angle.y;
		}

		// we arent moving.
		else
		{
			// time for an update.
			if (g_cl.m_anim_time > g_cl.m_body_pred)
			{
				g_cl.m_body_pred = g_cl.m_anim_time + CSGO_ANIM_LOWER_REALIGN_DELAY;
				g_cl.m_body = m_angle.y;
			}
		}
	}

	// save updated data.
	m_rotation = g_cl.m_local->m_angAbsRotation();
	m_speed = state->m_flVelocityLengthXY;
	m_ground = state->m_bOnGround;
}

void Client::print(const std::string text, ...) {
	va_list     list;
	int         size;
	std::string buf;

	if (text.empty())
		return;

	va_start(list, text);

	// count needed size.
	size = std::vsnprintf(0, 0, text.c_str(), list);

	// allocate.
	buf.resize(size);

	// print to buffer.
	std::vsnprintf(buf.data(), size + 1, text.c_str(), list);

	va_end(list);

	// print to console.
	g_csgo.m_cvar->ConsoleColorPrintf(colors::burgundy, XOR("[supremacy] "));
	g_csgo.m_cvar->ConsoleColorPrintf(colors::white, buf.c_str());
}

bool Client::CanFireWeapon() {
	// the player cant fire.
	if (!m_player_fire)
		return false;

	if (m_weapon_type == WEAPONTYPE_GRENADE)
		return false;

	// if we have no bullets, we cant shoot.
	if (m_weapon_type != WEAPONTYPE_KNIFE && m_weapon->m_iClip1() < 1)
		return false;

	// do we have any burst shots to handle?
	if ((m_weapon_id == GLOCK || m_weapon_id == FAMAS) && m_weapon->m_iBurstShotsRemaining() > 0) {
		// new burst shot is coming out.
		if (g_csgo.m_globals->m_curtime >= m_weapon->m_fNextBurstShot())
			return true;
	}

	// r8 revolver.
	if (m_weapon_id == REVOLVER) {
		int act = m_weapon->m_Activity();

		// mouse1.
		if (!m_revolver_fire) {
			if ((act == 185 || act == 193) && m_revolver_cock == 0)
				return g_csgo.m_globals->m_curtime >= m_weapon->m_flNextPrimaryAttack();

			return false;
		}
	}

	// yeez we have a normal gun.
	if (g_csgo.m_globals->m_curtime >= m_weapon->m_flNextPrimaryAttack())
		return true;

	return false;
}

void Client::UpdateRevolverCock() {
	// default to false.
	m_revolver_fire = false;

	// reset properly.
	if (m_revolver_cock == -1)
		m_revolver_cock = 0;

	// we dont have a revolver.
	// we have no ammo.
	// player cant fire
	// we are waiting for we can shoot again.
	if (m_weapon_id != REVOLVER || m_weapon->m_iClip1() < 1 || !m_player_fire || g_csgo.m_globals->m_curtime < m_weapon->m_flNextPrimaryAttack()) {
		// reset.
		m_revolver_cock = 0;
		m_revolver_query = 0;
		return;
	}

	// calculate max number of cocked ticks.
	// round to 6th decimal place for custom tickrates..
	int shoot = (int)(0.25f / (std::round(g_csgo.m_globals->m_interval * 1000000.f) / 1000000.f));

	// amount of ticks that we have to query.
	m_revolver_query = shoot - 1;

	// we held all the ticks we needed to hold.
	if (m_revolver_query == m_revolver_cock) {
		// reset cocked ticks.
		m_revolver_cock = -1;

		// we are allowed to fire, yay.
		m_revolver_fire = true;
	}

	else {
		// we still have ticks to query.
		// apply inattack.
		if (g_menu.main.config.mode.get() == 0 && m_revolver_query > m_revolver_cock)
			m_cmd->m_buttons |= IN_ATTACK;

		// count cock ticks.
		// do this so we can also count 'legit' ticks
		// that didnt originate from the hack.
		if (m_cmd->m_buttons & IN_ATTACK)
			m_revolver_cock++;

		// inattack was not held, reset.
		else m_revolver_cock = 0;
	}

	// remove inattack2 if cocking.
	if (m_revolver_cock > 0)
		m_cmd->m_buttons &= ~IN_ATTACK2;
}

void Client::UpdateIncomingSequences() {
	if (!g_csgo.m_net)
		return;

	if (m_sequences.empty() || g_csgo.m_net->m_in_seq > m_sequences.front().m_seq) {
		// store new stuff.
		m_sequences.emplace_front(g_csgo.m_globals->m_realtime, g_csgo.m_net->m_in_rel_state, g_csgo.m_net->m_in_seq);
	}

	// do not save too many of these.
	while (m_sequences.size() > 2048)
		m_sequences.pop_back();
}