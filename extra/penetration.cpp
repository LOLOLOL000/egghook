#include "includes.h"

bool CGameTrace::DidHitWorld() {
	if (!m_entity)
		return false;

	return m_entity == g_csgo.m_entlist->GetClientEntity(0);
}


bool CGameTrace::DidHitNonWorldEntity() {
	return m_entity != nullptr && !DidHitWorld();
}

bool IsArmored(Player* player, int hitgroup)
{
	switch (hitgroup)
	{
	case Hitgroups_t::HITGROUP_HEAD:
		return player->m_bHasHelmet();
	case Hitgroups_t::HITGROUP_GENERIC:
	case Hitgroups_t::HITGROUP_CHEST:
	case Hitgroups_t::HITGROUP_STOMACH:
	case Hitgroups_t::HITGROUP_LEFTARM:
	case Hitgroups_t::HITGROUP_RIGHTARM:
		return true;
	default:
		return false;
	}
}

float penetration::scale(Player* player, float& fl_damage, float fl_armor_ratio, int n_hitgroup) {
	if (!player || !player->GetActiveWeapon())
		return -1.f;

	bool hasHeavyArmor = player->m_bHasHeavyArmor();
	int armorValue = player->m_ArmorValue();

	switch (n_hitgroup)
	{
	case Hitgroups_t::HITGROUP_HEAD:
		fl_damage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
		break;
	case Hitgroups_t::HITGROUP_STOMACH:
		fl_damage *= 1.25f;
		break;
	case Hitgroups_t::HITGROUP_LEFTLEG:
	case Hitgroups_t::HITGROUP_RIGHTLEG:
		fl_damage *= 0.75f;
		break;
	default:
		break;
	}

	if (armorValue > 0 && IsArmored(player, n_hitgroup))
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = fl_armor_ratio / 2.f;

		//Damage gets modified for heavy armor users
		if (hasHeavyArmor)
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = fl_damage * armorRatio;

		if (hasHeavyArmor)
			NewDamage *= 0.85f;

		if (((fl_damage - (fl_damage * armorRatio)) * (bonusValue * armorBonusRatio)) > armorValue)
			NewDamage = fl_damage - (armorValue / armorBonusRatio);

		fl_damage = NewDamage;
	}

	return fl_damage;
}

bool penetration::TraceToExit( const vec3_t &start, const vec3_t& dir, vec3_t& out, CGameTrace* enter_trace, CGameTrace* exit_trace ) {
    static CTraceFilterSimple_game filter{};

	float  dist{};
	vec3_t new_end;
	int    contents, first_contents{};

	// max pen distance is 90 units.
	while( dist <= 90.f ) {
		// step forward a bit.
		dist += 4.f;

		// set out pos.
		out = start + ( dir * dist );

		if( !first_contents )
			first_contents = g_csgo.m_engine_trace->GetPointContents( out, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );

		contents = g_csgo.m_engine_trace->GetPointContents( out, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );

		if( ( contents & MASK_SHOT_HULL ) && ( !( contents & CONTENTS_HITBOX ) || ( contents == first_contents ) ) )
			continue;

		// move end pos a bit for tracing.
		new_end = out - ( dir * 4.f );

		// do first trace.
		g_csgo.m_engine_trace->TraceRay( Ray( out, new_end ), MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, exit_trace );

        // we hit an ent's hitbox, do another trace.
        if( exit_trace->m_startsolid && ( exit_trace->m_surface.m_flags & SURF_HITBOX ) ) {
			filter.SetPassEntity( exit_trace->m_entity );
        
			g_csgo.m_engine_trace->TraceRay( Ray( out, start ), MASK_SHOT_HULL, (ITraceFilter *)&filter, exit_trace );
        
			if( exit_trace->hit( ) && !exit_trace->m_startsolid ) {
                out = exit_trace->m_endpos;
                return true;
            }

            continue;
		}

        if( exit_trace->hit( ) || !exit_trace->m_startsolid ) {

			//Is the wall a breakable? If so, let's shoot through it.
			if (game::IsBreakable(exit_trace->m_entity) && game::IsBreakable(enter_trace->m_entity))
				return true;

			if (enter_trace->m_surface.m_flags & SURF_NODRAW || !(exit_trace->m_surface.m_flags & SURF_NODRAW) && (exit_trace->m_plane.m_normal.dot(dir) <= 1.f))
			{
				float multAmount = exit_trace->m_fraction * 4.f;
				out -= dir * multAmount;
				return true;
			}

            continue;
        }

		if (!exit_trace->hit() || exit_trace->m_startsolid)
		{
			if (enter_trace->DidHitNonWorldEntity() && game::IsBreakable(enter_trace->m_entity))
			{
				*exit_trace = *enter_trace;
				exit_trace->m_endpos = start + dir;
				return true;
			}

			continue;
		}
	}

	return false;
}

void penetration::ClipTraceToPlayer( const vec3_t& start, const vec3_t& end, uint32_t mask, CGameTrace* tr, Player* player, float min ) {
	vec3_t     pos, to, dir, on_ray;
	float      len, range_along, range;
	Ray        ray;
	CGameTrace new_trace;

	// reference: https://github.com/alliedmodders/hl2sdk/blob/3957adff10fe20d38a62fa8c018340bf2618742b/game/shared/util_shared.h#L381

	// set some local vars.
	pos         = player->m_vecOrigin( ) + ( ( player->m_vecMins( ) + player->m_vecMaxs( ) ) * 0.5f );
	to          = pos - start;
	dir         = start - end;
	len         = dir.normalize( );
	range_along = dir.dot( to );

	// off start point.
	if( range_along < 0.f )	
		range = -( to ).length( );

	// off end point.
	else if( range_along > len ) 
		range = -( pos - end ).length( );

	// within ray bounds.
	else {
		on_ray = start + ( dir * range_along );
		range  = ( pos - on_ray ).length( );
	}

	if( /*min <= range &&*/ range <= 60.f ) {
		// clip to player.
		g_csgo.m_engine_trace->ClipRayToEntity( Ray( start, end ), mask, player, &new_trace );

		if( tr->m_fraction > new_trace.m_fraction )
			*tr = new_trace;
	}
}

bool penetration::bTraceMeantForHitbox(const vec3_t& vecEyePosition, const vec3_t& vecEnd, int iHitbox, Player* player)
{
	// Initialize our trace data & information
	CGameTrace traceData = CGameTrace();
	Ray traceRay = Ray(vecEyePosition, vecEnd);

	// trace a ray to the entity
	g_csgo.m_engine_trace->ClipRayToCollideable(traceRay, CS_MASK_SHOT, player->GetCollideable(), &traceData);

	// check if trace did hit the desired hitbox and not other
	// example: aiming for head -> its behind his chest -> return chest == head
	return traceData.m_hitbox == iHitbox;
}

bool penetration::run( PenetrationInput_t* in, PenetrationOutput_t* out ) {
    static CTraceFilterSkipTwoEntities_game filter{};

	int			  pen{ 4 }, enter_material, exit_material;
	float		  damage, penetration, penetration_mod, player_damage, remaining, trace_len{}, total_pen_mod, damage_mod, modifier, damage_lost;
	surfacedata_t *enter_surface, *exit_surface;
	bool		  nodraw, grate;
	vec3_t		  start, dir, end, pen_end;
	CGameTrace	  trace, exit_trace;
	Weapon		  *weapon;
	WeaponInfo    *weapon_info;

	// if we are tracing from our local player perspective.
	if( in->m_from->m_bIsLocalPlayer( ) ) {
		weapon      = g_cl.m_weapon;
		weapon_info = g_cl.m_weapon_info;
		start       = g_cl.m_shoot_pos;
	}

	// not local player.
	else {
		weapon = in->m_from->GetActiveWeapon( );
		if( !weapon )
			return false;

		// get weapon info.
		weapon_info = weapon->GetWpnData( );
		if( !weapon_info )
			return false;

		// set trace start.
		start = in->m_from->GetShootPosition( );
	}

	// get some weapon data.
	damage      = ( float )weapon_info->m_damage;
	penetration = weapon_info->m_penetration;

    // used later in calculations.
    penetration_mod = std::max( 0.f, ( 3.f / penetration ) * 1.25f );

	// get direction to end point.
	dir = ( in->m_pos - start ).normalized( );

    // setup trace filter for later.
    filter.SetPassEntity( in->m_from );
    filter.SetPassEntity2( nullptr );

    while( damage > 0.f ) {
		// calculating remaining len.
		remaining = weapon_info->m_range - trace_len;

		// set trace end.
		end = start + ( dir * remaining );

		// setup ray and trace.
		// TODO; use UTIL_TraceLineIgnoreTwoEntities?
		g_csgo.m_engine_trace->TraceRay( Ray( start, end ), MASK_SHOT_HULL | CONTENTS_HITBOX, (ITraceFilter *)&filter, &trace );

		// check for player hitboxes extending outside their collision bounds.
		// if no target is passed we clip the trace to a specific player, otherwise we clip the trace to any player.
		if( in->m_target )
			ClipTraceToPlayer( start, end + ( dir * 40.f ), MASK_SHOT_HULL | CONTENTS_HITBOX, &trace, in->m_target, 60.f );

		else
			game::UTIL_ClipTraceToPlayers( start, end + ( dir * 40.f ), MASK_SHOT_HULL | CONTENTS_HITBOX, (ITraceFilter *)&filter, &trace, 60.f );

		// we didn't hit anything.
		if (trace.m_fraction == 1.f)
			return false;
		
		// calculate damage based on the distance the bullet traveled.
		trace_len += trace.m_fraction * remaining;
		damage    *= std::pow( weapon_info->m_range_modifier, trace_len / 500.f );

		// if a target was passed.
		if( in->m_target ) {

			// validate that we hit the target we aimed for.
			if( trace.m_entity && trace.m_entity == in->m_target && game::IsValidHitgroup( trace.m_hitgroup ) ) {
				int group = ( weapon->m_iItemDefinitionIndex( ) == ZEUS ) ? HITGROUP_GENERIC : trace.m_hitgroup;

				// scale damage based on the hitgroup we hit.
				player_damage = scale( in->m_target, damage, weapon_info->m_armor_ratio, group );

				// set result data for when we hit a player.
			    out->m_pen      = pen != 4;
			    out->m_hitgroup = group;
				out->m_hitbox = trace.m_hitbox;
			    out->m_damage   = player_damage;
			    out->m_target   = in->m_target;

				// non-penetrate damage.
				if( pen == 4 )
					return player_damage >= in->m_damage || player_damage >= out->m_target->m_iHealth();
					
				// penetration damage.
				return player_damage >= in->m_damage_pen || player_damage >= out->m_target->m_iHealth();
			}
		}

		// no target was passed, check for any player hit or just get final damage done.
		else {
			out->m_pen = pen != 4;

			// todo - dex; team checks / other checks / etc.
			if( trace.m_entity && trace.m_entity->IsPlayer( ) && game::IsValidHitgroup( trace.m_hitgroup ) ) {
				int group = ( weapon->m_iItemDefinitionIndex( ) == ZEUS ) ? HITGROUP_GENERIC : trace.m_hitgroup;

				player_damage = scale( trace.m_entity->as< Player* >( ), damage, weapon_info->m_armor_ratio, group );

				// set result data for when we hit a player.
				out->m_hitgroup = group;
				out->m_damage   = player_damage;
				out->m_hitbox = trace.m_hitbox;
				out->m_target   = trace.m_entity->as< Player* >( );

				// non-penetrate damage.
				if( pen == 4 )
					return player_damage >= in->m_damage || player_damage >= out->m_target->m_iHealth();

				// penetration damage.
				return player_damage >= in->m_damage_pen || player_damage >= out->m_target->m_iHealth();
			}

            // if we've reached here then we didn't hit a player yet, set damage and hitgroup.
            out->m_damage = damage;
		}

		// don't run pen code if it's not wanted.
		if( !in->m_can_pen )
			return false;

		// get surface at entry point.
		enter_surface = g_csgo.m_phys_props->GetSurfaceData( trace.m_surface.m_surface_props );

		// this happens when we're too far away from a surface and can penetrate walls or the surface's pen modifier is too low.
		if( ( trace_len > 3000.f && penetration ) || enter_surface->m_game.m_penetration_modifier < 0.1f )
			return false;

		// store data about surface flags / contents.
		nodraw = ( trace.m_surface.m_flags & SURF_NODRAW );
		grate  = ( trace.m_contents & CONTENTS_GRATE );

		// get material at entry point.
		enter_material = enter_surface->m_game.m_material;

		// note - dex; some extra stuff the game does.
		if( !pen && !nodraw && !grate && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS )
			return false;

		// no more pen.
		if( penetration <= 0.f || pen <= 0 )
			return false;

		// try to penetrate object.
		if( !TraceToExit( trace.m_endpos, dir, pen_end, &trace, &exit_trace ) ) {
			if( !( g_csgo.m_engine_trace->GetPointContents( pen_end, MASK_SHOT_HULL ) & MASK_SHOT_HULL ) )
				return false;
		}

		// get surface / material at exit point.
		exit_surface  = g_csgo.m_phys_props->GetSurfaceData( exit_trace.m_surface.m_surface_props );
        exit_material = exit_surface->m_game.m_material;

        // todo - dex; check for CHAR_TEX_FLESH and ff_damage_bullet_penetration / ff_damage_reduction_bullets convars?
        //             also need to check !isbasecombatweapon too.
		if( enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS ) {
			total_pen_mod = 3.f;
			damage_mod    = 0.05f;
		}

		else if( nodraw || grate ) {
			total_pen_mod = 1.f;
			damage_mod    = 0.16f;
		}
		else {
			total_pen_mod = ( enter_surface->m_game.m_penetration_modifier + exit_surface->m_game.m_penetration_modifier ) / 2.f;
			damage_mod    = 0.16f;
		}

		// thin metals, wood and plastic get a penetration bonus.
		if( enter_material == exit_material ) {
			if( exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD )
				total_pen_mod = 3.f;

			else if( exit_material == CHAR_TEX_PLASTIC )
				total_pen_mod = 2.f;
		}

		//Calculate thickness of the wall by getting the length of the range of the trace and squaring
		trace_len = (exit_trace.m_endpos - trace.m_endpos).length_sqr();
		modifier = fmaxf(1.f / total_pen_mod, 0.f);

		//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		damage_lost = fmaxf(
			((modifier * trace_len) / 24.f)
			+ ((damage * damage_mod)
				+ (fmaxf(3.75f / weapon_info->m_penetration, 0.f) * 3.f * modifier)), 0.f);

		//Did we loose too much damage?
		if (damage_lost > damage)
			return false;

		//We can't use any of the damage that we've lost
		if (damage_lost > 0.f)
			damage -= damage_lost;

		//Do we still have enough damage to deal?
		if (damage < 1.f)
			return false;

		// set new start pos for successive trace.
		start = exit_trace.m_endpos;

		// decrement pen.
		--pen;
	}

	return false;
}