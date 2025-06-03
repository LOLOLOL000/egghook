#include "lagcomp.h"

LagCompensation g_lagcomp;

LagRecord* LagCompensation::StartPrediction(AimPlayer* data, LagRecord* previous)
{
	if (!data || data->m_records.size() < 3)
		return nullptr;

	LagRecord* front = data->m_records.front().get();
	if (!front)
		return nullptr;

	int lag = game::TIME_TO_TICKS(previous->m_sim_time - data->m_records[2].get()->m_sim_time);

	// get the delta in ticks between the last server net update
	// and the net update on which we created this record.
	int updatedelta = g_cl.m_server_tick - front->m_tick;

	// if the lag delta that is remaining is less than the current netlag
	// that means that we can shoot now and when our shot will get processed
	// the origin will still be valid, therefore we do not have to predict.
	if (g_cl.m_latency_ticks <= lag - updatedelta)
		return front;

	return nullptr;
}
