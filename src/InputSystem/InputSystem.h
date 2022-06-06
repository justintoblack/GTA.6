#pragma once
#ifndef _INPUTSYSTEM_H
#define _INPUTSYSTEM_H

#include "ActionMap.h"

class InputSyetem
{
private:
	ActionMap* m_actionMap;
public:
	void InputAction();
	void SetActionMap(ActionMap newMap);
};
#endif // !_INPUTSYSTEM_H
