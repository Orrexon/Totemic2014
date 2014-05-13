#pragma once

#include <iostream>
#include "Math.h"
#include "Box2D\Box2D.h"

class UserData;

class ContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* p_contact);
	void EndContact(b2Contact* p_contact);

	void playerContactBegin(UserData* userDataA, UserData* userDataB);
	void playerContactEnd(b2Contact* p_contact);
};