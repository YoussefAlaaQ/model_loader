char* 
loadFile(const char* path){
	size_t fileSize = 0;
    void* loadedFile = SDL_LoadFile(path, &fileSize);
    if (loadedFile == NULL) {
        printf("Failed to load level file '%s': %s\n", path, SDL_GetError());
        return NULL;
    }

    // Create a c-string 
    char* text = (char*)malloc(fileSize + 1);
    memcpy(text, loadedFile, fileSize);
    text[fileSize] = '\0';

    SDL_free(loadedFile);
    return text;
}

