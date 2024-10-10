#pragma once

enum
{
	S_TEST = 1
};

// 패킷 설계 TEMP
struct BuffData
{
	uint64 buffId;
	float remainTime;
};
struct Data_S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;
	vector<BuffData> buffs;
	wstring name;
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);
};

