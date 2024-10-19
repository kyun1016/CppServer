#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "Player.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	XmlNode root;
	XmlParser parser;
	if (parser.ParseFromFile(L"GameDB.xml", OUT root) == false)
		return 0;

	Vector<XmlNode> tables = root.FindChildren(L"Table");
	for (XmlNode& table : tables)
	{
		String name = table.GetStringAttr(L"name");
		String desc = table.GetStringAttr(L"desc");

		Vector<XmlNode> columns = table.FindChildren(L"Column");
		for (XmlNode& column : columns)
		{
			String colName = column.GetStringAttr(L"name");
			String colType = column.GetStringAttr(L"type");
			bool nullable = !column.GetBoolAttr(L"notnull", false);
			String identity = column.GetStringAttr(L"identity");
			String colDefault = column.GetStringAttr(L"default");
			// Etc...
		}

		Vector<XmlNode> indices = table.FindChildren(L"Index");
		for (XmlNode& index : indices)
		{
			String indexType = index.GetStringAttr(L"type");
			bool primaryKey = index.FindChild(L"PrimaryKey").IsValid();
			bool uniqueConstraint = index.FindChild(L"UniqueKey").IsValid();

			Vector<XmlNode> columns = index.FindChildren(L"Column");
			for (XmlNode& column : columns)
			{
				String colName = column.GetStringAttr(L"name");
			}
		}
	}

	Vector<XmlNode> procedures = root.FindChildren(L"Procedure");
	for (XmlNode& procedure : procedures)
	{
		String name = procedure.GetStringAttr(L"name");
		String body = procedure.FindChild(L"Body").GetStringValue();

		Vector<XmlNode> params = procedure.FindChildren(L"Param");
		for (XmlNode& param : params)
		{
			String paramName = param.GetStringAttr(L"name");
			String paramType = param.GetStringAttr(L"type");
			// TODO..
		}
	}
	//
	
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));

	// Create Table
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold];			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY, \
				[gold] INT NULL,						\
				[name] NVARCHAR(50) NULL,				\
				[createDate] DATETIME NULL				\
			);";

		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	// Add Data
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);
		WCHAR name[100] = L"승균";
		dbBind.BindParam(1, name);
		TIMESTAMP_STRUCT ts = { 2021, 6, 5 };
		dbBind.BindParam(2, ts);

		ASSERT_CRASH(dbBind.Execute());

		GDBConnectionPool->Push(dbConn);
	}

	// Read
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);

		int32 outId = 0;
		int32 outGold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT outDate = {};
		dbBind.BindCol(0, OUT outId);
		dbBind.BindCol(1, OUT outGold);
		dbBind.BindCol(2, OUT outName);
		dbBind.BindCol(3, OUT outDate);

		ASSERT_CRASH(dbBind.Execute());

		wcout.imbue(locale("kor"));

		while (dbConn->Fetch())
		{
			wcout << "Id: " << outId << " Gold : " << outGold << " Name: " << outName << endl;
			wcout << "Date : " << outDate.year << "/" << outDate.month << "/" << outDate.day << endl;
		}

		GDBConnectionPool->Push(dbConn);
	}

	//

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}