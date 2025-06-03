#include "includes.h"
#include "pred.h"

InputPrediction g_inputpred{};;

void InputPrediction::update() {
	bool        valid{ g_csgo.m_cl->delta_tick > 0 };
	//int         outgoing_command, current_command;
	//CUserCmd    *cmd;

	int start = g_csgo.m_cl->last_command_ack;
	int stop = g_csgo.m_cl->last_outgoing_command + g_csgo.m_cl->choked_commands;

	// call CPrediction::Update.
	if (g_cl.m_stage == FRAME_NET_UPDATE_END)
		g_csgo.m_prediction->Update(g_csgo.m_cl->delta_tick, valid, start, stop);
}

void InputPrediction::run() {
	static CMoveData data{};

	// CPrediction::StartCommand
	g_cl.m_local->m_pCurrentCommand() = g_cl.m_cmd;
	g_cl.m_local->m_PlayerCommand() = *g_cl.m_cmd;
	*g_csgo.m_nPredictionRandomSeed = g_cl.m_cmd->m_random_seed;
	g_csgo.m_pPredictionPlayer = g_cl.m_local;

	// restore the globals
	m_curtime = g_csgo.m_globals->m_curtime;
	m_frametime = g_csgo.m_globals->m_frametime;

	// Set globals appropriately
	g_csgo.m_globals->m_curtime = g_cl.m_local->m_nTickBase() * g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_frametime = g_csgo.m_prediction->m_engine_paused ? 0 : g_csgo.m_globals->m_interval;

	g_csgo.m_move_helper->SetHost(g_cl.m_local);
	g_csgo.m_game_movement->StartTrackPredictionErrors(g_cl.m_local);


	// Get button states
	auto backup_buttons = g_cl.m_cmd->m_buttons;

	auto changed_buttons = backup_buttons ^ *(int*)((std::uintptr_t)g_cl.m_local + 0x31e8);

	*(int*)((std::uintptr_t)g_cl.m_local + 0x31dc) = *(int*)((std::uintptr_t)g_cl.m_local + 0x31e8);
	*(int*)((std::uintptr_t)g_cl.m_local + 0x31e8) = backup_buttons;
	*(int*)((std::uintptr_t)g_cl.m_local + 0x31e0) = backup_buttons & changed_buttons; // button pressed
	*(int*)((std::uintptr_t)g_cl.m_local + 0x31e4) = changed_buttons & ~backup_buttons; // button released

	g_csgo.m_prediction->CheckMovingGround(g_cl.m_local, g_csgo.m_globals->m_frametime);

	// Copy from command to player unless game .dll has set angle using fixangle
	g_csgo.m_prediction->SetLocalViewAngles(g_cl.m_cmd->m_view_angles);

	// Call standard client pre-think
	g_cl.m_local->PreThink();

	// Call Think if one is set
	g_cl.m_local->Think();

	g_csgo.m_prediction->SetupMove(g_cl.m_local, g_cl.m_cmd, g_csgo.m_move_helper, &data);

	// CPrediction::ProcessMovement
	g_csgo.m_game_movement->ProcessMovement(g_cl.m_local, &data);

	g_csgo.m_prediction->FinishMove(g_cl.m_local, g_cl.m_cmd, &data);

	// Let server invoke any needed impact functions
	g_csgo.m_move_helper->ProcessImpacts();

	g_cl.m_local->PostThink();
}

void InputPrediction::restore() {
	// finish it
	g_csgo.m_game_movement->FinishTrackPredictionErrors(g_cl.m_local);

	// reset target player ( host ).
	g_csgo.m_move_helper->SetHost(nullptr);

	// CPrediction::FinishCommand
	g_cl.m_local->m_pCurrentCommand() = nullptr;
	*g_csgo.m_nPredictionRandomSeed = -1;
	g_csgo.m_pPredictionPlayer = nullptr;

	// fixes a crash: when loading highlights twice or after previously loaded map, there was a dirty player pointer in game movement
	g_csgo.m_game_movement->Reset();

	if (g_csgo.m_globals->m_frametime > 0)
		g_cl.m_local->m_nTickBase()++;

	// restore globals.
	g_csgo.m_globals->m_curtime = m_curtime;
	g_csgo.m_globals->m_frametime = m_frametime;
}

void InputPrediction::UpdatePitch(const float& pitch)
{
	auto state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;

	const auto old_abs_angles = g_cl.m_local->GetAbsAngles();
	const auto old_poses = *(float*)((uintptr_t)g_cl.m_local + (g_entoffsets.m_flPoseParameter + 48));

	g_cl.m_local->SetAbsAngles({ 0.f, state->m_flFootYaw, 0.f });

	auto eye_pitch = math::NormalizedAngle(pitch);

	if (eye_pitch > 180.f)
		eye_pitch = eye_pitch - 360.f;

	eye_pitch = std::clamp(eye_pitch, -90.f, 90.f);
	*(float*)((uintptr_t)g_cl.m_local + (g_entoffsets.m_flPoseParameter + 48)) = std::clamp((eye_pitch + 90.f) / 180.f, 0.0f, 1.0f);

	g_cl.m_local->InvalidateBoneCache();

	const auto old_abs_origin = g_cl.m_local->GetAbsOrigin();
	g_cl.m_local->SetAbsOrigin(g_cl.m_local->m_vecOrigin());

	g_bones.BuildLocalBones(g_cl.m_local, 0x100, nullptr, g_cl.m_local->m_flSimulationTime());

	g_cl.m_local->SetAbsOrigin(old_abs_origin);

	g_cl.m_shoot_pos = g_cl.m_local->GetShootPosition();

	*(float*)((uintptr_t)g_cl.m_local + (g_entoffsets.m_flPoseParameter + 48)) = old_poses;
	g_cl.m_local->SetAbsAngles(old_abs_angles);
}
