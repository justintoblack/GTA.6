#include "InputSystem.h"

void InputSyetem::InputAction()
{
	m_actionMap->InputAction();
}

void InputSyetem::SetActionMap(ActionMap newMap)
{
	*m_actionMap = newMap;
}
