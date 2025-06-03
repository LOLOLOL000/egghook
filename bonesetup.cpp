#include "includes.h"

Bones g_bones{};;


bool Bones::setup(Player* player, BoneArray* out, LagRecord* record)
{
	// if the record isnt setup yet.
	if (!record->m_setup) {
		// run setupbones rebuilt.
		if (!BuildBones(player, 0x7FF00, record->m_bones, record))
			return false;

		// we have setup this record bones.
		record->m_setup = true;
	}

	// record is setup.
	if (out && record->m_setup)
		std::memcpy(out, record->m_bones, sizeof(BoneArray) * 128);

	return true;
}

bool Bones::BuildBones(Player* target, int mask, BoneArray* out, LagRecord* record) {
	vec3_t		     pos[128];
	quaternion_t     q[128];
	vec3_t           backup_origin;
	ang_t            backup_angles;
	float            backup_poses[24];
	C_AnimationLayer backup_layers[13];

	// get hdr.
	CStudioHdr* hdr = target->GetModelPtr();
	if (!hdr)
		return false;

	// get ptr to bone accessor.
	CBoneAccessor* accessor = &target->m_BoneAccessor();
	if (!accessor)
		return false;

	// store origial output matrix.
	// likely cachedbonedata.
	BoneArray* backup_matrix = accessor->m_pBones;
	if (!backup_matrix)
		return false;

	// prevent the game from calling ShouldSkipAnimationFrame.
	auto bSkipAnimationFrame = *reinterpret_cast<int*>(uintptr_t(target) + 0x260);
	*reinterpret_cast<int*>(uintptr_t(target) + 0x260) = NULL;

	// backup original.
	backup_origin = target->GetAbsOrigin();
	backup_angles = target->GetAbsAngles();
	target->GetPoseParameters(backup_poses);
	target->GetAnimLayers(backup_layers);

	// compute transform from raw data.
	matrix3x4_t transform;
	math::AngleMatrix(record->m_abs_ang, record->m_origin, transform);

	// set non interpolated data.
	target->AddEffect(EF_NOINTERP);
	target->SetAbsOrigin(record->m_origin);
	target->SetAbsAngles(record->m_abs_ang);
	target->SetPoseParameters(record->m_poses);
	target->SetAnimLayers(record->m_layers);

	// force game to call AccumulateLayers - pvs fix.
	m_running = true;

	// set bone array for write.
	accessor->m_pBones = out;

	// compute and build bones.
	target->StandardBlendingRules(hdr, pos, q, record->m_pred_time, mask);

	uint8_t computed[0x100];
	std::memset(computed, 0, 0x100);
	target->BuildTransformations(hdr, pos, q, transform, mask, computed);

	// restore old matrix.
	accessor->m_pBones = backup_matrix;

	// restore original interpolated entity data.
	target->m_fEffects() &= ~EF_NOINTERP;
	target->SetAbsOrigin(backup_origin);
	target->SetAbsAngles(backup_angles);
	target->SetPoseParameters(backup_poses);
	target->SetAnimLayers(backup_layers);

	// revert to old game behavior.
	m_running = false;

	// allow the game to call ShouldSkipAnimationFrame.
	*reinterpret_cast<int*>(uintptr_t(target) + 0x260) = bSkipAnimationFrame;

	return true;
}

bool Bones::BuildEnemyBones(Player* player, int mask, matrix3x4_t* out, float time, LagRecord* record)
{
	const auto m_pIk = player->m_pIK();
	const auto client_ent_flags = player->m_ClientEntEffects();
	const auto effects = player->m_fEffects();
	const auto animlod = player->m_nAnimLODflags();

	player->m_pIK() = 0;
	player->m_BoneAccessor().m_ReadableBones = 0;
	player->m_iMostRecentModelBoneCounter() = 0;
	player->m_nLastNonSkippedFrame() = 0;
	player->m_nAnimLODflags() &= ~2u; //flag: ANIMLODFLAG_OUTSIDEVIEWFRUSTUM
	player->m_nCustomBlendingRuleMask() = -1;
	player->m_ClientEntEffects() |= 2u; //flag: NO_IK
	player->m_fEffects() |= 8u;

	float prev_layer = -1;

	if (player->m_AnimOverlayCount() >= 12) {
		prev_layer = player->m_AnimOverlay()[12].m_weight;
		player->m_AnimOverlay()[12].m_weight = 0;
	}

	const auto pastedframe = g_csgo.m_globals->m_frame;

	g_csgo.m_globals->m_frame = -999;

	auto boneSnapshot1 = (float*)(uintptr_t(player) + (0x3AD0 + 4));
	auto boneSnapshot2 = (float*)(uintptr_t(player) + (0x6F20 + 4));

	auto bk_snapshot1 = *boneSnapshot1;
	auto bk_snapshot2 = *boneSnapshot2;

	*boneSnapshot1 = 0.0f;
	*boneSnapshot2 = 0.0f;

	g_hooks.m_updating_bones[player->index()] = true;
	auto bones = player->SetupBones(out, 128, mask, time);
	g_hooks.m_updating_bones[player->index()] = false;

	g_csgo.m_globals->m_frame = pastedframe;
	*boneSnapshot1 = bk_snapshot1;
	*boneSnapshot2 = bk_snapshot2;

	if (player->m_AnimOverlayCount() >= 12 && prev_layer >= 0.f) {
		player->m_AnimOverlay()[12].m_weight = prev_layer;
	}

	player->m_pIK() = m_pIk;
	player->m_fEffects() = effects;
	player->m_ClientEntEffects() = client_ent_flags;
	player->m_nAnimLODflags() = animlod;

	return bones;
}

bool Bones::BuildLocalBones(Player* player, int mask, matrix3x4_t* out, float time)
{
	const auto m_pIk = player->m_pIK();
	const auto client_ent_flags = player->m_ClientEntEffects();
	const auto effects = player->m_fEffects();
	const auto animlod = player->m_nAnimLODflags();

	player->m_pIK() = 0;
	player->m_BoneAccessor().m_ReadableBones = 0;
	player->m_iMostRecentModelBoneCounter() = 0;
	player->m_nLastNonSkippedFrame() = 0;
	player->m_nAnimLODflags() &= ~2u; //flag: ANIMLODFLAG_OUTSIDEVIEWFRUSTUM
	player->m_nCustomBlendingRuleMask() = -1;
	player->m_ClientEntEffects() |= 2u; //flag: NO_IK
	player->m_fEffects() |= 8u;

	float prev_layer = -1;

	if (player->m_AnimOverlayCount() >= 12) {
		prev_layer = player->m_AnimOverlay()[12].m_weight;
		player->m_AnimOverlay()[12].m_weight = 0;
	}

	const auto pastedframe = g_csgo.m_globals->m_frame;

	g_csgo.m_globals->m_frame = -999;

	auto boneSnapshot1 = (float*)(uintptr_t(this) + (0x3AD0 + 4));
	auto boneSnapshot2 = (float*)(uintptr_t(this) + (0x6F20 + 4));

	auto bk_snapshot1 = *boneSnapshot1;
	auto bk_snapshot2 = *boneSnapshot2;

	*boneSnapshot1 = 0.0f;
	*boneSnapshot2 = 0.0f;

	g_hooks.m_updating_bones[player->index()] = true;
	auto bones = player->SetupBones(out, 128, mask, time);
	g_hooks.m_updating_bones[player->index()] = false;

	g_csgo.m_globals->m_frame = pastedframe;
	*boneSnapshot1 = bk_snapshot1;
	*boneSnapshot2 = bk_snapshot2;

	if (player->m_AnimOverlayCount() >= 12 && prev_layer >= 0.f) {
		player->m_AnimOverlay()[12].m_weight = prev_layer;
	}
	
	player->m_pIK() = m_pIk;
	player->m_fEffects() = effects;
	player->m_ClientEntEffects() = client_ent_flags;
	player->m_nAnimLODflags() = animlod;

	return bones;
}

bool Bones::SetupBonesRebuild(Player* entity, BoneArray* pBoneMatrix, int nBoneCount, int boneMask, float time, int flags)
{
	if (*(int*)(uintptr_t(entity) + g_entoffsets.m_nSequence) == -1) {
		return false;
	}

	if (boneMask == -1) {
		boneMask = entity->m_iPrevBoneMask();
	}

	boneMask = boneMask | 0x80000;

	// If we're setting up LOD N, we have set up all lower LODs also
	// because lower LODs always use subsets of the bones of higher LODs.
	int nLOD = 0;
	int nMask = BONE_USED_BY_VERTEX_LOD0;
	for (; nLOD < 8; ++nLOD, nMask <<= 1) {
		if (boneMask & nMask)
			break;
	}
	for (; nLOD < 8; ++nLOD, nMask <<= 1) {
		boneMask |= nMask;
	}

	auto model_bone_counter = **(unsigned long**)(g_csgo.InvalidateBoneCache + 0x000A);

	CBoneAccessor backup_bone_accessor = entity->m_BoneAccessor();
	CBoneAccessor* bone_accessor = &entity->m_BoneAccessor();
	if (!bone_accessor)
		return false;

	if (entity->m_iMostRecentModelBoneCounter() != model_bone_counter || (flags & BoneSetupFlags::ForceInvalidateBoneCache))
	{
		if (FLT_MAX >= entity->m_flLastBoneSetupTime() || time < entity->m_flLastBoneSetupTime()) {
			bone_accessor->m_ReadableBones = 0;
			bone_accessor->m_WritableBones = 0;
			entity->m_flLastBoneSetupTime() = (time);
		}

		entity->m_iPrevBoneMask() = entity->m_iAccumulatedBoneMask();
		entity->m_iAccumulatedBoneMask() = 0;

		auto hdr = entity->m_studioHdr();
		if (hdr) { // profiler stuff
			((CStudioHdrEx*)hdr)->m_nPerfAnimatedBones = 0;
			((CStudioHdrEx*)hdr)->m_nPerfUsedBones = 0;
			((CStudioHdrEx*)hdr)->m_nPerfAnimationLayers = 0;
		}
	}

	// Keep track of everything asked for over the entire frame
	// But not those things asked for during bone setup
	entity->m_iAccumulatedBoneMask() |= boneMask;

	// fix enemy poses getting raped when going out of pvs
	entity->m_iOcclusionFramecount() = 0;
	entity->m_iOcclusionFlags() = 0;

	// Make sure that we know that we've already calculated some bone stuff this time around.
	entity->m_iMostRecentModelBoneCounter() = model_bone_counter;

	bool bReturnCustomMatrix = (flags & BoneSetupFlags::UseCustomOutput) && pBoneMatrix;
	CStudioHdr* hdr = entity->m_studioHdr();
	if (!hdr) {
		return false;
	}

	// Setup our transform based on render angles and origin.
	vec3_t origin = (flags & BoneSetupFlags::UseInterpolatedOrigin) ? entity->GetAbsOrigin() : entity->m_vecOrigin();
	ang_t angles = entity->GetAbsAngles();

	alignas(16) matrix3x4_t parentTransform;
	math::AngleMatrix(angles, origin, parentTransform);

	boneMask |= entity->m_iPrevBoneMask();

	if (bReturnCustomMatrix) {
		bone_accessor->m_pBones = pBoneMatrix;
	}

	// Allow access to the bones we're setting up so we don't get asserts in here.
	int oldReadableBones = bone_accessor->m_ReadableBones;
	int oldWritableBones = bone_accessor->m_WritableBones;
	int newWritableBones = oldReadableBones | boneMask;
	bone_accessor->m_WritableBones = newWritableBones;
	bone_accessor->m_ReadableBones = newWritableBones;

	if (!(hdr->m_pStudioHdr->m_flags &0x00000010)) {
		entity->m_fEffects() |= EF_NOINTERP;

		entity->m_iEFlags() |= 8;

		entity->m_pIK() = 0;
		entity->m_ClientEntEffects()  |= 2; // ENTCLIENTFLAGS_DONTUSEIK

		alignas(16) vec3_t pos[128];
		alignas(16) quaternion_t q[128];
		uint8_t computed[0x100];

		entity->StandardBlendingRules(hdr, pos, q, time, boneMask);

		std::memset(computed, 0, 0x100);
		entity->BuildTransformations(hdr, pos, q, parentTransform, boneMask, computed);

		entity->m_iEFlags() &= ~8;

		// entity->ControlMouth( hdr );

		if (!bReturnCustomMatrix /*&& !bSkipAnimFrame*/) {
			memcpy(entity->m_vecBonePos(), &pos[0], sizeof(vec3_t) * hdr->m_pStudioHdr->m_num_bones);
			memcpy(entity->m_quatBoneRot(), &q[0], sizeof(quaternion_t) * hdr->m_pStudioHdr->m_num_bones);
		}
	}
	else {
		parentTransform = bone_accessor->m_pBones[0];
	}

	if ( /*boneMask & BONE_USED_BY_ATTACHMENT*/ flags & BoneSetupFlags::AttachmentHelper) {
		entity->attachments_helper();
	}

	// don't override bone cache if we're just generating a standalone matrix
	if (bReturnCustomMatrix) {
		*bone_accessor = backup_bone_accessor;

		return true;
	}

	return true;
}
