#pragma once

enum BoneSetupFlags {
	None = 0,
	UseInterpolatedOrigin = (1 << 0),
	UseCustomOutput = (1 << 1),
	ForceInvalidateBoneCache = (1 << 2),
	AttachmentHelper = (1 << 3),
};

class Bones {

public:
	bool m_running;

public:
	bool setup(Player* player, BoneArray* out, LagRecord* record);
	bool BuildBones( Player* target, int mask, BoneArray* out, LagRecord* record );
	bool BuildEnemyBones(Player* player, int mask, matrix3x4_t* out, float time, LagRecord* record);
	bool BuildLocalBones(Player* player, int mask, matrix3x4_t* out, float time);

	bool SetupBonesRebuild(Player* entity, BoneArray* pBoneMatrix, int nBoneCount, int boneMask, float time, int flags);
};

extern Bones g_bones;