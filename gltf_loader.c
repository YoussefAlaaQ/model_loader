u8* 
base64_to_Bytes(Arena *arena, char *input, u32 byteLength){
	u32 inputLength = strlen(input);
	u8 *bytes = PushArray(arena, u8, byteLength);

	u32 out = 0;
	for (u32 i = 0; i < inputLength; i += 4)
    {
		int a = b64invs[(u8)input[i] - '+'];
		int b = b64invs[(u8)input[i+1] - '+'];
		int c = input[i+2] == '=' ? 0 : b64invs[(u8)input[i+2] - '+'];
		int d = input[i+3] == '=' ? 0 : b64invs[(u8)input[i+3] - '+'];
    
		u32 value = (a << 18) | 
					(b << 12) | 
					(c << 6)  | 
					 d;

		Assert(out < byteLength);
		bytes[out++] = (value >> 16) & 0xff;
		if(input[i+2] != '='){
			Assert(out < byteLength);
			bytes[out++] = (value >> 8) & 0xff;
		}
		if(input[i+3] != '='){
			Assert(out < byteLength);
			bytes[out++] = value & 0xff;
		}

    }

    return bytes;
}

internal void
process_numeric_token(Arena *arena, char **cursor, gltf_token* token){
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
process_real_numeric_token(Arena *arena, char **cursor, gltf_token* token){
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
process_word_token(Arena *arena, char **cursor, gltf_token* token){
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

internal void
process_gltf_word_token(Arena *arena, char **cursor, gltf_token* token){
	char* tokenStart = *cursor; 
	size_t  tokenSize, i;
	
	while(**cursor != '"'){
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

//GLTF PARSER
internal gltf_token
lex_gltf(Arena *arena, char **cursor){
	gltf_token token = {};
	token.gltf_type = GLTF_UNIDENTIFIED_TOKEN;
	while(**cursor == ' ' || **cursor == '\t' || **cursor == '\r' || **cursor == '\n'){
		++*cursor;
	}

	if(**cursor == '{'){
		token.gltf_type = GLTF_BRACE_OPEN_TOKEN;
	}
	else if(**cursor == '}'){
		token.gltf_type = GLTF_BRACE_CLOSE_TOKEN;
	}
	else if(**cursor == '['){
		token.gltf_type = GLTF_BRACKET_OPEN_TOKEN;
	}
	else if(**cursor == ']'){
		token.gltf_type = GLTF_BRACKET_CLOSE_TOKEN;
	}
	else if(**cursor == ','){
		token.gltf_type = GLTF_COMMA_TOKEN;
	}
	else if(**cursor == ':'){
		token.gltf_type = GLTF_COLON_TOKEN;
	}
	else if(**cursor == '"' || isalpha(**cursor)){
		// process word token
		//NOTE: the if is just made because i don't want to break the existing word parsing
		// and i didn't know if it will be compatable with the standered word parsing
		// the only diffence between both is that the gltf one will start from " to " 
		if(**cursor == '"'){
			++(*cursor);
			process_gltf_word_token(arena, cursor, &token);
		}
		else{
			process_word_token(arena, cursor, &token);
		}
		token.gltf_type = GLTF_WORD_TOKEN;
	}
	else if(isdigit(**cursor) || **cursor == '-'){
		//process numaric token
		process_numeric_token(arena, cursor, &token);
		// process_real_numeric_token(arena, cursor, &token);
		token.gltf_type = GLTF_NUMBER_TOKEN;
	}
	else {
		token.gltf_type = GLTF_UNIDENTIFIED_TOKEN;
	}
	return token;
}

internal gltf_token
get_gltf_token_and_advance(Arena *arena, char** cursor){
	gltf_token t = lex_gltf(arena, cursor);
	++*cursor;
	return t;
}


internal void
parse_gltf_array(Arena *arena, Gltf_info *info, const char *arrayName, char **cursor){
    u32 counter = 0;
	gltf_token token = get_gltf_token_and_advance(arena, cursor);
    while(token.gltf_type != GLTF_BRACKET_CLOSE_TOKEN){
        if(token.gltf_type == GLTF_NUMBER_TOKEN){
            if(!strcmp(arrayName, "nodes")){
				info->scenesInfo[info->scenesInfoCounter].nodeIdxs[counter++] = token.indexData;
            }
            else if(!strcmp(arrayName, "min")){
				info->accessors[info->accCounter].min[counter++] = token.realData;
            }
            else if(!strcmp(arrayName, "max")){
				info->accessors[info->accCounter].max[counter++] = token.realData;
            }
            else if(!strcmp(arrayName, "children")){
				info->nodesInfo[info->nodesInfoCounter].childrenIdxs[counter++] = token.indexData;
            }
            else if(!strcmp(arrayName, "joints")){
				info->skinsInfo[info->skinsInfoCounter].joints[counter++] = token.indexData;
            }
            else if(!strcmp(arrayName, "matrix")){
				info->nodesInfo[info->nodesInfoCounter].matrix.e[counter++] = token.realData;
				info->nodesInfo[info->nodesInfoCounter].hasMatrix = true;
            }
            else if(!strcmp(arrayName, "translation")){
				info->nodesInfo[info->nodesInfoCounter].translation.e[counter++] = token.realData;
				info->nodesInfo[info->nodesInfoCounter].hasTranslation = true;
            }
            else if(!strcmp(arrayName, "rotation")){
				info->nodesInfo[info->nodesInfoCounter].rotation.e[counter++] = token.realData;
            }
            else if(!strcmp(arrayName, "scale")){
				info->nodesInfo[info->nodesInfoCounter].scale.e[counter++] = token.realData;
				info->nodesInfo[info->nodesInfoCounter].hasScale = true;
            }
            else if(!strcmp(arrayName, "emissiveFactor")){
				info->materials[info->matCounter].emissiveFactor.e[counter++] = token.realData;
            }
        }
        else if(token.gltf_type == GLTF_BRACE_OPEN_TOKEN){
            if(!strcmp(info->parentArrayName, "accessors")){
				info->accCounter++;
			}
			else if(!strcmp(info->parentArrayName, "bufferViews")){
				info->bvCounter++;
			}
			else if(!strcmp(info->parentArrayName, "buffers")){
				info->dbCounter++;
			}
			else if(!strcmp(info->parentArrayName, "images")){
				info->imgCounter++;
			}
			else if(!strcmp(info->parentArrayName, "materials")){
				info->matCounter++;
			}
			else if(!strcmp(info->parentArrayName, "meshes") && strcmp(arrayName, "primitives")){
				info->meshesInfoCounter++;
			}
			else if(!strcmp(info->parentArrayName, "nodes")){
				info->nodesInfoCounter++;
			}
			else if(!strcmp(info->parentArrayName, "samplers")){
				info->samplersCounter++;
			}
			else if(!strcmp(info->parentArrayName, "scenes")){
				info->scenesInfoCounter++;
			}
			else if(!strcmp(info->parentArrayName, "textures")){
				info->textureCounter++;
			}
			else if(!strcmp(info->parentArrayName, "animations")){
					if(strcmp(arrayName, "channels") && strcmp(arrayName, "samplers")){
						info->animationsInfoCounter++;
					}
			}
			else if(!strcmp(info->parentArrayName, "skins")){
				info->skinsInfoCounter++;
			}
            
            if(!strcmp(arrayName, "primitives")){
				info->meshesInfo[info->meshesInfoCounter].primativesCounter++;
			}
			else if(!strcmp(arrayName, "channels")){
				info->animationsInfo[info->animationsInfoCounter].animChannelsCounter++;
			}
			else if(!strcmp(arrayName, "samplers") && !strcmp(info->parentArrayName, "animations")){
				info->animationsInfo[info->animationsInfoCounter].animSamplersCounter++;
			}
            parse_gltf_object(arena, info, cursor); // array of objects
        }
        token = get_gltf_token_and_advance(arena, cursor);
    }
}

const char *pArrayName = 0;

internal void
parse_gltf_object(Arena *arena, Gltf_info *info, char **cursor){
	gltf_token token = get_gltf_token_and_advance(arena, cursor);
	const char *word = "";
	gltf_token prevToken = {};
	//TODO: remove of strcmp into a function and use enum here instead
	while(token.gltf_type != GLTF_BRACE_CLOSE_TOKEN){
		switch(token.gltf_type){
			case GLTF_BRACE_OPEN_TOKEN:{
				const char *oldObjectName = info->objectName;
				info->objectName = word;
				parse_gltf_object(arena, info, cursor);
				info->objectName = oldObjectName;
			}break;
			case GLTF_BRACKET_OPEN_TOKEN:{
				info->arrayName = word;
				if(!strcmp(info->arrayName, "accessors")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "bufferViews")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "buffers")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "images")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "materials")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "meshes")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "nodes") && strcmp(info->parentArrayName, "scenes")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "samplers") && strcmp(info->parentArrayName, "animations")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "scenes")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "skins")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "animations")){
					info->parentArrayName = info->arrayName;
				}
				else if(!strcmp(info->arrayName, "textures")){
					info->parentArrayName = info->arrayName;
				}
				parse_gltf_array(arena, info, info->arrayName, cursor);
			}break;
			case GLTF_WORD_TOKEN:{
				word = token.stringData;
				if(!strcmp(info->parentArrayName, "accessors")){
					if(!strcmp(word, "type")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						if(!strcmp(token.stringData, "SCALAR")){
							info->accessors[info->accCounter].DataType = SCALOR;
						}
						else if(!strcmp(token.stringData, "VEC2")){
							info->accessors[info->accCounter].DataType = VEC2;
						}
						else if(!strcmp(token.stringData, "VEC3")){
							info->accessors[info->accCounter].DataType = VEC3;
						}
						else if(!strcmp(token.stringData, "VEC4")){
							info->accessors[info->accCounter].DataType = VEC4;
						}
						else if(!strcmp(token.stringData, "MAT4")){
							info->accessors[info->accCounter].DataType = MAT4;
						}
					}
				}
				else if(!strcmp(info->parentArrayName, "bufferViews")){
					if(!strcmp(word, "name")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						info->bufferViews[info->bvCounter].name = token.stringData;
					}
				}
				else if(!strcmp(info->parentArrayName, "buffers")){
					if(!strcmp(word, "uri")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						// this can be a file path or raw date and will later be processed
						info->buffers[info->dbCounter].stringData = token.stringData;
					}
				}
				else if(!strcmp(info->parentArrayName, "images")){
					if(!strcmp(word, "uri")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						// this can be a file path or raw date and will later be processed
						info->images[info->imgCounter].path = token.stringData;
					}
				}
				else if(!strcmp(info->parentArrayName, "materials")){
					if(!strcmp(word, "doubleSided")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						// this can be a file path or raw date and will later be processed
						if(!strcmp(token.stringData, "true")){
							info->materials[info->matCounter].doubleSided = true;
						}
						else{
							info->materials[info->matCounter].doubleSided = false;
						}
					}
					else if(!strcmp(word, "name")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						// this can be a file path or raw date and will later be processed
						info->materials[info->matCounter].name = token.stringData;
					}
				}
				else if(!strcmp(info->parentArrayName, "meshes")){
					if(!strcmp(word, "name")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						info->meshesInfo[info->meshesInfoCounter].name = token.stringData;
					}
				}
				else if(!strcmp(info->parentArrayName, "nodes")){
					if(!strcmp(word, "name")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						info->nodesInfo[info->nodesInfoCounter].name = token.stringData;
					}
				}
				else if(!strcmp(info->parentArrayName, "animations")){
					if(!strcmp(word, "name")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						info->animationsInfo[info->animationsInfoCounter].name = token.stringData;
					}
					else if(!strcmp(info->arrayName, "channels")){
						if(!strcmp(word, "path")){
							token = get_gltf_token_and_advance(arena, cursor);
							token = get_gltf_token_and_advance(arena, cursor);
							u32 counter = info->animationsInfo[info->animationsInfoCounter].animChannelsCounter;
							if(!strcmp(token.stringData, "translation")){
								info->animationsInfo[info->animationsInfoCounter].animChannels[counter].path = TRANSLATION;
							}
							else if(!strcmp(token.stringData, "rotation")){
								info->animationsInfo[info->animationsInfoCounter].animChannels[counter].path = ROTATION;
							}
							else if(!strcmp(token.stringData, "scale")){
								info->animationsInfo[info->animationsInfoCounter].animChannels[counter].path = SCALE;
							}
						}
					}
					else if(!strcmp(info->arrayName, "samplers")){
						if(!strcmp(word, "interpolation")){
							token = get_gltf_token_and_advance(arena, cursor);
							token = get_gltf_token_and_advance(arena, cursor);
							u32 counter = info->animationsInfo[info->animationsInfoCounter].animSamplersCounter;
							if(!strcmp(token.stringData, "LINEAR")){
								info->animationsInfo[info->animationsInfoCounter].animSamplers[counter].interpolation = LINEAR;
							}
						}
					}
				}
				else if(!strcmp(info->parentArrayName, "scenes")){
					if(!strcmp(word, "name")){
						token = get_gltf_token_and_advance(arena, cursor);
						token = get_gltf_token_and_advance(arena, cursor);
						info->scenesInfo[info->scenesInfoCounter].name = token.stringData;
					}
				}
			}break;
			case GLTF_NUMBER_TOKEN:{
				if(prevToken.gltf_type == GLTF_COLON_TOKEN){
					if(!strcmp(word, "scene")){
						info->defaultScene = token.indexData;
					}
					else if(!strcmp(info->parentArrayName, "accessors")){
						if(!strcmp(word, "bufferView")){
							info->accessors[info->accCounter].bufferViewIdx = token.indexData;
						}
						else if(!strcmp(word, "byteOffset")){
							info->accessors[info->accCounter].byteOffset = token.indexData;
						}
						else if(!strcmp(word, "componentType")){
							info->accessors[info->accCounter].componentType = token.indexData;
						}
						else if(!strcmp(word, "count")){
							info->accessors[info->accCounter].count = token.indexData;
						}
					}
					else if(!strcmp(info->parentArrayName, "bufferViews")){
						if(!strcmp(word, "buffer")){
							info->bufferViews[info->bvCounter].bufferIdx = token.indexData;
						}
						else if(!strcmp(word, "byteLength")){
							info->bufferViews[info->bvCounter].byteLength = token.indexData;
						}
						else if(!strcmp(word, "byteOffset")){
							info->bufferViews[info->bvCounter].byteOffset = token.indexData;
						}
						else if(!strcmp(word, "byteStride")){
							info->bufferViews[info->bvCounter].byteStride = token.indexData;
						}
						else if(!strcmp(word, "target")){
							info->bufferViews[info->bvCounter].targetIdx = token.indexData;
						}
					}
					else if(!strcmp(info->parentArrayName, "buffers")){
						if(!strcmp(word, "byteLength")){
							info->buffers[info->dbCounter].byteLength = token.indexData;
						}
					}
					else if(!strcmp(info->parentArrayName, "materials")){
						if(!strcmp(word, "metallicFactor")){
							info->materials[info->matCounter].metallicFactor = token.realData;
						}
						else if(!strcmp(word, "roughnessFactor")){
							info->materials[info->matCounter].roughnessFactor = token.realData;
						}
						else if(!strcmp(info->objectName, "emissiveTexture")){
							if(!strcmp(word, "index")){
								info->materials[info->matCounter].emissiveTextureIdx = token.indexData;
							}
						}
						else if(!strcmp(info->objectName, "baseColorTexture")){
							if(!strcmp(word, "index")){
								info->materials[info->matCounter].baseColorTextureIdx = token.indexData;
							}
						}
						else if(!strcmp(info->objectName, "diffuseTexture")){
							if(!strcmp(word, "index")){
								info->materials[info->matCounter].diffuseTextureIdx = token.indexData;
							}
						}
						
					}
					else if(!strcmp(info->parentArrayName, "meshes")){
						if(!strcmp(info->arrayName, "primitives")){
							u32 counter = info->meshesInfo[info->meshesInfoCounter].primativesCounter;
							if(!strcmp(word, "NORMAL")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].normalIdx = token.indexData;
							}
							else if(!strcmp(word, "POSITION")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].positionIdx = token.indexData;
							}
							else if(!strcmp(word, "JOINTS_0")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].jointsIdx = token.indexData;
							}
							else if(!strcmp(word, "WEIGHTS_0")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].weightIdx = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_0")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[0] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_1")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[1] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_2")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[2] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_3")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[3] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_4")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[4] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_5")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[5] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_6")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[6] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_7")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[7] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_8")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[8] = token.indexData;
							}
							else if(!strcmp(word, "TEXCOORD_9")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].TexCoords[9] = token.indexData;
							}
							else if(!strcmp(word, "indices")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].indicesIdx = token.indexData;
							}
							else if(!strcmp(word, "material")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].materialIdx = token.indexData;
							}
							else if(!strcmp(word, "mode")){
								info->meshesInfo[info->meshesInfoCounter].primativesInfo[counter].mode = token.indexData;
							}
						}
					}
					else if(!strcmp(info->parentArrayName, "nodes")){
						if(!strcmp(word, "mesh")){
							info->nodesInfo[info->nodesInfoCounter].meshIdx = token.indexData;
						}
						else if(!strcmp(word, "skin")){
							info->nodesInfo[info->nodesInfoCounter].skinIdx = token.indexData;
						}
					}
					else if(!strcmp(info->parentArrayName, "animations")){
						if(!strcmp(info->arrayName, "channels")){
							u32 counter = info->animationsInfo[info->animationsInfoCounter].animChannelsCounter;
							if(!strcmp(word, "sampler")){
								info->animationsInfo[info->animationsInfoCounter].animChannels[counter].samplerIdx = token.indexData;
							}
							else if(!strcmp(word, "node")){
								info->animationsInfo[info->animationsInfoCounter].animChannels[counter].targetNode = token.indexData;
							}
						}
						else if(!strcmp(info->arrayName, "samplers")){
							u32 counter = info->animationsInfo[info->animationsInfoCounter].animSamplersCounter;
							if(!strcmp(word, "input")){
								info->animationsInfo[info->animationsInfoCounter].animSamplers[counter].inputIdx = token.indexData;
							}
							else if(!strcmp(word, "output")){
								info->animationsInfo[info->animationsInfoCounter].animSamplers[counter].outPutIdx = token.indexData;
							}
						}
					}
					else if(!strcmp(info->parentArrayName, "skins")){
						if(!strcmp(word, "inverseBindMatrices")){
							info->skinsInfo[info->skinsInfoCounter].inverseBindMatricesIdx = token.indexData;
						}
						else if(!strcmp(word, "skeleton")){
							info->skinsInfo[info->skinsInfoCounter].skeletonIdx = token.indexData;
						}
					}
					else if(!strcmp(info->parentArrayName, "samplers")){
						if(!strcmp(word, "magFilter")){
							info->samplers[info->samplersCounter].magFilter = token.indexData;
						}
						else if(!strcmp(word, "minFilter")){
							info->samplers[info->samplersCounter].minFilter = token.indexData;
						}
						else if(!strcmp(word, "wrapS")){
							info->samplers[info->samplersCounter].wrapS = token.indexData;
						}
						else if(!strcmp(word, "wrapT")){
							info->samplers[info->samplersCounter].wrapT = token.indexData;
						}
					}
					else if(!strcmp(info->parentArrayName, "textures")){
						if(!strcmp(word, "sampler")){
							info->textures[info->textureCounter].sampler = token.indexData;
						}
						else if(!strcmp(word, "source")){
							info->textures[info->textureCounter].srcIdx = token.indexData;
						}
					}
					
				}
			}break;
		}
		prevToken = token;
		token = get_gltf_token_and_advance(arena, cursor);
	}
}

internal void
skip_object(Arena *arena, char **cursor){
	gltf_token token = get_gltf_token_and_advance(arena, cursor);
	while(token.gltf_type != GLTF_BRACE_CLOSE_TOKEN){
		if(token.gltf_type == GLTF_BRACE_OPEN_TOKEN){
			skip_object(arena, cursor);
		}
		token = get_gltf_token_and_advance(arena, cursor);
	}
}

internal u32
get_gltf_array_size(Arena *arena, Gltf_info *info, char *cursor){
	gltf_token token = get_gltf_token_and_advance(arena, &cursor);
	u32 result = 0;
	while(token.gltf_type != GLTF_BRACKET_CLOSE_TOKEN){
		if(token.gltf_type == GLTF_COMMA_TOKEN){
			result++;
		}
		else if(token.gltf_type == GLTF_BRACE_OPEN_TOKEN){
			skip_object(arena, &cursor);
		}
		token = get_gltf_token_and_advance(arena, &cursor);
	}
	return result + 1;
}

internal void
handle_gltf_object(Arena *arena, Gltf_info *info, char **cursor){
	gltf_token token = get_gltf_token_and_advance(arena, cursor);
	u32 size = 0;
	const char *word = "";
	const char *arrayName = info->arrayName;
	const char *objectName = "";
	while(token.gltf_type != GLTF_BRACE_CLOSE_TOKEN){
		switch(token.gltf_type){
			case GLTF_BRACE_OPEN_TOKEN:{
				objectName = word;
				handle_gltf_object(arena, info, cursor);
				if(info->parentArrayName != NULL){
					if(!strcmp(info->parentArrayName, "nodes") && strcmp(objectName, "extras")){ // gaurd against this spacific case
						info->nodesInfoCounter++;
						if(info->nodesInfoCounter == 14){
							int x = 1;
						}
					}
					else if(!strcmp(info->parentArrayName, "animations")){
						if(arrayName && !strcmp(arrayName, "animations")) {
							info->animationsInfoCounter++;
						}
					}
				}
			}break;
			case GLTF_BRACKET_OPEN_TOKEN:{
				arrayName = word;
				info->arrayName = arrayName;
				info->currentArraySize = get_gltf_array_size(arena, info, *cursor);
				if(!strcmp(arrayName, "accessors")){
					info->accessors = PushArrayZero(arena, Accessor, info->currentArraySize);
					
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "bufferViews")){
					info->bufferViews = PushArrayZero(arena, BufferView, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "buffers")){
					info->buffers = PushArrayZero(arena, DataBuffer, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "images")){
					info->images = PushArrayZero(arena, Gltf_image, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "materials")){
					info->materials = PushArrayZero(arena, Gltf_materials, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "meshes")){
					info->meshesInfo = PushArrayZero(arena, Gltf_meshes_info, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "nodes") && strcmp(info->parentArrayName, "scenes")){
					info->nodesInfo = PushArrayZero(arena, Gltf_nodes_info, info->currentArraySize);
					for(int i = 0; i < info->currentArraySize; i++){
						info->nodesInfo[i].matrix = mat4_identity();
						info->nodesInfo[i].meshIdx = INVALID_INDEX;
						info->nodesInfo[i].skinIdx = INVALID_INDEX;
						info->nodesInfo[i].rotation.w = 1.0f;
						info->nodesInfo[i].scale = {1.0f, 1.0f, 1.0f};
					}
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "samplers") && strcmp(info->parentArrayName, "animations")){
					info->samplers = PushArrayZero(arena, Gltf_samplers, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "scenes")){
					info->scenesInfo = PushArrayZero(arena, Gltf_scenes_info, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "animations")){
					info->animationsInfo = PushArrayZero(arena, Gltf_animations_info, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "skins")){
					info->skinsInfo = PushArrayZero(arena, Gltf_skins_info, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "textures")){
					info->textures = PushArrayZero(arena, Gltf_texture, info->currentArraySize);
					info->parentArrayName = arrayName;
				}
				else if(!strcmp(arrayName, "primitives") && !strcmp(info->parentArrayName, "meshes")){
					info->meshesInfo[info->meshesInfoCounter].primativesCounter = -1;
					info->meshesInfo[info->meshesInfoCounter].primativesInfo = PushArrayZero(arena, mesh_primative_info, info->currentArraySize);
					for(u32 i = 0; i < info->currentArraySize; ++i){
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].positionIdx    = INVALID_INDEX;
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].normalIdx      = INVALID_INDEX;
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].jointsIdx      = INVALID_INDEX;
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].weightIdx      = INVALID_INDEX;
						for(int texIdx = 0; texIdx < MAX_TEXCOORD; texIdx++){
							info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].TexCoords[texIdx]  = INVALID_INDEX;
						}
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].indicesIdx     = INVALID_INDEX;
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].materialIdx    = INVALID_INDEX;
						info->meshesInfo[info->meshesInfoCounter].primativesInfo[i].mode           = 4;
					}
					info->meshesInfoCounter++;
				}
				else if(!strcmp(arrayName, "children") &&!strcmp(info->parentArrayName, "nodes")){
					info->nodesInfo[info->nodesInfoCounter].childrenIdxs = PushArray(arena, u32, info->currentArraySize);
					info->nodesInfo[info->nodesInfoCounter].childrenCount = info->currentArraySize;
				}
				else if( !strcmp(arrayName, "joints") &&!strcmp(info->parentArrayName, "skins")){
					info->skinsInfo[info->skinsInfoCounter].joints = PushArray(arena, u32, info->currentArraySize);
					info->skinsInfo[info->skinsInfoCounter].jointsCount = info->currentArraySize;
				}
				else if(!strcmp(info->parentArrayName, "scenes") && !strcmp(arrayName, "nodes")){
					info->scenesInfo[info->scenesInfoCounter].nodeCount = info->currentArraySize;
					info->scenesInfo[info->scenesInfoCounter++].nodeIdxs = PushArrayZero(arena, u32, info->currentArraySize);
				}
				else if(!strcmp(arrayName, "samplers") && !strcmp(info->parentArrayName, "animations")){
					// printf("#%i animation has %i samplers\n", info->animationsInfoCounter, info->currentArraySize);
					info->animationsInfo[info->animationsInfoCounter].animSamplersCounter = -1;
					info->animationsInfo[info->animationsInfoCounter].animSamplers = PushArrayZero(arena, Animation_samplers_info, info->currentArraySize);
				}
				else if(!strcmp(arrayName, "channels") && !strcmp(info->parentArrayName, "animations")){
					info->animationsInfo[info->animationsInfoCounter].animChannels = PushArrayZero(arena, Animation_channels_info, info->currentArraySize);
					info->animationsInfo[info->animationsInfoCounter].animChannelsCounter = -1;
					// printf("#%i animation has %i channels\n", info->animationsInfoCounter, info->currentArraySize);
				}
			}break;
			case GLTF_WORD_TOKEN:{
				word = token.stringData;
			}break;
		}
		token = get_gltf_token_and_advance(arena, cursor);
	}
}


internal void 
initialize_gltf_info(Arena *arena, Gltf_info *info, char *cursor){
	gltf_token token = get_gltf_token_and_advance(arena, &cursor);
	while(token.gltf_type != GLTF_UNIDENTIFIED_TOKEN){
		if(token.gltf_type == GLTF_BRACE_OPEN_TOKEN){
			handle_gltf_object(arena, info, &cursor);
		}
		token = get_gltf_token_and_advance(arena, &cursor);
	}
	info->defaultScene = INVALID_INDEX;
	
	info->bvCounter 			= -1;
	info->accCounter 	   	= -1; 
	info->dbCounter 			= -1;
	info->imgCounter  	  	= -1;
	info->matCounter 	   	= -1;
	info->meshesInfoCounter 	= -1;
	info->nodesInfoCounter  	= -1;
	info->samplersCounter   	= -1;
	info->scenesInfoCounter 	= -1;
	info->textureCounter    	= -1;
	info->skinsInfoCounter  	= -1;
	info->animationsInfoCounter = -1;
	
	info->parentArrayName = "";
	info->arrayName = "";
}


internal void
compute_node_transforms(Scene_node *outNode, u32 nodeIdx, mat4 parentTransforms){
	Scene_node *node = &outNode[nodeIdx];
	node->worldTransforms = node->localTransforms * parentTransforms;
	
	for(u32 i = 0; i < node->childrenCount; ++i){
		compute_node_transforms(outNode, node->childrenIdxs[i], node->worldTransforms);
	}
}

internal mat4*
extract_mat4_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx]; // don't forget it has count ??!
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx]; 
	
	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	
	u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(mat4);
	u32 count = acc.count;
	mat4 *result = PushArray(arena, mat4, count);
	for(int i = 0; i < acc.count; ++i){
		u8 *src = base + i * stride;
		memcpy(&result[i], src, sizeof(mat4));
	}
	return result;
}

internal v4i*
extract_v4i_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx];
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx];

	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	u32 count = acc.count;
	v4i *result = PushArray(arena, v4i, count);

	if(acc.componentType == 5121){
		u32 stride = (bv.byteStride != 0) ? bv.byteStride : (4 * sizeof(u8));
		for(u32 i = 0; i < count; ++i){
			u8 *src = base + i * stride;
			result[i].x = src[0];
			result[i].y = src[1];
			result[i].z = src[2];
			result[i].w = src[3];
		}
	}
	else if(acc.componentType == 5123){
		u32 stride = (bv.byteStride != 0) ? bv.byteStride : (4 * sizeof(u16));
		for(u32 i = 0; i < count; ++i){
			u16 *src = (u16*)(base + i * stride);
			result[i].x = src[0];
			result[i].y = src[1];
			result[i].z = src[2];
			result[i].w = src[3];
		}
	}
	else if(acc.componentType == 5125){
		u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(v4i);
		for(u32 i = 0; i < count; ++i){
			u32 *src = (u32*)(base + i * stride);
			result[i].x = src[0];
			result[i].y = src[1];
			result[i].z = src[2];
			result[i].w = src[3];
		}
	}
	else{
		Assert(false);
	}

	return result;
}

internal v4*
extract_v4_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx]; // don't forget it has count ??!
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx]; 
	
	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	
	u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(v4);
	u32 count = acc.count;
	v4 *result = PushArray(arena, v4, count);
	for(int i = 0; i < acc.count; ++i){
		u8 *src = base + i * stride;
		memcpy(&result[i], src, sizeof(v4));
	}
	return result;
}

internal v3*
extract_v3_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx]; // don't forget it has count ??!
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx]; 
	
	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	
	u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(v3);
	u32 count = acc.count;
	v3 *result = PushArray(arena, v3, count);
	for(int i = 0; i < acc.count; ++i){
		u8 *src = base + i * stride;
		memcpy(&result[i], src, sizeof(v3));
	}
	return result;
}

internal v2*
extract_v2_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx]; // don't forget it has count ??!
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx]; 
	
	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	
	u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(v2);
	u32 count = acc.count;
	v2 *result = PushArray(arena, v2, count);
	for(int i = 0; i < acc.count; ++i){
		u8 *src = base + i * stride;
		memcpy(&result[i], src, sizeof(v2));
	}
	return result;
}

internal u32*
extract_u32_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx]; // don't forget it has count ??!
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx]; 
	
	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	
	u32 count = acc.count;
	u32 *result = PushArray(arena, u32, count);
	if(acc.componentType == 5125){
        u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(u32);
        for(u32 i = 0; i < count; ++i){
            memcpy(&result[i], base + i * stride, sizeof(u32));
        }
    }
    else if(acc.componentType == 5123){
        u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(u16);
        for(u32 i = 0; i < count; ++i){
            u16 v;
            memcpy(&v, base + i * stride, sizeof(u16));
            result[i] = (u32)v;
        }
    }
	return result;
}

internal f32*
extract_f32_From_Buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	Accessor acc = info->accessors[accIdx]; // don't forget it has count ??!
	BufferView bv = info->bufferViews[acc.bufferViewIdx];
	DataBuffer buffer = info->buffers[bv.bufferIdx]; 
	
	u8 *base = buffer.data + bv.byteOffset + acc.byteOffset;
	
	u32 stride = (bv.byteStride != 0) ? bv.byteStride : sizeof(f32);
	u32 count = acc.count;
	f32 *result = PushArray(arena, f32, count);
    for(u32 i = 0; i < count; ++i){
        memcpy(&result[i], base + i * stride, sizeof(f32));
    }
	return result;
}

//TODO: add checking for the type if it is short or int 
#define EXTRACT_DATA_FROM_BUFFER(arena, info, accIdx, type) (type*)extract_data_from_buffer((arena), (info), (accIdx))
internal void*
extract_data_from_buffer(Arena *arena, Gltf_info *info, u32 accIdx){
	switch(info->accessors[accIdx].DataType){
		case MAT4:{
			return extract_mat4_From_Buffer(arena, info, accIdx);
		}break;
		case VEC4:{
			if(info->accessors[accIdx].componentType == 5125 || 
			   info->accessors[accIdx].componentType == 5123 || 
			   info->accessors[accIdx].componentType == 5121)
			{
				return extract_v4i_From_Buffer(arena, info, accIdx);
			}
			else if(info->accessors[accIdx].componentType == 5126){
				return extract_v4_From_Buffer(arena, info, accIdx);
			}
		}break;
		case VEC3:{
			return extract_v3_From_Buffer(arena, info, accIdx);
		}break;
		case VEC2:{
			return extract_v2_From_Buffer(arena, info, accIdx);
		}break;
		case SCALOR:{
			if(info->accessors[accIdx].componentType == 5125 || info->accessors[accIdx].componentType == 5123){
				return extract_u32_From_Buffer(arena, info, accIdx);
			}
			else if(info->accessors[accIdx].componentType == 5126){
				return extract_f32_From_Buffer(arena, info, accIdx);
			}
		}break;
	}

	return NULL;
}

internal GLTF_Scene
parse_gltf(Arena *permArena, Arena *scratchArena, const char *path){
	char *cursor = loadFile(path);

	Gltf_info gltfInfo = {};
	initialize_gltf_info(scratchArena, &gltfInfo, cursor);

	gltf_token token = {};
	token = get_gltf_token_and_advance(permArena, &cursor);
	while(token.gltf_type != GLTF_UNIDENTIFIED_TOKEN){
		if(token.gltf_type == GLTF_BRACE_OPEN_TOKEN){
			parse_gltf_object(scratchArena, &gltfInfo, &cursor);
		}
		token = get_gltf_token_and_advance(scratchArena, &cursor);
	}

	u32 bufferCount = gltfInfo.dbCounter  + 1;
	u32 imgCount    = gltfInfo.imgCounter + 1;
	u32 accCount    = gltfInfo.accCounter + 1;
	u32 bvCount     = gltfInfo.bvCounter  + 1;
	u32 matCount    = gltfInfo.matCounter + 1;
	u32 nodeCount   = gltfInfo.nodesInfoCounter  + 1;
	u32 sceneCount  = gltfInfo.scenesInfoCounter + 1; 
	u32 meshCount   = gltfInfo.meshesInfoCounter + 1;
	u32 skinCount   = gltfInfo.skinsInfoCounter + 1;
	u32 animCount   = gltfInfo.animationsInfoCounter + 1; 
	char folderPath[256] = {};
	remove_file_name_from_path(folderPath, path);
	char binPath[128] = {};
	size_t size = 0;
	for(int i = 0; i < bufferCount; ++i){
		concat_small_string(binPath, folderPath, gltfInfo.buffers[i].stringData);
		u8 *data = (u8*)SDL_LoadFile(binPath, &size); 
		if(data != NULL){
			gltfInfo.buffers[i].data = PushArray(permArena, u8, size);
			memcpy(gltfInfo.buffers[i].data, data, size);
			SDL_free(data);
		}
	}

	char texFilePath[128] = {};
	for(u32 i = 0; i < imgCount; ++i){
		concat_small_string(texFilePath, folderPath, gltfInfo.images[i].path);
		gltfInfo.images[i].texture = loadTextureRGB(texFilePath, false);
		// gltfInfo.images[i].texture = loadTextureSRGB(texFilePath, false);
		
	}

	// prepare scene nodes & the scene
	GLTF_Scene result = {};
	result.scene = PushArray(permArena, Scene, sceneCount);
	result.sceneCount = sceneCount;
	for(u32 i = 0; i < sceneCount; ++i){
		result.scene[i].rootCount = gltfInfo.scenesInfo[i].nodeCount;
		result.scene[i].rootNodes = PushArray(permArena, u32, result.scene[i].rootCount);
		for(u32 j = 0; j < result.scene[i].rootCount; ++j){
			result.scene[i].rootNodes[j] = gltfInfo.scenesInfo[i].nodeIdxs[j];
		}
	}

	result.nodes = PushArray(permArena, Scene_node, nodeCount);
	result.nodeCount = nodeCount;
	for(u32 i = 0; i < nodeCount; ++i){
		result.nodes[i].translation = gltfInfo.nodesInfo[i].translation;
		result.nodes[i].rotation    = gltfInfo.nodesInfo[i].rotation;
		result.nodes[i].scale       = gltfInfo.nodesInfo[i].scale;
		
		if(gltfInfo.nodesInfo[i].hasMatrix){
			result.nodes[i].localTransforms = mat4_transpose(gltfInfo.nodesInfo[i].matrix);
		}
		else{
			mat4 rotationMat    = Quat_to_mat4(gltfInfo.nodesInfo[i].rotation);
			mat4 translationMat = mat4_translate(gltfInfo.nodesInfo[i].translation);
			mat4 scaleMat       = mat4_scale(gltfInfo.nodesInfo[i].scale);
			result.nodes[i].localTransforms = mat4_identity();
			result.nodes[i].localTransforms *= scaleMat;
			result.nodes[i].localTransforms *= rotationMat;
			result.nodes[i].localTransforms *= translationMat;
		}
		
		result.nodes[i].meshIdx = gltfInfo.nodesInfo[i].meshIdx;
		result.nodes[i].skinIdx = gltfInfo.nodesInfo[i].skinIdx;
		
		result.nodes[i].childrenCount = gltfInfo.nodesInfo[i].childrenCount;
		result.nodes[i].childrenIdxs = PushArray(permArena, u32, result.nodes[i].childrenCount);
		for(u32 j = 0; j < result.nodes[i].childrenCount; ++j){
			result.nodes[i].childrenIdxs[j] = gltfInfo.nodesInfo[i].childrenIdxs[j];
		}
	}

	// chech when should i calc the node transforms if there is multiple scenes 
	for(int i = 0; i < sceneCount; ++i){
		for(int j = 0; j < result.scene[i].rootCount; j++){
			compute_node_transforms(result.nodes, result.scene[i].rootNodes[j], mat4_identity());
		}
	}

	// print_mat4(result.nodes[nodeCount-1].worldTransforms);

	result.meshes = PushArray(permArena, Mesh_geometry, meshCount);
	result.meshCount = meshCount;
	result.mat = PushArray(permArena, Material, matCount);
	result.matCount = matCount;
	
	u32 currentResultMesh = 0;
	u32 currentResultMat = 0;

	for(u32 i = 0; i < matCount; ++i){
		Gltf_materials mat = gltfInfo.materials[i];
		if(imgCount > 0){
			// result.mat[i].emissiveTexture   = gltfInfo.images[mat.emissiveTextureIdx].texture;
			// result.mat[i].baseColorTexture  = gltfInfo.images[mat.baseColorTextureIdx].texture;
			u32 baseColorIdx = gltfInfo.textures[mat.baseColorTextureIdx].srcIdx;
			u32 diffuseTextureIdx = gltfInfo.textures[mat.diffuseTextureIdx].srcIdx;
			u32 emissiveIdx = gltfInfo.textures[mat.emissiveTextureIdx].srcIdx;
			
			result.mat[i].emissiveTexture   = gltfInfo.images[emissiveIdx].texture;
			result.mat[i].baseColorTexture  = gltfInfo.images[baseColorIdx].texture;
			result.mat[i].diffuseTexture = gltfInfo.images[diffuseTextureIdx].texture;
		}
		result.mat[i].emissiveFactor    = mat.emissiveFactor;
		result.mat[i].isDoubleSided     = mat.doubleSided;
		result.mat[i].metallicFactor    = mat.metallicFactor;
		result.mat[i].roughnessFactor   = mat.roughnessFactor;
	}
	
	for(int i = 0; i < nodeCount; ++i){
		if(result.nodes[i].meshIdx != INVALID_INDEX){
			Gltf_meshes_info mesh = gltfInfo.meshesInfo[result.nodes[i].meshIdx];
			u32 primativesCount = mesh.primativesCounter + 1;
			result.meshes[currentResultMesh].primCount = primativesCount;
			result.meshes[currentResultMesh].primatives = PushArray(permArena, Primatives, primativesCount);
			for(int j = 0; j < primativesCount; j++){
				u32 posIdx = mesh.primativesInfo[j].positionIdx;
				result.meshes[currentResultMesh].primatives[j].positions   = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, posIdx, v3);
				result.meshes[currentResultMesh].primatives[j].posCount    = gltfInfo.accessors[posIdx].count;
				
				u32 normIdx = mesh.primativesInfo[j].normalIdx;
				if(normIdx != INVALID_INDEX){
					result.meshes[currentResultMesh].primatives[j].normals     = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, normIdx, v3);
					result.meshes[currentResultMesh].primatives[j].normCount   = gltfInfo.accessors[normIdx].count;
				}
				
				u32 uvIdx = mesh.primativesInfo[j].TexCoords[0]; // todo: make it an array 
				if(uvIdx != INVALID_INDEX){
					result.meshes[currentResultMesh].primatives[j].uvs[0]     = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, uvIdx, v2);
					result.meshes[currentResultMesh].primatives[j].uvCount[0] = gltfInfo.accessors[uvIdx].count;
				}
				
				u32 indicesIdx = mesh.primativesInfo[j].indicesIdx;
				result.meshes[currentResultMesh].primatives[j].indices     = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, indicesIdx, u32);
				result.meshes[currentResultMesh].primatives[j].indexCount  = gltfInfo.accessors[indicesIdx].count;
				result.meshes[currentResultMesh].primatives[j].matIdx 	 = mesh.primativesInfo[j].materialIdx;
				result.meshes[currentResultMesh].primatives[j].mode 	   = mesh.primativesInfo[j].mode; // not used 
			
				// do a skin and weight here 
				u32 jointIdx = mesh.primativesInfo[j].jointsIdx;
				if(jointIdx != INVALID_INDEX){
					result.meshes[currentResultMesh].primatives[j].joints     = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, jointIdx, v4i);
					result.meshes[currentResultMesh].primatives[j].jointCount = gltfInfo.accessors[jointIdx].count;
				}
				
				u32 weightIdx = mesh.primativesInfo[j].weightIdx;
				if(weightIdx != INVALID_INDEX){
					result.meshes[currentResultMesh].primatives[j].weights 	= EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, weightIdx, v4);
					result.meshes[currentResultMesh].primatives[j].weightCount = gltfInfo.accessors[weightIdx].count;
				}
				
			}
			

			currentResultMesh++;
		}
	}

	// Animation loading
	result.animations = PushArray(permArena, Animation, animCount);
	result.animationCount = animCount;
	for(int i = 0; i < animCount; ++i){
		Gltf_animations_info animation = gltfInfo.animationsInfo[i];
		u32 samplerCount = animation.animSamplersCounter + 1;
		u32 channelCount = animation.animChannelsCounter + 1;
		result.animations[i].channels  = PushArray(permArena, Animation_channel, channelCount);
		result.animations[i].channelsCount = channelCount;
		result.animations[i].animationStartTime = FLT_MAX;
		result.animations[i].animationEndTime = -FLT_MAX;
		for(int c = 0; c < channelCount; c++){
			Animation_channels_info channel = animation.animChannels[c];
			Animation_samplers_info sampler = animation.animSamplers[channel.samplerIdx];

			result.animations[i].channels[c].timeStamps    = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, sampler.inputIdx, f32);
			result.animations[i].channels[c].transType 	= channel.path;
			result.animations[i].channels[c].nodeIdx 	  = channel.targetNode;
			result.animations[i].channels[c].interpoType   = sampler.interpolation;
			result.animations[i].channels[c].keyframeCount = gltfInfo.accessors[sampler.inputIdx].count;
			
			u32 lastAnimation = result.animations[i].channels[c].keyframeCount - 1;
			result.animations[i].channels[c].animation_max_time = result.animations[i].channels[c].timeStamps[lastAnimation];
			result.animations[i].channels[c].animation_min_time = result.animations[i].channels[c].timeStamps[0];
			
			if(result.animations[i].animationStartTime > result.animations[i].channels[c].animation_min_time){
				result.animations[i].animationStartTime = result.animations[i].channels[c].animation_min_time;
			}
			if(result.animations[i].animationEndTime < result.animations[i].channels[c].animation_max_time){
				result.animations[i].animationEndTime = result.animations[i].channels[c].animation_max_time;
			}
			
			switch(channel.path){
				case TRANSLATION:{
					result.animations[i].channels[c].translation = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, sampler.outPutIdx, v3);
				}break;
				case ROTATION:{
					result.animations[i].channels[c].rotation = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, sampler.outPutIdx, Quaternion);
				}break;
				case SCALE:{
					result.animations[i].channels[c].scale = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, sampler.outPutIdx, v3);
					
				}break;
			}
			
		}
	}
	
	// skin loading
	result.skins = PushArray(permArena, Skins, skinCount);
	result.skinCount = skinCount;
	for(int i = 0; i < skinCount; ++i){
		Gltf_skins_info skin = gltfInfo.skinsInfo[i];
		u32 jointCount  = skin.jointsCount;
		u32 inverseBindMatIdx = skin.inverseBindMatricesIdx;
		result.skins[i].jointsCount 		= jointCount;
		result.skins[i].jointsNodesIdx 	 = PushArray(permArena, u32, jointCount);
		result.skins[i].jointMatrices 	  = PushArray(permArena, mat4, jointCount); // is this used ????!
		result.skins[i].inverseBindMatrices = EXTRACT_DATA_FROM_BUFFER(permArena, &gltfInfo, inverseBindMatIdx, mat4);
		for(int joint = 0; joint < jointCount; ++joint){
			result.skins[i].jointsNodesIdx[joint] = skin.joints[joint];
			// result.skins[i].inverseBindMatrices[joint] = mat4_transpose(result.skins[i].inverseBindMatrices[joint]);
		}
	}

	printf("meshCount : %i \n", meshCount);
	
	return result;
}

internal GPU_mesh
update_mesh_geometry(Arena *arena, Mesh_geometry mesh){
	GPU_mesh result = {};
	result.primCount = mesh.primCount;
    result.primitives = PushArray(arena, GPU_Primitive, mesh.primCount);
	for(int p = 0; p < result.primCount; ++p){
		Primatives *src = &mesh.primatives[p];
		GPU_Primitive *dst = &result.primitives[p];
        dst->matIdx = src->matIdx;
        dst->indexCount = src->indexCount;

		glGenVertexArrays(1, &dst->vao);
        glBindVertexArray(dst->vao);

        glGenBuffers(1, &dst->posVbo);
        glBindBuffer(GL_ARRAY_BUFFER, dst->posVbo);
        glBufferData(GL_ARRAY_BUFFER, src->posCount * sizeof(v3), src->positions, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void*)0);

        if(src->normals != NULL){
            glGenBuffers(1, &dst->normVbo);
            glBindBuffer(GL_ARRAY_BUFFER, dst->normVbo);
            glBufferData(GL_ARRAY_BUFFER, src->normCount * sizeof(v3), src->normals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void*)0);
        }

        // FIX THIS LATER
        if(src->uvs[0] != NULL){
            glGenBuffers(1, &dst->uvVbo);
            glBindBuffer(GL_ARRAY_BUFFER, dst->uvVbo);
            glBufferData(GL_ARRAY_BUFFER, src->uvCount[0] * sizeof(v2), src->uvs[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(v2), (void*)0);
        }

		// joints
		// NOTE: this code assume that we have 4 weights and 4 joints that can affect each mesh
		if(src->joints != NULL){
            glGenBuffers(1, &dst->jointVbo);
            glBindBuffer(GL_ARRAY_BUFFER, dst->jointVbo);
            glBufferData(GL_ARRAY_BUFFER, src->jointCount * sizeof(v4i), src->joints, GL_STATIC_DRAW);
            glEnableVertexAttribArray(3);
			// remember: because opengl is a bitch you need to use glVertexAttribIPointer (with I) when
			// passing int data to the shader
			glVertexAttribIPointer(3, MAX_WEIGHTS, GL_INT, sizeof(v4i), (void*)0);
        }
        else{
        	// i feel bad that claud is the one who figured it out but i don't know how i will figure that out my self
        	// i hope one day i will stop using AI and be a giga chad programmer
        	glVertexAttribI4i(3, 0, 0, 0, 0); // remember so when a non skin vertex is send this is the default value 
        }
		
		// weights
		if(src->weights != NULL){
            glGenBuffers(1, &dst->weightVbo);
            glBindBuffer(GL_ARRAY_BUFFER, dst->weightVbo);
            glBufferData(GL_ARRAY_BUFFER, src->weightCount * sizeof(v4), src->weights, GL_STATIC_DRAW);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(v4), (void*)0);
        }
        else{
			glVertexAttrib4f(4, 1.0f, 0.0f, 0.0f, 0.0f); 
        }
		
        // indices
        glGenBuffers(1, &dst->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dst->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, dst->indexCount * sizeof(u32), src->indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
	}

	return result;
}

internal GPU_mesh*
update_scene_mesh(Arena *arena, GLTF_Scene scene){
	GPU_mesh *result = PushArray(arena, GPU_mesh, scene.meshCount);
	for(int i = 0; i < scene.meshCount; i++){
		result[i] = update_mesh_geometry(arena, scene.meshes[i]);
	}
	return result;
}

internal void
render_scene_node(GLTF_Scene scene, GPU_mesh *gpuMeshes, shader shader, u32 nodeIdx){
	Scene_node node = scene.nodes[nodeIdx];
	if(node.meshIdx != INVALID_INDEX){
		GPU_mesh mesh = gpuMeshes[node.meshIdx];
		mat4 transforms = mat4_identity();
        Quaternion orientation = Quat_from_axis_angle({0.0f, 1.0f, 0.0f}, (float)(SDL_GetTicks()/1000.0f));
		// transforms *= mat4_scale(4.0f);
		transforms *= mat4_scale(0.7f);
		// transforms *= mat4_scale(0.007f);
        transforms *= Quat_to_mat4(orientation);
        transforms *= mat4_translate(0.0f, -0.4f, 0.0f);
		setShaderValue_mat4(shader, "model", transforms);

		if(node.skinIdx != INVALID_INDEX){
			Skins skin = scene.skins[node.skinIdx];
			mat4 jointFinalMatrices[MAX_JOINTS] = {};
			for(int i = 0; i < skin.jointsCount; ++i){
				mat4 worldTransform = scene.nodes[skin.jointsNodesIdx[i]].worldTransforms;
				mat4 inverseBindMatrix = skin.inverseBindMatrices[i];
				jointFinalMatrices[i] = mat4_multiply(inverseBindMatrix, worldTransform);
			}
			
			setShaderValue_mat4_Array(shader, "jointFinalMatrices", jointFinalMatrices, skin.jointsCount);
		}

		for(u32 p = 0; p < mesh.primCount; ++p){
			GPU_Primitive prim = mesh.primitives[p];
			if(prim.matIdx != INVALID_INDEX){
				Material mat = scene.mat[prim.matIdx];
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mat.diffuseTexture);
			}
			else{
				
			}
			glBindVertexArray(prim.vao);
			glDrawElements(GL_TRIANGLES, prim.indexCount, GL_UNSIGNED_INT, 0);
		}
	}

	for(u32 c = 0; c < node.childrenCount; ++c){
        render_scene_node(scene, gpuMeshes, shader, node.childrenIdxs[c]);
    }
}


global_variable u32 keyframe = 0;
global_variable f32 animationTime = 0;
global_variable u32 currentAnimation = 0;
global_variable u32 maxAnimations = 5;

internal void
UpdateAnimation(GLTF_Scene scene, f32 updateTimer){
	//UPDATE ANIMATION HERE 
	//NOTE: right now there is no updating
	Animation animation = scene.animations[currentAnimation];
	for(int i = 0; i < animation.channelsCount; ++i){
		Animation_channel channel = animation.channels[i];
		u32 targetNode = channel.nodeIdx;
		Scene_node *n = &scene.nodes[targetNode];
		// printf("animationTime: %f, animation max time: %f \n", animationTime, channel.animation_max_time);
		//NOTE: THIS IS A HACK FOR NOW 
		if(animationTime < channel.animation_min_time) animationTime = channel.animation_min_time;
		if(animationTime < channel.animation_max_time){
			//1- get the low
			u32 lowIdx = 0;
			for(int t = 0; t < channel.keyframeCount; ++t){
				if(channel.timeStamps[t] > animationTime) break;
				lowIdx = t;
			}
			f32 lowTime = channel.timeStamps[lowIdx]; 
			//2- get the high
			u32 highIdx = (lowIdx + 1 < channel.keyframeCount) ? lowIdx + 1 : lowIdx;
			f32 highTime = channel.timeStamps[highIdx];
			//3- get the [0 , 1]
			f32 percent = (highTime > lowTime) ? (animationTime - lowTime) / (highTime - lowTime) : 0.0f;
			//4- check for internpolation method (won't do it for now)
			switch(channel.transType){
				case TRANSLATION:{
					// n->translation = channel.translation[keyframe];
					v3 low_translation = channel.translation[lowIdx];
					v3 high_translation = channel.translation[highIdx];
					n->translation = v3_lerp(low_translation, high_translation, percent);
				}break;
				case ROTATION:{
					// n->rotation = channel.rotation[keyframe];
					Quaternion low_rotation = channel.rotation[lowIdx];
					Quaternion high_rotation = channel.rotation[highIdx];
					n->rotation = Quat_slerp(low_rotation, high_rotation, percent);
				}break;
				case SCALE:{
					// n->scale = channel.scale[keyframe];
					v3 low_scale = channel.scale[lowIdx];
					v3 high_scale = channel.scale[highIdx];
					n->scale = v3_lerp(low_scale, high_scale, percent);
				}break;
			}
			n->localTransforms = mat4_scale(n->scale) * Quat_to_mat4(n->rotation) * mat4_translate(n->translation);
		}
	}

	for(int j = 0; j < scene.scene[0].rootCount; j++){
		compute_node_transforms(scene.nodes, scene.scene[0].rootNodes[j], mat4_identity());
	}

	// update animation time
	animationTime += updateTimer;
	if(animationTime > animation.animationEndTime){
		animationTime = animation.animationStartTime;
	}
}
