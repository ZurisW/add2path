#include <stdio.h>
#include <string.h>
#include <Windows.h>

int add_to_path(const char *path) {
    HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 1024;
	char buffer[1024] = "";

	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
	    if (RegQueryValueEx(hKey, "PATH", NULL, &dwType, (LPBYTE)buffer, &dwSize) != ERROR_SUCCESS)
	    {
	        fprintf(stderr, "Failed to retrieve PATH value from registry\n");
	    }
	    RegCloseKey(hKey);
	} else {
	    fprintf(stderr, "Failed to open HKEY_CURRENT_USER\\Environment key\n");
	}
    
    char current_dir[FILENAME_MAX];
    GetCurrentDirectory(FILENAME_MAX, current_dir);
    if (current_dir == NULL) {
        printf("Error getting current directory\n");
        return 1;
    }

    if (path != NULL) {
    	 if (path[0] != '/' && !(path[1] == ':' && path[2] == '\\')) {
        // relative path
        if (strstr(path, "..\\") != NULL) {
            // go one folder backwards
            char *last_slash = strrchr(current_dir, '\\');
            if (last_slash != NULL) {
                *last_slash = '\0';
            }
            // remove "..\" from the path
            path += 3;
        }
        
        sprintf(current_dir, "%s\\%s", current_dir, path);
        
	    } else {
	        // absolute path
	        strcpy(current_dir, path);
	    }
    
    	
    	char *occurence = strstr(buffer, current_dir);
		int len = strlen(current_dir);
			
	    // check if provided path is in PATH and is identical
		if (occurence != NULL && (occurence[len] == ';')){	
			char new_path[1024];
		    strcpy(new_path, buffer);
		    char *occurence = strstr(new_path, current_dir);
		    int len = strlen(current_dir);
		        
		    // check for ';' after current_dir in buffer
		    if (*(occurence + len) == ';') {
		        len++;
		    }
			memcpy(occurence, occurence + len, strlen(occurence + len) + 1);
			char cmd[1024];
			sprintf(cmd, "setx PATH \"%s\" > NUL", new_path);
			system(cmd);
			printf("Removed provided path from PATH\n");
			return 0;
	    	
		} else {
			// add provided path to PATH
			char final_path[1024];
			sprintf(final_path, "%s;%s", current_dir, buffer);
			char cmd[1024];
			sprintf(cmd, "setx PATH \"%s\" > NUL", final_path);
			system(cmd);
			printf("Added provided path to PATH\n");
			return 0;
		}
	}
	
	int hujj = 0;
	char *chuj = strstr(buffer, current_dir);
	int len = strlen(current_dir);
			
	while(chuj != NULL){
			if((chuj[len] == ';')){
				hujj++;
				break;
			}
			chuj = strstr(chuj+1, current_dir);
		}

    if (hujj == 1) {
	    // remove current directory from PATH
	    char new_path[1024];
	    strcpy(new_path, buffer);
	
	    char *huj = strstr(new_path, current_dir);
	
		while(huj != NULL){
			if((huj[len] == ';')){
				memcpy(huj, huj + len + 1, strlen(huj + len) + 1);
			    char cmd[1024];
			    sprintf(cmd, "setx PATH \"%s\" > NUL", new_path);
			    system(cmd);
			    printf("Removed current directory from PATH\n");
			}
			huj = strstr(huj+1, current_dir);
		}

	    return 0;
	} else {
        // add current directory to PATH
        char final_path[1024];
        sprintf(final_path, "%s;%s", current_dir, buffer);
        char cmd[1024];
        sprintf(cmd, "setx PATH \"%s\" > NUL", final_path);
        system(cmd);
        printf("Added current directory to PATH\n");
        return 0;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return add_to_path(argv[1]);
    } else {
        return add_to_path(NULL);
    }
}
