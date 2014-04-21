#include "UserData.h"
#include "Player.h"

void UserData::setType(int type)
{
	m_UDType = type;
}

int UserData::getType()
{
	return m_UDType;
}

bool UserData::isType(int type)
{
	return type == m_UDType;
}


void PlayerUD::setData(PlayerCharBase* charBase)
{
	m_charBase = charBase;
}
PlayerCharBase* PlayerUD::getData()
{
	return m_charBase;
}