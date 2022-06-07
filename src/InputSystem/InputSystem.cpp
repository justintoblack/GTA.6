#include "InputSystem.h"

 void InputSyetem::InputAction()
{
	m_actionMap->InputAction();
}

void InputSyetem::SetCharacterMap(CharacterActionMap &newMap)
{
	m_actionMap = &newMap;
}

void InputSyetem::SetVehicleMap(VehicleActionMap& newMap)
{
	m_actionMap = &newMap;
}

