#pragma once

struct PlayerCharBase;

class UserData
{
public:
	UserData() {}
	virtual ~UserData() {}
	
	void setType(int type);
	int getType();
	bool isType(int type);

private:
	int m_UDType; // user data type
};


enum UserDataType
{
	PLAYER
};

// Define all user data classes below.


class PlayerUD: public UserData
{
public:
	void setData(PlayerCharBase* charBase);
	PlayerCharBase* getData();

private:
	PlayerCharBase* m_charBase;
};