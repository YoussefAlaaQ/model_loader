#define global_variable static
#define local_Persist static
#define internal static

#define Kilobyte(size) size * 1024LL 
#define Megabyte(size) Kilobyte(size) * 1024LL
#define Gigebyte(size) Megabyte(size) * 1024LL 

#if GAME_SLOW
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}
#else 
#define Assert(Expression)
#endif


typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef s32 b32;

#define arraySize(a) sizeof(a)/sizeof(a[0])

char* loadFile(const char* path);
