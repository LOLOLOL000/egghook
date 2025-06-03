#pragma once

struct mstudiobbox_t {
	int     m_bone;                 // 0x0000
	int     m_group;                // 0x0004
	vec3_t  m_mins;                 // 0x0008
	vec3_t  m_maxs;                 // 0x0014
	int     m_name_id;				// 0x0020
	ang_t   m_angle;                // 0x0024
	float   m_radius;               // 0x0030
	PAD( 0x10 );                    // 0x0034
};

struct mstudiohitboxset_t {
	int	m_name_id;
	int	m_hitboxes;
	int	m_hitbox_id;

	__forceinline mstudiobbox_t* GetHitbox( int index ) const { 
		return ( mstudiobbox_t* )( ( ( byte* )this ) + m_hitbox_id ) + index; 
	};
};

struct mstudiobone_t {
	int m_bone_name_index;
	int m_parent;
	PAD( 0x98 );
	int m_flags;
	PAD( 0x34 );
};

struct studiohdr_t {
	int    m_id;					// 0x0000
	int    m_version;				// 0x0004
	int    m_checksum;				// 0x0008
	char   m_name[ 64 ];			// 0x000C
	int    m_length;				// 0x004C
	vec3_t m_eye_pos;				// 0x0050
	vec3_t m_illum_pos;				// 0x005C
	vec3_t m_hull_mins;				// 0x0068
	vec3_t m_hull_maxs;             // 0x0074
	vec3_t m_view_mins;             // 0x0080
	vec3_t m_view_maxs;             // 0x008C
	int    m_flags;					// 0x0098
	int    m_num_bones;				// 0x009C
	int    m_bone_id;				// 0x00A0
	int    m_num_controllers;		// 0x00A4
	int    m_controller_id;			// 0x00A8
	int    m_num_sets;				// 0x00AC
	int    m_set_id;				// 0x00B0

	__forceinline mstudiobone_t* GetBone( int index ) const {
		return ( mstudiobone_t* )( ( ( byte* )this ) + m_bone_id ) + index;
	}

	__forceinline mstudiohitboxset_t* GetHitboxSet( int index ) const {
		return ( mstudiohitboxset_t* )( ( ( byte* )this ) + m_set_id ) + index;
	}

	__forceinline mstudiobbox_t* GetHitbox(int hitbox, int set) const {
		const mstudiohitboxset_t* hitboxSet = GetHitboxSet(set);

		if (hitboxSet == nullptr)
			return nullptr;

		return hitboxSet->GetHitbox(hitbox);
	}
};

class model_t {
public:
	std::uint32_t  m_handle;
	char   m_name[ 260 ];
	int    m_load_flags;
	int    m_server_count;
	int    m_type;
	int    m_flags;
	vec3_t m_mins;
	vec3_t m_maxs;
	float  m_radius;
};

class IVModelInfo {
public:
	// indexes for virtuals and hooks.
	enum indices : size_t {
        GETMODEL           = 1,
		GETMODELINDEX      = 2,
        GETMODELFRAMECOUNT = 8,
		GETSTUDIOMODEL     = 30,
        FINDORLOADMODEL    = 43
	};

public:
    __forceinline const model_t *GetModel( int modelindex ) {
        return util::get_method< const model_t *(__thiscall *)( void *, int ) >( this, GETMODEL )( this, modelindex );
    }

	__forceinline int GetModelIndex( const char* model ) {
		return util::get_method< int( __thiscall* )( void*, const char* ) >( this, GETMODELINDEX )( this, model );
	}

    __forceinline int GetModelFrameCount( const model_t *model ) {
        return util::get_method< int( __thiscall* )( void*, const model_t * ) >( this, GETMODELFRAMECOUNT )( this, model );
    }

	__forceinline studiohdr_t* GetStudioModel( const model_t *model ) {
		return util::get_method< studiohdr_t*( __thiscall* )( void*, const model_t* ) >( this, GETSTUDIOMODEL )( this, model );
	}

    __forceinline const model_t *FindOrLoadModel( const char *name ) {
        return util::get_method< const model_t *( __thiscall* )( void*, const char * ) >( this, FINDORLOADMODEL )( this, name );
    }
};

class IMaterial;
struct studio_lod_data_t
{
	void* mesh_data{ };
	float switch_point{ };
	int num_materials{ };
	IMaterial** material_pointers{ };
	int* material_flags{ };
	int* morph_decal_bone_map{ };
	int decal_bone_count{ };
};

struct studio_hw_data_t
{
	int root_lods{ };
	int num_lods{ };

	studio_lod_data_t* lods{ };

	int studio_meshes{ };

	inline float get_lod_metrics(float sphere_size) const
	{
		return (sphere_size != 0.f) ? (100.f / sphere_size) : 0.f;
	}

	inline int get_lod_for_metric(float lod_metric) const
	{
		if (!num_lods)
			return 0;

		int num_lods = (lods[num_lods - 1].switch_point < 0.0f) ? num_lods - 1 : num_lods;

		for (int i = root_lods; i < num_lods - 1; i++)
		{
			if (lods[i + 1].switch_point > lod_metric)
				return i;
		}

		return num_lods - 1;
	}
};


using create_interface_fn = void* (*)(const char* name, int* return_code);
class c_app_system
{
public:
	virtual bool connect(create_interface_fn factory) = 0;
	virtual void disconnect() = 0;
	virtual void* query_interface(const char* interface_name) = 0;
	virtual int init() = 0;
	virtual void shutdown() = 0;
	virtual const void* get_dependencies() = 0;
	virtual int get_tier() = 0;
	virtual void reconnect(create_interface_fn factory, const char* interface_name) = 0;
	virtual void unkfunc() = 0;
};

class c_mdl_cache : public c_app_system
{
public:
	__forceinline std::uint16_t lookup_hw_data(std::uint16_t handle) {
		return util::get_method<std::uintptr_t(__thiscall*)(decltype(this), std::uint16_t)>(this, 15)(this, handle);
	}
};