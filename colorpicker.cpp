#include "includes.h"

// static variables.
int Colorpicker::texture = 0;;
std::unique_ptr< Color[] > Colorpicker::gradient = nullptr;;

void Colorpicker::init( ) {
	const int w{ COLORPICKER_PICKER_SIZE };
	const int h{ COLORPICKER_PICKER_SIZE };

	// should never happen.
	if( gradient )
		return;

	// allocate.
	gradient = std::make_unique< Color[] >( w * h );

	// init.
	float hue{}, sat{ 0.99f }, lum{ 1.f };

	// iterate width.
	for( int i{}; i < w; ++i ) {

		// iterate height.
		for( int j{}; j < h; ++j ) {

			// write back to array.
			*( Color* )( gradient.get( ) + i + j * w ) = Color::hsl_to_rgb( hue, sat, lum );

			hue += ( 1.f / w );
		}

		lum -= ( 1.f / h );
		hue = 0.f;
	}

	// allocate new texture in engine.
	texture = g_csgo.m_surface->CreateNewTextureID( true );

	// assign allocated memory containing the picker to said texture.
	g_csgo.m_surface->DrawSetTextureRGBA( texture, gradient.get( ), w, h );
}

void Colorpicker::draw( ) {
	Rect  area{ m_parent->GetElementsRect( ) };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	// label.
	render::menu_shade.string( p.x + LABEL_OFFSET, p.y - 2, { 205, 205, 205, m_parent->m_alpha }, m_label );

	// outline.
	render::rect( p.x + m_w - COLORPICKER_WIDTH, p.y, COLORPICKER_WIDTH, COlORPICKER_HEIGHT, { 0, 0, 0, m_parent->m_alpha } );

	Color preview = m_color;
	Color preview2 = m_color;
	preview.a() = m_parent->m_alpha;

	render::rect_filled(p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 1, COLORPICKER_WIDTH - 2, COlORPICKER_HEIGHT - 2, preview);
	render::rect_filled_fade(p.x + m_w - COLORPICKER_WIDTH + 1, p.y + 1, COLORPICKER_WIDTH - 2, COlORPICKER_HEIGHT - 2, { 50, 50, 35, m_parent->m_alpha }, 0, 150);

	if (m_open) {

		p.y += 8;

		render::rect_filled(p.x + m_w - COLORPICKER_WIDTH - 3, p.y + COlORPICKER_HEIGHT + 2 - 3,
			COLORPICKER_PICKER_SIZE + 6, COLORPICKER_PICKER_SIZE + 15 + 6, { 43, 43, 42, m_parent->m_alpha });

		//render::rect( p.x + m_w - COLORPICKER_WIDTH - 3, p.y + COlORPICKER_HEIGHT + 2 - 3,
		//					 COLORPICKER_PICKER_SIZE + 6, COLORPICKER_PICKER_SIZE + 6, { 0, 0, 0, m_parent->m_alpha } );

		auto m_x = p.x + m_w - COLORPICKER_WIDTH - 6;
		auto m_y = p.y + COlORPICKER_HEIGHT + 2 - 6;
		auto m_width = COLORPICKER_PICKER_SIZE + 12;
		auto m_height = COLORPICKER_PICKER_SIZE + 12 + 15;
		auto m_height2 = COLORPICKER_PICKER_SIZE + 12;
		auto m_alpha = m_parent->m_alpha;


		render::rect(m_x, m_y, m_width, m_height2, { 5, 5, 5, m_alpha });
		render::rect(m_x + 1, m_y + 1, m_width - 2, m_height2 - 2, { 60, 60, 60, m_alpha });
		render::rect(m_x + 2, m_y + 2, m_width - 4, m_height2 - 4, { 40, 40, 40, m_alpha });
		render::rect(m_x + 3, m_y + 3, m_width - 6, m_height2 - 6, { 40, 40, 40, m_alpha });
		render::rect(m_x + 4, m_y + 4, m_width - 8, m_height2 - 8, { 40, 40, 40, m_alpha });
		render::rect(m_x + 5, m_y + 5, m_width - 10, m_height2 - 10, { 60, 60, 60, m_alpha });

		render::rect(m_x, m_y, m_width, m_height, { 5, 5, 5, m_alpha });
		render::rect(m_x + 1, m_y + 1, m_width - 2, m_height - 2, { 60, 60, 60, m_alpha });
		render::rect(m_x + 2, m_y + 2, m_width - 4, m_height - 4, { 40, 40, 40, m_alpha });
		render::rect(m_x + 3, m_y + 3, m_width - 6, m_height - 6, { 40, 40, 40, m_alpha });
		render::rect(m_x + 4, m_y + 4, m_width - 8, m_height - 8, { 40, 40, 40, m_alpha });
		render::rect(m_x + 5, m_y + 5, m_width - 10, m_height - 10, { 60, 60, 60, m_alpha });


		/* alpha bar */
		int fill_width = static_cast<int>((m_width - 14) * (static_cast<float>(m_color.a()) / 255.0f));

		render::rect_filled(m_x + 7, p.y + COLORPICKER_PICKER_SIZE + 17, fill_width, SLIDER_HEIGHT - 2, preview2);
		render::rect_filled_fade(m_x + 7, p.y + COLORPICKER_PICKER_SIZE + 17, m_width - 14, SLIDER_HEIGHT - 2, { 50, 50, 35, m_parent->m_alpha }, 0, 150);

		int text_x = m_x + fill_width;
		render::menu_shade.string(text_x, p.y + COLORPICKER_PICKER_SIZE + 18, { 205, 205, 205, m_parent->m_alpha }, std::to_string(m_color.a()));

		g_csgo.m_surface->DrawSetColor(colors::white);
		g_csgo.m_surface->DrawSetTexture(texture);
		g_csgo.m_surface->DrawTexturedRect(p.x + m_w - COLORPICKER_WIDTH,
			p.y + COlORPICKER_HEIGHT + 2,
			p.x + m_w - COLORPICKER_WIDTH + COLORPICKER_PICKER_SIZE,
			p.y + COlORPICKER_HEIGHT + 2 + COLORPICKER_PICKER_SIZE);

	}
}

void Colorpicker::think( ) {
	Rect  area{ m_parent->GetElementsRect( ) };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	if (m_open) {
		Rect picker{ p.x + m_w - COLORPICKER_WIDTH, p.y + COlORPICKER_HEIGHT + 2, COLORPICKER_PICKER_SIZE, COLORPICKER_PICKER_SIZE };
		auto m_x = p.x + m_w - COLORPICKER_WIDTH - 6;
		auto m_y = p.y + COlORPICKER_HEIGHT + 2 - 6;
		auto m_width = COLORPICKER_PICKER_SIZE + 12;
		auto m_height = COLORPICKER_PICKER_SIZE + 12 + 15;
		auto m_height2 = COLORPICKER_PICKER_SIZE + 12;
		auto m_alpha = m_parent->m_alpha;
		Rect alpha_bar{ m_x + 6, p.y + COLORPICKER_PICKER_SIZE + 24, m_width - 12, SLIDER_HEIGHT - 2 };

		if (g_input.IsCursorInRect(picker) && g_input.GetKeyState(VK_LBUTTON)) {
			/* grab the color we want to update to and store it */
			auto color_to_upd = Colorpicker::ColorFromPos(g_input.m_mouse.x - (p.x + m_w - COLORPICKER_WIDTH), g_input.m_mouse.y - (p.y + COlORPICKER_HEIGHT + 2));

			/* set the color that we stored and update it to main color var, but dont update the color alpha */
			m_color = { color_to_upd.r(), color_to_upd.g(), color_to_upd.b(), m_color.a() };

			/* kepp the window prioritized */
			m_parent->m_active_element = this;
		}

		if (g_input.IsCursorInRect(alpha_bar) && g_input.GetKeyState(VK_LBUTTON)) {
			int input_value = g_input.m_mouse.x - (m_x + 6);
			int max_input_range = m_width - 12;
			int mapped_value = static_cast<int>(255.0f * (input_value / static_cast<float>(max_input_range)));
			m_color.a() = std::max(0, std::min(255, mapped_value));

			/* clamp this shit since our input system is alchoolic */
			if (m_color.a() <= 1) {
				m_color.a() = 0;
			}

			/* clamp this shit since our input system is alchoolic */
			if (m_color.a() >= 250) {
				m_color.a() = 255;
			}

			/* kepp the window prioritized */
			m_parent->m_active_element = this;
		}

		if (m_parent->m_active_element != this) {
			/* close this shit up */
			m_open = false;

			if (m_callback)
				m_callback();
		}
	}

	if (m_ptr)
		*m_ptr = m_color;
}

void Colorpicker::click( ) {
	Rect  area{ m_parent->GetElementsRect() };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	Rect preview{ p.x + m_w - COLORPICKER_WIDTH, p.y, COLORPICKER_WIDTH, COlORPICKER_HEIGHT };
	Rect picker{ p.x + m_w - COLORPICKER_WIDTH, p.y + COlORPICKER_HEIGHT + 2, COLORPICKER_PICKER_SIZE, COLORPICKER_PICKER_SIZE };

	if (g_input.GetKeyPress(VK_LBUTTON) && g_input.IsCursorInRect(preview))
		m_open = true;
	else if (g_input.GetKeyPress(VK_LBUTTON) && !g_input.IsCursorInRect(picker))
		m_open = false;
}