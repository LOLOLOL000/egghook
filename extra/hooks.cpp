#include "includes.h"

Hooks                g_hooks{ };;
CustomEntityListener g_custom_entity_listener{ };;

void Pitch_proxy(CRecvProxyData* data, Address ptr, Address out) {
	// normalize this fucker.
	math::NormalizeAngle(data->m_Value.m_Float);

	// clamp to remove retardedness.
	math::clamp(data->m_Value.m_Float, -90.f, 90.f);

	// call original netvar proxy.
	if (g_hooks.m_Pitch_original)
		g_hooks.m_Pitch_original(data, ptr, out);
}

void Body_proxy(CRecvProxyData* data, Address ptr, Address out) {
	Stack stack;

	static Address RecvTable_Decode{ pattern::find(g_csgo.m_engine_dll, XOR("EB 0D FF 77 10")) };

	// call from entity going into pvs.
	if (stack.next().next().ReturnAddress() != RecvTable_Decode) {
		// convert to player.
		Player* player = ptr.as< Player* >();

		// store data about the update.
		g_resolver.OnBodyUpdate(player, data->m_Value.m_Float);
	}

	// call original proxy.
	if (g_hooks.m_Body_original)
		g_hooks.m_Body_original(data, ptr, out);
}

void AbsYaw_proxy(CRecvProxyData* data, Address ptr, Address out) {
	// convert to ragdoll.
	//Ragdoll* ragdoll = ptr.as< Ragdoll* >( );

	// get ragdoll owner.
	//Player* player = ragdoll->GetPlayer( );

	// get data for this player.
	/*AimPlayer* aim = &g_aimbot.m_players[ player->index( ) - 1 ];

	if( player && aim ) {
	if( !aim->m_records.empty( ) ) {
	LagRecord* match{ nullptr };

	// iterate records.
	for( const auto &it : aim->m_records ) {
	// find record that matches with simulation time.
	if( it->m_sim_time == player->m_flSimulationTime( ) ) {
	match = it.get( );
	break;
	}
	}

	// we have a match.
	// and it is standing
	// TODO; add air?
	if( match /*&& match->m_mode == Resolver::Modes::RESOLVE_STAND*/// ) {
	/*	RagdollRecord record;
	record.m_record   = match;
	record.m_rotation = math::NormalizedAngle( data->m_Value.m_Float );
	record.m_delta    = math::NormalizedAngle( record.m_rotation - match->m_lbyt );

	float death = math::NormalizedAngle( ragdoll->m_flDeathYaw( ) );

	// store.
	//aim->m_ragdoll.push_front( record );

	//g_cl.print( tfm::format( XOR( "rot %f death %f delta %f\n" ), record.m_rotation, death, record.m_delta ).data( ) );
	}
	}*/
	//}

	// call original netvar proxy.
	if (g_hooks.m_AbsYaw_original)
		g_hooks.m_AbsYaw_original(data, ptr, out);
}

void Force_proxy(CRecvProxyData* data, Address ptr, Address out) {
	// convert to ragdoll.
	Ragdoll* ragdoll = ptr.as< Ragdoll* >();

	// get ragdoll owner.
	Player* player = ragdoll->GetPlayer();

	// we only want this happening to noobs we kill.
	if (g_menu.main.misc.ragdoll_force.get() && g_cl.m_local && player && player->enemy(g_cl.m_local)) {
		// get m_vecForce.
		vec3_t vel = { data->m_Value.m_Vector[0], data->m_Value.m_Vector[1], data->m_Value.m_Vector[2] };

		// give some speed to all directions.
		vel *= 1000.f;

		// boost z up a bit.
		if (vel.z <= 1.f)
			vel.z = 2.f;

		vel.z *= 2.f;

		// don't want crazy values for this... probably unlikely though?
		math::clamp(vel.x, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());
		math::clamp(vel.y, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());
		math::clamp(vel.z, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());

		// set new velocity.
		data->m_Value.m_Vector[0] = vel.x;
		data->m_Value.m_Vector[1] = vel.y;
		data->m_Value.m_Vector[2] = vel.z;
	}

	if (g_hooks.m_Force_original)
		g_hooks.m_Force_original(data, ptr, out);
}

void Hooks::init() {
	// hook wndproc.
	m_old_wndproc = (WNDPROC)g_winapi.SetWindowLongA(g_csgo.m_game->m_hWindow, GWL_WNDPROC, util::force_cast<LONG>(Hooks::WndProc));

	// setup normal VMT hooks.
	m_panel.init(g_csgo.m_panel);
	m_panel.add(IPanel::PAINTTRAVERSE, util::force_cast(&Hooks::PaintTraverse));

	m_client.init(g_csgo.m_client);
	m_client.add(CHLClient::LEVELINITPREENTITY, util::force_cast(&Hooks::LevelInitPreEntity));
	m_client.add(CHLClient::LEVELINITPOSTENTITY, util::force_cast(&Hooks::LevelInitPostEntity));
	m_client.add(CHLClient::LEVELSHUTDOWN, util::force_cast(&Hooks::LevelShutdown));
	//m_client.add( CHLClient::INKEYEVENT, util::force_cast( &Hooks::IN_KeyEvent ) );
	m_client.add(CHLClient::FRAMESTAGENOTIFY, util::force_cast(&Hooks::FrameStageNotify));

	m_engine.init(g_csgo.m_engine);
	m_engine.add(IVEngineClient::ISCONNECTED, util::force_cast(&Hooks::IsConnected));
	m_engine.add(IVEngineClient::ISHLTV, util::force_cast(&Hooks::IsHLTV));
	m_engine.add(IVEngineClient::ISPAUSED, util::force_cast(&Hooks::IsPaused));

	m_engine_sound.init(g_csgo.m_sound);
	m_engine_sound.add(IEngineSound::EMITSOUND, util::force_cast(&Hooks::EmitSound));

	m_prediction.init(g_csgo.m_prediction);
	m_prediction.add(CPrediction::INPREDICTION, util::force_cast(&Hooks::InPrediction));
	m_prediction.add(CPrediction::RUNCOMMAND, util::force_cast(&Hooks::RunCommand));

	m_client_mode.init(g_csgo.m_client_mode);
	m_client_mode.add(IClientMode::SHOULDDRAWPARTICLES, util::force_cast(&Hooks::ShouldDrawParticles));
	m_client_mode.add(IClientMode::SHOULDDRAWFOG, util::force_cast(&Hooks::ShouldDrawFog));
	m_client_mode.add(IClientMode::OVERRIDEVIEW, util::force_cast(&Hooks::OverrideView));
	m_client_mode.add(IClientMode::CREATEMOVE, util::force_cast(&Hooks::CreateMove));
	m_client_mode.add(IClientMode::DOPOSTSPACESCREENEFFECTS, util::force_cast(&Hooks::DoPostScreenSpaceEffects));

	m_surface.init(g_csgo.m_surface);
	//m_surface.add( ISurface::GETSCREENSIZE, util::force_cast( &Hooks::GetScreenSize ) );
	m_surface.add(ISurface::LOCKCURSOR, util::force_cast(&Hooks::LockCursor));
	m_surface.add(ISurface::PLAYSOUND, util::force_cast(&Hooks::PlaySound));
	m_surface.add(ISurface::ONSCREENSIZECHANGED, util::force_cast(&Hooks::OnScreenSizeChanged));
	m_surface.add(ISurface::DRAWSETCOLOR, util::force_cast(&Hooks::HKDrawSetColor));

	m_model_render.init(g_csgo.m_model_render);
	m_model_render.add(IVModelRender::DRAWMODELEXECUTE, util::force_cast(&Hooks::DrawModelExecute));

	m_render_view.init(g_csgo.m_render_view);
	m_render_view.add(IVRenderView::SCENEEND, util::force_cast(&Hooks::SceneEnd));

	m_shadow_mgr.init(g_csgo.m_shadow_mgr);
	m_shadow_mgr.add(IClientShadowMgr::COMPUTESHADOWDEPTHTEXTURES, util::force_cast(&Hooks::ComputeShadowDepthTextures));

	m_view_render.init(g_csgo.m_view_render);
	m_view_render.add(CViewRender::ONRENDERSTART, util::force_cast(&Hooks::OnRenderStart));
	m_view_render.add(CViewRender::RENDER2DEFFECTSPOSTHUD, util::force_cast(&Hooks::Render2DEffectsPostHUD));
	m_view_render.add(CViewRender::RENDERSMOKEOVERLAY, util::force_cast(&Hooks::RenderSmokeOverlay));

	m_match_framework.init(g_csgo.m_match_framework);
	m_match_framework.add(CMatchFramework::GETMATCHSESSION, util::force_cast(&Hooks::GetMatchSession));

	m_material_system.init(g_csgo.m_material_system);
	m_material_system.add(IMaterialSystem::OVERRIDECONFIG, util::force_cast(&Hooks::OverrideConfig));

	m_fire_bullets.init(g_csgo.TEFireBullets);
	m_fire_bullets.add(7, util::force_cast(&Hooks::PostDataUpdate));

	m_client_state.init(g_csgo.m_hookable_cl);
	m_client_state.add(CClientState::TEMPENTITIES, util::force_cast(&Hooks::TempEntities));
	m_client_state.add(CClientState::PACKETSTART, util::force_cast(&Hooks::packet_start));
	m_client_state.add(CClientState::PACKETEND, util::force_cast(&Hooks::packet_end));

	// register our custom entity listener.
	// todo - dex; should we push our listeners first? should be fine like this.
	g_custom_entity_listener.init();

	//Address m_pVtable = pattern::find(g_csgo.m_client_dll, "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C").add(0x45).add(0x2);

	HOOK(pattern::find(g_csgo.m_engine_dll, "55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80"), min_hooks::check_file_crc_with_server, min_hooks::orig_check_file_crc_with_server);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"), min_hooks::update_client_side_anim, min_hooks::orig_update_client_side_anim);
	HOOK(pattern::find(g_csgo.m_client_dll, "53 56 57 8B 3D ? ? ? ? 8B F1 85 FF"), min_hooks::want_reticle_shown, min_hooks::orig_want_reticle_shown);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 83 E4 F0 B8 D8"), min_hooks::setup_bones, min_hooks::orig_setup_bones);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 56 8B 75 ? 57 FF 75 ? 8B F9 56"), min_hooks::build_transformations, min_hooks::orig_build_transformations);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 56 8B 75 08 57 FF 75 18"), min_hooks::add_renderable, min_hooks::orig_add_renderable);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 83 E4 F8 83 EC 58 56 57 8B F9 83 7F 60"), min_hooks::modify_eye_position, min_hooks::orig_modify_eye_position);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 51 80 3D ? ? ? ? ? 0F 57"), min_hooks::GetExposureRange, min_hooks::OrigGetExposureRange);
	//HOOK(pattern::find(g_csgo.m_client_dll, "56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21"), min_hooks::physics_simulate, min_hooks::orig_physics_simulate);
	//HOOK(pattern::find(g_csgo.m_engine_dll, "55 8B EC 81 EC ?? ?? ?? ?? 56 8B F1 8D 4D 98 E8"), min_hooks::CL_DispatchSound, min_hooks::OrigCLDispatchSound);
	HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 83 EC 18 56 8B F1 C7 45"), min_hooks::init_new_particles_scalar, min_hooks::orig_init_new_particles_scalar);
	//HOOK(pattern::find(g_csgo.m_client_dll, "55 8B EC 53 8B D9 56 57 8B 53 5C"), min_hooks::SetVisualsData, min_hooks::fn_set_visuals_data);
	//HOOK(pattern::find(g_csgo.m_client_dll, "53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C ? ? 8B EC 83 EC ? 56 57 8B F9 89 7D ? 8B 4F"), min_hooks::render_glow_boxes, min_hooks::orig_render_glow_boxes);
	//HOOK(pattern::find(g_csgo.m_client_dll, "E8 ? ? ? ? 8B 0E 83 C1 10"), min_hooks::CParticleCollection_Simulate, min_hooks::OrigCParticleCollection_Simulate);
	HOOK_VFUNC(g_csgo.m_engine->bsp_tree_query(), 6, min_hooks::list_leaves_in_box, min_hooks::orig_list_leaves_in_box);
	//HOOK_VFUNC(m_pVtable, 164, min_hooks::eye_angles, min_hooks::orig_eye_angles);

	//HOOK(pattern::find(g_csgo.m_materialsystem_dll, "55 8B EC 83 EC ? 56 8B F1 8A 46"), min_hooks::GetColorModulation, min_hooks::OrigGetColorModulation);
	HOOK(pattern::find(g_csgo.m_engine_dll, "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D"),
		min_hooks::IsUsingStaticPropDebugModes, min_hooks::OrigIsUsingStaticPropDebugModes);


	// set netvar proxies.
	g_netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_angEyeAngles[0]"), Pitch_proxy, m_Pitch_original);
	g_netvars.SetProxy(HASH("DT_CSPlayer"), HASH("m_flLowerBodyYawTarget"), Body_proxy, m_Body_original);
	g_netvars.SetProxy(HASH("DT_CSRagdoll"), HASH("m_vecForce"), Force_proxy, m_Force_original);
	g_netvars.SetProxy(HASH("DT_CSRagdoll"), HASH("m_flAbsYaw"), AbsYaw_proxy, m_AbsYaw_original);
}

void __fastcall min_hooks::build_transformations(Player* ecx, void* edx, CStudioHdr* hdr, int a3, int a4, int a5, int a6, int a7)
{
	if (g_csgo.m_cl->delta_tick == -1 || !ecx || !ecx->IsPlayer() || !ecx->alive())
		return orig_build_transformations(ecx, edx, hdr, a3, a4, a5, a6, a7);

	auto studio_hdr = ecx->m_studioHdr();
	auto& jiggle_bones = ecx->get<bool>(0x291C);

	// remove bone jiggling a.k.a weapon shaking
	auto old_jiggle_bones = jiggle_bones;
	jiggle_bones = false;

	auto& use_new_animstate = *(bool*)((uintptr_t)ecx + 0x39E1);

	// disable bone snapshots (advanced body anims that breaks whole model)
	auto old_use_state = use_new_animstate;
	use_new_animstate = false;

	orig_build_transformations(ecx, edx, hdr, a3, a4, a5, a6, a7);

	use_new_animstate = old_use_state;
	jiggle_bones = old_jiggle_bones;
}

void __fastcall min_hooks::modify_eye_position(CCSGOPlayerAnimState* ecx, void* edx, vec3_t& pos)
{
	auto old = ecx->m_bSmoothHeightValid;

	ecx->m_bSmoothHeightValid = false;

	orig_modify_eye_position(ecx, edx, pos);

	ecx->m_bSmoothHeightValid = old;
}

void __fastcall min_hooks::add_renderable(void* ecx, void* edx, IClientRenderable* pRenderable, bool bRenderWithViewModels, int nType, int nModelType, int nSplitscreenEnabled)
{
	auto renderable_addr = (uintptr_t)pRenderable;
	if (!renderable_addr || renderable_addr == 0x4)
		return orig_add_renderable(ecx, edx, pRenderable, bRenderWithViewModels, nType, nModelType, nSplitscreenEnabled);

	auto entity = (Entity*)(renderable_addr - 0x4);
	int index = *(int*)((uintptr_t)entity + 0x64);

	if (index < 1 || index > 64)
		return orig_add_renderable(ecx, edx, pRenderable, bRenderWithViewModels, nType, nModelType, nSplitscreenEnabled);

	if (index == g_csgo.m_engine->GetLocalPlayer())
		nType = 1;
	else
		nType = 2;

	orig_add_renderable(ecx, edx, pRenderable, bRenderWithViewModels, nType, nModelType, nSplitscreenEnabled);
}

bool __fastcall min_hooks::want_reticle_shown(void* ecx, void* edx)
{
	static auto return_addr_process_input = pattern::find(g_csgo.m_client_dll, XOR("84 C0 74 ? 68 ? ? ? ? 8D 8C 24"));

	if (!g_cl.m_local)
		return orig_want_reticle_shown(ecx, edx);

	Weapon* weapon = g_cl.m_weapon;
	if (!weapon)
		return orig_want_reticle_shown(ecx, edx);

	if (!g_menu.main.visuals.force_xhair.get() || g_cl.m_local->m_bIsScoped())
		return orig_want_reticle_shown(ecx, edx);

	if (!weapon || !weapon->is_scoping_weapon())
		return orig_want_reticle_shown(ecx, edx);

	if ((uintptr_t)_ReturnAddress() != return_addr_process_input.as< uintptr_t >())
		return orig_want_reticle_shown(ecx, edx);

	return true;
}

void __fastcall min_hooks::physics_simulate(Player* const ecx, const std::uintptr_t edx)
{
	if (ecx != g_cl.m_local
		|| !ecx->alive()
		|| ecx->sim_tick() == g_csgo.m_globals->m_tick_count)
		return orig_physics_simulate(ecx, edx);

	auto& ctx = ecx->cmd_context();

	auto old_mod = ecx->velocity_modifier();

	if (ctx.m_user_cmd.m_command_number == g_csgo.m_cl->last_command_ack + 1)
		ecx->velocity_modifier() = g_cl.velocity_modifier;

	orig_physics_simulate(ecx, edx);

	ecx->velocity_modifier() = old_mod;
}

void __fastcall min_hooks::check_file_crc_with_server(void* ecx, void* edx)
{
	return;
}

void __fastcall min_hooks::SetVisualsData(void* ecx, void* edx, char* pCompositingShaderName)
{
	return fn_set_visuals_data(ecx, edx, pCompositingShaderName);
}

bool __fastcall min_hooks::setup_bones(const std::uintptr_t ecx, const std::uintptr_t edx, matrix3x4_t* const bones, int max_bones, int mask, float time)
{
	const auto player = reinterpret_cast <Player*> (ecx - sizeof(std::ptrdiff_t));

	if (!player
		|| !player->alive()
		|| player->index() < 1
		|| player->index() > 64
		|| player != g_cl.m_local)
		return orig_setup_bones(ecx, edx, bones, max_bones, mask, time);

	if (g_hooks.m_updating_bones[player->index()])
		return orig_setup_bones(ecx, edx, bones, max_bones, mask, time);

	else
	{
		if (bones)
			std::memcpy(bones, player->m_BoneCache().m_pCachedBones, player->m_BoneCache().m_CachedBoneCount * sizeof(matrix3x4_t));
		
		return true;
	}
}

void __fastcall min_hooks::update_client_side_anim(Player* const player, const std::uintptr_t edx)
{
	if (!player
		|| !player->alive()
		|| player->index() < 1
		|| player->index() > 64
		|| !g_cl.m_local)
		return orig_update_client_side_anim(player, edx);

	if (player->m_iTeamNum() == g_cl.m_local->m_iTeamNum() && player != g_cl.m_local)
		return orig_update_client_side_anim(player, edx);

	if (g_hooks.m_updating_anims[player->index()])
		return orig_update_client_side_anim(player, edx);

	return;
}

void __fastcall min_hooks::GetColorModulation(IMaterial* ecx, void* edx, float* r, float* g, float* b)
{
	OrigGetColorModulation(ecx, edx, r, g, b);

	if (ecx->is_error_material())
		return;

	static std::string smoke = XOR("smoke");
	const char* mat_name = ecx->get_name();

	// don't change color on active smoke
	// WHY SMOKE IS STATIC PROP??? VALVE???
	if (std::strstr(mat_name, smoke.c_str()))
		return;

	const char* name = ecx->get_texture_group_name();

	static std::string world_tex = XOR("World");
	static std::string sky_tex = XOR("SkyBox");
	static std::string prop_tex = XOR("StaticProp");

}

void __fastcall min_hooks::GetExposureRange(float* pflAutoExposureMin, float* pflAutoExposureMax)
{
	*pflAutoExposureMin = 1.f;
	*pflAutoExposureMax = 1.f;

	OrigGetExposureRange(pflAutoExposureMin, pflAutoExposureMax);
}

bool __stdcall min_hooks::IsUsingStaticPropDebugModes()
{
	return g_menu.main.visuals.world.get();
}

void __fastcall min_hooks::CL_DispatchSound(SoundInfo_t* info)
{
	if (info->vOrigin.is_zero())
		return OrigCLDispatchSound(info);

	if (info->entityIndex < 1 || info->entityIndex > g_csgo.m_globals->m_max_clients || info->entityIndex == g_csgo.m_engine->GetLocalPlayer())
		return OrigCLDispatchSound(info);

	Player* plr = g_csgo.m_entlist->GetClientEntity<Player*>(info->entityIndex);
	if (!plr || !plr->enemy(g_cl.m_local))
		return OrigCLDispatchSound(info);

	if (plr->dormant())
	{
		vec3_t pos = info->vOrigin;

		plr->m_fFlags() = info->nFlags;
		plr->m_vecOrigin() = pos;
		plr->SetAbsOrigin(pos);
	}

	return OrigCLDispatchSound(info);
}

ang_t* __fastcall min_hooks::eye_angles(Player* ecx, void* edx)
{
	Stack stack;

	if (ecx != g_cl.m_local)
		return orig_eye_angles(ecx, edx);

	static auto ret_to_thirdperson_pitch = pattern::find(g_csgo.m_client_dll, "8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55 ?");
	static auto ret_to_thirdperson_yaw = pattern::find(g_csgo.m_client_dll, "F3 0F 10 55 ? 51 8B 8E ? ? ? ?");

	/// Are we even in thirdperson ?
	if (!g_visuals.m_thirdperson)
		return orig_eye_angles(ecx, edx);

	/// Is the call coming from somewhere relevant?
	if (stack.ReturnAddress() == ret_to_thirdperson_pitch
		|| stack.ReturnAddress() == ret_to_thirdperson_yaw)
		return &g_cl.m_interp_angle;

	return orig_eye_angles(ecx, edx);
}

int __fastcall min_hooks::list_leaves_in_box(const std::uintptr_t ecx, const std::uintptr_t edx, const vec3_t& mins, const vec3_t& maxs, const uint16_t* const list, const int max)
{
	if (!g_cl.m_local)
		return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

	if (*(uint32_t*)_ReturnAddress() != 0x8B087D8B)
		return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

	struct renderable_info_t {
		IClientRenderable* m_renderable{ };
		std::uintptr_t	m_alpha_property{ };
		int				m_enum_count{ };
		int				m_render_frame{ };
		std::uint16_t	m_first_shadow{ };
		std::uint16_t	m_leaf_list{ };
		short			m_area{ };
		std::uint16_t	m_flags0{ };
		std::uint16_t	m_flags1{ };
		vec3_t			m_bloated_abs_min{ };
		vec3_t			m_bloated_abs_max{ };
		vec3_t			m_abs_min{ };
		vec3_t			m_abs_max{ };
		char			pad0[4u]{ };
	};

	const auto info = *reinterpret_cast<renderable_info_t**> (
		reinterpret_cast<std::uintptr_t> (_AddressOfReturnAddress()) + 0x14u
		);
	if (!info
		|| !info->m_renderable)
		return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

	const auto entity = info->m_renderable->get_client_unknown()->GetBaseEntity();
	if (!entity
		|| !entity->IsPlayer())
		return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

	info->m_flags0 &= ~0x100;
	info->m_flags1 |= 0xC0;

	return orig_list_leaves_in_box(
		ecx, edx,
		{ -16384.f, -16384.f, -16384.f },
		{ 16384.f, 16384.f, 16384.f },
		list, max
	);
}

constexpr uint32_t hash_runtime_for_main(const char* str) noexcept
{
	auto value = 0x811c9dc5;

	while (*str)
	{
		value ^= *str++;
		value *= 0x1000193;
	}

	return value;
}

void __fastcall min_hooks::init_new_particles_scalar(C_INIT_RandomColor* ecx, void* edx, CParticleCollection* particles, int start_position, int particles_count, int attribute_write_mask, void* ctx)
{
	if(!g_menu.main.visuals.custom_molotov.get())
		return orig_init_new_particles_scalar(ecx, edx, particles, start_position, particles_count, attribute_write_mask, ctx);

	vec3_t o_min = ecx->m_normal_color_min;
	vec3_t o_max = ecx->m_normal_color_max;

	const char* mat_name = *(char**)(*(uintptr_t*)((uintptr_t)particles + 0x48) + 0x40);
	assert(mat_name);

	auto nigga = g_menu.main.visuals.molotov_color.get();
	auto niggab = g_menu.main.visuals.smoke_color.get();

	switch (hash_runtime_for_main(mat_name))
	{
	case HASH("particle\\fire_burning_character\\fire_env_fire.vmt"):
	case HASH("particle\\fire_burning_character\\fire_env_fire_depthblend.vmt"):
	case HASH("particle\\fire_burning_character\\fire_burning_character_depthblend.vmt"):
	case HASH("particle\\fire_burning_character\\fire_burning_character.vmt"):
	case HASH("particle\\fire_burning_character\\fire_burning_character_nodepth.vmt"):
	case HASH("particle\\particle_flares\\particle_flare_001.vmt"):
	case HASH("particle\\particle_flares\\particle_flare_004.vmt"):
	case HASH("particle\\particle_flares\\particle_flare_004b_mod_ob.vmt"):
	case HASH("particle\\particle_flares\\particle_flare_004b_mod_z.vmt"):
	case HASH("particle\\fire_explosion_1\\fire_explosion_1_bright.vmt"):
	case HASH("particle\\fire_explosion_1\\fire_explosion_1b.vmt"):
	case HASH("particle\\fire_particle_4\\fire_particle_4.vmt"):
	case HASH("particle\\fire_explosion_1\\fire_explosion_1_oriented.vmt"):
		ecx->m_normal_color_min = ecx->m_normal_color_max = vec3_t(nigga.r() / 255.f, nigga.g() / 255.f, nigga.b() / 255.f);
		break;
	}

	orig_init_new_particles_scalar(ecx, edx, particles, start_position, particles_count, attribute_write_mask, ctx);

	ecx->m_normal_color_min = o_min;
	ecx->m_normal_color_max = o_max;
}

void __fastcall min_hooks::CParticleCollection_Simulate(CParticleCollection* thisPtr, void* edx)
{
	//call the original particle simulate
	OrigCParticleCollection_Simulate(thisPtr, edx);

	for (auto i = 0; i < thisPtr->m_nActiveParticles; i++)
	{
		//*thisPtr->m_ParticleAttributes.FloatAttributePtr(PARTICLE_ATTRIBUTE_GLOW_ALPHA, i) = 0.f;

		// is translucent?
		//auto alpha = thisPtr->m_ParticleAttributes.FloatAttributePtr(PARTICLE_ATTRIBUTE_ALPHA, i);
		//*alpha = std::clamp(*alpha, 0.f, 0.02f);
	}

	/*if (g_menu.main.visuals.custom_molotov.get())
	{
		CParticleCollection* root_colection = thisPtr;
		while (root_colection->m_pParent)
			root_colection = root_colection->m_pParent;

		const char* root_name = root_colection->m_pDef.m_pObject->m_Name.buffer;
		switch (hash_runtime_for_main(root_name))
		{
		case HASH("molotov_groundfire"):
		case HASH("molotov_groundfire_00MEDIUM"):
		case HASH("molotov_groundfire_00HIGH"):
		case HASH("molotov_groundfire_fallback"):
		case HASH("molotov_groundfire_fallback2"):
		case HASH("molotov_explosion"):
		case HASH("explosion_molotov_air"):
		case HASH("extinguish_fire"):
		case HASH("weapon_molotov_held"):
		case HASH("weapon_molotov_fp"):
		case HASH("weapon_molotov_thrown"):
		case HASH("incgrenade_thrown_trail"):
			switch (hash_runtime_for_main(thisPtr->m_pDef.m_pObject->m_Name.buffer))
			{
			case HASH("explosion_molotov_air_smoke"):
			case HASH("molotov_smoking_ground_child01"):
			case HASH("molotov_smoking_ground_child02"):
			case HASH("molotov_smoking_ground_child02_cheapo"):
			case HASH("molotov_smoking_ground_child03"):
			case HASH("molotov_smoking_ground_child03_cheapo"):
			case HASH("molotov_smoke_screen"):
				for (int i = 0; i < thisPtr->m_nActiveParticles; i++) {
					float* pColor = thisPtr->m_ParticleAttributes.FloatAttributePtr(PARTICLE_ATTRIBUTE_ALPHA, i);
					*pColor = 0.f;
				}
				break;
			default:
				for (int i = 0; i < thisPtr->m_nActiveParticles; i++) {
					float* pColor = thisPtr->m_ParticleAttributes.FloatAttributePtr(PARTICLE_ATTRIBUTE_TINT_RGB, i);
					pColor[0] = g_menu.main.visuals.molotov_color.get().r();
					pColor[4] = g_menu.main.visuals.molotov_color.get().g();
					pColor[8] = g_menu.main.visuals.molotov_color.get().b();
				}
				break;
			}
			break;
		}
	}*/
}

void __fastcall min_hooks::render_glow_boxes(CGlowObjectManager* ecx, void* edx, int iPass, void* pRenderContext)
{
	// make it so the glow boxes r actually glowing
	orig_render_glow_boxes(ecx, edx, 1, pRenderContext);
}
