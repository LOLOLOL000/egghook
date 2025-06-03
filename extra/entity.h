#pragma once

#include <optional>

#define MAX_WEAPONS	48

class CAttribute_String
{
public:
	virtual ~CAttribute_String();

	int32_t FieldSet;
	char** Value;

private:
	int32_t pad0[2];
};

struct cmd_context_t {
	bool		m_needs_processing{ };
	CUserCmd	m_user_cmd{ };
	int			m_cmd_number{ };
};

template < typename Key, typename Value >
struct Node_t {
	int previous_id;    //0x0000
	int next_id;        //0x0004
	void* _unknown_ptr; //0x0008
	int _unknown;       //0x000C
	Key key;            //0x0010
	Value value;        //0x0014
};

template < typename Key, typename Value >
struct Head_t {
	Node_t< Key, Value >* memory; //0x0000
	int allocation_count;         //0x0004
	int grow_size;                //0x0008
	int start_element;            //0x000C
	int next_available;           //0x0010
	int _unknown;                 //0x0014
	int last_element;             //0x0018
};

union attribute_data_union_t
{
	bool bValue;
	int iValue;
	float fValue;
	vec3_t* vValue;
	CAttribute_String* szValue;
};

class CEconItemAttributeDefinition;
class ISchemaAttributeType
{
public:
	virtual ~ISchemaAttributeType();
	virtual int GetTypeUniqueIdentifier();
	virtual void LoadEconAttributeValue();
	virtual void ConvertEconAttributeValueToByteStream();
	virtual bool BConvertStringToEconAttributeValue(const CEconItemAttributeDefinition*, const char*, attribute_data_union_t*);
};

struct attr_type_t
{
	char* name;
	ISchemaAttributeType* type;
};

struct AlternateIconData_t
{
private:
	int32_t pad0;

public:
	CUtlString icon_path;
	CUtlString icon_path_large;

private:
	CUtlString pad1[2];
};

struct kill_eater_score_type_t
{
	int id;
	char* type_name;
	char* model_attribute;
	char* level_data;
	bool use_level_data;
};

class CEconItemRarityDefinition
{
public:
	int value;
	int color;
	CUtlString name;
	CUtlString loc_key;
	CUtlString loc_key_weapon;
	CUtlString loot_list;
	CUtlString recycle_list;
	CUtlString drop_sound;
	CUtlString next_rarity;

private:
	int32_t pad0[2];

public:
	float weight;
};

class CEconItemQualityDefinition
{
public:
	int value;
	char* name;
	int weight;
	bool canSupportSet;
	char* hexColor;
};

class CEconItemAttributeDefinition
{
public:
	virtual ~CEconItemAttributeDefinition();

	KeyValues* kv;
	uint16_t id;
	ISchemaAttributeType* attribute_type;
	bool hidden;
	bool force_output_description;
	bool stored_as_integer;
	bool instance_data;
	int asset_class_export;
	int asset_class_bucket;
	int effect_type;
	int description_format;
	char* description_string;
	char* description_tag;
	char* armory_desc;
	int score;
	char* name;
	char* attribute_class;

private:
	int32_t pad0;
};

class CEconCraftingRecipeDefinition
{
public:
	virtual ~CEconCraftingRecipeDefinition();

	int definitionindex; // Don't ask me why Valve sometimes uses dword and sometimes word.
	CUtlString name;
	CUtlString n_A;
	CUtlString desc_inputs;
	CUtlString desc_outputs;
	CUtlString di_A;
	CUtlString di_B;
	CUtlString di_C;
	CUtlString do_A;
	CUtlString do_B;
	CUtlString do_C;
	wchar_t name_localized[64];
	wchar_t requires_produces_localized[512];
	bool all_same_class;
	bool all_same_slot;
	int add_class_usage_to_output;
	int add_slot_usage_to_output;
	int add_set_to_output;
	bool always_known;
	bool premium_only;
	int category;
	int filter;

private: // Some vectors and other things
	int32_t pad0[47];
};

struct item_list_entry_t
{
	int item;
	int paintkit;

private:
	int32_t pad0[5];
};

struct itemset_attrib_t;

class CEconItemSetDefinition
{
public:
	virtual ~CEconItemSetDefinition();

	char* key_name;
	char* name;
	char* unlocalized_name;
	char* set_description;
	CUtlVector<item_list_entry_t> items;
	int store_bundle;
	bool is_collection;
	bool is_hidden_set;
	CUtlVector<itemset_attrib_t> attributes;
};

class CEconLootListDefinition
{
public:
	virtual ~CEconLootListDefinition();

private:
	int32_t pad0[5]; // a vector

public:
	char* name;
	CUtlVector<item_list_entry_t> items;
	int hero;
	bool public_list_contents;
	bool contains_stickers_autographed_by_proplayers;
	bool will_produce_stattrak;
	float totalweights;
	CUtlVector<float> weights;

private:
	int32_t pad1[6]; // a vector + something else
};

class CPaintKit
{
public:
	int id;

	CUtlString name;
	CUtlString description_string;
	CUtlString description_tag;
	CUtlString pattern;
	CUtlString normal;
	CUtlString logo_material;

	bool pad0;

	int rarity;
	int style;

	Color color0;
	Color color1;
	Color color2;
	Color color3;

	Color logocolor0;
	Color logocolor1;
	Color logocolor2;
	Color logocolor3;

	float wear_default;
	float wear_remap_min;
	float wear_remap_max;

	uint8_t seed;

	uint8_t phongexponent;
	uint8_t phongalbedoboost;
	uint8_t phongintensity;

	float pattern_scale;
	float pattern_offset_y_start;
	float pattern_offset_y_end;
	float pattern_offset_x_start;
	float pattern_offset_x_end;
	float pattern_rotate_start;
	float pattern_rotate_end;

	float logo_scale;
	float logo_offset_x;
	float logo_offset_y;
	float logo_rotation;

	bool ignore_weapon_size_scale;
	int view_model_exponent_override_size;
	bool only_first_material;
	bool use_normal;

	CUtlString vmt_path;
	KeyValues* vmt_overrides;
};

class CStickerKit
{
public:
	int id;

	int item_rarity;

	CUtlString name;
	CUtlString description_string;
	CUtlString item_name;
	CUtlString sticker_material;
	CUtlString sticker_material_nodrips;
	CUtlString image_inventory;

	int tournament_event_id;
	int tournament_team_id;
	int tournament_player_id;
	bool cannot_trade;

	float rotate_start;
	float rotate_end;

	float scale_min;
	float scale_max;

	float wear_min;
	float wear_max;

	CUtlString image_inventory2;
	CUtlString image_inventory_large;

	KeyValues* kv;
};

class CEconColorDefinition
{
public:
	char* name;
	char* color_name;
	char* hex_color;
};

class CEconGraffitiTintDefinition
{
public:
	int id;
	char* name;
	char* hex_color;

	uint8_t b;
	uint8_t g;
	uint8_t r;
};

class CEconMusicDefinition
{
public:
	int id;
	char* name;
	char* loc_name;
	char* loc_description;
	char* pedestal_display_model;
	char* image_inventory;

private:
	int32_t pad0[2];
};

class CEconQuestDefinition
{
public:
	int id;
	char* name;
	char* mapgroup;
	char* map;
	char* gamemode;
	char* quest_reward;
	char* expression;
	char* expr_bonus;
	CCopyableUtlVector<unsigned int> points;

	enum
	{
		is_an_event = 1,
		event_allow_individual_maps
	} flags;

	int difficulty;
	int operational_points;
	int xp_reward;
	int xp_bonus_percent;
	int target_team;
	int required_event;

private:
	int32_t pad0;

public:
	int gametype; // gamemode & 15 | ( mapgroup << 8 )
	char* loc_name;
	char* loc_shortname;
	char* loc_description;
	char* loc_huddescription;
	KeyValues* string_tokens;
	char* loc_bonus;
	char* quest_icon;
};

class CEconCampaignDefinition
{
public:
	int id;

private:
	int32_t pad0;

public:
	char* loc_name;
	char* loc_description;

	class CEconCampaignNodeDefinition
	{
	public:
		class CEconCampaignNodeStoryBlockDefinition
		{
		public:
			char* content_file;
			char* character_name;
			char* expression;
			char* description;
		};
		CUtlVector<CEconCampaignNodeStoryBlockDefinition*> story_blocks;

		int id;
		int quest_index;
		int parentid;
		CUtlVector<unsigned int> questunlocks;
	};
	CUtlHashMapLarge<int, CEconCampaignNodeDefinition> quests;
	CUtlHashMapLarge<int, CEconCampaignNodeDefinition> quests2; // ???

	int season_number;
};

class CSkirmishModeDefinition
{
public:
	int id;
	char* name;
	char* gamemode;
	int igamemode;
	char* server_exec;
	char* loc_name;
	char* loc_rules;
	char* loc_description;
	char* loc_details;
	char* icon;
};

class CProPlayerData
{
public:
	KeyValues* kv;
	int id;
	CUtlString name;
	CUtlString code;
	int dob;
	CUtlString geo;
};

class CItemLevelingDefinition
{
public:
	int id;
	int score;
	char* rank_name;

private:
	int32_t pad0[2];
};

class CPipRankData
{
public:
	class PipRankInfo
	{
	private:
		int32_t pad0;

	public:
		int8_t id;
		int8_t pips;
		int8_t winstreak;
		int8_t loss;
	};

	CUtlVector<PipRankInfo> rank_info;
};

struct static_attrib_t // Get a CEconItemAttributeDefinition* with m_Attributes[static_attrib_t.id]
{
	uint16_t id;
	attribute_data_union_t value;
	bool force_gc_to_generate;
};

struct StickerData_t
{
	char viewmodel_geometry[128];
	char viewmodel_material[128];
	vec3_t worldmodel_decal_pos;
	vec3_t worldmodel_decal_end;
	char worldmodel_decal_bone[32];
};

struct codecontrolledbodygroupdata_t;
struct attachedmodel_t;
struct attachedparticle_t;
class CEconStyleInfo;
class AssetInfo
{
private:
	int32_t pad0;

public:
	CUtlMap<const char*, int, unsigned short> player_bodygroups;
	CUtlMap<const char*, codecontrolledbodygroupdata_t, unsigned short> code_controlled_bodygroup;
	int skin;
	bool use_per_class_bodygroups;
	CUtlVector<attachedmodel_t> attached_models;
	CUtlVector<attachedparticle_t> attached_particles;
	char* custom_sounds[10];
	char* material_override;
	int muzzle_flash;

private:
	int32_t pad1;

public:
	char* particle_effect;
	char* particle_snapshot;
	char* sounds[18];
	char* primary_ammo;
	char* secondary_ammo;
	char* weapon_type;
	char* addon_location;
	char* eject_brass_effect;
	char* muzzle_flash_effect_1st_person;
	char* muzzle_flash_effect_1st_person_alt;
	char* muzzle_flash_effect_3rd_person;
	char* muzzle_flash_effect_3rd_person_alt;
	char* heat_effect;
	char* player_animation_extension;
	vec3_t grenade_smoke_color;
	int vm_bodygroup_override;
	int vm_bodygroup_state_override;
	int wm_bodygroup_override;
	int wm_bodygroup_state_override;
	bool skip_model_combine;
	CUtlVector<const char*> animation_modifiers;
	CUtlVector<CEconStyleInfo*> styles;
};

struct WeaponPaintableMaterial_t
{
	char Name[128];
	char OrigMat[128];
	char FolderName[128];
	int ViewmodelDim;
	int WorldDim;
	float WeaponLength;
	float UVScale;
	bool BaseTextureOverride;
	bool MirrorPattern;
};

enum ClassID
{
	CAI_BaseNPC,
	CAK47,
	CBaseAnimating,
	CBaseAnimatingOverlay,
	CBaseAttributableItem,
	CBaseButton,
	CBaseCombatCharacter,
	CBaseCombatWeapon,
	CBaseCSGrenade,
	CBaseCSGrenadeProjectile,
	CBaseDoor,
	CBaseEntity,
	CBaseFlex,
	CBaseGrenade,
	CBaseParticleEntity,
	CBasePlayer,
	CBasePropDoor,
	CBaseTeamObjectiveResource,
	CBaseTempEntity,
	CBaseToggle,
	CBaseTrigger,
	CBaseViewModel,
	CBaseVPhysicsTrigger,
	CBaseWeaponWorldModel,
	CBeam,
	CBeamSpotlight,
	CBoneFollower,
	CBreakableProp,
	CBreakableSurface,
	CC4,
	CCascadeLight,
	CChicken,
	CColorCorrection,
	CColorCorrectionVolume,
	CCSGameRulesProxy,
	CCSPlayer,
	CCSPlayerResource,
	CCSRagdoll,
	CCSTeam,
	CDEagle,
	CDecoyGrenade,
	CDecoyProjectile,
	CDynamicLight,
	CDynamicProp,
	CEconEntity,
	CEconWearable,
	CEmbers,
	CEntityDissolve,
	CEntityFlame,
	CEntityFreezing,
	CEntityParticleTrail,
	CEnvAmbientLight,
	CEnvDetailController,
	CEnvDOFController,
	CEnvParticleScript,
	CEnvProjectedTexture,
	CEnvQuadraticBeam,
	CEnvScreenEffect,
	CEnvScreenOverlay,
	CEnvTonemapController,
	CEnvWind,
	CFEPlayerDecal,
	CFireCrackerBlast,
	CFireSmoke,
	CFireTrail,
	CFish,
	CFlashbang,
	CFogController,
	CFootstepControl,
	CFunc_Dust,
	CFunc_LOD,
	CFuncAreaPortalWindow,
	CFuncBrush,
	CFuncConveyor,
	CFuncLadder,
	CFuncMonitor,
	CFuncMoveLinear,
	CFuncOccluder,
	CFuncReflectiveGlass,
	CFuncRotating,
	CFuncSmokeVolume,
	CFuncTrackTrain,
	CGameRulesProxy,
	CHandleTest,
	CHEGrenade,
	CHostage,
	CHostageCarriableProp,
	CIncendiaryGrenade,
	CInferno,
	CInfoLadderDismount,
	CInfoOverlayAccessor,
	CItem_Healthshot,
	CItemDogtags,
	CKnife,
	CKnifeGG,
	CLightGlow,
	CMaterialModifyControl,
	CMolotovGrenade,
	CMolotovProjectile,
	CMovieDisplay,
	CParticleFire,
	CParticlePerformanceMonitor,
	CParticleSystem,
	CPhysBox,
	CPhysBoxMultiplayer,
	CPhysicsProp,
	CPhysicsPropMultiplayer,
	CPhysMagnet,
	CPlantedC4,
	CPlasma,
	CPlayerResource,
	CPointCamera,
	CPointCommentaryNode,
	CPointWorldText,
	CPoseController,
	CPostProcessController,
	CPrecipitation,
	CPrecipitationBlocker,
	CPredictedViewModel,
	CProp_Hallucination,
	CPropDoorRotating,
	CPropJeep,
	CPropVehicleDriveable,
	CRagdollManager,
	CRagdollProp,
	CRagdollPropAttached,
	CRopeKeyframe,
	CSCAR17,
	CSceneEntity,
	CSensorGrenade,
	CSensorGrenadeProjectile,
	CShadowControl,
	CSlideshowDisplay,
	CSmokeGrenade,
	CSmokeGrenadeProjectile,
	CSmokeStack,
	CSpatialEntity,
	CSpotlightEnd,
	CSprite,
	CSpriteOriented,
	CSpriteTrail,
	CStatueProp,
	CSteamJet,
	CSun,
	CSunlightShadowControl,
	CTeam,
	CTeamplayRoundBasedRulesProxy,
	CTEArmorRicochet,
	CTEBaseBeam,
	CTEBeamEntPoint,
	CTEBeamEnts,
	CTEBeamFollow,
	CTEBeamLaser,
	CTEBeamPoints,
	CTEBeamRing,
	CTEBeamRingPoint,
	CTEBeamSpline,
	CTEBloodSprite,
	CTEBloodStream,
	CTEBreakModel,
	CTEBSPDecal,
	CTEBubbles,
	CTEBubbleTrail,
	CTEClientProjectile,
	CTEDecal,
	CTEDust,
	CTEDynamicLight,
	CTEEffectDispatch,
	CTEEnergySplash,
	CTEExplosion,
	CTEFireBullets,
	CTEFizz,
	CTEFootprintDecal,
	CTEFoundryHelpers,
	CTEGaussExplosion,
	CTEGlowSprite,
	CTEImpact,
	CTEKillPlayerAttachments,
	CTELargeFunnel,
	CTEMetalSparks,
	CTEMuzzleFlash,
	CTEParticleSystem,
	CTEPhysicsProp,
	CTEPlantBomb,
	CTEPlayerAnimEvent,
	CTEPlayerDecal,
	CTEProjectedDecal,
	CTERadioIcon,
	CTEShatterSurface,
	CTEShowLine,
	CTesla,
	CTESmoke,
	CTESparks,
	CTESprite,
	CTESpriteSpray,
	CTest_ProxyToggle_Networkable,
	CTestTraceline,
	CTEWorldDecal,
	CTriggerPlayerMovement,
	CTriggerSoundOperator,
	CVGuiScreen,
	CVoteController,
	CWaterBullet,
	CWaterLODControl,
	CWeaponAug,
	CWeaponAWP,
	CWeaponBaseItem,
	CWeaponBizon,
	CWeaponCSBase,
	CWeaponCSBaseGun,
	CWeaponCycler,
	CWeaponElite,
	CWeaponFamas,
	CWeaponFiveSeven,
	CWeaponG3SG1,
	CWeaponGalil,
	CWeaponGalilAR,
	CWeaponGlock,
	CWeaponHKP2000,
	CWeaponM249,
	CWeaponM3,
	CWeaponM4A1,
	CWeaponMAC10,
	CWeaponMag7,
	CWeaponMP5Navy,
	CWeaponMP7,
	CWeaponMP9,
	CWeaponNegev,
	CWeaponNOVA,
	CWeaponP228,
	CWeaponP250,
	CWeaponP90,
	CWeaponSawedoff,
	CWeaponSCAR20,
	CWeaponScout,
	CWeaponSG550,
	CWeaponSG552,
	CWeaponSG556,
	CWeaponSSG08,
	CWeaponTaser,
	CWeaponTec9,
	CWeaponTMP,
	CWeaponUMP45,
	CWeaponUSP,
	CWeaponXM1014,
	CWorld,
	CWorldVguiText,
	DustTrail,
	MovieExplosion,
	ParticleSmokeGrenade,
	RocketTrail,
	SmokeTrail,
	SporeExplosion,
	SporeTrail,
};
class IEconTool;
class CCStrike15ItemSystem;
class CCStrike15ItemDefinition
{
public:
	virtual uint16_t GetDefinitionIndex() const = 0;
	virtual const char* GetPrefabName() const = 0;
	virtual const char* GetItemBaseName() const = 0;
	virtual const char* GetItemTypeName() const = 0;
	virtual const char* GetItemDesc() const = 0;
	virtual const char* GetInventoryImage() const = 0;
	virtual const char* GetBasePlayerDisplayModel() const = 0;
	virtual const char* GetWorldDisplayModel() const = 0;
	virtual const char* GetExtraWearableModel() const = 0;
	virtual int GetLoadoutSlot() const = 0;
	virtual KeyValues* GetRawDefinition() const = 0;
	virtual int GetHeroID() const = 0;
	virtual int GetRarity() const = 0;
	virtual CUtlVector<int>* GetItemSets() const = 0;
	virtual int GetBundleItemCount() const = 0;
	virtual void* GetBundleItem(int) const = 0;
	virtual bool IsBaseItem() const = 0;
	virtual bool IsPublicItem() const = 0;
	virtual bool IsBundle() const = 0;
	virtual bool IsPackBundle() const = 0;
	virtual bool IsPackItem() const = 0;
	virtual void* BInitVisualBlockFromKV(KeyValues*, void*, void*) = 0;
	virtual void* BInitFromKV(KeyValues*, void*, void*) = 0;
	virtual void* BInitFromTestItemKVs(int, KeyValues*, void*) = 0;
	virtual void* GeneratePrecacheModelStrings(bool, void*) const = 0;
	virtual void* GeneratePrecacheSoundStrings(void*) const = 0;
	virtual void* GeneratePrecacheEffectStrings(void*) const = 0;
	virtual void* CopyPolymorphic(const CCStrike15ItemDefinition*) = 0;
	virtual int GetItemTypeID() const = 0;
	virtual bool IsDefaultSlotItem() const = 0;
	virtual bool IsPreviewableInStore() const = 0;
	virtual int GetBundleItemPaintKitID(int) const = 0;
	virtual const char* GetWorldDroppedModel() const = 0;
	virtual const char* GetHolsteredModel() const = 0;
	virtual const char* GetZoomInSound() const = 0;
	virtual const char* GetZoomOutSound() const = 0;
	virtual const char* GetIconDisplayModel() const = 0;
	virtual const char* GetBuyMenuDisplayModel() const = 0;
	virtual const char* GetPedestalDisplayModel() const = 0;
	virtual const char* GetMagazineModel() const = 0;
	virtual const char* GetScopeLensMaskModel() const = 0;
	virtual const char* GetUidModel() const = 0;
	virtual const char* GetStatTrakModelByType(unsigned int) const = 0;
	virtual int GetNumSupportedStickerSlots() const = 0;
	virtual const char* GetStickerSlotModelBySlotIndex(unsigned int) const = 0;
	virtual void* GetStickerSlotWorldProjectionStartBySlotIndex(unsigned int) const = 0;
	virtual void* GetStickerSlotWorldProjectionEndBySlotIndex(unsigned int) const = 0;
	virtual void* GetStickerWorldModelBoneParentNameBySlotIndex(unsigned int) const = 0;
	virtual void* GetStickerSlotMaterialBySlotIndex(unsigned int) const = 0;
	virtual const char* GetIconDefaultImage() const = 0;
	virtual void* GetParticleFile() const = 0;
	virtual void* GetParticleSnapshotFile() const = 0;
	virtual bool IsRecent() const = 0;
	virtual bool IsContentStreamable() const = 0;
	virtual void* IgnoreInCollectionView() const = 0;
	virtual void* GeneratePrecacheModelStrings(bool, void*) = 0;

public:
	KeyValues* kv;
	uint16_t id;
	CUtlVector<uint16_t> associated_items;

private:
	int32_t pad0[2];

public:
	uint8_t min_ilevel;
	uint8_t max_ilevel;
	uint8_t item_rarity;
	uint8_t item_quality;
	uint8_t forced_item_quality;
	uint8_t default_drop_quality;
	uint8_t default_drop_quantity;
	CUtlVector<static_attrib_t> attributes;
	uint8_t popularity_seed;
	KeyValues* portraits;
	char* item_name;
	bool propername;
	char* item_type_name;
	int type_id; // crc32( item_type_name )
	char* item_description;
	int expiration_date;
	int creation_date;
	char* model_inventory;
	char* image_inventory;
	CUtlVector<const char*> image_inventory_overlay;
	int image_inventory_pos_x;
	int image_inventory_pos_y;
	int image_inventory_size_w;
	int image_inventory_size_h;
	char* model_player;

private:
	bool pad1;

public:
	bool hide_bodygroups_deployed_only;
	char* model_world;
	char* model_dropped;
	char* model_holstered;
	char* extra_wearable;

private:
	int32_t pad2[20];

public:
	CUtlVector<StickerData_t> stickers;
	char* icon_default_image;
	bool attach_to_hands;
	bool attach_to_hands_vm_only;
	bool flip_viewmodel;
	bool act_as_wearable;
	CUtlVector<int> item_sets;
	AssetInfo* visuals;

private:
	int32_t pad3;

public:
	bool allow_purchase_standalone;
	char* brass_eject_model;
	char* zoom_in_sound;
	char* zoom_out_sound;

	IEconTool* tool;

private:
	int32_t pad4[3];

public:
	int sound_material;
	bool disable_style_selector;

private:
	int32_t pad5[8];

public:
	char* particle_file;
	char* particle_snapshot;
	char* loot_list_name;

private:
	int32_t pad6[5];

public:
	CUtlVector<WeaponPaintableMaterial_t> paint_data;

	struct
	{
		ang_t* camera_angles;
		vec3_t* camera_offset;
		float camera_fov;
		LightDesc_t* lights[4];

	private:
		int32_t pad0;
	} *inventory_image_data;

	char* item_class;
	char* item_logname;
	char* item_iconname;
	char* name;
	bool hidden;
	bool show_in_armory;
	bool baseitem;
	bool default_slot_item;
	bool import;
	bool one_per_account_cdkey;

private:
	int32_t pad7;

public:
	char* armory_desc;
	CCStrike15ItemDefinition* armory_remapdef;
	CCStrike15ItemDefinition* store_remapdef;
	char* armory_remap;
	char* store_remap;
	char* class_token_id;
	char* slot_token_id;
	int drop_type;
	int holiday_restriction;
	int subtype;

private:
	int32_t pad8[4];

public:
	CUtlMap<unsigned int, const char*, unsigned short> alternate_icons;

private:
	int32_t pad9[9];

public:
	bool not_developer;
	bool ignore_in_collection_view;

	// This is where CCStrike15ItemDefinition begins
	int item_sub_position;
	int item_gear_slot;
	int item_gear_slot_position;
	int anim_slot;
	char* model_player_per_class[4];
	int class_usage[4];

private:
	int32_t pad10[2];
};

class CCStrike15ItemSchema
{
public:
	virtual ~CCStrike15ItemSchema();

	CEconItemAttributeDefinition* GetAttributeDefinitionByName(const char* Name) const;

private:
	int32_t pad0[11];

public:
	KeyValues* kv;

private:
	int32_t pad1;

public:
	uint16_t first_valid_class;
	uint16_t last_valid_class;
	uint16_t first_valid_item_slot;
	uint16_t last_valid_item_slot;
	int num_item_presets;
	int item_level_min;
	int item_level_max;

private:
	int32_t pad2;

public:
	CUtlVector<attr_type_t> attribute_types;
	CUtlHashMapLarge<int, CEconItemRarityDefinition> rarities;
	CUtlHashMapLarge<int, CEconItemQualityDefinition> qualities;

private:
	int32_t pad3[18];

public:
	CUtlHashMapLarge<int, CCStrike15ItemDefinition*> items;
	CCStrike15ItemDefinition* m_DefaultItem; // The first item
	CUtlVector<CEconItemAttributeDefinition*> attributes;
	CUtlHashMapLarge<int, CEconCraftingRecipeDefinition*> recipes;
	CUtlMap<const char*, CEconItemSetDefinition> item_sets;
	CUtlHashMapLarge<int, const char*> revolving_loot_lists;
	CUtlHashMapLarge<int, const char*> quest_reward_loot_lists;
	CUtlMap<const char*, CEconLootListDefinition> client_loot_lists;

private:
	int32_t pad4[14];

public:
	CUtlHashMapLarge<int, AlternateIconData_t> alternate_icons2;

private:
	int32_t pad5[18];

public:
	CUtlHashMapLarge<int, CPaintKit*> paint_kits;
	CUtlHashMapLarge<int, CStickerKit*> sticker_kits;
	CUtlMap<const char*, CStickerKit*> sticker_kits_namekey;

private:
	int32_t pad6[24];

public:
	CUtlMap<const char*, KeyValues*> prefabs;
	CUtlVector<CEconColorDefinition*> colors;
	CUtlVector<CEconGraffitiTintDefinition*> graffiti_tints;

private:
	int32_t pad7[20];

public:
	CUtlHashMapLarge<int, CEconMusicDefinition*> music_definitions;
	CUtlHashMapLarge<int, CEconQuestDefinition*> quest_definitions;
	CUtlHashMapLarge<int, CEconCampaignDefinition*> campaign_definitions;

private:
	int32_t pad8[27];

public:
	CUtlHashMapLarge<int, CSkirmishModeDefinition*> skirmish_modes;
	CUtlHashMapLarge<unsigned int, CProPlayerData*> pro_players;

private:
	int32_t pad9[53];

public:
	CUtlMap<unsigned int, kill_eater_score_type_t, unsigned short> kill_eater_score_types;
	CUtlMap<const char*, CUtlVector<CItemLevelingDefinition>*> item_levels;
	CPipRankData skirmish_rank_info;

private:
	int32_t pad10[43];

public: // This is where CCStrike15ItemSchema begins
	CUtlVector<const char*> m_ClassUsabilityStrings;
	CUtlVector<const char*> m_LoadoutStrings;
	CUtlVector<const char*> m_LoadoutStringsSubPositions;
	CUtlVector<const char*> m_LoadoutStringsForDisplay;

private:
	int32_t pad[5];

public:
	CUtlVector<const char*> m_LoadoutStringsOrder; // idk how to name this. 1st = primary, 2nd = secondary, 3rd = melee, 4th = grenade, 5th = item
};

enum Hitboxes_t : int {
	HITBOX_HEAD = 0,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_R_THIGH,
	HITBOX_L_THIGH,
	HITBOX_R_CALF,
	HITBOX_L_CALF,
	HITBOX_R_FOOT,
	HITBOX_L_FOOT,
	HITBOX_R_HAND,
	HITBOX_L_HAND,
	HITBOX_R_UPPER_ARM,
	HITBOX_R_FOREARM,
	HITBOX_L_UPPER_ARM,
	HITBOX_L_FOREARM,
	HITBOX_MAX
};

enum RenderFlags_t : uint32_t {
	STUDIO_NONE = 0x00000000,
	STUDIO_RENDER = 0x00000001,
	STUDIO_VIEWXFORMATTACHMENTS = 0x00000002,
	STUDIO_DRAWTRANSLUCENTSUBMODELS = 0x00000004,
	STUDIO_TWOPASS = 0x00000008,
	STUDIO_STATIC_LIGHTING = 0x00000010,
	STUDIO_WIREFRAME = 0x00000020,
	STUDIO_ITEM_BLINK = 0x00000040,
	STUDIO_NOSHADOWS = 0x00000080,
	STUDIO_WIREFRAME_VCOLLIDE = 0x00000100,
	STUDIO_NOLIGHTING_OR_CUBEMAP = 0x00000200,
	STUDIO_SKIP_FLEXES = 0x00000400,
	STUDIO_DONOTMODIFYSTENCILSTATE = 0x00000800,
	STUDIO_TRANSPARENCY = 0x80000000,
	STUDIO_SHADOWDEPTHTEXTURE = 0x40000000,
	STUDIO_SHADOWTEXTURE = 0x20000000,
	STUDIO_SKIP_DECALS = 0x10000000
};

enum BoneFlags_t : int {
	BONE_USED_BY_ANYTHING = 0x0007FF00,
	BONE_USED_BY_HITBOX = 0x00000100, // bone (or child) is used by a hit box
	BONE_USED_BY_ATTACHMENT = 0x00000200, // bone (or child) is used by an attachment point
	BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
	BONE_USED_BY_VERTEX_LOD0 = 0x00000400, // bone (or child) is used by the toplevel model via skinned vertex
	BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
	BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
	BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
	BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
	BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
	BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
	BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
	BONE_USED_BY_BONE_MERGE = 0x00040000,
	BONE_USED_BY_SERVER = BONE_USED_BY_HITBOX | BONE_USED_BY_VERTEX_LOD0 | BONE_USED_BY_VERTEX_LOD1 | BONE_USED_BY_VERTEX_LOD2 | BONE_USED_BY_VERTEX_LOD3
	| BONE_USED_BY_VERTEX_LOD4 | BONE_USED_BY_VERTEX_LOD5 | BONE_USED_BY_VERTEX_LOD6 | BONE_USED_BY_VERTEX_LOD7
};

enum CSWeaponType : int {
	WEAPONTYPE_UNKNOWN = -1,
	WEAPONTYPE_KNIFE,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_TASER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_HEALTHSHOT = 11,
	WEAPONTYPE_MELEE = 16
};

enum teams_t : int {
	TEAM_NOTEAM = 0,
	TEAM_SPECTATOR,
	TEAM_TERRORISTS,
	TEAM_COUNTERTERRORISTS
};

enum effects_t : int {
	EF_BONEMERGE = 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT = 0x004,	// player flashlight
	EF_NOINTERP = 0x008,	// don't interpolate the next frame
	EF_NOSHADOW = 0x010,	// Don't cast no shadow
	EF_NODRAW = 0x020,	// don't draw entity
	EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
	EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
	EF_MAX_BITS = 10
};

enum InvalidatePhysicsBits_t : int {
	POSITION_CHANGED = 0x1,
	ANGLES_CHANGED = 0x2,
	VELOCITY_CHANGED = 0x4,
	ANIMATION_CHANGED = 0x8,
};

enum DataUpdateType_t : int {
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

enum LifeStates_t : int {
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

enum Activity
{
	ACT_RESET,
	ACT_IDLE,
	ACT_TRANSITION,
	ACT_COVER,
	ACT_COVER_MED,
	ACT_COVER_LOW,
	ACT_WALK,
	ACT_WALK_AIM,
	ACT_WALK_CROUCH,
	ACT_WALK_CROUCH_AIM,
	ACT_RUN,
	ACT_RUN_AIM,
	ACT_RUN_CROUCH,
	ACT_RUN_CROUCH_AIM,
	ACT_RUN_PROTECTED,
	ACT_SCRIPT_CUSTOM_MOVE,
	ACT_RANGE_ATTACK1,
	ACT_RANGE_ATTACK2,
	ACT_RANGE_ATTACK1_LOW,
	ACT_RANGE_ATTACK2_LOW,
	ACT_DIESIMPLE,
	ACT_DIEBACKWARD,
	ACT_DIEFORWARD,
	ACT_DIEVIOLENT,
	ACT_DIERAGDOLL,
	ACT_FLY,
	ACT_HOVER,
	ACT_GLIDE,
	ACT_SWIM,
	ACT_JUMP,
	ACT_HOP,
	ACT_LEAP,
	ACT_LAND,
	ACT_CLIMB_UP,
	ACT_CLIMB_DOWN,
	ACT_CLIMB_DISMOUNT,
	ACT_SHIPLADDER_UP,
	ACT_SHIPLADDER_DOWN,
	ACT_STRAFE_LEFT,
	ACT_STRAFE_RIGHT,
	ACT_ROLL_LEFT,
	ACT_ROLL_RIGHT,
	ACT_TURN_LEFT,
	ACT_TURN_RIGHT,
	ACT_CROUCH,
	ACT_CROUCHIDLE,
	ACT_STAND,
	ACT_USE,
	ACT_ALIEN_BURROW_IDLE,
	ACT_ALIEN_BURROW_OUT,
	ACT_SIGNAL1,
	ACT_SIGNAL2,
	ACT_SIGNAL3,
	ACT_SIGNAL_ADVANCE,
	ACT_SIGNAL_FORWARD,
	ACT_SIGNAL_GROUP,
	ACT_SIGNAL_HALT,
	ACT_SIGNAL_LEFT,
	ACT_SIGNAL_RIGHT,
	ACT_SIGNAL_TAKECOVER,
	ACT_LOOKBACK_RIGHT,
	ACT_LOOKBACK_LEFT,
	ACT_COWER,
	ACT_SMALL_FLINCH,
	ACT_BIG_FLINCH,
	ACT_MELEE_ATTACK1,
	ACT_MELEE_ATTACK2,
	ACT_RELOAD,
	ACT_RELOAD_START,
	ACT_RELOAD_FINISH,
	ACT_RELOAD_LOW,
	ACT_ARM,
	ACT_DISARM,
	ACT_DROP_WEAPON,
	ACT_DROP_WEAPON_SHOTGUN,
	ACT_PICKUP_GROUND,
	ACT_PICKUP_RACK,
	ACT_IDLE_ANGRY,
	ACT_IDLE_RELAXED,
	ACT_IDLE_STIMULATED,
	ACT_IDLE_AGITATED,
	ACT_IDLE_STEALTH,
	ACT_IDLE_HURT,
	ACT_WALK_RELAXED,
	ACT_WALK_STIMULATED,
	ACT_WALK_AGITATED,
	ACT_WALK_STEALTH,
	ACT_RUN_RELAXED,
	ACT_RUN_STIMULATED,
	ACT_RUN_AGITATED,
	ACT_RUN_STEALTH,
	ACT_IDLE_AIM_RELAXED,
	ACT_IDLE_AIM_STIMULATED,
	ACT_IDLE_AIM_AGITATED,
	ACT_IDLE_AIM_STEALTH,
	ACT_WALK_AIM_RELAXED,
	ACT_WALK_AIM_STIMULATED,
	ACT_WALK_AIM_AGITATED,
	ACT_WALK_AIM_STEALTH,
	ACT_RUN_AIM_RELAXED,
	ACT_RUN_AIM_STIMULATED,
	ACT_RUN_AIM_AGITATED,
	ACT_RUN_AIM_STEALTH,
	ACT_CROUCHIDLE_STIMULATED,
	ACT_CROUCHIDLE_AIM_STIMULATED,
	ACT_CROUCHIDLE_AGITATED,
	ACT_WALK_HURT,
	ACT_RUN_HURT,
	ACT_SPECIAL_ATTACK1,
	ACT_SPECIAL_ATTACK2,
	ACT_COMBAT_IDLE,
	ACT_WALK_SCARED,
	ACT_RUN_SCARED,
	ACT_VICTORY_DANCE,
	ACT_DIE_HEADSHOT,
	ACT_DIE_CHESTSHOT,
	ACT_DIE_GUTSHOT,
	ACT_DIE_BACKSHOT,
	ACT_FLINCH_HEAD,
	ACT_FLINCH_CHEST,
	ACT_FLINCH_STOMACH,
	ACT_FLINCH_LEFTARM,
	ACT_FLINCH_RIGHTARM,
	ACT_FLINCH_LEFTLEG,
	ACT_FLINCH_RIGHTLEG,
	ACT_FLINCH_PHYSICS,
	ACT_FLINCH_HEAD_BACK,
	ACT_FLINCH_HEAD_LEFT,
	ACT_FLINCH_HEAD_RIGHT,
	ACT_FLINCH_CHEST_BACK,
	ACT_FLINCH_STOMACH_BACK,
	ACT_FLINCH_CROUCH_FRONT,
	ACT_FLINCH_CROUCH_BACK,
	ACT_FLINCH_CROUCH_LEFT,
	ACT_FLINCH_CROUCH_RIGHT,
	ACT_IDLE_ON_FIRE,
	ACT_WALK_ON_FIRE,
	ACT_RUN_ON_FIRE,
	ACT_RAPPEL_LOOP,
	ACT_180_LEFT,
	ACT_180_RIGHT,
	ACT_90_LEFT,
	ACT_90_RIGHT,
	ACT_STEP_LEFT,
	ACT_STEP_RIGHT,
	ACT_STEP_BACK,
	ACT_STEP_FORE,
	ACT_GESTURE_RANGE_ATTACK1,
	ACT_GESTURE_RANGE_ATTACK2,
	ACT_GESTURE_MELEE_ATTACK1,
	ACT_GESTURE_MELEE_ATTACK2,
	ACT_GESTURE_RANGE_ATTACK1_LOW,
	ACT_GESTURE_RANGE_ATTACK2_LOW,
	ACT_MELEE_ATTACK_SWING_GESTURE,
	ACT_GESTURE_SMALL_FLINCH,
	ACT_GESTURE_BIG_FLINCH,
	ACT_GESTURE_FLINCH_BLAST,
	ACT_GESTURE_FLINCH_BLAST_SHOTGUN,
	ACT_GESTURE_FLINCH_BLAST_DAMAGED,
	ACT_GESTURE_FLINCH_BLAST_DAMAGED_SHOTGUN,
	ACT_GESTURE_FLINCH_HEAD,
	ACT_GESTURE_FLINCH_CHEST,
	ACT_GESTURE_FLINCH_STOMACH,
	ACT_GESTURE_FLINCH_LEFTARM,
	ACT_GESTURE_FLINCH_RIGHTARM,
	ACT_GESTURE_FLINCH_LEFTLEG,
	ACT_GESTURE_FLINCH_RIGHTLEG,
	ACT_GESTURE_TURN_LEFT,
	ACT_GESTURE_TURN_RIGHT,
	ACT_GESTURE_TURN_LEFT45,
	ACT_GESTURE_TURN_RIGHT45,
	ACT_GESTURE_TURN_LEFT90,
	ACT_GESTURE_TURN_RIGHT90,
	ACT_GESTURE_TURN_LEFT45_FLAT,
	ACT_GESTURE_TURN_RIGHT45_FLAT,
	ACT_GESTURE_TURN_LEFT90_FLAT,
	ACT_GESTURE_TURN_RIGHT90_FLAT,
	ACT_BARNACLE_HIT,
	ACT_BARNACLE_PULL,
	ACT_BARNACLE_CHOMP,
	ACT_BARNACLE_CHEW,
	ACT_DO_NOT_DISTURB,
	ACT_SPECIFIC_SEQUENCE,
	ACT_VM_DRAW,
	ACT_VM_HOLSTER,
	ACT_VM_IDLE,
	ACT_VM_FIDGET,
	ACT_VM_PULLBACK,
	ACT_VM_PULLBACK_HIGH,
	ACT_VM_PULLBACK_LOW,
	ACT_VM_THROW,
	ACT_VM_PULLPIN,
	ACT_VM_PRIMARYATTACK,
	ACT_VM_SECONDARYATTACK,
	ACT_VM_RELOAD,
	ACT_VM_DRYFIRE,
	ACT_VM_HITLEFT,
	ACT_VM_HITLEFT2,
	ACT_VM_HITRIGHT,
	ACT_VM_HITRIGHT2,
	ACT_VM_HITCENTER,
	ACT_VM_HITCENTER2,
	ACT_VM_MISSLEFT,
	ACT_VM_MISSLEFT2,
	ACT_VM_MISSRIGHT,
	ACT_VM_MISSRIGHT2,
	ACT_VM_MISSCENTER,
	ACT_VM_MISSCENTER2,
	ACT_VM_HAULBACK,
	ACT_VM_SWINGHARD,
	ACT_VM_SWINGMISS,
	ACT_VM_SWINGHIT,
	ACT_VM_IDLE_TO_LOWERED,
	ACT_VM_IDLE_LOWERED,
	ACT_VM_LOWERED_TO_IDLE,
	ACT_VM_RECOIL1,
	ACT_VM_RECOIL2,
	ACT_VM_RECOIL3,
	ACT_VM_PICKUP,
	ACT_VM_RELEASE,
	ACT_VM_ATTACH_SILENCER,
	ACT_VM_DETACH_SILENCER,
	ACT_VM_EMPTY_FIRE,
	ACT_VM_EMPTY_RELOAD,
	ACT_VM_EMPTY_DRAW,
	ACT_VM_EMPTY_IDLE,
	ACT_SLAM_STICKWALL_IDLE,
	ACT_SLAM_STICKWALL_ND_IDLE,
	ACT_SLAM_STICKWALL_ATTACH,
	ACT_SLAM_STICKWALL_ATTACH2,
	ACT_SLAM_STICKWALL_ND_ATTACH,
	ACT_SLAM_STICKWALL_ND_ATTACH2,
	ACT_SLAM_STICKWALL_DETONATE,
	ACT_SLAM_STICKWALL_DETONATOR_HOLSTER,
	ACT_SLAM_STICKWALL_DRAW,
	ACT_SLAM_STICKWALL_ND_DRAW,
	ACT_SLAM_STICKWALL_TO_THROW,
	ACT_SLAM_STICKWALL_TO_THROW_ND,
	ACT_SLAM_STICKWALL_TO_TRIPMINE_ND,
	ACT_SLAM_THROW_IDLE,
	ACT_SLAM_THROW_ND_IDLE,
	ACT_SLAM_THROW_THROW,
	ACT_SLAM_THROW_THROW2,
	ACT_SLAM_THROW_THROW_ND,
	ACT_SLAM_THROW_THROW_ND2,
	ACT_SLAM_THROW_DRAW,
	ACT_SLAM_THROW_ND_DRAW,
	ACT_SLAM_THROW_TO_STICKWALL,
	ACT_SLAM_THROW_TO_STICKWALL_ND,
	ACT_SLAM_THROW_DETONATE,
	ACT_SLAM_THROW_DETONATOR_HOLSTER,
	ACT_SLAM_THROW_TO_TRIPMINE_ND,
	ACT_SLAM_TRIPMINE_IDLE,
	ACT_SLAM_TRIPMINE_DRAW,
	ACT_SLAM_TRIPMINE_ATTACH,
	ACT_SLAM_TRIPMINE_ATTACH2,
	ACT_SLAM_TRIPMINE_TO_STICKWALL_ND,
	ACT_SLAM_TRIPMINE_TO_THROW_ND,
	ACT_SLAM_DETONATOR_IDLE,
	ACT_SLAM_DETONATOR_DRAW,
	ACT_SLAM_DETONATOR_DETONATE,
	ACT_SLAM_DETONATOR_HOLSTER,
	ACT_SLAM_DETONATOR_STICKWALL_DRAW,
	ACT_SLAM_DETONATOR_THROW_DRAW,
	ACT_SHOTGUN_RELOAD_START,
	ACT_SHOTGUN_RELOAD_FINISH,
	ACT_SHOTGUN_PUMP,
	ACT_SMG2_IDLE2,
	ACT_SMG2_FIRE2,
	ACT_SMG2_DRAW2,
	ACT_SMG2_RELOAD2,
	ACT_SMG2_DRYFIRE2,
	ACT_SMG2_TOAUTO,
	ACT_SMG2_TOBURST,
	ACT_PHYSCANNON_UPGRADE,
	ACT_RANGE_ATTACK_AR1,
	ACT_RANGE_ATTACK_AR2,
	ACT_RANGE_ATTACK_AR2_LOW,
	ACT_RANGE_ATTACK_AR2_GRENADE,
	ACT_RANGE_ATTACK_HMG1,
	ACT_RANGE_ATTACK_ML,
	ACT_RANGE_ATTACK_SMG1,
	ACT_RANGE_ATTACK_SMG1_LOW,
	ACT_RANGE_ATTACK_SMG2,
	ACT_RANGE_ATTACK_SHOTGUN,
	ACT_RANGE_ATTACK_SHOTGUN_LOW,
	ACT_RANGE_ATTACK_PISTOL,
	ACT_RANGE_ATTACK_PISTOL_LOW,
	ACT_RANGE_ATTACK_SLAM,
	ACT_RANGE_ATTACK_TRIPWIRE,
	ACT_RANGE_ATTACK_THROW,
	ACT_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_RANGE_ATTACK_RPG,
	ACT_MELEE_ATTACK_SWING,
	ACT_RANGE_AIM_LOW,
	ACT_RANGE_AIM_SMG1_LOW,
	ACT_RANGE_AIM_PISTOL_LOW,
	ACT_RANGE_AIM_AR2_LOW,
	ACT_COVER_PISTOL_LOW,
	ACT_COVER_SMG1_LOW,
	ACT_GESTURE_RANGE_ATTACK_AR1,
	ACT_GESTURE_RANGE_ATTACK_AR2,
	ACT_GESTURE_RANGE_ATTACK_AR2_GRENADE,
	ACT_GESTURE_RANGE_ATTACK_HMG1,
	ACT_GESTURE_RANGE_ATTACK_ML,
	ACT_GESTURE_RANGE_ATTACK_SMG1,
	ACT_GESTURE_RANGE_ATTACK_SMG1_LOW,
	ACT_GESTURE_RANGE_ATTACK_SMG2,
	ACT_GESTURE_RANGE_ATTACK_SHOTGUN,
	ACT_GESTURE_RANGE_ATTACK_PISTOL,
	ACT_GESTURE_RANGE_ATTACK_PISTOL_LOW,
	ACT_GESTURE_RANGE_ATTACK_SLAM,
	ACT_GESTURE_RANGE_ATTACK_TRIPWIRE,
	ACT_GESTURE_RANGE_ATTACK_THROW,
	ACT_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_GESTURE_MELEE_ATTACK_SWING,
	ACT_IDLE_RIFLE,
	ACT_IDLE_SMG1,
	ACT_IDLE_ANGRY_SMG1,
	ACT_IDLE_PISTOL,
	ACT_IDLE_ANGRY_PISTOL,
	ACT_IDLE_ANGRY_SHOTGUN,
	ACT_IDLE_STEALTH_PISTOL,
	ACT_IDLE_PACKAGE,
	ACT_WALK_PACKAGE,
	ACT_IDLE_SUITCASE,
	ACT_WALK_SUITCASE,
	ACT_IDLE_SMG1_RELAXED,
	ACT_IDLE_SMG1_STIMULATED,
	ACT_WALK_RIFLE_RELAXED,
	ACT_RUN_RIFLE_RELAXED,
	ACT_WALK_RIFLE_STIMULATED,
	ACT_RUN_RIFLE_STIMULATED,
	ACT_IDLE_AIM_RIFLE_STIMULATED,
	ACT_WALK_AIM_RIFLE_STIMULATED,
	ACT_RUN_AIM_RIFLE_STIMULATED,
	ACT_IDLE_SHOTGUN_RELAXED,
	ACT_IDLE_SHOTGUN_STIMULATED,
	ACT_IDLE_SHOTGUN_AGITATED,
	ACT_WALK_ANGRY,
	ACT_POLICE_HARASS1,
	ACT_POLICE_HARASS2,
	ACT_IDLE_MANNEDGUN,
	ACT_IDLE_MELEE,
	ACT_IDLE_ANGRY_MELEE,
	ACT_IDLE_RPG_RELAXED,
	ACT_IDLE_RPG,
	ACT_IDLE_ANGRY_RPG,
	ACT_COVER_LOW_RPG,
	ACT_WALK_RPG,
	ACT_RUN_RPG,
	ACT_WALK_CROUCH_RPG,
	ACT_RUN_CROUCH_RPG,
	ACT_WALK_RPG_RELAXED,
	ACT_RUN_RPG_RELAXED,
	ACT_WALK_RIFLE,
	ACT_WALK_AIM_RIFLE,
	ACT_WALK_CROUCH_RIFLE,
	ACT_WALK_CROUCH_AIM_RIFLE,
	ACT_RUN_RIFLE,
	ACT_RUN_AIM_RIFLE,
	ACT_RUN_CROUCH_RIFLE,
	ACT_RUN_CROUCH_AIM_RIFLE,
	ACT_RUN_STEALTH_PISTOL,
	ACT_WALK_AIM_SHOTGUN,
	ACT_RUN_AIM_SHOTGUN,
	ACT_WALK_PISTOL,
	ACT_RUN_PISTOL,
	ACT_WALK_AIM_PISTOL,
	ACT_RUN_AIM_PISTOL,
	ACT_WALK_STEALTH_PISTOL,
	ACT_WALK_AIM_STEALTH_PISTOL,
	ACT_RUN_AIM_STEALTH_PISTOL,
	ACT_RELOAD_PISTOL,
	ACT_RELOAD_PISTOL_LOW,
	ACT_RELOAD_SMG1,
	ACT_RELOAD_SMG1_LOW,
	ACT_RELOAD_SHOTGUN,
	ACT_RELOAD_SHOTGUN_LOW,
	ACT_GESTURE_RELOAD,
	ACT_GESTURE_RELOAD_PISTOL,
	ACT_GESTURE_RELOAD_SMG1,
	ACT_GESTURE_RELOAD_SHOTGUN,
	ACT_BUSY_LEAN_LEFT,
	ACT_BUSY_LEAN_LEFT_ENTRY,
	ACT_BUSY_LEAN_LEFT_EXIT,
	ACT_BUSY_LEAN_BACK,
	ACT_BUSY_LEAN_BACK_ENTRY,
	ACT_BUSY_LEAN_BACK_EXIT,
	ACT_BUSY_SIT_GROUND,
	ACT_BUSY_SIT_GROUND_ENTRY,
	ACT_BUSY_SIT_GROUND_EXIT,
	ACT_BUSY_SIT_CHAIR,
	ACT_BUSY_SIT_CHAIR_ENTRY,
	ACT_BUSY_SIT_CHAIR_EXIT,
	ACT_BUSY_STAND,
	ACT_BUSY_QUEUE,
	ACT_DUCK_DODGE,
	ACT_DIE_BARNACLE_SWALLOW,
	ACT_GESTURE_BARNACLE_STRANGLE,
	ACT_PHYSCANNON_DETACH,
	ACT_PHYSCANNON_ANIMATE,
	ACT_PHYSCANNON_ANIMATE_PRE,
	ACT_PHYSCANNON_ANIMATE_POST,
	ACT_DIE_FRONTSIDE,
	ACT_DIE_RIGHTSIDE,
	ACT_DIE_BACKSIDE,
	ACT_DIE_LEFTSIDE,
	ACT_DIE_CROUCH_FRONTSIDE,
	ACT_DIE_CROUCH_RIGHTSIDE,
	ACT_DIE_CROUCH_BACKSIDE,
	ACT_DIE_CROUCH_LEFTSIDE,
	ACT_OPEN_DOOR,
	ACT_DI_ALYX_ZOMBIE_MELEE,
	ACT_DI_ALYX_ZOMBIE_TORSO_MELEE,
	ACT_DI_ALYX_HEADCRAB_MELEE,
	ACT_DI_ALYX_ANTLION,
	ACT_DI_ALYX_ZOMBIE_SHOTGUN64,
	ACT_DI_ALYX_ZOMBIE_SHOTGUN26,
	ACT_READINESS_RELAXED_TO_STIMULATED,
	ACT_READINESS_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_AGITATED_TO_STIMULATED,
	ACT_READINESS_STIMULATED_TO_RELAXED,
	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED,
	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_PISTOL_AGITATED_TO_STIMULATED,
	ACT_READINESS_PISTOL_STIMULATED_TO_RELAXED,
	ACT_IDLE_CARRY,
	ACT_WALK_CARRY,
	ACT_STARTDYING,
	ACT_DYINGLOOP,
	ACT_DYINGTODEAD,
	ACT_RIDE_MANNED_GUN,
	ACT_VM_SPRINT_ENTER,
	ACT_VM_SPRINT_IDLE,
	ACT_VM_SPRINT_LEAVE,
	ACT_FIRE_START,
	ACT_FIRE_LOOP,
	ACT_FIRE_END,
	ACT_CROUCHING_GRENADEIDLE,
	ACT_CROUCHING_GRENADEREADY,
	ACT_CROUCHING_PRIMARYATTACK,
	ACT_OVERLAY_GRENADEIDLE,
	ACT_OVERLAY_GRENADEREADY,
	ACT_OVERLAY_PRIMARYATTACK,
	ACT_OVERLAY_SHIELD_UP,
	ACT_OVERLAY_SHIELD_DOWN,
	ACT_OVERLAY_SHIELD_UP_IDLE,
	ACT_OVERLAY_SHIELD_ATTACK,
	ACT_OVERLAY_SHIELD_KNOCKBACK,
	ACT_SHIELD_UP,
	ACT_SHIELD_DOWN,
	ACT_SHIELD_UP_IDLE,
	ACT_SHIELD_ATTACK,
	ACT_SHIELD_KNOCKBACK,
	ACT_CROUCHING_SHIELD_UP,
	ACT_CROUCHING_SHIELD_DOWN,
	ACT_CROUCHING_SHIELD_UP_IDLE,
	ACT_CROUCHING_SHIELD_ATTACK,
	ACT_CROUCHING_SHIELD_KNOCKBACK,
	ACT_TURNRIGHT45,
	ACT_TURNLEFT45,
	ACT_TURN,
	ACT_OBJ_ASSEMBLING,
	ACT_OBJ_DISMANTLING,
	ACT_OBJ_STARTUP,
	ACT_OBJ_RUNNING,
	ACT_OBJ_IDLE,
	ACT_OBJ_PLACING,
	ACT_OBJ_DETERIORATING,
	ACT_OBJ_UPGRADING,
	ACT_DEPLOY,
	ACT_DEPLOY_IDLE,
	ACT_UNDEPLOY,
	ACT_CROSSBOW_DRAW_UNLOADED,
	ACT_GAUSS_SPINUP,
	ACT_GAUSS_SPINCYCLE,
	ACT_VM_PRIMARYATTACK_SILENCED,
	ACT_VM_RELOAD_SILENCED,
	ACT_VM_DRYFIRE_SILENCED,
	ACT_VM_IDLE_SILENCED,
	ACT_VM_DRAW_SILENCED,
	ACT_VM_IDLE_EMPTY_LEFT,
	ACT_VM_DRYFIRE_LEFT,
	ACT_VM_IS_DRAW,
	ACT_VM_IS_HOLSTER,
	ACT_VM_IS_IDLE,
	ACT_VM_IS_PRIMARYATTACK,
	ACT_PLAYER_IDLE_FIRE,
	ACT_PLAYER_CROUCH_FIRE,
	ACT_PLAYER_CROUCH_WALK_FIRE,
	ACT_PLAYER_WALK_FIRE,
	ACT_PLAYER_RUN_FIRE,
	ACT_IDLETORUN,
	ACT_RUNTOIDLE,
	ACT_VM_DRAW_DEPLOYED,
	ACT_HL2MP_IDLE_MELEE,
	ACT_HL2MP_RUN_MELEE,
	ACT_HL2MP_IDLE_CROUCH_MELEE,
	ACT_HL2MP_WALK_CROUCH_MELEE,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,
	ACT_HL2MP_GESTURE_RELOAD_MELEE,
	ACT_HL2MP_JUMP_MELEE,
	ACT_VM_FIZZLE,
	ACT_MP_STAND_IDLE,
	ACT_MP_CROUCH_IDLE,
	ACT_MP_CROUCH_DEPLOYED_IDLE,
	ACT_MP_CROUCH_DEPLOYED,
	ACT_MP_DEPLOYED_IDLE,
	ACT_MP_RUN,
	ACT_MP_WALK,
	ACT_MP_AIRWALK,
	ACT_MP_CROUCHWALK,
	ACT_MP_SPRINT,
	ACT_MP_JUMP,
	ACT_MP_JUMP_START,
	ACT_MP_JUMP_FLOAT,
	ACT_MP_JUMP_LAND,
	ACT_MP_JUMP_IMPACT_N,
	ACT_MP_JUMP_IMPACT_E,
	ACT_MP_JUMP_IMPACT_W,
	ACT_MP_JUMP_IMPACT_S,
	ACT_MP_JUMP_IMPACT_TOP,
	ACT_MP_DOUBLEJUMP,
	ACT_MP_SWIM,
	ACT_MP_DEPLOYED,
	ACT_MP_SWIM_DEPLOYED,
	ACT_MP_VCD,
	ACT_MP_ATTACK_STAND_PRIMARYFIRE,
	ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_STAND_SECONDARYFIRE,
	ACT_MP_ATTACK_STAND_GRENADE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_SECONDARYFIRE,
	ACT_MP_ATTACK_CROUCH_GRENADE,
	ACT_MP_ATTACK_SWIM_PRIMARYFIRE,
	ACT_MP_ATTACK_SWIM_SECONDARYFIRE,
	ACT_MP_ATTACK_SWIM_GRENADE,
	ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,
	ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE,
	ACT_MP_ATTACK_AIRWALK_GRENADE,
	ACT_MP_RELOAD_STAND,
	ACT_MP_RELOAD_STAND_LOOP,
	ACT_MP_RELOAD_STAND_END,
	ACT_MP_RELOAD_CROUCH,
	ACT_MP_RELOAD_CROUCH_LOOP,
	ACT_MP_RELOAD_CROUCH_END,
	ACT_MP_RELOAD_SWIM,
	ACT_MP_RELOAD_SWIM_LOOP,
	ACT_MP_RELOAD_SWIM_END,
	ACT_MP_RELOAD_AIRWALK,
	ACT_MP_RELOAD_AIRWALK_LOOP,
	ACT_MP_RELOAD_AIRWALK_END,
	ACT_MP_ATTACK_STAND_PREFIRE,
	ACT_MP_ATTACK_STAND_POSTFIRE,
	ACT_MP_ATTACK_STAND_STARTFIRE,
	ACT_MP_ATTACK_CROUCH_PREFIRE,
	ACT_MP_ATTACK_CROUCH_POSTFIRE,
	ACT_MP_ATTACK_SWIM_PREFIRE,
	ACT_MP_ATTACK_SWIM_POSTFIRE,
	ACT_MP_STAND_PRIMARY,
	ACT_MP_CROUCH_PRIMARY,
	ACT_MP_RUN_PRIMARY,
	ACT_MP_WALK_PRIMARY,
	ACT_MP_AIRWALK_PRIMARY,
	ACT_MP_CROUCHWALK_PRIMARY,
	ACT_MP_JUMP_PRIMARY,
	ACT_MP_JUMP_START_PRIMARY,
	ACT_MP_JUMP_FLOAT_PRIMARY,
	ACT_MP_JUMP_LAND_PRIMARY,
	ACT_MP_SWIM_PRIMARY,
	ACT_MP_DEPLOYED_PRIMARY,
	ACT_MP_SWIM_DEPLOYED_PRIMARY,
	ACT_MP_ATTACK_STAND_PRIMARY,
	ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_PRIMARY,
	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_SWIM_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_PRIMARY,
	ACT_MP_RELOAD_STAND_PRIMARY,
	ACT_MP_RELOAD_STAND_PRIMARY_LOOP,
	ACT_MP_RELOAD_STAND_PRIMARY_END,
	ACT_MP_RELOAD_CROUCH_PRIMARY,
	ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP,
	ACT_MP_RELOAD_CROUCH_PRIMARY_END,
	ACT_MP_RELOAD_SWIM_PRIMARY,
	ACT_MP_RELOAD_SWIM_PRIMARY_LOOP,
	ACT_MP_RELOAD_SWIM_PRIMARY_END,
	ACT_MP_RELOAD_AIRWALK_PRIMARY,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_END,
	ACT_MP_ATTACK_STAND_GRENADE_PRIMARY,
	ACT_MP_ATTACK_CROUCH_GRENADE_PRIMARY,
	ACT_MP_ATTACK_SWIM_GRENADE_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_PRIMARY,
	ACT_MP_STAND_SECONDARY,
	ACT_MP_CROUCH_SECONDARY,
	ACT_MP_RUN_SECONDARY,
	ACT_MP_WALK_SECONDARY,
	ACT_MP_AIRWALK_SECONDARY,
	ACT_MP_CROUCHWALK_SECONDARY,
	ACT_MP_JUMP_SECONDARY,
	ACT_MP_JUMP_START_SECONDARY,
	ACT_MP_JUMP_FLOAT_SECONDARY,
	ACT_MP_JUMP_LAND_SECONDARY,
	ACT_MP_SWIM_SECONDARY,
	ACT_MP_ATTACK_STAND_SECONDARY,
	ACT_MP_ATTACK_CROUCH_SECONDARY,
	ACT_MP_ATTACK_SWIM_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_SECONDARY,
	ACT_MP_RELOAD_STAND_SECONDARY,
	ACT_MP_RELOAD_STAND_SECONDARY_LOOP,
	ACT_MP_RELOAD_STAND_SECONDARY_END,
	ACT_MP_RELOAD_CROUCH_SECONDARY,
	ACT_MP_RELOAD_CROUCH_SECONDARY_LOOP,
	ACT_MP_RELOAD_CROUCH_SECONDARY_END,
	ACT_MP_RELOAD_SWIM_SECONDARY,
	ACT_MP_RELOAD_SWIM_SECONDARY_LOOP,
	ACT_MP_RELOAD_SWIM_SECONDARY_END,
	ACT_MP_RELOAD_AIRWALK_SECONDARY,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_END,
	ACT_MP_ATTACK_STAND_GRENADE_SECONDARY,
	ACT_MP_ATTACK_CROUCH_GRENADE_SECONDARY,
	ACT_MP_ATTACK_SWIM_GRENADE_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_SECONDARY,
	ACT_MP_STAND_MELEE,
	ACT_MP_CROUCH_MELEE,
	ACT_MP_RUN_MELEE,
	ACT_MP_WALK_MELEE,
	ACT_MP_AIRWALK_MELEE,
	ACT_MP_CROUCHWALK_MELEE,
	ACT_MP_JUMP_MELEE,
	ACT_MP_JUMP_START_MELEE,
	ACT_MP_JUMP_FLOAT_MELEE,
	ACT_MP_JUMP_LAND_MELEE,
	ACT_MP_SWIM_MELEE,
	ACT_MP_ATTACK_STAND_MELEE,
	ACT_MP_ATTACK_STAND_MELEE_SECONDARY,
	ACT_MP_ATTACK_CROUCH_MELEE,
	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY,
	ACT_MP_ATTACK_SWIM_MELEE,
	ACT_MP_ATTACK_AIRWALK_MELEE,
	ACT_MP_ATTACK_STAND_GRENADE_MELEE,
	ACT_MP_ATTACK_CROUCH_GRENADE_MELEE,
	ACT_MP_ATTACK_SWIM_GRENADE_MELEE,
	ACT_MP_ATTACK_AIRWALK_GRENADE_MELEE,
	ACT_MP_STAND_ITEM1,
	ACT_MP_CROUCH_ITEM1,
	ACT_MP_RUN_ITEM1,
	ACT_MP_WALK_ITEM1,
	ACT_MP_AIRWALK_ITEM1,
	ACT_MP_CROUCHWALK_ITEM1,
	ACT_MP_JUMP_ITEM1,
	ACT_MP_JUMP_START_ITEM1,
	ACT_MP_JUMP_FLOAT_ITEM1,
	ACT_MP_JUMP_LAND_ITEM1,
	ACT_MP_SWIM_ITEM1,
	ACT_MP_ATTACK_STAND_ITEM1,
	ACT_MP_ATTACK_STAND_ITEM1_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM1,
	ACT_MP_ATTACK_CROUCH_ITEM1_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM1,
	ACT_MP_ATTACK_AIRWALK_ITEM1,
	ACT_MP_STAND_ITEM2,
	ACT_MP_CROUCH_ITEM2,
	ACT_MP_RUN_ITEM2,
	ACT_MP_WALK_ITEM2,
	ACT_MP_AIRWALK_ITEM2,
	ACT_MP_CROUCHWALK_ITEM2,
	ACT_MP_JUMP_ITEM2,
	ACT_MP_JUMP_START_ITEM2,
	ACT_MP_JUMP_FLOAT_ITEM2,
	ACT_MP_JUMP_LAND_ITEM2,
	ACT_MP_SWIM_ITEM2,
	ACT_MP_ATTACK_STAND_ITEM2,
	ACT_MP_ATTACK_STAND_ITEM2_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM2,
	ACT_MP_ATTACK_CROUCH_ITEM2_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM2,
	ACT_MP_ATTACK_AIRWALK_ITEM2,
	ACT_MP_GESTURE_FLINCH,
	ACT_MP_GESTURE_FLINCH_PRIMARY,
	ACT_MP_GESTURE_FLINCH_SECONDARY,
	ACT_MP_GESTURE_FLINCH_MELEE,
	ACT_MP_GESTURE_FLINCH_ITEM1,
	ACT_MP_GESTURE_FLINCH_ITEM2,
	ACT_MP_GESTURE_FLINCH_HEAD,
	ACT_MP_GESTURE_FLINCH_CHEST,
	ACT_MP_GESTURE_FLINCH_STOMACH,
	ACT_MP_GESTURE_FLINCH_LEFTARM,
	ACT_MP_GESTURE_FLINCH_RIGHTARM,
	ACT_MP_GESTURE_FLINCH_LEFTLEG,
	ACT_MP_GESTURE_FLINCH_RIGHTLEG,
	ACT_MP_GRENADE1_DRAW,
	ACT_MP_GRENADE1_IDLE,
	ACT_MP_GRENADE1_ATTACK,
	ACT_MP_GRENADE2_DRAW,
	ACT_MP_GRENADE2_IDLE,
	ACT_MP_GRENADE2_ATTACK,
	ACT_MP_PRIMARY_GRENADE1_DRAW,
	ACT_MP_PRIMARY_GRENADE1_IDLE,
	ACT_MP_PRIMARY_GRENADE1_ATTACK,
	ACT_MP_PRIMARY_GRENADE2_DRAW,
	ACT_MP_PRIMARY_GRENADE2_IDLE,
	ACT_MP_PRIMARY_GRENADE2_ATTACK,
	ACT_MP_SECONDARY_GRENADE1_DRAW,
	ACT_MP_SECONDARY_GRENADE1_IDLE,
	ACT_MP_SECONDARY_GRENADE1_ATTACK,
	ACT_MP_SECONDARY_GRENADE2_DRAW,
	ACT_MP_SECONDARY_GRENADE2_IDLE,
	ACT_MP_SECONDARY_GRENADE2_ATTACK,
	ACT_MP_MELEE_GRENADE1_DRAW,
	ACT_MP_MELEE_GRENADE1_IDLE,
	ACT_MP_MELEE_GRENADE1_ATTACK,
	ACT_MP_MELEE_GRENADE2_DRAW,
	ACT_MP_MELEE_GRENADE2_IDLE,
	ACT_MP_MELEE_GRENADE2_ATTACK,
	ACT_MP_ITEM1_GRENADE1_DRAW,
	ACT_MP_ITEM1_GRENADE1_IDLE,
	ACT_MP_ITEM1_GRENADE1_ATTACK,
	ACT_MP_ITEM1_GRENADE2_DRAW,
	ACT_MP_ITEM1_GRENADE2_IDLE,
	ACT_MP_ITEM1_GRENADE2_ATTACK,
	ACT_MP_ITEM2_GRENADE1_DRAW,
	ACT_MP_ITEM2_GRENADE1_IDLE,
	ACT_MP_ITEM2_GRENADE1_ATTACK,
	ACT_MP_ITEM2_GRENADE2_DRAW,
	ACT_MP_ITEM2_GRENADE2_IDLE,
	ACT_MP_ITEM2_GRENADE2_ATTACK,
	ACT_MP_STAND_BUILDING,
	ACT_MP_CROUCH_BUILDING,
	ACT_MP_RUN_BUILDING,
	ACT_MP_WALK_BUILDING,
	ACT_MP_AIRWALK_BUILDING,
	ACT_MP_CROUCHWALK_BUILDING,
	ACT_MP_JUMP_BUILDING,
	ACT_MP_JUMP_START_BUILDING,
	ACT_MP_JUMP_FLOAT_BUILDING,
	ACT_MP_JUMP_LAND_BUILDING,
	ACT_MP_SWIM_BUILDING,
	ACT_MP_ATTACK_STAND_BUILDING,
	ACT_MP_ATTACK_CROUCH_BUILDING,
	ACT_MP_ATTACK_SWIM_BUILDING,
	ACT_MP_ATTACK_AIRWALK_BUILDING,
	ACT_MP_ATTACK_STAND_GRENADE_BUILDING,
	ACT_MP_ATTACK_CROUCH_GRENADE_BUILDING,
	ACT_MP_ATTACK_SWIM_GRENADE_BUILDING,
	ACT_MP_ATTACK_AIRWALK_GRENADE_BUILDING,
	ACT_MP_STAND_PDA,
	ACT_MP_CROUCH_PDA,
	ACT_MP_RUN_PDA,
	ACT_MP_WALK_PDA,
	ACT_MP_AIRWALK_PDA,
	ACT_MP_CROUCHWALK_PDA,
	ACT_MP_JUMP_PDA,
	ACT_MP_JUMP_START_PDA,
	ACT_MP_JUMP_FLOAT_PDA,
	ACT_MP_JUMP_LAND_PDA,
	ACT_MP_SWIM_PDA,
	ACT_MP_ATTACK_STAND_PDA,
	ACT_MP_ATTACK_SWIM_PDA,
	ACT_MP_GESTURE_VC_HANDMOUTH,
	ACT_MP_GESTURE_VC_FINGERPOINT,
	ACT_MP_GESTURE_VC_FISTPUMP,
	ACT_MP_GESTURE_VC_THUMBSUP,
	ACT_MP_GESTURE_VC_NODYES,
	ACT_MP_GESTURE_VC_NODNO,
	ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY,
	ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY,
	ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY,
	ACT_MP_GESTURE_VC_NODYES_PRIMARY,
	ACT_MP_GESTURE_VC_NODNO_PRIMARY,
	ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY,
	ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY,
	ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY,
	ACT_MP_GESTURE_VC_NODYES_SECONDARY,
	ACT_MP_GESTURE_VC_NODNO_SECONDARY,
	ACT_MP_GESTURE_VC_HANDMOUTH_MELEE,
	ACT_MP_GESTURE_VC_FINGERPOINT_MELEE,
	ACT_MP_GESTURE_VC_FISTPUMP_MELEE,
	ACT_MP_GESTURE_VC_THUMBSUP_MELEE,
	ACT_MP_GESTURE_VC_NODYES_MELEE,
	ACT_MP_GESTURE_VC_NODNO_MELEE,
	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM1,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM1,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM1,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM1,
	ACT_MP_GESTURE_VC_NODYES_ITEM1,
	ACT_MP_GESTURE_VC_NODNO_ITEM1,
	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM2,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM2,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM2,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM2,
	ACT_MP_GESTURE_VC_NODYES_ITEM2,
	ACT_MP_GESTURE_VC_NODNO_ITEM2,
	ACT_MP_GESTURE_VC_HANDMOUTH_BUILDING,
	ACT_MP_GESTURE_VC_FINGERPOINT_BUILDING,
	ACT_MP_GESTURE_VC_FISTPUMP_BUILDING,
	ACT_MP_GESTURE_VC_THUMBSUP_BUILDING,
	ACT_MP_GESTURE_VC_NODYES_BUILDING,
	ACT_MP_GESTURE_VC_NODNO_BUILDING,
	ACT_MP_GESTURE_VC_HANDMOUTH_PDA,
	ACT_MP_GESTURE_VC_FINGERPOINT_PDA,
	ACT_MP_GESTURE_VC_FISTPUMP_PDA,
	ACT_MP_GESTURE_VC_THUMBSUP_PDA,
	ACT_MP_GESTURE_VC_NODYES_PDA,
	ACT_MP_GESTURE_VC_NODNO_PDA,
	ACT_VM_UNUSABLE,
	ACT_VM_UNUSABLE_TO_USABLE,
	ACT_VM_USABLE_TO_UNUSABLE,
	ACT_PRIMARY_VM_DRAW,
	ACT_PRIMARY_VM_HOLSTER,
	ACT_PRIMARY_VM_IDLE,
	ACT_PRIMARY_VM_PULLBACK,
	ACT_PRIMARY_VM_PRIMARYATTACK,
	ACT_PRIMARY_VM_SECONDARYATTACK,
	ACT_PRIMARY_VM_RELOAD,
	ACT_PRIMARY_VM_DRYFIRE,
	ACT_PRIMARY_VM_IDLE_TO_LOWERED,
	ACT_PRIMARY_VM_IDLE_LOWERED,
	ACT_PRIMARY_VM_LOWERED_TO_IDLE,
	ACT_SECONDARY_VM_DRAW,
	ACT_SECONDARY_VM_HOLSTER,
	ACT_SECONDARY_VM_IDLE,
	ACT_SECONDARY_VM_PULLBACK,
	ACT_SECONDARY_VM_PRIMARYATTACK,
	ACT_SECONDARY_VM_SECONDARYATTACK,
	ACT_SECONDARY_VM_RELOAD,
	ACT_SECONDARY_VM_DRYFIRE,
	ACT_SECONDARY_VM_IDLE_TO_LOWERED,
	ACT_SECONDARY_VM_IDLE_LOWERED,
	ACT_SECONDARY_VM_LOWERED_TO_IDLE,
	ACT_MELEE_VM_DRAW,
	ACT_MELEE_VM_HOLSTER,
	ACT_MELEE_VM_IDLE,
	ACT_MELEE_VM_PULLBACK,
	ACT_MELEE_VM_PRIMARYATTACK,
	ACT_MELEE_VM_SECONDARYATTACK,
	ACT_MELEE_VM_RELOAD,
	ACT_MELEE_VM_DRYFIRE,
	ACT_MELEE_VM_IDLE_TO_LOWERED,
	ACT_MELEE_VM_IDLE_LOWERED,
	ACT_MELEE_VM_LOWERED_TO_IDLE,
	ACT_PDA_VM_DRAW,
	ACT_PDA_VM_HOLSTER,
	ACT_PDA_VM_IDLE,
	ACT_PDA_VM_PULLBACK,
	ACT_PDA_VM_PRIMARYATTACK,
	ACT_PDA_VM_SECONDARYATTACK,
	ACT_PDA_VM_RELOAD,
	ACT_PDA_VM_DRYFIRE,
	ACT_PDA_VM_IDLE_TO_LOWERED,
	ACT_PDA_VM_IDLE_LOWERED,
	ACT_PDA_VM_LOWERED_TO_IDLE,
	ACT_ITEM1_VM_DRAW,
	ACT_ITEM1_VM_HOLSTER,
	ACT_ITEM1_VM_IDLE,
	ACT_ITEM1_VM_PULLBACK,
	ACT_ITEM1_VM_PRIMARYATTACK,
	ACT_ITEM1_VM_SECONDARYATTACK,
	ACT_ITEM1_VM_RELOAD,
	ACT_ITEM1_VM_DRYFIRE,
	ACT_ITEM1_VM_IDLE_TO_LOWERED,
	ACT_ITEM1_VM_IDLE_LOWERED,
	ACT_ITEM1_VM_LOWERED_TO_IDLE,
	ACT_ITEM2_VM_DRAW,
	ACT_ITEM2_VM_HOLSTER,
	ACT_ITEM2_VM_IDLE,
	ACT_ITEM2_VM_PULLBACK,
	ACT_ITEM2_VM_PRIMARYATTACK,
	ACT_ITEM2_VM_SECONDARYATTACK,
	ACT_ITEM2_VM_RELOAD,
	ACT_ITEM2_VM_DRYFIRE,
	ACT_ITEM2_VM_IDLE_TO_LOWERED,
	ACT_ITEM2_VM_IDLE_LOWERED,
	ACT_ITEM2_VM_LOWERED_TO_IDLE,
	ACT_RELOAD_SUCCEED,
	ACT_RELOAD_FAIL,
	ACT_WALK_AIM_AUTOGUN,
	ACT_RUN_AIM_AUTOGUN,
	ACT_IDLE_AUTOGUN,
	ACT_IDLE_AIM_AUTOGUN,
	ACT_RELOAD_AUTOGUN,
	ACT_CROUCH_IDLE_AUTOGUN,
	ACT_RANGE_ATTACK_AUTOGUN,
	ACT_JUMP_AUTOGUN,
	ACT_IDLE_AIM_PISTOL,
	ACT_WALK_AIM_DUAL,
	ACT_RUN_AIM_DUAL,
	ACT_IDLE_DUAL,
	ACT_IDLE_AIM_DUAL,
	ACT_RELOAD_DUAL,
	ACT_CROUCH_IDLE_DUAL,
	ACT_RANGE_ATTACK_DUAL,
	ACT_JUMP_DUAL,
	ACT_IDLE_SHOTGUN,
	ACT_IDLE_AIM_SHOTGUN,
	ACT_CROUCH_IDLE_SHOTGUN,
	ACT_JUMP_SHOTGUN,
	ACT_IDLE_AIM_RIFLE,
	ACT_RELOAD_RIFLE,
	ACT_CROUCH_IDLE_RIFLE,
	ACT_RANGE_ATTACK_RIFLE,
	ACT_JUMP_RIFLE,
	ACT_SLEEP,
	ACT_WAKE,
	ACT_FLICK_LEFT,
	ACT_FLICK_LEFT_MIDDLE,
	ACT_FLICK_RIGHT_MIDDLE,
	ACT_FLICK_RIGHT,
	ACT_SPINAROUND,
	ACT_PREP_TO_FIRE,
	ACT_FIRE,
	ACT_FIRE_RECOVER,
	ACT_SPRAY,
	ACT_PREP_EXPLODE,
	ACT_EXPLODE,
	ACT_DOTA_IDLE,
	ACT_DOTA_RUN,
	ACT_DOTA_ATTACK,
	ACT_DOTA_ATTACK_EVENT,
	ACT_DOTA_DIE,
	ACT_DOTA_FLINCH,
	ACT_DOTA_DISABLED,
	ACT_DOTA_CAST_ABILITY_1,
	ACT_DOTA_CAST_ABILITY_2,
	ACT_DOTA_CAST_ABILITY_3,
	ACT_DOTA_CAST_ABILITY_4,
	ACT_DOTA_OVERRIDE_ABILITY_1,
	ACT_DOTA_OVERRIDE_ABILITY_2,
	ACT_DOTA_OVERRIDE_ABILITY_3,
	ACT_DOTA_OVERRIDE_ABILITY_4,
	ACT_DOTA_CHANNEL_ABILITY_1,
	ACT_DOTA_CHANNEL_ABILITY_2,
	ACT_DOTA_CHANNEL_ABILITY_3,
	ACT_DOTA_CHANNEL_ABILITY_4,
	ACT_DOTA_CHANNEL_END_ABILITY_1,
	ACT_DOTA_CHANNEL_END_ABILITY_2,
	ACT_DOTA_CHANNEL_END_ABILITY_3,
	ACT_DOTA_CHANNEL_END_ABILITY_4,
	ACT_MP_RUN_SPEEDPAINT,
	ACT_MP_LONG_FALL,
	ACT_MP_TRACTORBEAM_FLOAT,
	ACT_MP_DEATH_CRUSH,
	ACT_MP_RUN_SPEEDPAINT_PRIMARY,
	ACT_MP_DROWNING_PRIMARY,
	ACT_MP_LONG_FALL_PRIMARY,
	ACT_MP_TRACTORBEAM_FLOAT_PRIMARY,
	ACT_MP_DEATH_CRUSH_PRIMARY,
	ACT_DIE_STAND,
	ACT_DIE_STAND_HEADSHOT,
	ACT_DIE_CROUCH,
	ACT_DIE_CROUCH_HEADSHOT,
	ACT_CSGO_NULL,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM,
};

enum PlayerFlags_t : int {
	FL_ONGROUND = (1 << 0),
	FL_DUCKING = (1 << 1),
	FL_WATERJUMP = (1 << 3),
	FL_ONTRAIN = (1 << 4),
	FL_INRAIN = (1 << 5),
	FL_FROZEN = (1 << 6),
	FL_ATCONTROLS = (1 << 7),
	FL_CLIENT = (1 << 8),
	FL_FAKECLIENT = (1 << 9),
	FL_INWATER = (1 << 10),
};

enum MoveType_t : int {
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4,
};

enum Weapons_t : int {
	DEAGLE = 1,
	ELITE = 2,
	FIVESEVEN = 3,
	GLOCK = 4,
	AK47 = 7,
	AUG = 8,
	AWP = 9,
	FAMAS = 10,
	G3SG1 = 11,
	GALIL = 13,
	M249 = 14,
	M4A4 = 16,
	MAC10 = 17,
	P90 = 19,
	UMP45 = 24,
	XM1014 = 25,
	BIZON = 26,
	MAG7 = 27,
	NEGEV = 28,
	SAWEDOFF = 29,
	TEC9 = 30,
	ZEUS = 31,
	P2000 = 32,
	MP7 = 33,
	MP9 = 34,
	NOVA = 35,
	P250 = 36,
	SCAR20 = 38,
	SG553 = 39,
	SSG08 = 40,
	KNIFE_T = 42,
	FLASHBANG = 43,
	HEGRENADE = 44,
	SMOKE = 45,
	MOLOTOV = 46,
	DECOY = 47,
	FIREBOMB = 48,
	C4 = 49,
	MUSICKIT = 58,
	KNIFE_CT = 59,
	M4A1S = 60,
	USPS = 61,
	TRADEUPCONTRACT = 62,
	CZ75A = 63,
	REVOLVER = 64,
	KNIFE_BAYONET = 500,
	KNIFE_FLIP = 505,
	KNIFE_GUT = 506,
	KNIFE_KARAMBIT = 507,
	KNIFE_M9_BAYONET = 508,
	KNIFE_HUNTSMAN = 509,
	KNIFE_FALCHION = 512,
	KNIFE_BOWIE = 514,
	KNIFE_BUTTERFLY = 515,
	KNIFE_SHADOW_DAGGERS = 516,
};

struct RenderableInstance_t {
	uint8_t m_alpha;
	__forceinline RenderableInstance_t() : m_alpha{ 255ui8 } {}
};

class IClientUnknown;
class IHandleEntity {
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const ulong_t& GetRefEHandle() const = 0;
};

enum SolidType_t {
	solid_none = 0,
	solid_bsp,
	solid_bbox,
	solid_obb,
	solid_obb_yaw,
	solid_custom,
	solid_vphysics,
	solid_last
};


class IClientCollideable {
public:
	virtual IHandleEntity* GetEntityHandle() = 0;
	virtual vec3_t& OBBMins() const = 0;
	virtual vec3_t& OBBMaxs() const = 0;
	virtual void WorldSpaecTriggerBounds(vec3_t* world_mins, vec3_t* world_maxs) const = 0;
	virtual bool TestCollision(const Ray& ray, unsigned int contents_mask, CGameTrace& tr) = 0;
	virtual bool TestHitboxes(const Ray& ray, unsigned int contents_mask, CGameTrace& tr) = 0;
	virtual int GetCollisionModelIndex() = 0;
	virtual const model_t* GetCollisionModel() = 0;
	virtual vec3_t& GetCollisionOrigin() const = 0;
	virtual ang_t& GetCollisionAngles() const = 0;
	virtual const matrix3x4_t& CollisionToWorldTransform() const = 0;
	virtual SolidType_t GetSolid() const = 0;
	virtual int GetSolidFlags() const = 0;
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual int GetCollisionGroup() const = 0;
	virtual void WorldSpaceSurroundingBounds(vec3_t* mins, vec3_t* maxs) = 0;
	virtual unsigned int GetRequiredTriggerFlags() const = 0;
	virtual const matrix3x4_t* GetRootParentToWorldTransform() const = 0;
	virtual void* GetVPhysicsObject() const = 0;
};


class IClientUnknown : public IHandleEntity {
public:
	virtual IClientCollideable* GetCollideable() = 0;
	virtual void* GetClientNetworkable() = 0;
	virtual void* GetClientRenderable() = 0;
	virtual void* GetIClientEntity() = 0;
	virtual Entity* GetBaseEntity() = 0;
	virtual void* GetClientThinkable() = 0;
	virtual void* GetClientAlphaProperty() = 0;
};

class Entity : IClientUnknown {
public:
	// helper methods.
	template< typename t >
	__forceinline t &get(size_t offset) {
		return *(t *)((uintptr_t)this + offset);
	}

	template< typename t >
	__forceinline void set(size_t offset, const t &val) {
		*(t *)((uintptr_t)this + offset) = val;
	}

	template< typename t >
	__forceinline t as() {
		return (t)this;
	}

public:
	__forceinline matrix3x4_t& m_hCoordFrame() {
		return get< matrix3x4_t >(g_entoffsets.coordinate_frame);
	}

	// netvars / etc.
	__forceinline vec3_t &m_vecOrigin() {
		return get< vec3_t >(g_entoffsets.m_vecOrigin);
	}

	__forceinline vec3_t &m_vecOldOrigin() {
		return get< vec3_t >(g_entoffsets.m_vecOldOrigin);
	}

	__forceinline vec3_t &m_vecVelocity() {
		return get< vec3_t >(g_entoffsets.m_vecVelocity);
	}

	__forceinline matrix3x4_t& m_pCoordFrame() {
		return get< matrix3x4_t >(g_entoffsets.m_pCoordFrame);
	}


	__forceinline vec3_t &m_vecMins() {
		return get< vec3_t >(g_entoffsets.m_vecMins);
	}

	__forceinline vec3_t &m_vecMaxs() {
		return get< vec3_t >(g_entoffsets.m_vecMaxs);
	}

	__forceinline int &m_iTeamNum() {
		return get< int >(g_entoffsets.m_iTeamNum);
	}

	__forceinline int &m_nSequence() {
		return get< int >(g_entoffsets.m_nSequence);
	}

	__forceinline float &m_flCycle() {
		return get< float >(g_entoffsets.m_flCycle);
	}

	__forceinline float &m_flC4Blow() {
		return get< float >(g_entoffsets.m_flC4Blow);
	}

	__forceinline bool &m_bBombTicking() {
		return get< bool >(g_entoffsets.m_bBombTicking);
	}

	__forceinline int &m_fEffects() {
		// todo; netvar.
		return get< int >(g_entoffsets.m_fEffects);
	}

	__forceinline bool& m_bIsWalking()
	{
		return get< bool >(g_entoffsets.m_bIsWalking);
	}

	__forceinline BYTE& m_ubEFNoInterpParity()
	{
		return *(BYTE*)(uintptr_t(this) + g_entoffsets.m_ubEFNoInterpParity);
	}

	__forceinline BYTE& m_ubEFNoInterpParityOld()
	{
		return *(BYTE*)(uintptr_t(this) + g_entoffsets.m_ubEFNoInterpParity + 1);
	}

	__forceinline int &m_nModelIndex() {
		return get< int >(g_entoffsets.m_nModelIndex);
	}

	__forceinline bool &m_bReadyToDraw() {
		return get< bool >(g_entoffsets.m_bReadyToDraw);
	}

public:
	// virtual indices
	enum indices : size_t {
		WORLDSPACECENTER = 78,
		GETMAXHEALTH = 122,
		ISPLAYER = 152,
		ISBASECOMBATWEAPON = 160,
	};

public:
	// virtuals.
	// renderable table.
	__forceinline void *renderable() {
		return (void *)((uintptr_t)this + 0x4);
	}

	__forceinline datamap_t* get_pred_desc_map()
	{
		using fn_t = datamap_t* (__thiscall*)(decltype(this));

		return (*reinterpret_cast<fn_t**> (this))[17u](this);
	}

	__forceinline vec3_t &GetRenderOrigin() {
		return util::get_method< vec3_t &(__thiscall *)(void *) >(renderable(), 1)(renderable());
	}

	__forceinline ang_t &GetRenderAngles() {
		return util::get_method< ang_t &(__thiscall *)(void *) >(renderable(), 2)(renderable());
	}

	__forceinline const model_t *GetModel() {
		return util::get_method< const model_t *(__thiscall *)(void *) >(renderable(), 8)(renderable());
	}

	__forceinline std::uint16_t mdl_instance() {
		return util::get_method< std::uint16_t(__thiscall*)(void*) >(renderable(), 30)(renderable());
	}

	__forceinline void DrawModel(int flags = STUDIO_RENDER, const RenderableInstance_t &instance = {}) {
		return util::get_method< void(__thiscall *)(void *, int, const RenderableInstance_t &)>(renderable(), 9)(renderable(), flags, instance);
	}

	__forceinline bool SetupBones(matrix3x4_t *out, int max, int mask, float time) {
		return util::get_method< bool(__thiscall *)(void *, matrix3x4_t *, int, int, float)>(renderable(), 13)(renderable(), out, max, mask, time);
	}


	// networkable table.
	__forceinline void *networkable() {
		return (void *)((uintptr_t)this + 0x8);
	}

	__forceinline void Release() {
		return util::get_method< void(__thiscall *)(void *) >(networkable(), 1)(networkable());
	}

	__forceinline ClientClass *GetClientClass() {
		return util::get_method< ClientClass *(__thiscall *)(void *) >(networkable(), 2)(networkable());
	}

	__forceinline void OnDataChanged(DataUpdateType_t type) {
		return util::get_method< void(__thiscall *)(void *, DataUpdateType_t) >(networkable(), 5)(networkable(), type);
	}

	__forceinline void PreDataUpdate(DataUpdateType_t type) {
		return util::get_method< void(__thiscall *)(void *, DataUpdateType_t) >(networkable(), 6)(networkable(), type);
	}

	__forceinline void PostDataUpdate(DataUpdateType_t type) {
		return util::get_method< void(__thiscall *)(void *, DataUpdateType_t) >(networkable(), 7)(networkable(), type);
	}

	__forceinline bool dormant() {
		return util::get_method< bool(__thiscall *)(void *) >(networkable(), 9)(networkable());
	}

	__forceinline int& m_fireCount() {
		return get< int >(g_entoffsets.fireCount);
	}

	__forceinline bool* m_bFireIsBurning() {
		return (bool*)((uintptr_t)this + g_entoffsets.m_bFireIsBurning);
	}

	__forceinline int* m_fireXDelta() {
		return (int*)((uintptr_t)this + g_entoffsets.m_fireXDelta);
	}

	__forceinline int* m_fireYDelta() {
		return (int*)((uintptr_t)this + g_entoffsets.m_fireYDelta);
	}

	__forceinline int* m_fireZDelta() {
		return (int*)((uintptr_t)this + g_entoffsets.m_fireZDelta);
	}

	__forceinline int index() {
		return util::get_method< int(__thiscall *)(void *) >(networkable(), 10)(networkable());
	}

	__forceinline void SetDestroyedOnRecreateEntities() {
		return util::get_method< void(__thiscall *)(void *) >(networkable(), 13)(networkable());
	}

	// normal table.
	__forceinline const vec3_t &GetAbsOrigin() {
		return util::get_method< const vec3_t &(__thiscall *)(void *) >(this, 10)(this);
	}

	__forceinline const ang_t &GetAbsAngles() {
		return util::get_method< const ang_t &(__thiscall *)(void *) >(this, 11)(this);
	}

	__forceinline bool IsPlayer() {
		return util::get_method< bool(__thiscall *)(void *) >(this, ISPLAYER)(this);
	}

	__forceinline bool IsBaseCombatWeapon() {
		return util::get_method< bool(__thiscall *)(void *) >(this, ISBASECOMBATWEAPON)(this);
	}

	__forceinline std::string GetBombsiteName() {
		std::string out;

		// note - dex; bomb_target + 0x150 has a char array for site name... not sure how much memory gets allocated for it.
		out.resize(32u);

		std::memcpy(&out[0], (const void *)((uintptr_t)this + 0x150), 32u);

		return out;
	}

	__forceinline void InvalidatePhysicsRecursive(InvalidatePhysicsBits_t bits) {
		using InvalidatePhysicsRecursive_t = void(__thiscall *)(decltype(this), InvalidatePhysicsBits_t);
		g_csgo.InvalidatePhysicsRecursive.as< InvalidatePhysicsRecursive_t >()(this, bits);
	}

	__forceinline void SetAbsAngles(const ang_t &angles) {
		using SetAbsAngles_t = void(__thiscall *)(decltype(this), const ang_t &);
		g_csgo.SetAbsAngles.as< SetAbsAngles_t >()(this, angles);
	}

	__forceinline void SetAbsOrigin(const vec3_t &origin) {
		using SetAbsOrigin_t = void(__thiscall *)(decltype(this), const vec3_t &);
		g_csgo.SetAbsOrigin.as< SetAbsOrigin_t >()(this, origin);
	}

	__forceinline void SetAbsVelocity(const vec3_t &velocity) {
		using SetAbsVelocity_t = void(__thiscall *)(decltype(this), const vec3_t &);
		g_csgo.SetAbsVelocity.as< SetAbsVelocity_t >()(this, velocity);
	}

	__forceinline void AddEffect(int effects) {
		m_fEffects() |= effects;
	}

	__forceinline int get_class_id() {
		ClientClass *cc{ GetClientClass() };

		return (cc) ? cc->m_ClassID : -1;
	}

	__forceinline bool is(hash32_t hash) {
		return g_netvars.GetClientID(hash) == get_class_id();
	}
};

struct animstate_pose_param_cache_t {
	bool valid = false;
	int index = -1;
	const char* name;
	void SetValue(Player* player, float flValue);
};

struct procedural_foot_t
{
	vec3_t m_vecPosAnim;
	vec3_t m_vecPosAnimLast;
	vec3_t m_vecPosPlant;
	vec3_t m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;

	procedural_foot_t()
	{
		m_vecPosAnim.clear();
		m_vecPosAnimLast.clear();
		m_vecPosPlant.clear();
		m_vecPlantVel.clear();
		m_flLockAmount = 0;
		m_flLastPlantTime = 0.f;// gpGlobals->curtime;
	}

	void Init(vec3_t vecNew)
	{
		m_vecPosAnim = vecNew;
		m_vecPosAnimLast = vecNew;
		m_vecPosPlant = vecNew;
		m_vecPlantVel.clear();
		m_flLockAmount = 0;
		m_flLastPlantTime = 0.f;// gpGlobals->curtime;
	}
};

struct aimmatrix_transition_t
{
	float	m_flDurationStateHasBeenValid;
	float	m_flDurationStateHasBeenInValid;
	float	m_flHowLongToWaitUntilTransitionCanBlendIn;
	float	m_flHowLongToWaitUntilTransitionCanBlendOut;
	float	m_flBlendValue;

	void UpdateTransitionState(bool bStateShouldBeValid, float flTimeInterval, float flSpeed)
	{
		if (bStateShouldBeValid)
		{
			m_flDurationStateHasBeenInValid = 0;
			m_flDurationStateHasBeenValid += flTimeInterval;
			if (m_flDurationStateHasBeenValid >= m_flHowLongToWaitUntilTransitionCanBlendIn)
			{
				m_flBlendValue = math::ApproachAngle(1, m_flBlendValue, flSpeed);
			}
		}
		else
		{
			m_flDurationStateHasBeenValid = 0;
			m_flDurationStateHasBeenInValid += flTimeInterval;
			if (m_flDurationStateHasBeenInValid >= m_flHowLongToWaitUntilTransitionCanBlendOut)
			{
				m_flBlendValue = math::ApproachAngle(0, m_flBlendValue, flSpeed);
			}
		}
	}

	void Init(void)
	{
		m_flDurationStateHasBeenValid = 0;
		m_flDurationStateHasBeenInValid = 0;
		m_flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
		m_flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;
		m_flBlendValue = 0;
	}

	aimmatrix_transition_t()
	{
		Init();
	}
};

typedef const int* animlayerpreset;

// class created thanks to cs:go source leak :-)
class CCSGOPlayerAnimState {
public:
	animlayerpreset			m_pLayerOrderPreset;

	bool					m_bFirstRunSinceInit;

	bool					m_bFirstFootPlantSinceInit;
	int						m_iLastUpdateFrame;

	float					m_flEyePositionSmoothLerp;

	float					m_flStrafeChangeWeightSmoothFalloff;

	aimmatrix_transition_t	m_tStandWalkAim;
	aimmatrix_transition_t	m_tStandRunAim;
	aimmatrix_transition_t	m_tCrouchWalkAim;

	int						m_cachedModelIndex;

	float					m_flStepHeightLeft;
	float					m_flStepHeightRight;

	void* m_pWeaponLastBoneSetup;

	Player* m_pPlayer;
	void* m_pWeapon;
	void* m_pWeaponLast;

	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;

	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;

	float					m_flPrimaryCycle;
	float					m_flMoveWeight;
	float					m_flMoveWeightSmoothed;
	float					m_flAnimDuckAmount;
	float					m_flDuckAdditional;
	float					m_flRecrouchWeight;

	vec3_t					m_vecPositionCurrent;
	vec3_t					m_vecPositionLast;

	vec3_t					m_vecVelocity;
	vec3_t					m_vecVelocityNormalized;
	vec3_t					m_vecVelocityNormalizedNonZero;
	float					m_flVelocityLengthXY;
	float					m_flVelocityLengthZ;

	float					m_flSpeedAsPortionOfRunTopSpeed;
	float					m_flSpeedAsPortionOfWalkTopSpeed;
	float					m_flSpeedAsPortionOfCrouchTopSpeed;

	float					m_flDurationMoving;
	float					m_flDurationStill;

	bool					m_bOnGround;

	bool					m_bLanding;
	float					m_flJumpToFall;
	float					m_flDurationInAir;
	float					m_flLeftGroundHeight;
	float					m_flLandAnimMultiplier;

	float					m_flWalkToRunTransition;

	bool					m_bLandedOnGroundThisFrame;
	bool					m_bLeftTheGroundThisFrame;
	float					m_flInAirSmoothValue;

	bool					m_bOnLadder;
	float					m_flLadderWeight;
	float					m_flLadderSpeed;

	bool					m_bWalkToRunTransitionState;

	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;

	CUtlVector< uint16_t >	m_ActivityModifiers;

	float					m_flNextTwitchTime;

	float					m_flTimeOfLastKnownInjury;

	float					m_flLastVelocityTestTime;
	vec3_t					m_vecVelocityLast;
	vec3_t					m_vecTargetAcceleration;
	vec3_t					m_vecAcceleration;
	float					m_flAccelerationWeight;

	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;

	bool					m_bFlashed;

	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;

	float					m_flFootLerp;

	bool					m_bFeetCrossed;

	bool					m_bPlayerIsAccelerating;

	animstate_pose_param_cache_t m_tPoseParamMappings[20];

	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;

	int						m_nPreviousMoveState;
	float					m_flStutterStep;

	float					m_flActionWeightBiasRemainder;

	procedural_foot_t		m_footLeft;
	procedural_foot_t		m_footRight;

	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;

	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;

	//float					m_flMoveWalkWeight;
	//float					m_flMoveCrouchWalkWeight;
	//float					m_flMoveRunWeight;

	int						m_nAnimstateModelVersion;

	void update(const vec3_t& angle)
	{
		using update_animstate_fn = void(__vectorcall*)(CCSGOPlayerAnimState*, void*, float, float, float, void*);
		static auto update_animstate = pattern::find(g_csgo.m_client_dll, XOR("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24")).as< update_animstate_fn>();
		if (!update_animstate)
			return;

		update_animstate(this, NULL, NULL, angle.y, angle.x, NULL);
	}
};//Size=0x344

struct virtualmodel_t;
class CStudioHdrEx {
public:
	studiohdr_t* studio_hdr;
	virtualmodel_t* virtual_model;
	char pad[120];

	int m_nPerfAnimatedBones;
	int m_nPerfUsedBones;
	int m_nPerfAnimationLayers;
};

class CStudioHdr {
public:
	class mstudioposeparamdesc_t {
	public:
		int					sznameindex;
		__forceinline char *const name(void) const { return ((char *)this) + sznameindex; }
		int					flags;	// ????
		float				start;	// starting value
		float				end;	// ending value
		float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
	};

	studiohdr_t *m_pStudioHdr;
	void *m_pVModel;
	PAD(0x4);
	CUtlVector<const studiohdr_t*> m_pStudioHdrCache; // 0x000C 
	int			                   m_nFrameUnlockCounter; // 0x0020 
	int* m_pFrameUnlockCounter; // 0x0024 
	PAD(0x8);
	CUtlVector<int>	               m_boneFlags; // 0x0030
	CUtlVector<int>	               m_boneParent; // 0x0050
};

class C_AnimationLayer {
public:
	float   m_anim_time;			// 0x0000
	float   m_fade_out_time;		// 0x0004
	int     m_flags;				// 0x0008
	int     m_activty;				// 0x000C
	int     m_priority;				// 0x0010
	int     m_order;				// 0x0014
	int     m_sequence;				// 0x0018
	float   m_prev_cycle;			// 0x001C
	float   m_weight;				// 0x0020
	float   m_weight_delta_rate;	// 0x0024
	float   m_playback_rate;		// 0x0028
	float   m_cycle;				// 0x002C
	Entity *m_owner;				// 0x0030
	int     m_bits;					// 0x0034
}; // size: 0x0038

class CBoneAccessor {
public:
	void *m_pAnimating;
	BoneArray *m_pBones;
	int        m_ReadableBones;
	int        m_WritableBones;
};

class CBoneCache {
public:
	BoneArray *m_pCachedBones;
	PAD(0x8);
	int        m_CachedBoneCount;
};

class Ragdoll : public Entity {
public:
	__forceinline Player *GetPlayer() {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Player * >(m_hPlayer());
	}

	__forceinline EHANDLE &m_hPlayer() {
		return get< EHANDLE >(g_entoffsets.m_hPlayer);
	}

	__forceinline float &m_flDeathYaw() {
		return get< float >(g_entoffsets.m_flDeathYaw);
	}

	__forceinline float &m_flAbsYaw() {
		return get< float >(g_entoffsets.m_flAbsYaw);
	}
};


class VarMapEntry_t {
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
	// need Interpolate() called on it anymore.
	void* data;
	void* watcher;
};

struct VarMapping_t {
	VarMapping_t() {
		m_nInterpolatedEntries = 0;
	}

	VarMapEntry_t* m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

class IClientRenderable {
public:
	virtual IClientUnknown* get_client_unknown() = 0;
	virtual vec3_t const& get_render_origin() = 0;
	virtual ang_t const& get_render_angles() = 0;
	virtual bool					should_draw() = 0;
	virtual int						get_render_flags() = 0;
	virtual void					unknown0() const = 0;
	virtual uint16_t				get_shadow_handle() const = 0;
	virtual uint16_t& get_render_handle() = 0;
	virtual const model_t* get_model() const = 0;
	virtual int						draw_model(int flags, uint8_t alpha) = 0;
	virtual int						get_body() = 0;
	virtual void					get_color_modulation(float* color) = 0;
	virtual bool					lod_test() = 0;
	virtual bool					setup_bones(matrix3x4_t* bones, int max, int mask, float time) = 0;
	virtual void					setup_weights(const matrix3x4_t* bones, int count, float* weights, float* delayed_weights) = 0;
	virtual void					do_animation_events() = 0;
	virtual void* get_pvs_notify_interface() = 0;
	virtual void					get_render_bounds(vec3_t& mins, vec3_t& maxs) = 0;
	virtual void					get_render_bounds_worldspace(vec3_t& mins, vec3_t& maxs) = 0;
	virtual void					get_shadow_render_bounds(vec3_t& mins, vec3_t& maxs, int type) = 0;
	virtual bool					should_receive_projected_textures(int flags) = 0;
	virtual bool					get_shadow_cast_distance(float* dist, int type) const = 0;
	virtual bool					get_shadow_cast_direction(vec3_t* direction, int type) const = 0;
	virtual bool					is_shadow_dirty() = 0;
	virtual void					mark_shadow_dirty(bool dirty) = 0;
	virtual IClientRenderable* get_shadow_parent() = 0;
	virtual IClientRenderable* first_shadow_child() = 0;
	virtual IClientRenderable* next_shadow_peer() = 0;
	virtual int						shadow_cast_type() = 0;
	virtual void					create_model_instance() = 0;
	virtual uint16_t				get_model_instance() = 0;
	virtual const matrix3x4_t& renderable_to_world_transform() = 0;
	virtual int						lookup_attachment(const char* name) = 0;
	virtual bool					get_attachment(int index, vec3_t& origin, vec3_t& angles) = 0;
	virtual bool					get_attachment(int index, matrix3x4_t& matrix) = 0;
	virtual float* get_render_clip_plane() = 0;
	virtual int						get_skin() = 0;
	virtual void					on_threaded_draw_setup() = 0;
	virtual bool					uses_flex_delayed_weights() = 0;
	virtual void					record_tool_message() = 0;
	virtual bool					should_draw_for_split_screen_user(int slot) = 0;
	virtual uint8_t					override_alpha_modulation(uint8_t alpha) = 0;
	virtual uint8_t					override_shadow_alpha_modulation(uint8_t alpha) = 0;
};


class Player : public Entity {
public:

	int& m_iPrevBoneMask() {
		return *(int*)((uintptr_t)this + g_entoffsets.m_iPrevBoneMask);
	}

	int& m_iOcclusionFramecount() {
		return *(int*)((uintptr_t)this + g_entoffsets.m_iOcclusionFramecount);
	}

	int& m_iOcclusionFlags() {
		return *(int*)((uintptr_t)this + g_entoffsets.m_iOcclusionFlags);
	}

	int&  m_iAccumulatedBoneMask() {
		return *(int*)((uintptr_t)this + g_entoffsets.m_iAccumulatedBoneMask);
	}

	vec3_t get_hitbox_position(int hitbox, matrix3x4_t* matrix)
	{
		if (!matrix)
			matrix = this->m_BoneCache().m_pCachedBones;

		auto hdr = g_csgo.m_model_info->GetStudioModel(this->GetModel());

		if (!hdr)
			return vec3_t();

		auto hitbox_set = hdr->GetHitboxSet(this->m_nHitboxSet());

		if (!hitbox_set)
			return vec3_t();

		auto hdr_hitbox = hitbox_set->GetHitbox(hitbox);

		if (!hdr_hitbox)
			return vec3_t();

		vec3_t min, max;

		math::VectorTransform(hdr_hitbox->m_mins, matrix[hdr_hitbox->m_bone], min);
		math::VectorTransform(hdr_hitbox->m_maxs, matrix[hdr_hitbox->m_bone], max);

		return (min + max) * 0.5f;
	}

	_forceinline std::optional<vec3_t> GetHitboxPosition(const int hitbox) {
		assert(hitbox > -1 && hitbox < HITBOX_MAX); // given invalid hitbox index for gethitboxposition

		std::array<matrix3x4_t, 128> bonesToWorld = { };

		if (const auto model = this->GetModel(); model != nullptr) {
			if (const auto studioHdr = g_csgo.m_model_info->GetStudioModel(model); studioHdr != nullptr) {
				if (const auto studioHitbox = studioHdr->GetHitbox(hitbox, 0); studioHitbox != nullptr) {
					if (this->SetupBones(bonesToWorld.data(), bonesToWorld.size(), BONE_USED_BY_HITBOX, 0.f)) {
						// get mins/maxs by bone
						const vec3_t min = math::VectorTransform(studioHitbox->m_mins, bonesToWorld.at(studioHitbox->m_bone));
						const vec3_t max = math::VectorTransform(studioHitbox->m_maxs, bonesToWorld.at(studioHitbox->m_bone));

						return (min + max) * 0.5;
					}
				}
			}
		}

		return std::nullopt;
	}

	__forceinline int& m_nLastNonSkippedFrame() {
		return *(int*)(uintptr_t(this) + 0xA68);
	}

	__forceinline int& m_nCustomBlendingRuleMask() {
		return *(int*)(uintptr_t(this) + 0xA24);
	}

	__forceinline int& m_nAnimLODflags() {
		return *(int*)(uintptr_t(this) + 0xA28);
	}

	vec3_t* m_vecBonePos() {
		return (vec3_t*)((uintptr_t)this + g_csgo.m_nCachedBonesPosition);
	}

	quaternion_t* m_quatBoneRot() {
		return (quaternion_t*)((uintptr_t)this + g_csgo.m_nCachedBonesRotation);
	}

	__forceinline int& m_ClientEntEffects() {
		return *(int*)(uintptr_t(this) + 0x68);
	}

	__forceinline int& m_pIK() {
		return *(int*)(uintptr_t(this) + (g_entoffsets.m_nForceBone - 0x1C));
	}

	// netvars / etc.
	__forceinline vec3_t &m_vecAbsVelocity() {
		return get< vec3_t >(g_entoffsets.m_vecAbsVelocity);
	}

	__forceinline int &m_lifeState() {
		return get< int >(g_entoffsets.m_lifeState);
	}

	__forceinline int &m_fFlags() {
		return get< int >(g_entoffsets.m_fFlags);
	}

	__forceinline bool& m_bWaitForNoAttack() {
		return get< bool >(g_entoffsets.m_bWaitForNoAttack);
	}

	__forceinline bool& m_bIsDefusing() {
		return get< bool >(g_entoffsets.m_bIsDefusing);
	}

	__forceinline int& m_iPlayerState() {
		return get< int >(g_entoffsets.m_iPlayerState);
	}

	__forceinline int &m_MoveType() {
		return get< int >(g_entoffsets.m_MoveType);
	}

	__forceinline int &m_iHealth() {
		return get< int >(g_entoffsets.m_iHealth);
	}

	__forceinline int &m_iAccount() {
		return get< int >(g_entoffsets.m_iAccount);
	}

	__forceinline bool &m_bHasDefuser() {
		return get< bool >(g_entoffsets.m_bHasDefuser);
	}

	__forceinline int &m_nHitboxSet() {
		return get< int >(g_entoffsets.m_nHitboxSet);
	}

	__forceinline ang_t &m_angAbsRotation() {
		return get< ang_t >(g_entoffsets.m_angAbsRotation);
	}

	__forceinline ang_t &m_angRotation() {
		return get< ang_t >(g_entoffsets.m_angRotation);
	}

	__forceinline ang_t &m_angNetworkAngles() {
		return get< ang_t >(g_entoffsets.m_angNetworkAngles);
	}

	Entity* get_view_model()
	{
		Entity* view_model = g_csgo.m_entlist->GetClientEntityFromHandle(this->m_hViewModel());
		return view_model;
	}

	__forceinline CBaseHandle& m_hViewModel() {
		return get< CBaseHandle >(g_entoffsets.m_hViewModel);
	}

	__forceinline bool m_bIsLocalPlayer() {
		// .text:101E0078 674     84 C0				   test    al, al          ; Logical Compare
		// .text:101E007A 674     74 17				   jz      short loc_101E0093; Jump if Zero( ZF = 1 )
		// .text:101E007C 674     8A 83 F8 35 00 00	   mov     al, [ ebx + 35F8h ]
		return get< bool >(g_csgo.IsLocalPlayer);
	}

	__forceinline CCSGOPlayerAnimState *m_PlayerAnimState() {
		// .text:1037A5B8 00C     E8 E3 40 E6 FF         call    C_BasePlayer__Spawn ; Call Procedure
		// .text:1037A5BD 00C     80 BE E1 39 00 00 00   cmp     byte ptr[ esi + 39E1h ], 0; Compare Two Operands
		// .text:1037A5C4 00C     74 48                  jz      short loc_1037A60E; Jump if Zero( ZF = 1 )
		// .text:1037A5C6 00C     8B 8E 74 38 00 00      mov     ecx, [ esi + 3874h ]; this
		// .text:1037A5CC 00C     85 C9                  test    ecx, ecx; Logical Compare
		// .text:1037A5CE 00C     74 3E                  jz      short loc_1037A60E; Jump if Zero( ZF = 1 )
		return get< CCSGOPlayerAnimState * >(g_csgo.PlayerAnimState);
	}

	__forceinline CStudioHdr *m_studioHdr() {
		// .text:1017E902 08C    8B 86 3C 29 00 00    mov     eax, [ esi + 293Ch ]
		// .text:1017E908 08C    89 44 24 10          mov[ esp + 88h + var_78 ], eax
		return get< CStudioHdr * >(g_csgo.studioHdr);
	}

	__forceinline ulong_t &m_iMostRecentModelBoneCounter() {
		// .text:101AC9A9 000    89 81 80 26 00 00    mov[ ecx + 2680h ], eax
		return get< ulong_t >(g_csgo.MostRecentModelBoneCounter);
	}

	__forceinline float &m_flLastBoneSetupTime() {
		// .text:101AC99F 000    C7 81 14 29 00 00 FF FF+    mov     dword ptr [ecx+2914h], 0FF7FFFFFh;
		return get< float >(g_csgo.LastBoneSetupTime);
	}

	__forceinline int &m_nTickBase() {
		return get< int >(g_entoffsets.m_nTickBase);
	}

	__forceinline float &m_flNextAttack() {
		return get< float >(g_entoffsets.m_flNextAttack);
	}

	__forceinline float &m_flDuckAmount() {
		return get< float >(g_entoffsets.m_flDuckAmount);
	}

	__forceinline float &m_flSimulationTime() {
		return get< float >(g_entoffsets.m_flSimulationTime);
	}

	__forceinline float &m_flOldSimulationTime() {
		return get< float >(g_entoffsets.m_flOldSimulationTime);
	}

	__forceinline float &m_flLowerBodyYawTarget() {
		return get< float >(g_entoffsets.m_flLowerBodyYawTarget);
	}

	__forceinline float &m_fImmuneToGunGameDamageTime() {
		return get< float >(g_entoffsets.m_fImmuneToGunGameDamageTime);
	}

	__forceinline bool &m_bHasHelmet() {
		return get< bool >(g_entoffsets.m_bHasHelmet);
	}

	__forceinline bool &m_bClientSideAnimation() {
		return get< bool >(g_entoffsets.m_bClientSideAnimation);
	}

	__forceinline bool &m_bHasHeavyArmor() {
		return get< bool >(g_entoffsets.m_bHasHeavyArmor);
	}

	__forceinline bool &m_bIsScoped() {
		return get< bool >(g_entoffsets.m_bIsScoped);
	}

	__forceinline bool &m_bDucking() {
		return get< bool >(g_entoffsets.m_bDucking);
	}

	__forceinline bool &m_bSpotted() {
		return get< bool >(g_entoffsets.m_bSpotted);
	}

	__forceinline int &m_iObserverMode() {
		return get< int >(g_entoffsets.m_iObserverMode);
	}

	__forceinline int &m_ArmorValue() {
		return get< int >(g_entoffsets.m_ArmorValue);
	}

	__forceinline float &m_flMaxspeed() {
		return get< float >(g_entoffsets.m_flMaxspeed);
	}

	__forceinline float &m_surfaceFriction() {
		return get< float >(g_entoffsets.m_surfaceFriction);
	}

	__forceinline float &m_flFlashBangTime() {
		return get< float >(g_entoffsets.m_flFlashBangTime);
	}

	__forceinline ang_t &m_angEyeAngles() {
		return get< ang_t >(g_entoffsets.m_angEyeAngles);
	}

	__forceinline EHANDLE& m_MoveParent() {
		return get< EHANDLE >(g_entoffsets.moveparent);
	}

	__forceinline ang_t &m_aimPunchAngle() {
		return get< ang_t >(g_entoffsets.m_aimPunchAngle);
	}

	__forceinline ang_t &m_viewPunchAngle() {
		return get< ang_t >(g_entoffsets.m_viewPunchAngle);
	}

	__forceinline ang_t &m_aimPunchAngleVel() {
		return get< ang_t >(g_entoffsets.m_aimPunchAngleVel);
	}

	__forceinline vec3_t &m_vecViewOffset() {
		return get< vec3_t >(g_entoffsets.m_vecViewOffset);
	}

	__forceinline CUserCmd &m_PlayerCommand() {
		return get< CUserCmd >(g_entoffsets.m_PlayerCommand);
	}

	__forceinline CUserCmd *&m_pCurrentCommand() {
		return get< CUserCmd * >(g_entoffsets.m_pCurrentCommand);
	}

	__forceinline ang_t& render_angles() {
		return get< ang_t >(g_entoffsets.render_angles);
	}

	__forceinline int &m_iEFlags() {
		return get< int >(g_entoffsets.m_iEFlags);
	}

	__forceinline bool PhysicsRunThink(int unk01) {
		return g_csgo.m_uImplPhysicsRunThink.as<bool(__thiscall*)(void*, int)>()(this, unk01);
	}

	__forceinline int SetNextThink(int tick) {
		using fnSetNextThink = int(__thiscall*)(Player*, int tick);
		return g_csgo.m_NextTick.as<fnSetNextThink>()(this, tick);
	}

	__forceinline void Think() {
		const auto next_think = (int*)((uintptr_t)this + 0xF8);
		if (*next_think > 0 && *next_think <= this->m_nTickBase())
		{
			*next_think = -1;

			// unk func inside RunThink()
			g_csgo.think.as<void(__thiscall*)(void*, int)>()(this, 0);

			util::get_method< void(__thiscall*)(decltype(this)) >(this, 137)(this);
		}
	}

	__forceinline void PreThink() {
		if (this->PhysicsRunThink(0))
		{
			return util::get_method< void(__thiscall*)(decltype(this)) >(this, 307)(this);
		}
	}

	__forceinline float& m_flFallVelocity() {
		return get< float >(g_entoffsets.m_fall_velocity);
	}

	__forceinline float& velocity_modifier() {
		return get< float >(g_entoffsets.velocity_modifier);
	}

	__forceinline void PostThink() {
		util::get_method< void(__thiscall*)(void*) >(g_csgo.model_cache, 32)(g_csgo.model_cache);

		if (this->alive())
		{
			util::get_method< void(__thiscall*)(decltype(this)) >(this, 329)(this);

			if (this->m_fFlags() & FL_ONGROUND)
				this->m_flFallVelocity() = 0.f;

			if (this->m_nSequence() == -1)
				util::get_method< void(__thiscall*)(decltype(this), int) >(this, 213)(this, 0);

			util::get_method< void(__thiscall*)(decltype(this)) >(this, 214)(this);
			g_csgo.PostThinkVPhysics.as<bool(__thiscall*)(Player*)>()(this);
		}

		g_csgo.simulate_player_simulated_entities.as< void(__thiscall*)(Player*)>()(this);
		util::get_method< void(__thiscall*)(void*) >(g_csgo.model_cache, 33)(g_csgo.model_cache);
	}

	__forceinline float *m_flPoseParameter() {
		return (float *)((uintptr_t)this + g_entoffsets.m_flPoseParameter);
	}

	__forceinline CBaseHandle *m_hMyWearables() {
		return (CBaseHandle *)((uintptr_t)this + g_entoffsets.m_hMyWearables);
	}

	__forceinline CBoneCache &m_BoneCache() {
		// TODO; sig
		return get< CBoneCache >(g_entoffsets.m_BoneCache);
	}

	__forceinline EHANDLE &m_hObserverTarget() {
		return get< EHANDLE >(g_entoffsets.m_hObserverTarget);
	}

	__forceinline EHANDLE &m_hActiveWeapon() {
		return get< EHANDLE >(g_entoffsets.m_hActiveWeapon);
	}

	__forceinline EHANDLE &m_hGroundEntity() {
		return get< EHANDLE >(g_entoffsets.m_hGroundEntity);
	}

	__forceinline CBaseHandle *m_hMyWeapons() {
		return (CBaseHandle *)((uintptr_t)this + g_entoffsets.m_hMyWeapons);
	}

	__forceinline C_AnimationLayer *m_AnimOverlay() {
		// .text:1017EAB1 08C    8B 47 1C                mov     eax, [edi+1Ch]
		// .text:1017EAB4 08C    8D 0C D5 00 00 00 00    lea     ecx, ds:0[ edx * 8 ]; Load Effective Address
		// .text:1017EABB 08C    2B CA                   sub     ecx, edx; Integer Subtraction
		// .text:1017EABD 08C    8B 80 70 29 00 00       mov     eax, [ eax + 2970h ]
		// .text:1017EAC3 08C    8D 34 C8                lea     esi, [ eax + ecx * 8 ]; Load Effective Address
		// .text:1017EAC6
		return get< C_AnimationLayer * >(g_csgo.AnimOverlay);
	}

	__forceinline int m_AnimOverlayCount() {
		// .text:1017EAB1 08C    8B 47 1C                mov     eax, [edi+1Ch]
		// .text:1017EAB4 08C    8D 0C D5 00 00 00 00    lea     ecx, ds:0[ edx * 8 ]; Load Effective Address
		// .text:1017EABB 08C    2B CA                   sub     ecx, edx; Integer Subtraction
		// .text:1017EABD 08C    8B 80 70 29 00 00       mov     eax, [ eax + 2970h ]
		// .text:1017EAC3 08C    8D 34 C8                lea     esi, [ eax + ecx * 8 ]; Load Effective Address
		// .text:1017EAC6
		return get< int >(g_csgo.AnimOverlay + 0xC);
	}

	__forceinline float &m_flSpawnTime() {
		// .text:10381AB3 00C    F3 0F 10 49 10             movss   xmm1, dword ptr [ecx+10h] ; Move Scalar Single-FP
		// .text:10381AB8 00C    F3 0F 5C 88 90 A2 00 00    subss   xmm1, dword ptr[ eax + 0A290h ]; Scalar Single - FP Subtract
		return get< float >(g_csgo.SpawnTime);
	}

	__forceinline CBoneAccessor &m_BoneAccessor() {
		// .text:101A9253 1C4    C7 81 A0 26 00 00 00 FF 0F 00    mov     dword ptr[ ecx + 26A0h ], 0FFF00h
		// .text:101A925D 1C4    C7 81 9C 26 00 00 00 FF 0F 00    mov     dword ptr[ ecx + 269Ch ], 0FFF00h
		// .text:101A9267 1C4    8B 10                            mov     edx, [ eax ]
		// .text:101A9269 1C4    8D 81 94 26 00 00                lea     eax, [ ecx + 2694h ]; Load Effective Address
		// .text:101A926F 1C4    50                               push    eax
		return get< CBoneAccessor >(g_csgo.BoneAccessor);
	}

public:
	enum indices : size_t {
		GETREFEHANDLE = 2,
		TESTHITBOXES = 52,
		BUILDTRANSFORMATIONS = 184,
		DOEXTRABONEPROCESSING = 192,
		STANDARDBLENDINGRULES = 200,
		UPDATECLIENTSIDEANIMATION = 218, // 55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36
		GETACTIVEWEAPON = 262,
		GETEYEPOS = 163,
		GETFOV = 321,
		UPDATECOLLISIONBOUNDS = 329 // 56 57 8B F9 8B 0D ? ? ? ? F6 87 ? ? ? ? ?
	};

public:
	// virtuals.
	__forceinline ulong_t GetRefEHandle() {
		using GetRefEHandle_t = ulong_t(__thiscall *)(decltype(this));
		return util::get_method< GetRefEHandle_t >(this, GETREFEHANDLE)(this);
	}

	__forceinline void BuildTransformations(CStudioHdr *hdr, vec3_t *pos, quaternion_t *q, const matrix3x4_t &transform, int mask, uint8_t *computed) {
		using BuildTransformations_t = void(__thiscall *)(decltype(this), CStudioHdr *, vec3_t *, quaternion_t *, matrix3x4_t const &, int, uint8_t *);
		return util::get_method< BuildTransformations_t >(this, BUILDTRANSFORMATIONS)(this, hdr, pos, q, transform, mask, computed);
	}

	__forceinline void StandardBlendingRules(CStudioHdr *hdr, vec3_t *pos, quaternion_t *q, float time, int mask) {
		using StandardBlendingRules_t = void(__thiscall *)(decltype(this), CStudioHdr *, vec3_t *, quaternion_t *, float, int);
		return util::get_method< StandardBlendingRules_t >(this, STANDARDBLENDINGRULES)(this, hdr, pos, q, time, mask);
	}

	__forceinline float GetFOV() {
		return util::get_method< float(__thiscall *)(decltype(this)) >(this, GETFOV)(this);
	}

	__forceinline const vec3_t &WorldSpaceCenter() {
		return util::get_method< const vec3_t &(__thiscall *)(void *) >(this, WORLDSPACECENTER)(this);
	}

	__forceinline void GetEyePos(vec3_t *pos) {
		util::get_method< void(__thiscall *)(decltype(this), vec3_t *) >(this, GETEYEPOS)(this, pos);
	}

	__forceinline IClientCollideable* GetCollideable() {
		return util::get_method< IClientCollideable *(__thiscall*)(decltype(this)) >(this, 3)(this);
	}

	__forceinline void ModifyEyePosition(CCSGOPlayerAnimState *state, vec3_t *pos) {
		if (!state) {
			return;
		}

		//  if ( *(this + 0x50) && (*(this + 0x100) || *(this + 0x94) != 0.0 || !sub_102C9480(*(this + 0x50))) )
		if (state->m_pPlayer &&
			(state->m_bLanding || state->m_pPlayer->m_flDuckAmount() != 0.f || !state->m_pPlayer->GetGroundEntity())) {
			auto v5 = 8;

			if (v5 != -1 && state->m_pPlayer->m_BoneCache().m_pCachedBones) {
				vec3_t head_pos(
					state->m_pPlayer->m_BoneCache().m_pCachedBones[8][0][3],
					state->m_pPlayer->m_BoneCache().m_pCachedBones[8][1][3],
					state->m_pPlayer->m_BoneCache().m_pCachedBones[8][2][3]);

				auto v12 = head_pos;
				auto v7 = v12.z + 1.7;

				auto v8 = pos->z;
				if (v8 > v7) // if (v8 > (v12 + 1.7))
				{
					float v13 = 0.f;
					float v3 = (*pos).z - v7;

					float v4 = (v3 - 4.f) * 0.16666667;
					if (v4 >= 0.f)
						v13 = std::fminf(v4, 1.f);

					(*pos).z = (((v7 - (*pos).z)) * (((v13 * v13) * 3.0) - (((v13 * v13) * 2.0) * v13))) + (*pos).z;
				}
			}
		}
	}

	__forceinline vec3_t GetShootPosition() {
		vec3_t pos;

		GetEyePos(&pos);

		if (*reinterpret_cast <int32_t *> (uintptr_t(this) + 0x39E1)) {
			auto v3 = m_PlayerAnimState();
			if (v3) {
				ModifyEyePosition(v3, &pos);
			}
		}

		return pos;
	}

	__forceinline void UpdateClientSideAnimation() {
		return util::get_method< void(__thiscall *)(decltype(this)) >(this, UPDATECLIENTSIDEANIMATION)(this);
	}

	__forceinline void UpdateCollisionBounds() {
		return util::get_method< void(__thiscall *)(decltype(this)) >(this, UPDATECOLLISIONBOUNDS)(this);
	}

	void UpdateAnimationState(CCSGOPlayerAnimState* state, vec3_t angle)
	{
		static auto UpdateAnimState = pattern::find(g_csgo.m_client_dll, "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");
		if (!UpdateAnimState)
			return;

		__asm
		{
			mov ecx, state

			movss xmm1, dword ptr[angle + 4]
			movss xmm2, dword ptr[angle]

			call UpdateAnimState
		}
	}

	// misc funcs.
	__forceinline CStudioHdr *GetModelPtr() {
		using LockStudioHdr_t = void(__thiscall *)(decltype(this));

		if (!m_studioHdr())
			g_csgo.LockStudioHdr.as< LockStudioHdr_t >()(this);

		return m_studioHdr();
	}

	__forceinline Weapon *GetActiveWeapon() {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Weapon * >(m_hActiveWeapon());
	}

	__forceinline Entity *GetObserverTarget() {
		return g_csgo.m_entlist->GetClientEntityFromHandle(m_hObserverTarget());
	}

	__forceinline Entity *GetGroundEntity() {
		return g_csgo.m_entlist->GetClientEntityFromHandle(m_hGroundEntity());
	}

	__forceinline void SetAnimLayers(C_AnimationLayer *layers) {
		std::memcpy(m_AnimOverlay(), layers, sizeof(C_AnimationLayer) * 13);
	}

	__forceinline void GetAnimLayers(C_AnimationLayer *layers) {
		std::memcpy(layers, m_AnimOverlay(), sizeof(C_AnimationLayer) * 13);
	}

	__forceinline void SetPoseParameters(float *poses) {
		std::memcpy(m_flPoseParameter(), poses, sizeof(float) * 24);
	}

	__forceinline void GetPoseParameters(float *poses) {
		std::memcpy(poses, m_flPoseParameter(), sizeof(float) * 24);
	}

	__forceinline bool ComputeHitboxSurroundingBox(vec3_t *mins, vec3_t *maxs) {
		using ComputeHitboxSurroundingBox_t = bool(__thiscall *)(void *, vec3_t *, vec3_t *);

		return g_csgo.ComputeHitboxSurroundingBox.as< ComputeHitboxSurroundingBox_t >()(this, mins, maxs);
	}

	__forceinline int GetSequenceActivity(int sequence) {
		using GetSequenceActivity_t = int(__fastcall *)(CStudioHdr *, int);

		return g_csgo.GetSequenceActivity.as< GetSequenceActivity_t >()(GetModelPtr(), sequence);
	}

	__forceinline bool HasC4() {
		using HasC4_t = bool(__thiscall *)(decltype(this));
		return g_csgo.HasC4.as< HasC4_t >()(this);
	}

	void calc_absolute_position()
	{
		g_csgo.calc_absolute_position.as<void(__thiscall*)(decltype(this))>()(this);
	}

	__forceinline void store_bone_cache(matrix3x4_t* matrix) {
		std::memcpy(matrix, this->m_BoneCache().m_pCachedBones, sizeof(matrix3x4_t) * this->m_BoneCache().m_CachedBoneCount);
	}

	__forceinline void attachments_helper()
	{
		auto hdr = GetModelPtr();
		if (!hdr)
			return;

		g_csgo.attachments_helper.as<void(__thiscall*)(decltype(this), void*)>()(this, hdr);
	}

	__forceinline void set_bone_cache(matrix3x4_t* matrix)
	{
		auto cache = m_BoneCache();
		if (!cache.m_pCachedBones)
			return;

		std::memcpy(cache.m_pCachedBones, matrix, sizeof(matrix3x4_t) * cache.m_CachedBoneCount);
		std::memcpy(m_BoneAccessor().m_pBones, matrix, sizeof(matrix3x4_t) * cache.m_CachedBoneCount);
	}

	__forceinline void interpolate_moveparent_pos()
	{
		// update moveparent + local pos
		calc_absolute_position();

		// mark that we gonna change our pos
		InvalidatePhysicsRecursive(POSITION_CHANGED);

		auto moveparent = g_csgo.m_entlist->GetClientEntityFromHandle(m_MoveParent());
		if (moveparent)
		{
			// set new moveparent pos to interpolated one
			auto& frame = moveparent->m_hCoordFrame();
			frame.SetOrigin(GetAbsOrigin());
		}
	}

	__forceinline void InvalidateBoneCache() {
		uintptr_t cache = *g_csgo.InvalidateBoneCache.add(0xA).as<uintptr_t*>();

		this->m_iMostRecentModelBoneCounter() = (*(uint32_t*)cache) - 1;
		this->m_BoneAccessor().m_ReadableBones = this->m_BoneAccessor().m_WritableBones = -1;
		this->m_flLastBoneSetupTime() = -FLT_MAX;

		*(int*)((uintptr_t)this + 0xA30) = g_csgo.m_globals->m_frame;
		*(int*)((uintptr_t)this + 0xA28) = 0;
	}

	__forceinline bool alive() {
		return m_lifeState() == LIFE_ALIVE;
	}

	__forceinline cmd_context_t& cmd_context()
	{
		return *reinterpret_cast<cmd_context_t*> (
			reinterpret_cast<std::uintptr_t> (this) + 0x34d0u
			);
	}

	__forceinline int& sim_tick()
	{
		return *reinterpret_cast<int*> (
			reinterpret_cast<std::uintptr_t> (this) + 0x2a8u
			);
	}

	__forceinline bool enemy(Player *from) {
		if (m_iTeamNum() != from->m_iTeamNum())
			return true;

		else if (g_csgo.mp_teammates_are_enemies->GetInt())
			return true;

		return false;
	}
};

class WeaponInfo {
private:
	PAD(0x4);											// 0x0000

public:
	const char *m_weapon_name;						// 0x0004 -- actual weapon name, even for usp-s and revolver. ex: "weapon_revolver"
	PAD(0xC);												// 0x0008
	int               m_max_clip1;							// 0x0014
	int				  m_max_clip2;							// 0x0018
	int				  m_default_clip1;						// 0x001C
	int		          m_default_clip2;						// 0x0020
	int               m_max_reserve;						// 0x0024
	PAD(0x4);												// 0x0028
	const char *m_world_model;						// 0x002C
	const char *m_view_model;							// 0x0030
	const char *m_world_dropped_model;				// 0x0034
	PAD(0x48);											// 0x0038
	const char *m_ammo_type;							// 0x0080
	uint8_t           pad_0084[4];						// 0x0084
	const char *m_sfui_name;							// 0x0088
	const char *m_deprecated_weapon_name;				// 0x008C -- shitty weapon name, shows "weapon_deagle" for revolver / etc.
	uint8_t           pad_0090[56];						// 0x0090
	CSWeaponType      m_weapon_type;						// 0x00C8
	int			      m_in_game_price;						// 0x00CC
	int               m_kill_award;							// 0x00D0
	const char *m_animation_prefix;					// 0x00D4
	float			  m_cycletime;							// 0x00D8
	float			  m_cycletime_alt;						// 0x00DC
	float			  m_time_to_idle;						// 0x00E0
	float			  m_idle_interval;						// 0x00E4
	bool			  m_is_full_auto;						// 0x00E5
	PAD(0x3);												// 0x00E8
	int               m_damage;								// 0x00EC
	float             m_armor_ratio;						// 0x00F0
	int               m_bullets;							// 0x00F4
	float             m_penetration;						// 0x00F8
	float             m_flinch_velocity_modifier_large;		// 0x00FC
	float             m_flinch_velocity_modifier_small;		// 0x0100
	float             m_range;								// 0x0104
	float             m_range_modifier;						// 0x0108
	float			  m_throw_velocity;						// 0x010C
	PAD(0xC);												// 0x0118
	bool			  m_has_silencer;						// 0x0119
	PAD(0x3);												// 0x011C
	const char *m_silencer_model;						// 0x0120
	int				  m_crosshair_min_distance;				// 0x0124
	int				  m_crosshair_delta_distance;			// 0x0128
	float             m_max_player_speed;					// 0x012C
	float             m_max_player_speed_alt;				// 0x0130
	float			  m_spread;								// 0x0134
	float			  m_spread_alt;							// 0x0138
	float             m_inaccuracy_crouch;					// 0x013C
	float             m_inaccuracy_crouch_alt;				// 0x0140
	float             m_inaccuracy_stand;					// 0x0144
	float             m_inaccuracy_stand_alt;				// 0x0148
	float             m_inaccuracy_jump_initial;			// 0x014C
	float             m_inaccuracy_jump;					// 0x0150
	float             m_inaccuracy_jump_alt;				// 0x0154
	float             m_inaccuracy_land;					// 0x0158
	float             m_inaccuracy_land_alt;				// 0x015C
	float             m_inaccuracy_ladder;					// 0x0160
	float             m_inaccuracy_ladder_alt;				// 0x0164
	float             m_inaccuracy_fire;					// 0x0168
	float             m_inaccuracy_fire_alt;				// 0x016C
	float             m_inaccuracy_move;					// 0x0170
	float             m_inaccuracy_move_alt;				// 0x0174
	float             m_inaccuracy_reload;					// 0x0178
	int               m_recoil_seed;						// 0x017C
	float			  m_recoil_angle;						// 0x0180
	float             m_recoil_angle_alt;					// 0x0184
	float             m_recoil_angle_variance;				// 0x0188
	float             m_recoil_angle_variance_alt;			// 0x018C
	float             m_recoil_magnitude;					// 0x0190
	float             m_recoil_magnitude_alt;				// 0x0194
	float             m_recoil_magnitude_variance;			// 0x0198
	float             m_recoil_magnitude_variance_alt;		// 0x019C
	float             m_recovery_time_crouch;				// 0x01A0
	float             m_recovery_time_stand;				// 0x01A4
	float             m_recovery_time_crouch_final;			// 0x01A8
	float             m_recovery_time_stand_final;			// 0x01AC
	float             m_recovery_transition_start_bullet;	// 0x01B0
	float             m_recovery_transition_end_bullet;		// 0x01B4
	bool			  m_unzoom_after_shot;					// 0x01B5
	PAD(0x3);												// 0x01B8
	bool		      m_hide_view_model_zoomed;				// 0x01B9
	bool			  m_zoom_levels;						// 0x01BA
	PAD(0x2);												// 0x01BC
	int				  m_zoom_fov[2];						// 0x01C4
	float			  m_zoom_time[3];						// 0x01D0
	PAD(0x8);												// 0x01D8
	float             m_addon_scale;						// 0x01DC
	PAD(0x8);												// 0x01E4
	int				  m_tracer_frequency;					// 0x01E8
	int				  m_tracer_frequency_alt;				// 0x01EC
	PAD(0x18);											// 0x0200
	int				  m_health_per_shot;					// 0x0204
	PAD(0x8);												// 0x020C
	float			  m_inaccuracy_pitch_shift;				// 0x0210
	float			  m_inaccuracy_alt_sound_threshold;		// 0x0214
	float			  m_bot_audible_range;					// 0x0218
	PAD(0x8);												// 0x0220
	const char *m_wrong_team_msg;						// 0x0224
	bool			  m_has_burst_mode;						// 0x0225
	PAD(0x3);												// 0x0228
	bool			  m_is_revolver;						// 0x0229
	bool			  m_can_shoot_underwater;				// 0x022A
	PAD(0x2);												// 0x022C			
};

class IRefCounted {
private:
	volatile long refCount;

public:
	virtual void destructor(char bDelete) = 0;
	virtual bool OnFinalRelease() = 0;

	void unreference() {
		if (InterlockedDecrement(&refCount) == 0 && OnFinalRelease()) {
			destructor(1);
		}
	}
};

class Weapon : public Entity {
public:
	using ref_vec_t = CUtlVector< IRefCounted * >;

	// netvars / etc.
	__forceinline ref_vec_t &m_CustomMaterials() {
		return get< ref_vec_t >(g_entoffsets.m_CustomMaterials);
	}

	__forceinline ref_vec_t &m_CustomMaterials2() {
		return get< ref_vec_t >(g_entoffsets.m_CustomMaterials2);
	}

	__forceinline ref_vec_t &m_VisualsDataProcessors() {
		return get< ref_vec_t >(g_entoffsets.m_VisualsDataProcessors);
	}

	__forceinline bool &m_bCustomMaterialInitialized() {
		return get< bool >(g_entoffsets.m_bCustomMaterialInitialized);
	}

	__forceinline int &m_iItemDefinitionIndex() {
		return get< int >(g_entoffsets.m_iItemDefinitionIndex);
	}

	__forceinline int &m_iClip1() {
		return get< int >(g_entoffsets.m_iClip1);
	}

	__forceinline int &m_iPrimaryReserveAmmoCount() {
		return get< int >(g_entoffsets.m_iPrimaryReserveAmmoCount);
	}

	__forceinline int &m_Activity() {
		return get< int >(g_entoffsets.m_Activity);
	}

	__forceinline float &m_fFireDuration() {
		return get< float >(g_entoffsets.m_fFireDuration);
	}

	__forceinline int &m_iBurstShotsRemaining() {
		return get< int >(g_entoffsets.m_iBurstShotsRemaining);
	}

	__forceinline float &m_flNextPrimaryAttack() {
		return get< float >(g_entoffsets.m_flNextPrimaryAttack);
	}

	__forceinline float &m_flNextSecondaryAttack() {
		return get< float >(g_entoffsets.m_flNextSecondaryAttack);
	}

	__forceinline float &m_flThrowStrength() {
		return get< float >(g_entoffsets.m_flThrowStrength);
	}

	__forceinline float &m_fNextBurstShot() {
		return get< float >(g_entoffsets.m_fNextBurstShot);
	}

	__forceinline int &m_zoomLevel() {
		return get< int >(g_entoffsets.m_zoomLevel);
	}

	__forceinline float &m_flRecoilIndex() {
		return get< float >(g_entoffsets.m_flRecoilIndex);
	}

	__forceinline int &m_weaponMode() {
		return get< int >(g_entoffsets.m_weaponMode);
	}

	__forceinline int &m_nFallbackPaintKit() {
		return get< int >(g_entoffsets.m_nFallbackPaintKit);
	}

	__forceinline int &m_nFallbackStatTrak() {
		return get< int >(g_entoffsets.m_nFallbackStatTrak);
	}

	__forceinline int &m_nFallbackSeed() {
		return get< int >(g_entoffsets.m_nFallbackSeed);
	}

	__forceinline float &m_flFallbackWear() {
		return get< float >(g_entoffsets.m_flFallbackWear);
	}

	__forceinline int &m_iViewModelIndex() {
		return get< int >(g_entoffsets.m_iViewModelIndex);
	}

	__forceinline int &m_iWorldModelIndex() {
		return get< int >(g_entoffsets.m_iWorldModelIndex);
	}

	__forceinline int &m_iAccountID() {
		return get< int >(g_entoffsets.m_iAccountID);
	}

	__forceinline int &m_iItemIDHigh() {
		return get< int >(g_entoffsets.m_iItemIDHigh);
	}

	__forceinline int &m_iEntityQuality() {
		return get< int >(g_entoffsets.m_iEntityQuality);
	}

	__forceinline int &m_OriginalOwnerXuidLow() {
		return get< int >(g_entoffsets.m_OriginalOwnerXuidLow);
	}

	__forceinline int &m_OriginalOwnerXuidHigh() {
		return get< int >(g_entoffsets.m_OriginalOwnerXuidHigh);
	}

	__forceinline bool &m_bPinPulled() {
		return get< bool >(g_entoffsets.m_bPinPulled);
	}

	__forceinline float &m_fThrowTime() {
		return get< float >(g_entoffsets.m_fThrowTime);
	}

	__forceinline EHANDLE &m_hWeapon() {
		return get< EHANDLE >(g_entoffsets.m_hWeapon);
	}

	__forceinline EHANDLE &m_hWeaponWorldModel() {
		return get< EHANDLE >(g_entoffsets.m_hWeaponWorldModel);
	}

	__forceinline EHANDLE &m_hOwnerEntity() {
		return get< EHANDLE >(g_entoffsets.m_hOwnerEntity);
	}

	__forceinline float &m_flConstraintRadius() {
		return get< float >(g_entoffsets.m_flConstraintRadius);
	}

	__forceinline float &m_fLastShotTime() {
		return get< float >(g_entoffsets.m_fLastShotTime);
	}

public:
	enum indices : size_t {
		SETMODELINDEX = 75,
		GETMAXCLIP1 = 367,
		GETSPREAD = 439,
		GETWPNDATA = 446, // C_WeaponCSBaseGun::GetCSWpnData
		GETINACCURACY = 469,
		UPDATEACCURACYPENALTY = 471,
	};

public:
	// virtuals.
	__forceinline int GetMaxClip1() {
		return util::get_method< int(__thiscall *)(void *) >(this, GETMAXCLIP1)(this);
	}

	__forceinline void SetGloveModelIndex(int index) {
		return util::get_method< void(__thiscall *)(void *, int) >(this, SETMODELINDEX)(this, index);
	}

	__forceinline WeaponInfo *GetWpnData() {
		return util::get_method< WeaponInfo *(__thiscall *)(void *) >(this, GETWPNDATA)(this);
	}

	__forceinline float GetInaccuracy() {
		return util::get_method< float(__thiscall *)(void *) >(this, GETINACCURACY)(this);
	}

	__forceinline float GetSpread() {
		return util::get_method< float(__thiscall *)(void *) >(this, GETSPREAD)(this);
	}

	__forceinline void UpdateAccuracyPenalty() {
		return util::get_method< void(__thiscall *)(void *) >(this, UPDATEACCURACYPENALTY)(this);
	}

	// misc funcs.
	__forceinline Weapon *GetWeapon() {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Weapon * >(m_hWeapon());
	}

	__forceinline Weapon *GetWeaponWorldModel() {
		return g_csgo.m_entlist->GetClientEntityFromHandle< Weapon * >(m_hWeaponWorldModel());
	}

	__forceinline bool is_grenade() {
		short idx = this->m_iItemDefinitionIndex();
		return idx >= FLASHBANG && idx <= FIREBOMB;
	}

	__forceinline bool is_misc_weapon() {
		auto info = this->GetWpnData();
		if (!info)
			return false;

		short idx = this->m_iItemDefinitionIndex();
		int type = info->m_weapon_type;
		return idx == ZEUS || type == WEAPONTYPE_KNIFE || type >= WEAPONTYPE_C4 && type <= WEAPONTYPE_MELEE;
	}


	__forceinline bool is_taser() {
		auto info = this->GetWpnData();
		if (!info)
			return false;

		short idx = this->m_iItemDefinitionIndex();
		int type = info->m_weapon_type;
		return idx == ZEUS;
	}

	__forceinline bool IsKnife() {
		return (GetWpnData()->m_weapon_type == WEAPONTYPE_KNIFE && m_iItemDefinitionIndex() != ZEUS);
	}

	__forceinline bool is_sniper() {
		short idx = this->m_iItemDefinitionIndex();

		switch (idx) {
		case AWP:
		case G3SG1:
		case SCAR20:
		case SSG08:
			return true;
		default:
			return false;
		}
	}

	__forceinline bool is_scoping_weapon() {
		short idx = this->m_iItemDefinitionIndex();

		switch (idx) {
		case AWP:
		case G3SG1:
		case SCAR20:
		case SSG08:
		case SG553:
		case AUG:
			return true;
		default:
			return false;
		}
	}

	__forceinline vec3_t CalculateSpread(int seed, float inaccuracy, float spread, bool revolver2 = false) {
		WeaponInfo *wep_info;
		int        item_def_index;
		float      recoil_index, r1, r2, r3, r4, s1, c1, s2, c2;

		// if we have no bullets, we have no spread.
		wep_info = GetWpnData();
		if (!wep_info || !wep_info->m_bullets)
			return {};

		// get some data for later.
		item_def_index = m_iItemDefinitionIndex();
		recoil_index = m_flRecoilIndex();

		// seed randomseed.
		g_csgo.RandomSeed((seed & 0xff) + 1);

		// generate needed floats.
		r1 = g_csgo.RandomFloat(0.f, 1.f);
		r2 = g_csgo.RandomFloat(0.f, math::pi_2);

		// todo - dex; need to make sure this is right for shotguns still.
		//             the 3rd arg to get_shotgun_spread is actually using the bullet iterator
		//             should also probably check for if the weapon is a shotgun, but it seems like GetShotgunSpread modifies some the r1 - r4 vars...
		// for( int i{}; i < wep_info->m_bullets; ++i )
		/*
			// shotgun shit, or first bullet
			if ( !bullet_i
			  || ((int (__thiscall *)(void ***))weapon_accuracy_shotgun_spread_patterns[13])(&weapon_accuracy_shotgun_spread_patterns) )
			{
			  r1 = RandomFloat(0, 0x3F800000);// rand 0.f, 1.f
			  r2 = RandomFloat(0, 0x40C90FDB);// rand 0.f, pi * 2.f
			  v47 = *(_DWORD *)v45;
			  r2_ = r2;
			  v48 = (*(int (__thiscall **)(int))(v47 + 48))(v45);

			  // not revolver?
			  if ( v48 != sub_101D9B10(&dword_14FA0DE0) || a7 != 1 )
			  {

				// not negev?
				v50 = (*(int (__thiscall **)(int))(*(_DWORD *)v45 + 48))(v45);
				if ( v50 != sub_101D9B10(&dword_14FA0DEC) || recoil_index >= 3.0 )
				{
				  r1_ = r1;
				}
				else
				{
				  r1__ = r1;
				  recoil_index_it1 = 3;
				  do
				  {
					--recoil_index_it1;
					r1__ = r1__ * r1__;
				  }
				  while ( (float)recoil_index_it1 > recoil_index );
				  r1_ = 1.0 - r1__;
				}
			  }
			  else
			  {
				r1_ = 1.0 - (float)(r1 * r1);
			  }

			  // some convar stuff / etc
			  if ( max_inaccuracy )
				r1_ = 1.0;

			  bullet_i = v87;
			  bullet_i1 = r1_ * v104;

			  if ( only_up_is_not_0 )
				v53 = 1.5707964;
			  else
				v53 = r2_;

			  r2_ = v53;
			}

			if ( ((int (__thiscall *)(void ***))weapon_accuracy_shotgun_spread_patterns[13])(&weapon_accuracy_shotgun_spread_patterns) )
			{
			  null0 = v54;
			  m_iItemDefinitionIndex = (**(int (***)(void))LODWORD(v88))();
			  get_shotgun_spread(
				m_iItemDefinitionIndex,
				null0,
				bullet_i + m_BulletsPerShot * (signed int)recoil_index,
				&r4,
				&r3);
			}
			else
			{
			  r3 = RandomFloat(0, 0x3F800000);// rand 0.f, 1.f
			  r4 = RandomFloat(0, 0x40C90FDB);// rand 0.f, pi * 2.f
			}
		*/

		if ( /*wep_info->m_weapon_type == WEAPONTYPE_SHOTGUN &&*/ g_csgo.weapon_accuracy_shotgun_spread_patterns->GetInt() > 0)
			g_csgo.GetShotgunSpread(item_def_index, 0, 0 /*bullet_i*/ + wep_info->m_bullets * recoil_index, &r4, &r3);

		else {
			r3 = g_csgo.RandomFloat(0.f, 1.f);
			r4 = g_csgo.RandomFloat(0.f, math::pi_2);
		}

		// revolver secondary spread.
		if (item_def_index == REVOLVER && revolver2) {
			r1 = 1.f - (r1 * r1);
			r3 = 1.f - (r3 * r3);
		}

		// negev spread.
		else if (item_def_index == NEGEV && recoil_index < 3.f) {
			for (int i{ 3 }; i > recoil_index; --i) {
				r1 *= r1;
				r3 *= r3;
			}

			r1 = 1.f - r1;
			r3 = 1.f - r3;
		}

		// get needed sine / cosine values.
		c1 = std::cos(r2);
		c2 = std::cos(r4);
		s1 = std::sin(r2);
		s2 = std::sin(r4);

		// calculate spread vector.
		return {
			(c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
			(s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
			0.f
		};
	}

	__forceinline vec3_t CalculateSpread(int seed, bool revolver2 = false) {
		return CalculateSpread(seed, GetInaccuracy(), GetSpread(), revolver2);
	}

	vec2_t calc_spread_angle(int bullets, float recoil_index, int i, float innacuracy_p, float spread_p)
	{
		auto index = this->m_iItemDefinitionIndex();

		math::random_seed(i + 1u);

		auto v1 = math::random_float(0.f, 1.f);
		auto v2 = math::random_float(0.f, M_PI * 2.f);

		float v3{ }, v4{ };
		if (g_csgo.weapon_accuracy_shotgun_spread_patterns->GetInt() > 0)
			g_csgo.GetShotgunSpread(index, 0, static_cast<int>(bullets * recoil_index), &v4, &v3);
		else
		{
			v3 = math::random_float(0.f, 1.f);
			v4 = math::random_float(0.f, M_PI * 2.f);
		}

		if (recoil_index < 3.f && index == NEGEV)
		{
			for (auto i = 3; i > recoil_index; --i)
			{
				v1 *= v1;
				v3 *= v3;
			}

			v1 = 1.f - v1;
			v3 = 1.f - v3;
		}

		const auto inaccuracy = v1 * innacuracy_p;
		const auto spread = v3 * spread_p;

		return { std::cos(v2) * inaccuracy + std::cos(v4) * spread, std::sin(v2) * inaccuracy + std::sin(v4) * spread };
	}

	__forceinline std::string GetLocalizedName() {
		C_EconItemView *item_view;
		CEconItemDefinition *item_def;

		item_view = g_csgo.GetEconItemView(this);
		if (!item_view)
			return XOR("error");

		item_def = g_csgo.GetStaticData(item_view);
		if (!item_def)
			return XOR("error");

		return util::WideToMultiByte(g_csgo.m_localize->Find(item_def->GetItemBaseName()));
	}
};

class CTraceFilterSimple_game {
public:
	void *m_vmt;
	const Entity *m_pass_ent1;
	int             m_collision_group;
	ShouldHitFunc_t m_shouldhit_check_fn;

public:
	__forceinline CTraceFilterSimple_game() :
		m_vmt{ g_csgo.CTraceFilterSimple_vmt.as< void * >() },
		m_pass_ent1{},
		m_collision_group{},
		m_shouldhit_check_fn{} {}

	__forceinline CTraceFilterSimple_game(const Entity *pass_ent1, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr) :
		m_vmt{ g_csgo.CTraceFilterSimple_vmt.as< void * >() },
		m_pass_ent1{ pass_ent1 },
		m_collision_group{ collision_group },
		m_shouldhit_check_fn{ shouldhit_check_fn } {}

	__forceinline bool ShouldHitEntity(Entity *entity, int contents_mask) {
		// note - dex; game is dumb, this gets the real vmt.
		void *real_vmt = *(void **)m_vmt;

		return util::get_method< bool(__thiscall *)(void *, Entity *, int) >(real_vmt, 0)(real_vmt, entity, contents_mask);
	}

	// note - dex; don't really care about calling the virtuals for these two functions, they only set members in the class for us.
	__forceinline void SetPassEntity(Entity *pass_ent1) {
		m_pass_ent1 = pass_ent1;

		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 2 )( m_vmt, pass_ent1 );
	}

	__forceinline void SetCollisionGroup(int collision_group) {
		m_collision_group = collision_group;

		// util::get_method< void (__thiscall *)( void *, int ) >( m_vmt, 3 )( m_vmt, collision_group );
	}
};

class CTraceFilterSkipTwoEntities_game {
public:
	void *m_vmt;
	const Entity *m_pass_ent1;
	int             m_collision_group;
	ShouldHitFunc_t m_shouldhit_check_fn;
	const Entity *m_pass_ent2;

public:
	__forceinline CTraceFilterSkipTwoEntities_game() :
		m_vmt{ g_csgo.CTraceFilterSkipTwoEntities_vmt.as< void * >() },
		m_pass_ent1{},
		m_collision_group{},
		m_shouldhit_check_fn{},
		m_pass_ent2{} {}

	__forceinline CTraceFilterSkipTwoEntities_game(const Entity *pass_ent1, const Entity *pass_ent2, int collision_group = COLLISION_GROUP_NONE, ShouldHitFunc_t shouldhit_check_fn = nullptr) :
		m_vmt{ g_csgo.CTraceFilterSimple_vmt.as< void * >() },
		m_pass_ent1{ pass_ent1 },
		m_collision_group{ collision_group },
		m_shouldhit_check_fn{ shouldhit_check_fn },
		m_pass_ent2{ pass_ent2 } {}

	__forceinline bool ShouldHitEntity(Entity *entity, int contents_mask) {
		// note - dex; game is dumb, this gets the real vmt.
		void *real_vmt = *(void **)m_vmt;

		return util::get_method< bool(__thiscall *)(void *, Entity *, int) >(m_vmt, 0)(m_vmt, entity, contents_mask);
	}

	// note - dex; don't really care about calling the virtuals for these two functions, they only set members in the class for us.
	__forceinline void SetPassEntity(Entity *pass_ent1) {
		m_pass_ent1 = pass_ent1;

		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 2 )( m_vmt, pass_ent1 );
	}

	__forceinline void SetCollisionGroup(int collision_group) {
		m_collision_group = collision_group;

		// util::get_method< void (__thiscall *)( void *, int ) >( m_vmt, 3 )( m_vmt, collision_group );
	}

	__forceinline void SetPassEntity2(Entity *pass_ent2) {
		m_pass_ent2 = pass_ent2;
		// util::get_method< void (__thiscall *)( void *, Entity* ) >( m_vmt, 4 )( m_vmt, pass_ent2 );
	}
};