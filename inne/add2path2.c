#include <stdio.h>
#include <string.h>
#include <Windows.h>

void setx(const char *buffer, const char *current_dir) {
	int flag = 0;
	char *occ = strstr(buffer, current_dir);
	int len = strlen(current_dir);

	while(occ != NULL){
			if((occ[len] == ';')){
				flag++;
				break;
			}
			occ = strstr(occ+1, current_dir);
		}

    if (flag == 1) {
	    // remove directory from PATH
	    char new_path[1024];
	    strcpy(new_path, buffer);
	
	    char *occurence = strstr(new_path, current_dir);
	
		while(occurence != NULL){
			if((occurence[len] == ';')){
				memcpy(occurence, occurence + len + 1, strlen(occurence + len) + 1);
			    char cmd[1024];
			    sprintf(cmd, "setx PATH \"%s\" > NUL", new_path);
			    system(cmd);
			    printf("Removed directory from PATH\n");
			}
			occurence = strstr(occurence+1, current_dir);
		}
		
	} else {
        // add directory to PATH
        char final_path[1024];
        sprintf(final_path, "%s;%s", current_dir, buffer);
        
        char cmd[1024];
        sprintf(cmd, "setx PATH \"%s\" > NUL", final_path);
        
        system(cmd);
        printf("Added directory to PATH\n");
    }
}

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
	        
	        setx(buffer, current_dir);        
	    } else {
	        // absolute path
	        strcpy(current_dir, path);
	        
	        setx(buffer, current_dir);
	    }
	}
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return add_to_path(argv[1]);
    } else {
        return add_to_path(NULL);
    }
}
