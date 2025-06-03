#include "includes.h"

void Hooks::packet_end()
{
	if (!g_cl.m_local || !g_cl.m_local->alive())
		return g_hooks.m_client_state.GetOldMethod< PacketEnd_t >(CClientState::PACKETEND)(this);

	auto clientstate = (CClientState*)this;

	if (clientstate->clock_drift_mgr.cur_clock_offset == clientstate->clock_drift_mgr.server_tick)
	{
		auto ack_cmd = clientstate->last_command_ack;

		auto correct = std::find_if(g_hvh.choked_ticks.begin(), g_hvh.choked_ticks.end(), [&ack_cmd](const choked_ticks_t& other_data) { return other_data.cmd == ack_cmd; });
		if (correct != g_hvh.choked_ticks.end())
		{
			if (g_cl.velocity_modifier > g_cl.m_local->velocity_modifier() + 0.1f)
			{
				auto weapon = g_cl.m_weapon;

				if (!weapon || weapon && weapon->m_iItemDefinitionIndex() != REVOLVER && !weapon->is_grenade())
				{
					for (auto& number : g_hvh.choked_commands)
					{
						auto cmd = &g_csgo.m_input->m_commands[number % 150];
						auto verified = &g_csgo.m_input->m_verified[number % 150];

						if (cmd->m_buttons & (IN_ATTACK | IN_ATTACK2))
						{
							cmd->m_buttons &= ~IN_ATTACK;

							verified->m_cmd = *cmd;
							verified->m_crc = cmd->get_check_sum();
						}
					}
				}
			}

			if (g_cl.velocity_modifier != g_cl.m_local->velocity_modifier())
				g_cl.velocity_modifier = g_cl.m_local->velocity_modifier();
		}
	}

	return g_hooks.m_client_state.GetOldMethod< PacketEnd_t >(CClientState::PACKETEND)(this);
}

void Hooks::packet_start(int incoming, int outgoing)
{
	if(!g_cl.m_local || !g_cl.m_local->alive())
		return g_hooks.m_client_state.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, incoming , outgoing);

	auto& cmd = g_hvh.commands;
	if (cmd.empty())
		return g_hooks.m_client_state.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, incoming, outgoing);

	for (auto it = cmd.rbegin(); it != cmd.rend(); ++it)
	{
		if (!it->outgoing)
			continue;

		if (it->cmd == outgoing || outgoing > it->cmd && (!it->used || it->prev_cmd == outgoing))
		{
			it->prev_cmd = outgoing;
			it->used = true;
			g_hooks.m_client_state.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, incoming, outgoing);
			break;
		}
	}

	auto result = false;

	for (auto it = cmd.begin(); it != cmd.end(); )
	{
		if (outgoing == it->cmd || outgoing == it->prev_cmd)
			result = true;

		if (outgoing > it->cmd && outgoing > it->prev_cmd)
			it = cmd.erase(it);
		else
			++it;
	}

	if (!result)
		g_hooks.m_client_state.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, incoming, outgoing);
}

bool Hooks::TempEntities( void *msg ) {
	
	CClientState* state = (CClientState*)this;

	auto old_max_clients = state->max_clients;
	state->max_clients = 1;
	const bool ret = g_hooks.m_client_state.GetOldMethod< TempEntities_t >(CClientState::TEMPENTITIES)(this, msg);
	state->max_clients = old_max_clients;

	return ret;
}