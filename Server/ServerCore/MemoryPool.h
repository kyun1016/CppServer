#pragma once

/*-------------------
* MemoryHeader
-------------------*/

// ������ ũ���� �޸𸮸� �����ϴ� ���
struct MemoryHeader
{
	// [MemoryHeader][Data]
	MemoryHeader(int32 size)
		: allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);	// placement new
		return reinterpret_cast<void*>(++header);	// MemoryHeader ��ŭ ũ�⸦ �ǳʶٰ� ������ ��ȯ
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}


	int32 allocSize;

	// TODO: �ʿ��� �߰� ����
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
	int32 _allocSize = 0;			// ����ϰ� �ִ� �޸� ������
	atomic<int32> _allocCount = 0;	// ���� �ִ� �޸� ����

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

