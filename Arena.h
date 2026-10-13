#ifndef _ARENA_H
#define _ARENA_H 
struct Memory{
	void *persistentMemory;
	u8 *persistentMemoryCurrentBase;
	size_t persistentMemorySize;;
	void *transientMemory;
	u8 *transientMemoryCurrentBase;
	size_t transientMemorySize;;
};

struct Arena{
	u64 size;
	u64 used;
	u8 *base;
};

inline internal void ArenaAlloc(Arena *arena, u8 **base, u64 size);

inline internal void* ArenaPush(Arena* arena, u64 size);
void *ArenaPushZero(Arena *arena, u64 size);

#define PushArray(arena, type, count) (type*)ArenaPush((arena), (sizeof(type)) * (count))
#define PushArrayZero(arena, type, count) (type*)ArenaPushZero((arena), (sizeof(type)) * (count))
#define PushStruct(arena, type) (type*)PushArray(arena, type, 1)
#define PushStructZero(arena, type) PushArrayZero(arena, type, 1)
#define PushString(arena, count) (char*)ArenaPush((arena), (count))


inline internal void ArenaPop(Arena *arena, u64 size);
#define PopArray(arena, type, count) ArenaPop((arena), sizeof(type) * (count))
#define PopStruct(arena, type) PopArray((arena), (type), 1)

inline internal void ArenaReset(Arena *arena);

#endif
