#pragma once

/*-------------------
* MemoryHeader
-------------------*/

// 동일한 크기의 메모리를 관리하는 방법
struct MemoryHeader
{
	// [MemoryHeader][Data]
	MemoryHeader(int32 size)
		: allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);	// placement new
		return reinterpret_cast<void*>(++header);	// MemoryHeader 만큼 크기를 건너뛰고 포인터 반환
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}


	int32 allocSize;

	// TODO: 필요한 추가 정보
};

/*-------------------
* MemoryPool
-------------------*/
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	int32 _allocSize = 0;			// 담당하고 있는 메모리 사이즈
	atomic<int32> _allocCount = 0;	// 갖고 있는 메모리 개수

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

