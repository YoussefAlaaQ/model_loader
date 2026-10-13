inline internal void
ArenaAlloc(Arena *arena, u8 **base, u64 size){
	arena->size = size;
	arena->base = *base;
	arena->used = 0;

	*base += size;
}

inline internal void*
ArenaPush(Arena* arena, u64 size){
	Assert(arena->used + size <= arena->size);
	void *result = arena->base + arena->used;
	arena->used += size;
	return result;
}

inline internal void*
ArenaPushZero(Arena *arena, u64 size){
	void *result = ArenaPush(arena, size);
	memset(result, 0, size);
	return result;
}

inline internal void
ArenaPop(Arena *arena, u64 size){
	Assert(size <= arena->used);
	arena->used -= size;
}

inline internal void
ArenaReset(Arena *arena){
	arena->used = 0;
}
