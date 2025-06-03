#include "includes.h"

void Hooks::PaintTraverse(VPANEL panel, bool repaint, bool force) {
	std::string panel_name = g_csgo.m_panel->GetName(panel);
	if (strstr(g_csgo.m_panel->GetName(panel), XOR("HudZoom")))
		if (g_menu.main.visuals.noscope.get())
			return;

	if (!strcmp(g_csgo.m_panel->GetName(panel), "GameConsole") || !strcmp(g_csgo.m_panel->GetName(panel), "CompletionList")) {
		g_cl.m_console_open = true;
		g_hooks.m_panel.GetOldMethod< PaintTraverse_t >(IPanel::PAINTTRAVERSE)(this, panel, repaint, force);;
		g_cl.m_console_open = false;
		return;
	}

	g_hooks.m_panel.GetOldMethod< PaintTraverse_t >(IPanel::PAINTTRAVERSE)(this, panel, repaint, force);

	static auto draw_panel_id = 0u;

	if (!draw_panel_id) {
		if (panel_name.compare(XOR("MatSystemTopPanel")))
			return;

		draw_panel_id = panel;
	}

	if (panel != draw_panel_id)
		return;

	g_cl.OnPaint();

	g_visuals.drawMolotovHull();
}