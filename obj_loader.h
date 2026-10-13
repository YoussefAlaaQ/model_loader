enum obj_token_types{
	OBJ_V_TOKEN            = 'v',
	OBJ_VT_TOKEN 		  = 't',
	OBJ_VN_TOKEN		   = 'n',
	OBJ_F_TOKEN			= 'f',
	OBJ_WORD_TOKEN         = 'w',
	OBJ_NUMARIC_TOKEN      = 'N',
	OBJ_COMMENT_TOKEN	  = '#',
	OBJ_FILEPATH_TOKEN	 = 'p',
	OBJ_G_TOKEN			= 'g',
	OBJ_O_TOKEN			= 'o',
	// OBJ_USEMTL_TOKEN	   = 'm',
	// OBJ_MTLLIB_TOKEN	   = 'M',
	OBJ_UNIDENTIFIED_TOKEN = '$'
};

enum mtl_token_types{
	MTL_WORD_TOKEN    	 = 'w',
	MTL_NUMARIC_TOKEN 	 = 'n',
	MTL_UNIDENTIFIED_TOKEN = '$'
};

struct obj_token{
	obj_token_types  obj_type;
	mtl_token_types  mtl_type;
	char *stringData; 
	int   indexData;
	f32 realData;
};

struct obj_matrial_data{
	char* name;
	v3 ambiant;
	v3 diffuse;
	v3 specular;
	f32 ns;
	f32 ni;
	f32 d;
	u32 texture;
	s32 illum;
};

struct obj_matrial{
	obj_matrial_data *data;
	u32 matrialCount;
};

//NOTE: THOSE ARE PLACE HOLDERS AND ARE SUBJECT TO CHANGE

struct Vertex{
	v3 position;
	v3 normal  ;
	v2 uv	  ;
};

struct Vertcies{
	v3 *positions;
	v3 *normals  ;
	v2 *uvs      ;
};

struct Face{
	v3i posIndex;
	v3i normalIndex;
	v3i uvIndex;
};

struct Faces{
	v3 *posIndecies;
	v3 *normalIndecies;
	v3 *uvIndecies;
};

struct sub_mesh{
	u32 startVertexIndex;
	u32 vertexCount;
	u32 materialIndex;
	obj_matrial_data material;
};

struct Loaded_Model{
	Vertex *v;
	sub_mesh *subMeshes;

	obj_matrial mat;
	
	int subMeshCount;
	int vertexCount;
};

struct Mesh{
	u32 vbo;
	u32 vao;
	u32 ebo;

	u32 indexCount;

	u32 vertexCount;
	
	u32 subMeshCount;
	sub_mesh *subMeshes;
		
};

