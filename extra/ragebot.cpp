#include "ragebot.h"
#include "autowall.h"
#include "threading/threading.h"
#include "threading/shared_mutex.h"
/*
static SharedMutex smtx;

c_rage_bot g_rage_bot;

std::vector<int> c_rage_bot::get_hitboxes()
{
    std::vector< int > hitboxes{ };

    if (g_cl.m_weapon->is_taser())
    {
        hitboxes.emplace_back(HITBOX_BODY);
        hitboxes.emplace_back(HITBOX_PELVIS);
        return hitboxes;
    }

	std::vector< size_t > hitbox{ g_menu.main.aimbot.hitbox.GetActiveIndices() };
	if (hitbox.empty())
		return hitboxes;

	for (const auto& h : hitbox) {
		// head.
		if (h == 0)
			hitboxes.emplace_back(HITBOX_HEAD);

		// chest.
		if (h == 1) {
			hitboxes.emplace_back(HITBOX_THORAX);
			hitboxes.emplace_back(HITBOX_CHEST);
			hitboxes.emplace_back(HITBOX_UPPER_CHEST);
		}

		// stomach.
		if (h == 2) {
			hitboxes.emplace_back(HITBOX_PELVIS);
			hitboxes.emplace_back(HITBOX_BODY);
		}

		// arms.
		if (h == 3) {
			hitboxes.emplace_back(HITBOX_L_UPPER_ARM);
			hitboxes.emplace_back(HITBOX_R_UPPER_ARM);
		}

		// legs.
		if (h == 4) {
			hitboxes.emplace_back(HITBOX_L_THIGH);
			hitboxes.emplace_back(HITBOX_R_THIGH);
			hitboxes.emplace_back(HITBOX_L_CALF);
			hitboxes.emplace_back(HITBOX_R_CALF);
			hitboxes.emplace_back(HITBOX_L_FOOT);
			hitboxes.emplace_back(HITBOX_R_FOOT);
		}
	}

    return hitboxes;
}

std::vector<std::pair<vec3_t, bool>> c_rage_bot::get_multipoints(Player* player, int hitbox, matrix3x4_t* matrix)
{
	std::vector< std::pair< vec3_t, bool > > points = { };

	auto model = player->GetModel();
	if (!model)
		return points;

	auto hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return points;

	auto set = hdr->GetHitboxSet(0);
	if (!set)
		return points;

	auto bbox = set->GetHitbox(hitbox);
	if (!bbox)
		return points;

	if (bbox->m_radius <= 0.f)
	{
		matrix3x4_t rot_matrix = { };
		g_csgo.AngleMatrix(bbox->m_angle, rot_matrix);

		matrix3x4_t mat = { };
		math::ConcatTransforms(matrix[bbox->m_bone], rot_matrix, mat);

		vec3_t origin = mat.GetOrigin();

		vec3_t center = (bbox->m_mins + bbox->m_maxs) * 0.5f;

		if (hitbox == HITBOX_L_FOOT || hitbox == HITBOX_R_FOOT)
			points.emplace_back(center, true);

		if (points.empty())
			return points;

		for (auto& p : points)
		{
			p.first = { p.first.dot(mat[0]), p.first.dot(mat[1]), p.first.dot(mat[2]) };
			p.first += origin;
		}
	}
	else
	{
		vec3_t max = bbox->m_maxs;
		vec3_t min = bbox->m_mins;
		vec3_t center = (bbox->m_mins + bbox->m_maxs) * 0.5f;

		float head_slider = g_menu.main.aimbot.scale.get() / 100.f;
		float body_slider = g_menu.main.aimbot.body_scale.get() / 100.f;

		float head_scale = bbox->m_radius * head_slider;
		float body_scale = bbox->m_radius * body_slider;

		constexpr float rotation = 0.70710678f;
		float near_center_scale = bbox->m_radius * (head_slider / 2.f);

		if (hitbox == HITBOX_HEAD)
		{
			points.emplace_back(center, true);

			vec3_t point{ };
			point = { max.x + 0.70710678f * head_scale, max.y - 0.70710678f * head_scale, max.z };
			points.emplace_back(point, false);

			point = { max.x, max.y, max.z + head_scale };
			points.emplace_back(point, false);

			point = { max.x, max.y, max.z - head_scale };
			points.emplace_back(point, false);

			point = { max.x, max.y - head_scale, max.z };
			points.emplace_back(point, false);
		}
		else
		{
			if (hitbox == HITBOX_BODY)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t(center.x, center.y, min.z + body_scale), false);
				points.emplace_back(vec3_t(center.x, center.y, max.z - body_scale), false);
				points.emplace_back(vec3_t{ center.x, max.y - body_scale, center.z }, false);
			}
			else if (hitbox == HITBOX_PELVIS || hitbox == HITBOX_UPPER_CHEST)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t(center.x, center.y, max.z + body_scale), false);
				points.emplace_back(vec3_t(center.x, center.y, min.z - body_scale), false);
			}
			else if (hitbox == HITBOX_THORAX || hitbox == HITBOX_CHEST)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t(center.x, center.y, max.z + body_scale), false);
				points.emplace_back(vec3_t(center.x, center.y, min.z - body_scale), false);

				points.emplace_back(vec3_t{ center.x, max.y - body_scale, center.z }, false);
			}
			else if (hitbox == HITBOX_R_CALF || hitbox == HITBOX_L_CALF)
			{
				points.emplace_back(center, true);
				points.emplace_back(vec3_t{ max.x - (bbox->m_radius / 2.f), max.y, max.z }, false);
			}
			else if (hitbox == HITBOX_R_THIGH || hitbox == HITBOX_L_THIGH)
			{
				points.emplace_back(center, true);
			}
			else if (hitbox == HITBOX_R_UPPER_ARM || hitbox == HITBOX_L_UPPER_ARM)
			{
				points.emplace_back(vec3_t{ max.x + bbox->m_radius, center.y, center.z }, false);
			}
			else
				points.emplace_back(center, true);
		}

		if (points.empty())
			return points;

		for (auto& p : points)
			math::VectorTransform(p.first, matrix[bbox->m_bone], p.first);
	}

	return points;
}

void c_rage_bot::store(Player* player)
{
	auto& backup = this->backup[player->index()];
	backup.duck = player->m_flDuckAmount();
	backup.lby = player->m_flLowerBodyYawTarget();
	backup.angles = player->m_angEyeAngles();
	backup.origin = player->m_vecOrigin();
	backup.absorigin = player->GetAbsOrigin();
	backup.bbmin = player->m_vecMins();
	backup.bbmax = player->m_vecMaxs();
	backup.velocity = player->m_vecVelocity();
	player->store_bone_cache(backup.bonecache);
	player->GetPoseParameters(backup.poses.data());

	backup.filled = true;
}

void c_rage_bot::set_record(Player* player, LagRecord* record, matrix3x4_t* matrix)
{
	player->InvalidateBoneCache();

	player->m_angEyeAngles() = record->m_eye_angles;

	player->m_vecMins() = record->m_mins;
	player->m_vecMaxs() = record->m_maxs;

	player->m_vecVelocity() = record->m_anim_velocity;

	player->SetPoseParameters(record->m_poses);

	player->m_vecOrigin() = record->m_origin;
	player->SetAbsOrigin(record->m_origin);

	player->set_bone_cache(matrix ? matrix : record->m_bones);
}

void c_rage_bot::restore(Player* player)
{
	auto& backup = this->backup[player->index()];
	if (!backup.filled)
		return;

	player->InvalidateBoneCache();

	player->m_angEyeAngles() = backup.angles;

	player->m_vecMins() = backup.bbmin;
	player->m_vecMaxs() = backup.bbmax;

	player->SetPoseParameters(backup.poses.data());

	player->m_vecVelocity() = backup.velocity;

	player->m_vecOrigin() = backup.origin;
	player->SetAbsOrigin(backup.origin);

	player->set_bone_cache(backup.bonecache);
}


int c_rage_bot::get_min_damage(Player* player)
{
	if (!player)
		return 0;

	int health = player->m_iHealth();

	int menu_damage = g_aimbot.m_damage_toggle ? g_menu.main.aimbot.override_dmg_value.get() : g_menu.main.aimbot.minimal_damage.get();

	// hp + 1 slider
	if (menu_damage >= 100)
		return health + (menu_damage - 100);

	return menu_damage;
}

bool c_rage_bot::should_stop(bool shoot_check)
{
	if (g_cl.m_weapon->IsKnife() || g_cl.m_weapon->is_taser())
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

bool c_rage_bot::auto_stop()
{
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

	return !g_menu.main.movement.force_accuracy.get();
}

void c_rage_bot::start_stop()
{
	if (!g_cl.m_weapon || !stopping)
		return;

	if (this->should_stop())
		force_accuracy = this->auto_stop();
}

void c_rage_bot::on_predict_start()
{
	auto& players = g_custom_entity_listener.get_entity(ent_player);
	if (players.empty())
		return;

	for (auto& player : players)
	{
		auto entity = (Player*)player.m_entity;
		if (!entity)
			continue;

		if (entity == g_cl.m_local || entity->m_iTeamNum() == g_cl.m_local->m_iTeamNum())
			continue;

		if (!entity->alive() || entity->dormant())
			continue;

		this->store(entity);
	}

	this->proceed_aimbot();

	for (auto& player : players)
	{
		auto entity = (Player*)player.m_entity;
		if (!entity)
			continue;

		if (entity == g_cl.m_local || entity->m_iTeamNum() == g_cl.m_local->m_iTeamNum())
			continue;

		if (!entity->alive() || entity->dormant())
			continue;

		this->restore(entity);
	}
}

std::vector< int > backtrack_hitboxes{
  HITBOX_HEAD,
  HITBOX_CHEST,
  HITBOX_PELVIS,
  HITBOX_BODY,
  HITBOX_R_FOOT,
  HITBOX_L_FOOT,
};


int get_record_damage(Player* player, LagRecord* record)
{
	int total_dmg = 0;

	g_rage_bot.store(player);
	record->cache();

	for (auto& hitbox : backtrack_hitboxes)
	{
		auto position = player->get_hitbox_position(hitbox, record->m_bones);

		auto awall = g_autowall.fire_bullet(g_cl.m_local, player, g_cl.m_weapon_info, g_cl.m_weapon->is_taser(), g_cl.m_shoot_pos, position);

		total_dmg += awall.dmg;
	}

	g_rage_bot.restore(player);

	return total_dmg;
}

LagRecord* get_best_record(Player* player)
{
	AimPlayer* data = &g_aimbot.m_players[player->index() - 1];
	if (!data || data->m_records.empty())
		return nullptr;

	auto last = data->m_records.front().get();
	auto old = g_resolver.FindLastRecord(data);

	int damage_last = -1;
	if (last)
		damage_last = get_record_damage(player, last);

	int damage_old = -1;
	if (old)
		damage_old = get_record_damage(player, old);

	if (damage_old > damage_last)
		return old;

	return last;
}

bool is_point_predictive(Player* player, point_t& point)
{
	if (!g_rage_bot.should_stop() || !g_menu.main.movement.early.get())
		return false;

	int dmg = g_rage_bot.get_min_damage(player);

	vec3_t eye_position = g_cl.m_local->m_vecOrigin() + g_cl.m_local->m_vecViewOffset();

	float speed = std::max< float >(g_cl.m_local->m_vecVelocity().length(true), 1.f);

	int max_stop_ticks = std::max< int >(((speed / g_movement.m_max_speed) * 7.f) - 1, 0);
	if (max_stop_ticks == 0)
		return false;

	vec3_t last_predicted_velocity = g_cl.m_local->m_vecVelocity();
	for (int i = 0; i < max_stop_ticks; ++i)
	{
		auto pred_velocity = g_cl.m_local->m_vecVelocity() * game::TICKS_TO_TIME(i + 1);

		vec3_t origin = eye_position + pred_velocity;
		int flags = g_cl.m_local->m_fFlags();

		g_movement.extrapolate(g_cl.m_local, origin, pred_velocity, flags, flags & FL_ONGROUND);

		last_predicted_velocity = pred_velocity;
	}

	auto predicted_eye_pos = eye_position + last_predicted_velocity;

	if (player->dormant())
	{
		vec3_t poses[3]{ player->GetAbsOrigin(), player->GetAbsOrigin() + player->m_vecViewOffset(), player->GetAbsOrigin() + vec3_t(0.f, 0.f, player->m_vecViewOffset().z / 2.f) };

		for (int i = 0; i < 3; ++i)
		{
			CTraceFilterSimple_game filter{ };
			filter.m_pass_ent1 = g_cl.m_local;

			CGameTrace out{ };
			g_csgo.m_engine_trace->TraceRay(Ray(predicted_eye_pos, poses[i]), MASK_SHOT_HULL | CONTENTS_HITBOX, (ITraceFilter*) & filter, &out);

			if (out.m_fraction >= 0.97f)
				return true;
			else
				continue;
		}
	}
	else
		return g_autowall.can_hit_point(player, point.position, predicted_eye_pos, dmg);

	return false;
}

void force_scope()
{
	bool able_to_zoom = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase()) >= g_cl.m_weapon->m_flNextSecondaryAttack();

	if (able_to_zoom && g_menu.main.aimbot.zoom.get() && g_cl.m_weapon->m_zoomLevel() < 1 && g_movement.on_ground() && g_cl.m_weapon->is_sniper())
		g_cl.m_cmd->m_buttons |= IN_ATTACK2;
}

void thread_build_points(aim_cache_t* aim_cache)
{
	smtx.rlock();
	smtx.runlock();

	aim_cache->points.clear();

	if (!aim_cache->player)
		return;

	auto best_record = get_best_record(aim_cache->player);
	if (!best_record)
		return;

	g_rage_bot.store(aim_cache->player);
	best_record->cache();

	for (auto& hitbox : g_rage_bot.get_hitboxes())
	{
		const auto& pts = g_rage_bot.get_multipoints(aim_cache->player, hitbox, best_record->m_bones);
		for (auto& p : pts)
		{
			auto awall = g_autowall.fire_bullet(g_cl.m_local, aim_cache->player, g_cl.m_weapon_info, g_cl.m_weapon->is_taser(), g_cl.m_shoot_pos, p.first);

			auto new_point = point_t(hitbox, p.second, awall.dmg, best_record, p.first);

			if (p.second)
				new_point.predictive = is_point_predictive(aim_cache->player, new_point);

			aim_cache->points.emplace_back(new_point);
		}
	}

	g_rage_bot.restore(aim_cache->player);
}

void thread_get_best_point(aim_cache_t* aim_cache)
{
	if (!aim_cache->player)
		return;

	int health = aim_cache->player->m_iHealth();
	int lethal_dmg = health;

	aim_cache->best_point.reset();

	if (aim_cache->points.empty())
		return;

	int dmg = g_rage_bot.get_min_damage(aim_cache->player);

	// prepare points in right order
	// because we need to prefer the best points to tap enemy
	std::sort(aim_cache->points.begin(), aim_cache->points.end(), [&](point_t& a, point_t& b) { return a.center > b.center; });
	std::sort(aim_cache->points.begin(), aim_cache->points.end(), [&](point_t& a, point_t& b) { return a.damage > b.damage; });

	auto select_prefered_point = [&]() -> point_t
		{
			point_t best{ };
			for (auto& point : aim_cache->points)
			{
				// force stop before peek
				if (point.predictive)
				{
					force_scope();

					if (g_rage_bot.should_stop())
					{
						g_rage_bot.auto_stop();
					}

					point.predictive = false;
				}

				if (point.damage < dmg)
					continue;

				if (point.body && point.center && point.damage >= lethal_dmg)
					return point;

				// choose by best dmg
				else
				{
					if (point.damage > best.damage)
						best = point;
				}
			}

			return best;
		};

	aim_cache->best_point = select_prefered_point();
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

constexpr int total_seeds = 255;
bool is_accurate(Player* player, point_t& point, float amount, float* out_chance)
{
	if (amount <= 0.f)
		return true;

	auto model = g_csgo.m_model_info->GetStudioModel(player->GetModel());
	if (!model)
		return false;

	auto set = model->GetHitboxSet(player->m_nHitboxSet());

	if (!set)
		return false;

	auto studio_box = set->GetHitbox(point.hitbox);
	if (!studio_box)
		return false;

	auto weapon = g_cl.m_weapon;
	if (!weapon)
		return false;

	auto weapon_info = weapon->GetWpnData();
	if (!weapon_info)
		return false;

	auto state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return false;

	auto range = weapon_info->m_range;

	vec3_t forward, right, up;
	vec3_t start = g_cl.m_shoot_pos;
	ang_t pos = math::angle_from_vectors(start, point.position);
	math::AngleVectors(pos, &forward, &right, &up);

	int hits = 0;
	for (int i = 0; i < total_seeds; ++i)
	{
		auto spread_angle = g_cl.m_weapon->calc_spread_angle(weapon_info->m_bullets, weapon->m_flRecoilIndex(), i, g_cl.m_weapon->GetInaccuracy(), g_cl.m_weapon->GetSpread());

		auto direction = forward + (right * spread_angle.x) + (up * spread_angle.y);
		direction = direction.normalized();

		auto end = start + direction * range;

		if (can_hit_hitbox(start, end, player, point.hitbox, point.record))
			++hits;

		if ((float)(hits + total_seeds - i) / (float)(total_seeds) < amount)
			return false;
	}

	*out_chance = (float)hits / (float)total_seeds;
	return ((float)hits / (float)total_seeds) >= amount;
}

void c_rage_bot::proceed_aimbot()
{
	target = nullptr;
	working = false;
	stopping = false;
	force_accuracy = true;

	if (!g_cl.m_weapon || g_csgo.m_gamerules->m_bFreezePeriod() || g_cl.m_local->m_fFlags() & FL_FROZEN || g_cl.m_local->m_fImmuneToGunGameDamageTime())
	{
		return;
	}

	if (g_cl.m_weapon->IsKnife())
		return;

	bool invalid_weapon = g_cl.m_weapon->is_misc_weapon() && !g_cl.m_weapon->is_taser();

	if (!g_menu.main.aimbot.enable.get() || invalid_weapon)
	{
		return;
	}

	float hitchance = std::clamp(g_menu.main.aimbot.hitchance_amount.get() / 100.f, 0.f, 1.f);

	auto& players = g_custom_entity_listener.get_entity(ent_player);
	if (players.empty())
		return;

	point_t best_point{ };

	for (auto& player : players)
	{
		auto entity = player.m_entity->as<Player*>();
		if (!entity)
			continue;

		if (entity == g_cl.m_local || entity->m_iTeamNum() == g_cl.m_local->m_iTeamNum())
			continue;

		auto cache = &g_aimbot.m_players[entity->index() - 1];
		if (!entity->alive() || entity->dormant() || entity->m_fImmuneToGunGameDamageTime())
		{
			target = nullptr;

			if (!cache->cache.points.empty())
				cache->cache.points.clear();

			if (cache->cache.best_point.filled)
				cache->cache.best_point.reset();

			if (cache->cache.player)
				cache->cache.player = nullptr;

			continue;
		}

		cache->cache.player = entity;

		thread_build_points(&cache->cache);
		//Threading::QueueJobRef(thread_build_points, &cache);
	}

	//Threading::FinishQueue();

	for (auto& player : players)
	{
		auto entity = (Player*)player.m_entity;
		if (!entity || entity == g_cl.m_local)
			continue;

		if (entity->m_iTeamNum() == g_cl.m_local->m_iTeamNum() || !entity->alive() || entity->dormant() || entity->m_fImmuneToGunGameDamageTime())
			continue;

		auto& cache = aim_cache[entity->index()];
		if (!cache.player || cache.player != entity)
			continue;

		thread_get_best_point(&cache);
	}

	int highest_damage = INT_MIN;

	should_slide = false;

	for (auto& player : players)
	{
		auto entity = (Player*)player.m_entity;
		if (!entity || entity == g_cl.m_local)
			continue;

		if (entity->m_iTeamNum() == g_cl.m_local->m_iTeamNum() || !entity->alive() || entity->dormant() || entity->m_fImmuneToGunGameDamageTime())
			continue;

		auto cache = &aim_cache[entity->index()];
		if (!cache || !cache->player || cache->player != entity)
			continue;

		if (!cache->best_point.filled)
		{
			continue;
		}

		if (highest_damage < cache->best_point.damage)
		{
			target = entity;
			highest_damage = cache->best_point.damage;
		}
	}

	if (target)
		best_point = aim_cache[target->index()].best_point;

	if (best_point.filled)
	{
		working = true;
		stopping = true;

		force_scope();

		bool shoot_on_unlag = true;
		if (!g_input.GetKeyState(g_menu.main.movement.fakewalk.get()) && g_menu.main.aimbot.delayshot.get() &&
			g_csgo.m_cl->choked_commands > 2)
			shoot_on_unlag = false;

		if (shoot_on_unlag && force_accuracy && is_accurate(target, best_point, hitchance, &best_point.hitchance))
		{
			if (g_cl.is_able_to_shoot(true))
			{
				if (!g_csgo.m_cl->choked_commands)
					*g_cl.m_packet = false;

				g_cl.m_cmd->m_buttons |= IN_ATTACK;
			}

			if (g_cl.m_cmd->m_buttons & IN_ATTACK)
			{
				firing = true;

				g_cl.m_cmd->m_tick = game::TIME_TO_TICKS(best_point.record->m_sim_time + g_cl.m_lerp);

				g_cl.m_cmd->m_view_angles = math::normalize_ang(math::angle_from_vectors(g_cl.m_shoot_pos, best_point.position), true);
				g_cl.m_cmd->m_view_angles -= g_cl.m_local->m_aimPunchAngle() * g_csgo.weapon_recoil_scale->GetFloat();

				g_cl.m_cmd->m_view_angles = math::normalize_ang(g_cl.m_cmd->m_view_angles, true);

				// store fired shot.
				if (best_point.record)
					g_shots.add(
						g_cl.m_shoot_pos, best_point.record->m_player,
						-1, g_cl.m_cmd->m_command_number, g_csgo.m_globals->m_realtime,
						g_csgo.m_engine->GetNetChannelInfo()->GetLatency(INetChannel::FLOW_INCOMING) +
						g_csgo.m_engine->GetNetChannelInfo()->GetLatency(INetChannel::FLOW_INCOMING),
						best_point.record, best_point.hitbox
					);
			}
		}
	}
}*/