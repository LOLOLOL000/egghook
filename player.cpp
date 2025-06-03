#include "includes.h"

void Hooks::DoExtraBoneProcessing( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	return;
}

void Hooks::StandardBlendingRules(int a2, int a3, int a4, int a5, int a6)
{
    // cast thisptr to player ptr.
    Player* player = (Player*)this;

    if (!player
        || !player->IsPlayer()
        || !player->alive()
        || player->m_bIsLocalPlayer())
        return g_hooks.m_StandardBlendingRules(this, a2, a3, a4, a5, a6);

    // fix arms.
    a6 = BONE_USED_BY_SERVER;

    player->m_fEffects() |= EF_NOINTERP;

    // call og.
    g_hooks.m_StandardBlendingRules(player, a2, a3, a4, a5, a6);

    player->m_fEffects() &= ~EF_NOINTERP;
}

void Hooks::CalcView(vec3_t& eye_origin, vec3_t& eye_angles, float& z_near, float& z_far, float& fov)
{
    // cast thisptr to player ptr.
    Player* player = (Player*)this;

    const auto old_use_new_animation_state = player->get<bool>(0x39E1);

    // prevent calls to ModifyEyePosition
    player->set<int>(0x39E1, false);

    g_hooks.m_1CalcView(this, eye_origin, eye_angles, z_near, z_far, fov);

    player->set<int>(0x39E1, old_use_new_animation_state);
}


Weapon *Hooks::GetActiveWeapon( ) {
    Stack stack;

    static Address ret_1 = pattern::find( g_csgo.m_client_dll, XOR( "85 C0 74 1D 8B 88 ? ? ? ? 85 C9" ) );

    // note - dex; stop call to CIronSightController::RenderScopeEffect inside CViewRender::RenderView.
    if( g_menu.main.visuals.noscope.get( ) ) {
        if( stack.ReturnAddress( ) == ret_1 )
            return nullptr;
    }

    return g_hooks.m_GetActiveWeapon( this );
}

const std::vector<listened_entity_t>& CustomEntityListener::get_entity(int type)
{
    return this->ent_lists[type];
}

bool is_bomb(int class_id)
{
    if (class_id == CC4 || class_id == CPlantedC4)
        return true;

    return false;
}


bool is_grenade(int class_id)
{
    switch (class_id)
    {
    case(int)CBaseCSGrenade:
    case(int)CBaseCSGrenadeProjectile:
    case(int)CDecoyGrenade:
    case(int)CDecoyProjectile:
    case(int)CMolotovGrenade:
    case(int)CMolotovProjectile:
    case(int)CSensorGrenade:
    case(int)CSensorGrenadeProjectile:
    case(int)CSmokeGrenade:
    case(int)CSmokeGrenadeProjectile:
    case(int)CIncendiaryGrenade:
    case(int)CInferno:
        return true;
        break;
    }
    return false;
}

bool is_weapon(Entity* entity, int class_id)
{
    if (is_grenade(class_id))
        return true;

    if (is_bomb(class_id))
        return true;

    if (entity->IsBaseCombatWeapon())
        return true;

    return false;
}

bool should_delete_entity(int index, std::vector< listened_entity_t >& info)
{
    const auto it = std::find_if(info.begin(), info.end(), [&](const listened_entity_t& data) { return data.m_idx == index; });

    if (it == info.end())
        return false;

    info.erase(it);
    return true;
}

void CustomEntityListener::OnEntityCreated( Entity *ent ) {
    if( ent ) {

        int index = ent->index();
        ClientClass* client_class = ent->GetClientClass();
        int class_id = client_class->m_ClassID;

        auto get_entity_type = [&]()
            {
                if (class_id == CPlantedC4)
                    return ent_c4;

                if (is_weapon(ent, class_id))
                    return ent_weapon;

                if (ent->IsPlayer())
                    return ent_player;

                if (class_id == CFogController)
                    return ent_fog;

                if (class_id == CEnvTonemapController)
                    return ent_tonemap;

                if (class_id == CSprite)
                    return ent_light;

                if (class_id == CCSRagdoll)
                    return ent_ragdoll;

                return ent_invalid;
            };

        // player created.
        if( ent->IsPlayer( ) ) {
		    Player* player = ent->as< Player* >( );

		    // access out player data stucture and reset player data.
		    AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];
            if( data )
		        data->reset( );

		    // get ptr to vmt instance and reset tables.
		    VMT* vmt = &g_hooks.m_player[ player->index( ) - 1 ];
            if( vmt ) {
                // init vtable with new ptr.
		        vmt->reset( );
		        vmt->init( player );

		        // hook this on every player.
		        g_hooks.m_DoExtraBoneProcessing = vmt->add< Hooks::DoExtraBoneProcessing_t >( Player::DOEXTRABONEPROCESSING, util::force_cast( &Hooks::DoExtraBoneProcessing ) );
                g_hooks.m_StandardBlendingRules = vmt->add< Hooks::StandardBlendingRules_t >(Player::STANDARDBLENDINGRULES, util::force_cast(&Hooks::StandardBlendingRules));
              
		        // local gets special treatment.
		        if( player->index( ) == g_csgo.m_engine->GetLocalPlayer( ) ) {
                    g_hooks.m_1CalcView = vmt->add< Hooks::CalcView_t >(270, util::force_cast(&Hooks::CalcView));
                    g_hooks.m_GetActiveWeapon           = vmt->add< Hooks::GetActiveWeapon_t >( Player::GETACTIVEWEAPON, util::force_cast( &Hooks::GetActiveWeapon ) );
                }
            }
        }

        int type = get_entity_type();
        if (type == ent_invalid)
            return;

        this->ent_lists[type].emplace_back(ent);
	}
}

void CustomEntityListener::OnEntityDeleted( Entity *ent ) {
    // note; IsPlayer doesn't work here because the ent class is CBaseEntity.
	if( ent && ent->index( ) >= 1 && ent->index( ) <= 64 ) {
		Player* player = ent->as< Player* >( );

		// access out player data stucture and reset player data.
		AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];
        if( data )
		    data->reset( );

		// get ptr to vmt instance and reset tables.
		VMT* vmt = &g_hooks.m_player[ player->index( ) - 1 ];
		if( vmt )
		    vmt->reset( );
	}

    if (!ent)
        return;

    int index = ent->index();
    if (index < 0)
        return;

    for (int i = ent_fog; i < ent_max; i++)
        should_delete_entity(index, this->ent_lists[i]);
}