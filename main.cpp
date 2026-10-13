/* 
TODO:
- implement a simple string and array ds
- after finishing the obj implement glb
 */

#include <stdio.h>

#include <float.h> // i will see if i can remove this 

#include <glad/glad.h>
#include "glad.c"

#include <SDL3/SDL.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_opengl.h>

#include "Utilities.h"
#include "Arena.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#include "3dMath.h"
#include "3dMath.cpp"

#include "shader.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Utilities.cpp"
#include "Arena.cpp"

internal u32 loadTextureRGB(char const * path, bool verticalFlip);
internal void concat_small_string(char *dest, char * c1, char *c2);
internal void remove_file_name_from_path(char *path, const char *p);

#include "obj_loader.h"
#include "obj_loader.c"

#include "gltf_loader.h"
#include "gltf_loader.c"

b32 Running = true;

struct input{
	b32 left;
	b32 right;
	b32 forward;
	b32 backward;
};

struct Camera{
	v3 pos	  ;
	v3 direction;
	v3 up       ;	
	v3 front    ;	
	f32 yaw   ;
	f32 pitch ;
	
};

u32 loadTextureSRGB(char const * path, GLint edgeDealing)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        GLenum internalFormat;
        if (nrComponents == 1)
		{
			format = GL_RED;
			internalFormat = GL_R8;
		}
        else if (nrComponents == 2)
		{
			format = GL_RG;
			internalFormat = GL_R16;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
			internalFormat = GL_SRGB8;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
			internalFormat = GL_SRGB8_ALPHA8;
		}

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
		
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeDealing);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeDealing);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path: %s \n",path);
        stbi_image_free(data);
    }

    return textureID;
}

internal u32 
loadTextureRGB(char const * path, bool verticalFlip){
    stbi_set_flip_vertically_on_load(verticalFlip);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        switch (nrComponents)
		{
			case 1: format = GL_RED;  break;
			case 2: format = GL_RG;   break;
			case 3: format = GL_RGB;  break;
			case 4: format = GL_RGBA; break;
		}

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path: %s \n",path);
        stbi_image_free(data);
    }

    return textureID;
}


internal void
concat_small_string(char *dest, char * c1, char *c2){
	size_t s1 = strlen(c1);
	size_t s2 = strlen(c2);
	int i, j;
	j = 0;
	for(i = 0; i < s1; ++i){
		dest[j++] = c1[i];
	}
	for(i = 0; i < s2; ++i){
		dest[j++] = c2[i];
	}
	dest[j] = '\0';
}

internal void
remove_file_name_from_path(char *path, const char *p){
	size_t s = strlen(p);
	int ts = 0;
	for(int i = s-1; i > 0; --i){
		if(p[i] == '/') break;
		ts++;
	}
	int rs = s - ts;

	int i;
	for(i = 0; i < rs; ++i){
		path[i] = p[i];
	}
	path[i] = '\0';
}

bool 
init(){
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != true){
		printf("failed to initialize sdl");
		return false;
	}
		
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	return true;
}

void
playerInputHandle(input* gameInput, const bool* curruntKeyState){
	gameInput->left = curruntKeyState[SDL_SCANCODE_LEFT]  || curruntKeyState[SDL_SCANCODE_A];
    gameInput->right = curruntKeyState[SDL_SCANCODE_RIGHT] || curruntKeyState[SDL_SCANCODE_D];

	gameInput->forward = curruntKeyState[SDL_SCANCODE_UP] || curruntKeyState[SDL_SCANCODE_W];
	gameInput->backward = curruntKeyState[SDL_SCANCODE_DOWN] || curruntKeyState[SDL_SCANCODE_S];
}

int main(int argc, char **argv){
	if(!init()) return -1;
	SDL_Window* mainWindow = SDL_CreateWindow("Model loader", 
                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                SDL_WINDOW_OPENGL
                                );
    SDL_SetWindowRelativeMouseMode(mainWindow, true);
    if(!mainWindow) return -1;

    SDL_GLContext ctx = SDL_GL_CreateContext(mainWindow);
	if(ctx == NULL){
		printf("Couldn't create opengl context Error: %s\n", SDL_GetError());
		return -1;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);   
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glEnable(GL_CULL_FACE);

	Memory memory = {0};
	memory.persistentMemorySize = Megabyte(512);
	memory.persistentMemory = SDL_calloc(1, memory.persistentMemorySize);
	memory.persistentMemoryCurrentBase = (u8*)memory.persistentMemory;
	memory.transientMemorySize = Megabyte(20);
	memory.transientMemory = SDL_calloc(1, memory.transientMemorySize);
	memory.transientMemoryCurrentBase = (u8*)memory.transientMemory;
	
	shader simple_shader = make_shader("../shaders/simple.vert", "../shaders/simple.frag");
	// Mesh CubeMesh = Create_cube_mesh();
	
	// CubeMesh.textures[0] = loadTextureRGB("../data/container.jpg");

	Arena modelsArana = {};
	ArenaAlloc(&modelsArana, &memory.persistentMemoryCurrentBase, Megabyte(128));
	Arena parseArena = {};
	ArenaAlloc(&parseArena, &memory.transientMemoryCurrentBase, Megabyte(10));
	Arena meshArena = {};
	ArenaAlloc(&meshArena, &memory.persistentMemoryCurrentBase, Megabyte(1));
	
	// Loaded_Model bunny = parse_obj(&modelsArana, &parseArena, "../data/bunny.obj");
	// Loaded_Model bunny = parse_obj(&modelsArana, &parseArena, "../data/stanford-bunny.obj");
	// Mesh bunnyMesh = create_mesh(&meshArena, bunny);
	// ArenaReset(&modelsArana);
	
	// Loaded_Model suzzane = parse_obj(&modelsArana, &parseArena, "../data/suzanne.obj");
	// Mesh suzzaneMesh = create_mesh(&meshArena, suzzane);
	// ArenaReset(&modelsArana);

	Loaded_Model f16Model = parse_obj(&modelsArana, &parseArena, "../data/f16/f16.obj");
	Mesh f16 = create_mesh(&meshArena, f16Model);
	ArenaReset(&modelsArana);

	// Loaded_Model cruiserModel = parse_obj(&modelsArana, &parseArena, "../data/cruiser/cruiser.obj");
	// Mesh cruiser = create_mesh(&meshArena, cruiserModel);
	// ArenaReset(&modelsArana);

	// scene_node triangleNode = parse_gltf(&modelsArana, &parseArena, "../data/triangle.gltf");
	// Mesh triangleMesh = test_create_mesh(&meshArena, &triangleNode);

	/* GLTF_Scene cozzy = parse_gltf(&modelsArana, &parseArena, "../data/cozzy/scene.gltf"); */
	/* GPU_mesh *cozzyMeshes = update_scene_mesh(&meshArena, cozzy); */
	
	// GLTF_Scene torch = parse_gltf(&modelsArana, &parseArena, "../data/torch/scene.gltf");
	// GPU_mesh *torchMeshes = update_scene_mesh(&meshArena, torch);
	
	// GLTF_Scene building = parse_gltf(&modelsArana, &parseArena, "../data/building/scene.gltf");
	// GPU_mesh *buildingMesh = update_scene_mesh(&meshArena, building);
	
	// GLTF_Scene warrior = parse_gltf(&modelsArana, &parseArena, "../data/goatWarrior/scene.gltf");
	// GPU_mesh *warriorMeshes = update_scene_mesh(&meshArena, warrior);
	
	GLTF_Scene woman = parse_gltf(&modelsArana, &parseArena, "../data/animationTest/scene.gltf");
	GPU_mesh *womanMeshes = update_scene_mesh(&meshArena, woman);
	
	// GLTF_Scene hand = parse_gltf(&modelsArana, &parseArena, "../data/hand/scene.gltf");
	// GPU_mesh *handMeshes = update_scene_mesh(&meshArena, hand);
	
	// GLTF_Scene bat = parse_gltf(&modelsArana, &parseArena, "../data/bat/scene.gltf");
	// GPU_mesh *batMeshes = update_scene_mesh(&meshArena, bat);
	
	// NOTE: this model is ass and is either very large or very small
	// will look into it later
	GLTF_Scene traveler = parse_gltf(&modelsArana, &parseArena, "../data/traveler/scene.gltf");
	GPU_mesh *travelerMeshes = update_scene_mesh(&meshArena, traveler);
	
	input gameInput = {};
	
	u32 frameStartTime = 0;
	u32 frameEndTime = 0;
	f32 dt = 0.0f;

	SDL_Event e = {};
	
	mat4 projection = mat4_identity();
	projection = mat4_perspective(radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);

	u32 displayBoneIndex = 0;
	
	frameStartTime = SDL_GetTicks();
	while(Running){
		while(SDL_PollEvent(&e) != 0){
			if(e.type == SDL_EVENT_QUIT){
				Running = false;
			}
			if(e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat){
				if(e.key.key == SDLK_ESCAPE){
					Running = false;
				}
				else if(e.key.key == SDLK_RIGHT){
					// displayBoneIndex++;
					// displayBoneIndex = displayBoneIndex % 261;
					currentAnimation = (currentAnimation+1) % maxAnimations;
					printf("currentAnimation: %i\n", currentAnimation);
				}
				else if(e.key.key == SDLK_LEFT){
					// displayBoneIndex++;
					// displayBoneIndex = displayBoneIndex % 261;
					currentAnimation = (currentAnimation-1);
					if(currentAnimation < 0) currentAnimation = maxAnimations;
					printf("currentAnimation: %i\n", currentAnimation);
				}
			}
		}
		const bool* curruntKeyState = SDL_GetKeyboardState(NULL);
		playerInputHandle(&gameInput, curruntKeyState);

		// Render here
		glClearColor(0.5f, 0.0f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		UpdateAnimation(woman, dt);
		// UpdateAnimation(hand, dt);
		
		// create transformations
        mat4 view = mat4_identity();
        view  *= mat4_translate(make_vec3(0.0f, 0.0f, -3.0f));
        
        setShaderValue_mat4(simple_shader, "view", view);
        setShaderValue_mat4(simple_shader, "projection", projection);
		
		render_scene_node(woman, womanMeshes, simple_shader, woman.scene[0].rootNodes[0]);
		// render_scene_node(traveler, travelerMeshes, simple_shader, traveler.scene[0].rootNodes[0]);
		// render_scene_node(hand, handMeshes, simple_shader, hand.scene[0].rootNodes[0]);
		
		// render_mesh(&f16, simple_shader);
		// test_render_mesh(&triangleMesh, simple_shader);
		
		SDL_GL_SwapWindow(mainWindow);
		frameEndTime = SDL_GetTicks();
		dt = (frameEndTime - frameStartTime) / 1000.0f;
		int fps = 1.0 / dt;
		frameStartTime = frameEndTime;
		// printf("fps: %i\n", fps);
		SDL_Delay(1);
	}
	return 0;
}
