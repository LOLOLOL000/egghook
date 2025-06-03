#pragma once
#include "includes.h"

class LagCompensation
{
public:
	LagRecord* StartPrediction(AimPlayer* data, LagRecord* previous);
};

extern LagCompensation g_lagcomp;