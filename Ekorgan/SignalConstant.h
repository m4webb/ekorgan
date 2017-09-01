/*
Filename:
	SignalConstant.h
	
Description:
	Emit a constant value.
*/

#pragma once

#include <SignalTree.h>

Signal *Signal_InitConstant(float value);
void Signal_SetConstant(Signal *signal, float value);