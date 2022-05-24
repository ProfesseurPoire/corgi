#pragma once

class Component
{
public:

    Component() =default;
	Component(int val):number(val)
	{
		
	}
    ~Component() = default;

    int number;
};