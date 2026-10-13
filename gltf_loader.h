enum gltf_token_types{
	GLTF_BRACE_OPEN_TOKEN	  = '{',
	GLTF_BRACE_CLOSE_TOKEN	 = '}',
	GLTF_BRACKET_OPEN_TOKEN	= '[',
	GLTF_BRACKET_CLOSE_TOKEN   = ']',
	GLTF_WORD_TOKEN			= 'w',
	GLTF_NUMBER_TOKEN		  = 'n',
	GLTF_COMMA_TOKEN		   = ',',
	GLTF_COLON_TOKEN		   = ':',
	GLTF_UNIDENTIFIED_TOKEN	= '$'
};

struct gltf_token{
	gltf_token_types gltf_type;
	char *stringData; 
	int   indexData;
	f32 realData;
};

int b64invs[] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51 
};


#define INVALID_INDEX UINT32_MAX
//TODO: see if i should initialize every thing with invalid index 
struct BufferView{
	const char *name;
	u32 bufferIdx;
	u32 byteLength;
	u32 targetIdx;
	u32 byteOffset;
	u32 byteStride;
};

struct DataBuffer{
	char *stringData; // data before parsing 
	u8 *data;
	u32 byteLength;
};

struct Gltf_image{
	char *path;
	u32   texture;
};

struct Gltf_materials{
	char *name;
	b32 doubleSided;
	v3 emissiveFactor;
	u32 emissiveTextureIdx;
	u32 baseColorTextureIdx;
	u32 diffuseTextureIdx;
	f32 metallicFactor;
	f32 roughnessFactor;
};

enum accessorType{
	SCALOR = 1,
	VEC2   = 2,
	VEC3   = 3,
	VEC4   = 4,
	MAT4   = 5,
};

struct Accessor{
	f32 min[16];
	f32 max[16];
	accessorType DataType;
	u32 bufferViewIdx;
	u32 componentType;
	u32 count;
	u32 byteOffset;
};

#define MAX_TEXCOORD 10
struct mesh_primative_info{
	u32 TexCoords[MAX_TEXCOORD];
	u32 positionIdx;
	u32 normalIdx;
	u32 indicesIdx;
	u32 jointsIdx;
	u32 weightIdx;
	u32 materialIdx;
	u32 mode;
};

struct Gltf_meshes_info{
	const char *name;
	mesh_primative_info *primativesInfo;
	s32 primativesCounter;
};

struct Gltf_samplers{
	u32 magFilter;
	u32 minFilter;
	u32 wrapS;
	u32 wrapT;
};

struct Gltf_nodes_info{
	const char *name;
	u32 *childrenIdxs;
	mat4 matrix;
	
    Quaternion rotation;
	v3 translation;
	v3 scale;
	
	b32 hasMatrix;
	b32 hasTranslation;
	b32 hasRotation;
	b32 hasScale;
	
	u32 childrenCount;
	u32 meshIdx; // i think this is the start ???!!!
	u32 skinIdx;
};

struct Gltf_scenes_info{
	const char *name;
	u32  *nodeIdxs;
	u32 nodeCount;
	b32 is_nodeIdxs_allocated;
};

struct Gltf_texture{
	u32 sampler;
	u32 srcIdx;
};

enum Interpolation_type{
	LINEAR,
	STEP
};

struct Animation_samplers_info{
	Interpolation_type interpolation;
	u32 inputIdx;
	u32 outPutIdx;
};

enum Animation_path{
	TRANSLATION,
	ROTATION,
	SCALE
};

struct Animation_channels_info{
	Animation_path path;
	u32 samplerIdx;
	u32 targetNode;
};

struct Gltf_animations_info{
	const char *name;
	Animation_samplers_info *animSamplers;
	Animation_channels_info *animChannels;
	
	u32 animSamplersCounter;
	u32 animChannelsCounter;
};

struct Gltf_skins_info{
	u32 *joints;
	u32 inverseBindMatricesIdx;
	u32 jointsCount;
	u32 skeletonIdx;
};

struct Gltf_info{
	// For data and data access
	const char *parentArrayName;
	const char *arrayName;
	const char *objectName;
	
	BufferView *bufferViews;
	Accessor   *accessors;
	DataBuffer *buffers;
	Gltf_image *images;
	Gltf_materials  *materials;
	Gltf_meshes_info *meshesInfo;
	Gltf_nodes_info *nodesInfo;
	Gltf_samplers   *samplers;
	Gltf_scenes_info *scenesInfo;
	Gltf_texture *textures;
	Gltf_animations_info *animationsInfo;
	Gltf_skins_info *skinsInfo;
	
	s32 bvCounter;
	s32 accCounter; 
	s32 dbCounter;
	s32 imgCounter;
	s32 matCounter;
	s32 meshesInfoCounter;
	s32 nodesInfoCounter;
	s32 samplersCounter;
	s32 scenesInfoCounter;
	s32 textureCounter;
	s32 animationsInfoCounter;
	s32 skinsInfoCounter;
	
	u32 defaultScene;

	u32 currentArraySize;
};


void parse_gltf_object(Arena *arena, Gltf_info *info, char **cursor);
void parse_gltf_array(Arena *arena, Gltf_info *info, const char *arrayName, char **cursor);



// NOTE: i suppose i should put that into a scene & animation files or something 
struct Scene_node{
	u32 *childrenIdxs;
	mat4 localTransforms;
	mat4 worldTransforms;
	
	Quaternion rotation;
	v3 translation;
	v3 scale;
	
	u32 childrenCount;
	u32 meshIdx;
	u32 skinIdx;
};

struct Scene{
	u32 *rootNodes;
	u32 rootCount;
};

struct Material{
	v3 emissiveFactor;
	u32 emissiveTexture;
	u32 baseColorTexture;
	u32 diffuseTexture;
	
	b32 isDoubleSided;
	f32 metallicFactor;
	f32 roughnessFactor;
};

struct Primatives{
	v3  *positions;
	v3  *normals;
	v2  *uvs[MAX_TEXCOORD];
	u32 *indices;
	v4i *joints;
	v4  *weights;
	
	u32 posCount;
    u32 normCount;
    u32 uvCount[MAX_TEXCOORD];
    u32 indexCount;
    u32 jointCount;
    u32 weightCount;
	u32 matIdx;
	u32 mode;
};

struct Mesh_geometry{
	Primatives *primatives;
	u32 primCount;
};

struct Skins{
	mat4 *inverseBindMatrices;
	mat4 *jointMatrices;
	u32  *jointsNodesIdx;
	u32 jointsCount;
};

struct Animation_channel{
	// NOTE: just put all the data directly from the sampler
	Quaternion  *rotation;
	v3  *scale;
	v3  *translation;
	f32 *timeStamps;
	Interpolation_type interpoType;
	
	Animation_path transType;
	
	u32 nodeIdx;
	u32 keyframeCount; // claude forced me 
	f32 animation_max_time;
	f32 animation_min_time;
};

struct Animation{
	Animation_channel *channels;
	u32 channelsCount;

	f32 animationStartTime;
	f32 animationEndTime;
};

struct GLTF_Scene{
	Scene 		*scene;
	Scene_node    *nodes;
	Mesh_geometry *meshes;
	Material  	*mat;
	Skins     	*skins;
	Animation 	*animations;
	
	u32 sceneCount;
	u32 nodeCount;
	u32 meshCount;
	u32 matCount;
	u32 skinCount;
	u32 animationCount;
};


#define MAX_JOINTS  261
#define MAX_WEIGHTS 4
struct GPU_Primitive{
	u32 vao, ebo, posVbo, normVbo, uvVbo, jointVbo, weightVbo;
	u32 indexCount;
	u32 matIdx = INVALID_INDEX;
};

struct GPU_mesh{
	GPU_Primitive *primitives;
	u32 primCount;
};

