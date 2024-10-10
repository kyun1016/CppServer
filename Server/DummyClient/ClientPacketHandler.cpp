#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	Data_S_TEST recv;
	br >> recv.id >> recv.hp >> recv.attack;

	cout << "ID: " << recv.id << " HP : " << recv.hp << " ATT : " << recv.attack << endl;

	uint16 buffCount;
	br >> buffCount;
	cout << "BufCount : " << buffCount << endl;

	recv.buffs.resize(buffCount);
	for (int32 i = 0; i < buffCount; i++)
	{
		br >> recv.buffs[i].buffId >> recv.buffs[i].remainTime;
		cout << "BufInfo : " << recv.buffs[i].buffId << " " << recv.buffs[i].remainTime << endl;
	}
}
