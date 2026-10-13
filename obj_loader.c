internal void
process_numeric_token(Arena *arena, char **cursor, obj_token* token){
	b32 is_float = false;
	char* tokenStart = *cursor; 
	size_t tokenSize, i, j = 0;
	
	while(isdigit(**cursor) || **cursor == '_' || **cursor == '-' || **cursor == '+' || **cursor == '.' || **cursor == 'e' || **cursor == 'E'){
		if(**cursor == '.' || **cursor == 'e' || **cursor == 'E') is_float = true;
		++*cursor;
	}

	tokenSize = *cursor - tokenStart;
	--*cursor;
	char *numberString = PushString(arena, tokenSize+1);
	
	for(i = 0; i < tokenSize; i++){
		// if(isdigit(numberString) || numberString == '-' || numberString == ){
		numberString[j++] = *tokenStart;
		++tokenStart;
		// }
	}
	numberString[i] = '\0';

	if(is_float){
		token->realData = atof(numberString);
		token->indexData  = 0;
	}
	else{
		token->realData = 0.0f;
		token->indexData  = atoi(numberString); 
	}

	ArenaPop(arena, tokenSize+1);
}

internal void
process_real_numeric_token(Arena *arena, char **cursor, obj_token* token){
	b32 is_f32 = false;
	char* tokenStart = *cursor; 
	size_t tokenSize, i, j = 0;
	
	while(isdigit(**cursor) || **cursor == '_' || **cursor == '-' || **cursor == '.' || **cursor == 'e' || **cursor == 'E'){
		++*cursor;
	}

	tokenSize = *cursor - tokenStart;
	--*cursor;
	char *numberString = PushString(arena, tokenSize+1);
	
	for(i = 0; i < tokenSize; i++){
		// if(isdigit(numberString) || numberString == '-' || numberString == ){
		numberString[j++] = *tokenStart;
		++tokenStart;
		// }
	}
	numberString[i] = '\0';

	token->realData = atof(numberString);
	token->indexData  = 0;
	ArenaPop(arena, tokenSize+1);
}

internal void
process_word_token(Arena *arena, char **cursor, obj_token* token){
	char* tokenStart = *cursor; 
	size_t  tokenSize, i;
	
	while(isalnum(**cursor) || **cursor == '_' || **cursor == '/' || **cursor == '.' || **cursor == '-' || **cursor == '='){
		++*cursor;
	}
	
	tokenSize = *cursor - tokenStart;
	// --*cursor;
	// TODO: see how to pop this thing to free some memory 
	token->stringData = PushArray(arena, char, tokenSize+1);
	
	for(i = 0; i < tokenSize; i++){
		token->stringData[i] = *tokenStart++;
	}
	token->stringData[i] = '\0';
}

global_variable u32 line = 0;

internal void 
get_obj_metaData(Arena *arena, char *objData, u32 *posCount, u32 *normalCount, u32 *textureCount, u32 *faceCount){
	char *cursor = objData;
	char *lineCursor = cursor;
	int line = 0;

	while(*cursor != '\0'){
		while(*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n'){
			if(*cursor == '\n') ++line;
			++cursor;
		}
		if(isalpha(*cursor)){
			char *wordStart = cursor;
			while(isalnum(*cursor)){
				++cursor;
			}
			size_t dataSize = cursor - wordStart;
			--cursor;
			
			char *data = PushArray(arena, char, dataSize+1);
			if(data == NULL) printf("failed to allocate token memory\n");
			int i = 0;
			for(i = 0; i < dataSize; i++){
				data[i] = *wordStart++;
			}
			data[i] = '\0';

			if(!strcmp(data, "v")){
				++(*posCount);
			} 
			else if(!strcmp(data, "f")){
				++(*faceCount);
			}
			else if(!strcmp(data, "vn")){
				++(*normalCount);
			}
			else if(!strcmp(data, "vt")){
				++(*textureCount);
			}

			PopArray(arena, char, dataSize - 1);
		}
		++cursor;
	}
}

internal void
get_the_number_of_matrials(Arena *arena, char *cursor, int *matrialCount){
	while(*cursor != '\0'){
		while(*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n'){
			if(*cursor == '\n') ++line;
			++cursor;
		}
	
		if(isalpha(*cursor)){
			char *tokenStart = cursor;
			while(isalpha(*cursor)){
				++cursor;
			}
	
			size_t strSize = cursor - tokenStart;
			--cursor;
			char *s = PushString(arena, strSize+1);
			
			int i;
			for(i = 0; i < strSize; i++){
				s[i] = *tokenStart++;
			}
			s[i] = '\0';
		
			if(!strcmp(s, "newmtl")){
				++(*matrialCount);
			}
			// SDL_free(s);
			ArenaPop(arena, strSize+1);
		}

		++cursor;
	}
}

internal obj_token
lex_mtl(Arena *arena, char **cursor){
	while(**cursor == ' ' || **cursor == '\t' || **cursor == '\r' || **cursor == '\n'){
		if(**cursor == '\n') ++line;
		++*cursor;
	}

	obj_token token = {};
	if(isalpha(**cursor)){
		token.mtl_type = MTL_WORD_TOKEN;
		process_word_token(arena, cursor, &token);
	}
	else if(isdigit(**cursor)){
		token.mtl_type = MTL_NUMARIC_TOKEN;
		process_real_numeric_token(arena, cursor, &token);
	}

	return token;
}

internal obj_token
get_mtl_token_and_advance(Arena *arena, char** cursor){
	obj_token t = lex_mtl(arena, cursor);
	++*cursor;
	return t;
}

internal obj_matrial
parse_mtl(Arena *permArena, Arena *scratchArena, char *path){
	char *cursor = loadFile(path);
	int matrialCount = 0;
	get_the_number_of_matrials(scratchArena, cursor, &matrialCount);

	obj_matrial result = {};
	result.matrialCount = matrialCount;
	result.data = PushArray(permArena, obj_matrial_data, matrialCount);
	int currentMat = -1;
	obj_token token = {};

	while(*cursor != '\0'){
		//NOTE: parse one matrial at a time
		token = get_mtl_token_and_advance(scratchArena, &cursor);
		if(token.mtl_type == MTL_WORD_TOKEN){
			if(!strcmp(token.stringData, "newmtl")){
				currentMat++;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].name = token.stringData;
			}
			else if(!strcmp(token.stringData, "Ka") || !strcmp(token.stringData, "ka")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].ambiant.x = token.realData;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].ambiant.y = token.realData;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].ambiant.z = token.realData;
			}
			else if(!strcmp(token.stringData, "Kd") || !strcmp(token.stringData, "kd")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].diffuse.x = token.realData;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].diffuse.y = token.realData;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].diffuse.z = token.realData;
			}
			else if(!strcmp(token.stringData, "Ks") || !strcmp(token.stringData, "ks")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].specular.x = token.realData;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].specular.y = token.realData;
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].specular.z = token.realData;
			}
			else if(!strcmp(token.stringData, "Ns") || !strcmp(token.stringData, "ns")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].ns = token.realData;
			}
			else if(!strcmp(token.stringData, "Ni") || !strcmp(token.stringData, "ni")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].ni = token.realData;
			}
			else if(!strcmp(token.stringData, "D") || !strcmp(token.stringData, "d")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].d = token.realData;
			}
			else if(!strcmp(token.stringData, "illum")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				result.data[currentMat].illum = token.realData;
			}
			else if(!strcmp(token.stringData, "map_Kd")){
				token = get_mtl_token_and_advance(scratchArena, &cursor);
				char *texFileName = token.stringData;
				char mtlPath[256] = {};
				remove_file_name_from_path(mtlPath, path);
				char texPath[256] = {};
				concat_small_string(texPath, mtlPath, texFileName);

				result.data[currentMat].texture = loadTextureRGB(texPath, true);
			}
		}
	}
	
	return result;
}

internal obj_token
lex_obj(Arena *arena, char **cursor){
	obj_token token = {};
	token.obj_type = OBJ_UNIDENTIFIED_TOKEN;
	while(**cursor == ' ' || **cursor == '\t' || **cursor == '\r' || **cursor == '\n'){
		if(**cursor == '\n') ++line;
		++*cursor;
	}
	char nextChar = *(*cursor + 1);
	if(**cursor == '#'){
		token.obj_type = OBJ_COMMENT_TOKEN;
	}
	else if(**cursor == 'o' || **cursor == 'O'){
		if(nextChar == ' '){
			token.obj_type = OBJ_O_TOKEN;
		}
		else{
			token.obj_type = OBJ_WORD_TOKEN;
			process_word_token(arena, cursor, &token);
		}
	}
	else if(**cursor == 'g' || **cursor == 'G'){
		if(nextChar == ' '){
			token.obj_type = OBJ_G_TOKEN;
		}
		else{
			token.obj_type = OBJ_WORD_TOKEN;
			process_word_token(arena, cursor, &token);
		}
	}
	else if(**cursor == 'v'){
		
		if(nextChar == ' '){
			token.obj_type = OBJ_V_TOKEN;
		}
		else if(nextChar == 't'){
			token.obj_type = OBJ_VT_TOKEN;
			++*cursor;
		}
		else if(nextChar == 'n'){
			token.obj_type = OBJ_VN_TOKEN;
			++*cursor;
		}
		else{
			token.obj_type = OBJ_WORD_TOKEN;
			process_word_token(arena, cursor, &token);
		}
	}
	else if(**cursor == 'f'){
	    if(nextChar == ' '){
			token.obj_type = OBJ_F_TOKEN;
		}
		else{
			token.obj_type = OBJ_WORD_TOKEN;
			process_word_token(arena, cursor, &token);
		} 
	}
	else if(**cursor == '/'){
		if(isalpha(nextChar)){
			token.obj_type = OBJ_FILEPATH_TOKEN;
			process_word_token(arena, cursor, &token);
		}
	}
	else if(**cursor == '.'){
		if(nextChar == '/'){
			token.obj_type = OBJ_FILEPATH_TOKEN;
			process_word_token(arena, cursor, &token);
		}
	}
	else if(isdigit(**cursor) || **cursor == '-'){
		token.obj_type = OBJ_NUMARIC_TOKEN; 
		process_numeric_token(arena, cursor, &token);
	}
	else if(isalpha(**cursor)){
		token.obj_type = OBJ_WORD_TOKEN;
		process_word_token(arena, cursor, &token);
	}
	
	return token;
}

internal obj_token
get_obj_token_and_advance(Arena *arena, char** cursor){
	obj_token t = lex_obj(arena, cursor);
	++*cursor;
	return t;
}


internal Loaded_Model
parse_obj(Arena *permArena, Arena *scratchArena, const char *path){
	char *cursor = loadFile(path); // TODO: change to arena
	Loaded_Model result = {};
	
	u32 posCount = 0;
	u32 normalCount = 0;
	u32 textureCount = 0;
	u32 faceCount = 0;
	get_obj_metaData(scratchArena, cursor, &posCount, &normalCount, &textureCount, &faceCount);

	u32 vertexCount = faceCount * 3;
	v3  *posVertices    = PushArray(scratchArena, v3, posCount); 
	v3  *normalVertices = PushArray(scratchArena, v3, normalCount);
	v2  *uvVertices     = PushArray(scratchArena, v2, textureCount);
	u32 *posIndices     = PushArray(scratchArena, u32, vertexCount);
	u32 *normalIndices  = PushArray(scratchArena, u32, vertexCount);
	u32 *uvIndices      = PushArray(scratchArena, u32, vertexCount);

	u32 vCount = 0, vnCount = 0, vtCount = 0, piCount = 0, niCount = 0, uviCount = 0;
	
	result.subMeshes = PushArray(permArena, sub_mesh, faceCount);
	
	u32 currentVertexIndexInSubMesh = 0;
	u32 currentSubMesh = 0;
	s32 currentMaterialIndex = -1;

	obj_matrial mat = {};
	
	obj_token token = {};
	while(*cursor != '\0'){
		token = get_obj_token_and_advance(scratchArena, &cursor);
		switch(token.obj_type){
			case OBJ_COMMENT_TOKEN:{
				while(*cursor != '\n') ++cursor;
			}break;
			case OBJ_O_TOKEN:{
				while(*cursor != '\n') ++cursor;
			}break;
			case OBJ_G_TOKEN:{
				while(*cursor != '\n') ++cursor;
			}break;
			case OBJ_V_TOKEN:{
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 x = token.realData;
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 y = token.realData;
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 z = token.realData;
				posVertices[vCount++] = make_vec3(x, y, z);
			}break;
			case OBJ_VN_TOKEN:{
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 x = token.realData;
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 y = token.realData;
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 z = token.realData;
				normalVertices[vnCount++] = make_vec3(x, y, z);
			}break;
			case OBJ_VT_TOKEN:{
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 x = token.realData;
				token = get_obj_token_and_advance(scratchArena, &cursor); 
				f32 y = token.realData;
				uvVertices[vtCount++] = make_vec2(x, y);
			}break;
			case OBJ_F_TOKEN:{
				if(currentVertexIndexInSubMesh == 0){
					result.subMeshes[currentSubMesh].startVertexIndex = piCount;
					
					result.subMeshes[currentSubMesh].materialIndex = currentMaterialIndex;
					
				}
				//THIS WILL ONLY DEAL WITH TRAINGLE FACES 
				for(int i = 0; i < 3; ++i){
					s32 vi  = 0;
					s32 vti = 0;
					s32 vni = 0;
					token = get_obj_token_and_advance(scratchArena, &cursor); // v index
					vi = token.indexData;
					if(textureCount > 0){
						token = get_obj_token_and_advance(scratchArena, &cursor);
						while(token.obj_type != OBJ_NUMARIC_TOKEN){
							token = get_obj_token_and_advance(scratchArena, &cursor);
						}
						vti = token.indexData;
						if(normalCount > 0){
							token = get_obj_token_and_advance(scratchArena, &cursor);
							while(token.obj_type != OBJ_NUMARIC_TOKEN){
								token = get_obj_token_and_advance(scratchArena, &cursor);
							}
							vni = token.indexData;
						}
					}
					else if(normalCount > 0){
						token = get_obj_token_and_advance(scratchArena, &cursor);
						while(token.obj_type != OBJ_NUMARIC_TOKEN){
							token = get_obj_token_and_advance(scratchArena, &cursor);
						}
						vni = token.indexData;
					}
					posIndices[piCount++]    = (u32)(vi - 1);
					normalIndices[niCount++] = (u32)(vni - 1);
					uvIndices[uviCount++]    = (u32)(vti - 1);

					++currentVertexIndexInSubMesh;
				}
			}break;
			case OBJ_WORD_TOKEN:{
				if(!strcmp(token.stringData, "mtllib")){
					token = get_obj_token_and_advance(scratchArena, &cursor);
					char *mtlFileName = token.stringData;
					char objPath[256] = {};
					remove_file_name_from_path(objPath, path);
					char mtlPath[256] = {};
					concat_small_string(mtlPath, objPath, mtlFileName);

					mat = parse_mtl(permArena, scratchArena, mtlPath);
					result.mat = mat;
				}
				if(!strcmp(token.stringData, "usemtl")){
					token = get_obj_token_and_advance(scratchArena, &cursor);
					if(currentVertexIndexInSubMesh > 0){
						result.subMeshes[currentSubMesh++].vertexCount = currentVertexIndexInSubMesh;
						currentVertexIndexInSubMesh = 0;
					}

					int foundIDX = -1;
					for(int i = 0 ; i < mat.matrialCount; ++i){
						if(!strcmp(token.stringData, mat.data[i].name)){
							foundIDX = i;
							break;
						}
					}
					currentMaterialIndex = foundIDX;
				
				}
			}break;
		}
	}

	// PopArray(arena, v3, posCount);
	// PopArray(arena, v3, normalCount);
	// PopArray(arena, v2, textureCount);
	// PopArray(arena, u32, vertexCount);
	// PopArray(arena, u32, vertexCount);
	// PopArray(arena, u32, vertexCount);

	
	if (currentVertexIndexInSubMesh > 0) {
		result.subMeshes[currentSubMesh++].vertexCount = currentVertexIndexInSubMesh;
		// currentSubMeshIndex++;
	}
	result.subMeshCount = currentSubMesh;

	result.v = PushArray(permArena, Vertex, faceCount);
	for(u32 i = 0; i < faceCount * 3; ++i){
		u32 pIndex = posIndices[i];
		result.v[i].position = posVertices[pIndex];
		if(normalCount > 0){
			result.v[i].normal = normalVertices[normalIndices[i]];
		}
		if(textureCount > 0){
			result.v[i].uv = uvVertices[uvIndices[i]];
		}
	}
	result.vertexCount = vertexCount;

	ArenaReset(scratchArena);
	return result;
}

//NOTE: this is for testing and will be removed later
internal void
test_render_mesh(Mesh *mesh, shader shader){
	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

internal Mesh
create_mesh(Arena *arena, Loaded_Model model){
	Mesh result = {};
	result.vertexCount = model.vertexCount;

	result.subMeshCount = model.subMeshCount;
	result.subMeshes = PushArray(arena, sub_mesh, model.subMeshCount);
	for(u32 i = 0; i < model.subMeshCount; ++i) {
        result.subMeshes[i].startVertexIndex = model.subMeshes[i].startVertexIndex;
        result.subMeshes[i].vertexCount      = model.subMeshes[i].vertexCount;
        
        s32 matIdx = model.subMeshes[i].materialIndex;
        if (matIdx >= 0 && matIdx < (s32)model.mat.matrialCount) {
            result.subMeshes[i].material = model.mat.data[matIdx];
        } else {
            obj_matrial_data default_mat = {};
            result.subMeshes[i].material = default_mat;
        }
    }
	
	size_t positionBase = 0;
	size_t normalBase   = positionBase + sizeof(v3);
	size_t uvBase 	  = normalBase + sizeof(v3);
	
	glGenVertexArrays(1, &result.vao);
    glGenBuffers(1, &result.vbo);
    glBindVertexArray(result.vao);
    glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
    glBufferData(GL_ARRAY_BUFFER, model.vertexCount * sizeof(Vertex), model.v, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(model.v[0]), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(model.v[0]), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(model.v[0]), (void*)offsetof(Vertex, uv));
	glBindVertexArray(result.vao);

	return result;
}

Mesh
Create_cube_mesh(){
	Mesh result = {};
	Vertex cubeData[36] = {};
	//done
	cubeData[0]  = {.position = {-0.5f, -0.5f, -0.5f},  .uv = {0.0f, 0.0f}};
	cubeData[1]  = {.position = { 0.5f, -0.5f, -0.5f},  .uv = {1.0f, 0.0f}};
	cubeData[2]  = {.position = { 0.5f,  0.5f, -0.5f},  .uv = {1.0f, 1.0f}};
	cubeData[3]  = {.position = { 0.5f,  0.5f, -0.5f},  .uv = {1.0f, 1.0f}};
	cubeData[4]  = {.position = {-0.5f,  0.5f, -0.5f},  .uv = {0.0f, 1.0f}};
	cubeData[5]  = {.position = {-0.5f, -0.5f, -0.5f},  .uv = {0.0f, 0.0f}};
	//done
	cubeData[6]  = {.position = {-0.5f, -0.5f,  0.5f}, .uv = {0.0f, 0.0f}};
	cubeData[7]  = {.position = { 0.5f, -0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	cubeData[8]  = {.position = { 0.5f,  0.5f,  0.5f}, .uv = {1.0f, 1.0f}};
	cubeData[9]  = {.position = { 0.5f,  0.5f,  0.5f}, .uv = {1.0f, 1.0f}};
	cubeData[10] = {.position = {-0.5f,  0.5f,  0.5f}, .uv = {0.0f, 1.0f}};
	cubeData[11] = {.position = {-0.5f, -0.5f,  0.5f}, .uv = {0.0f, 0.0f}};
	//done
	cubeData[12] = {.position = {-0.5f,  0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	cubeData[13] = {.position = {-0.5f,  0.5f, -0.5f}, .uv = {1.0f, 1.0f}};
	cubeData[14] = {.position = {-0.5f, -0.5f, -0.5f}, .uv = {0.0f, 1.0f}};
	cubeData[15] = {.position = {-0.5f, -0.5f, -0.5f}, .uv = {0.0f, 1.0f}};
	cubeData[16] = {.position = {-0.5f, -0.5f,  0.5f}, .uv = {0.0f, 0.0f}};
	cubeData[17] = {.position = {-0.5f,  0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	
	cubeData[18] = {.position = {0.5f,  0.5f,  0.5f},  .uv = {1.0f, 0.0f}};
	cubeData[19] = {.position = {0.5f,  0.5f, -0.5f},  .uv = {1.0f, 1.0f}};
	cubeData[20] = {.position = {0.5f, -0.5f, -0.5f},  .uv = {0.0f, 1.0f}};
	cubeData[21] = {.position = {0.5f, -0.5f, -0.5f},  .uv = {0.0f, 1.0f}};
	cubeData[22] = {.position = {0.5f, -0.5f,  0.5f},  .uv = {0.0f, 0.0f}};
	cubeData[23] = {.position = {0.5f,  0.5f,  0.5f},  .uv = {1.0f, 0.0f}};
	
	cubeData[24] = {.position = {-0.5f, -0.5f, -0.5f}, .uv = {0.0f, 1.0f}};
	cubeData[25] = {.position = { 0.5f, -0.5f, -0.5f}, .uv = {1.0f, 1.0f}};
	cubeData[26] = {.position = { 0.5f, -0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	cubeData[27] = {.position = { 0.5f, -0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	cubeData[28] = {.position = {-0.5f, -0.5f,  0.5f}, .uv = {0.0f, 0.0f}};
	cubeData[29] = {.position = {-0.5f, -0.5f, -0.5f}, .uv = {0.0f, 1.0f}};
	
	cubeData[30] = {.position = {-0.5f,  0.5f, -0.5f}, .uv = {0.0f, 1.0f}};
	cubeData[31] = {.position = { 0.5f,  0.5f, -0.5f}, .uv = {1.0f, 1.0f}};
	cubeData[32] = {.position = { 0.5f,  0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	cubeData[33] = {.position = { 0.5f,  0.5f,  0.5f}, .uv = {1.0f, 0.0f}};
	cubeData[34] = {.position = {-0.5f,  0.5f,  0.5f}, .uv = {0.0f, 0.0f}};
	cubeData[35] = {.position = {-0.5f,  0.5f, -0.5f}, .uv = {0.0f, 1.0f}};

	// NOTE: see how this is done ??!!
	size_t positionBase = 0;
	size_t normalBase   = positionBase + sizeof(v3);
	size_t uvBase 	  = normalBase + sizeof(v3);
	
	glGenVertexArrays(1, &result.vao);
    glGenBuffers(1, &result.vbo);
    glBindVertexArray(result.vao);
    glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), &cubeData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(cubeData[0]), (void*)positionBase);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(cubeData[0]), (void*)uvBase);
	glBindVertexArray(result.vao);

	return result;
}

void
render_mesh(Mesh *mesh, shader shader){
	mat4 transforms = mat4_identity();
	transforms *= mat4_scale(0.5f);
	transforms *= mat4_translate(0.0f, -0.2f, 0.0f);
	Quaternion orientation = Quat_from_axis_angle({0.0f, 1.0f, 0.0f}, (float)(SDL_GetTicks()/1000.0f));
    mat4 rotation = Quat_to_mat4(orientation);
    transforms *= rotation;
	setShaderValue_mat4(shader, "model", transforms);
	
	glBindVertexArray(mesh->vao);
	for(u32 i = 0; i < mesh->subMeshCount; ++i){
		sub_mesh sub = mesh->subMeshes[i];
		obj_matrial_data mat = sub.material;
		
		if(mesh->subMeshes[i].materialIndex >= 0){
			glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mat.texture);
			setShaderValue_i1(shader, "tex", 0);
		}
	
		glDrawArrays(GL_TRIANGLES, sub.startVertexIndex, sub.vertexCount);
	}
}

