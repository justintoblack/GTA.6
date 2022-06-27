#pragma once
#ifndef _ZOMBIE_H
#define _ZOMBIE_H
#include "GameObject.h"
#include "MonoBehaviour.h"

class Zombie:public GameObject
{
public:
	void Awake()
	{
		cout << "Zombie Awake" << endl;
	}
	void Update() 
	{
		cout << "Zombie Update" << endl;
	}
};

#endif // !_ZOMBIE_H
