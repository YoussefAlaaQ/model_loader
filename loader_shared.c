internal void
process_numeric_token(Arena *arena, char **cursor, Token* token){
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
process_real_numeric_token(Arena *arena, char **cursor, Token* token){
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
process_word_token(Arena *arena, char **cursor, Token* token){
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

