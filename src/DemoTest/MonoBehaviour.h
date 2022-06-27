#pragma once
#ifndef _MONOBEHAVIOUR_H
#define _MONOBEHAVIOUR_H

class MonoBehaviour
{
public:
	virtual void Awake()=0;
	virtual void Update()=0;
};

#endif // !_MONOBEHAVIOUR_H
