#pragma once
#ifndef _INPUTSYSTEM_H
#define _INPUTSYSTEM_H

#include "ActionMap.h"

class InputSyetem
{
private:
	ActionMap* m_actionMap;
public:
	bool isVehicle;
	void InputAction();
	void SetCharacterMap(CharacterActionMap &newMap);
	void SetVehicleMap(VehicleActionMap &newMap);
};
#endif // !_INPUTSYSTEM_H
