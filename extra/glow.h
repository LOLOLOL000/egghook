#pragma once

#define END_OF_FREE_LIST	-1
#define ENTRY_IN_USE		-2

struct GlowObjectDefinition_t
{
	GlowObjectDefinition_t() { memset(this, 0, sizeof(*this)); }

	Entity* m_pEntity;    //0x0000
	union
	{
		vec3_t m_vGlowColor;           //0x0004
		struct
		{
			float   m_flRed;           //0x0004
			float   m_flGreen;         //0x0008
			float   m_flBlue;          //0x000C
		};
	};
	float   m_flAlpha;                 //0x0010
	uint8_t pad_0014[4];               //0x0014
	float   m_flSomeFloat;             //0x0018
	uint8_t pad_001C[4];               //0x001C
	float   m_flAnotherFloat;          //0x0020
	bool    m_bRenderWhenOccluded;     //0x0024
	bool    m_bRenderWhenUnoccluded;   //0x0025
	bool    m_bFullBloomRender;        //0x0026
	uint8_t pad_0027[5];               //0x0027
	int32_t m_nGlowStyle;              //0x002C
	int32_t m_nSplitScreenSlot;        //0x0030
	int32_t m_nNextFreeSlot;           //0x0034

	bool IsUnused() const { return m_nNextFreeSlot != ENTRY_IN_USE; }
};

/*struct GlowObjectDefinition_t {
	Entity *m_entity;
	vec3_t m_color;
	float  m_alpha;
	PAD( 0x8 );
	float  m_bloom_amount;
	PAD( 0x4 );
	bool   m_render_occluded;
	bool   m_render_unoccluded;
	bool   m_render_full_bloom;
	PAD( 0x1 );
	int    m_full_bloom_stencil;
	PAD( 0x4 );
	int    m_slot;
	int    m_next_free_slot;
};*/

class CGlowObjectManager {
public:
	CUtlVector< GlowObjectDefinition_t >	m_object_definitions;
	int										m_first_free_slot;
};