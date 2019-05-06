void compareManifests(char* manifestString, char* projName) {
  char* token = strtok(manifestString+1, " \n");
  int i = 0;
  while (token[i] != '$' ) {
    i++;
  }
  char* serverVersion = token+i+1;
  struct manifestNode * serverManifest = NULL;
  while (token != NULL) {
    token = strtok(NULL, " \n");
    char* path = token;
    token = strtok(NULL, " \n");
    char* version = token;
    token = strtok(NULL, " \n");
    char* hash = token;

    if (path != NULL) {
      addManifestList(&serverManifest, path, version, hash);
    }
  }
  char* path = malloc(strlen("client/") + strlen(projName) + strlen("/manifest.txt") + 1);
  strcpy(path, "client/");
  strcat(path, projName);
  strcat(path, "/manifest.txt");
  path[strlen("client/") + strlen(projName) + strlen("/manifest.txt")] = '\0';
  printf("client manifest\n");
  int fd = open(path, O_RDONLY);
  char* otherManifest = readFile(fd);
  //printf("manifestfile string %s\n", otherManifest);
  char* token2 = strtok(otherManifest+1, " \n");
  printf("version: %s\n", token2);
  char* clientVersion = token2;
  struct manifestNode * clientManifest = NULL;
  while (token2 != NULL) {
    token2 = strtok(NULL, " \n");
    char* path = token2;
    token2 = strtok(NULL, " \n");
    char* version = token2;
    token2 = strtok(NULL, " \n");
    char* hash = token2;

    if (path != NULL) {
      addManifestList(&clientManifest, path, version, hash);
      if (strcmp(serverVersion, clientVersion) == 0) {
            checkUpLoad(serverManifest, clientManifest);
              } else {
                    checkModify(serverManifest, clientManifest);
                        checkAdd(serverManifest, clientManifest);
                            checkDelete(serverManifest, clientManifest);
                              }
    }
