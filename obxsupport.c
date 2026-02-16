#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATHS 128
#define LINE_LEN 256

void scanmdpath(FILE *fp, char *mdpaths[], int *mdpathcnt) {
	char line[LINE_LEN];
	while (*mdpathcnt < MAX_PATHS && fgets(line, sizeof(line), fp)) {
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') {
			line[len - 1] = '\0';
			if (len > 1 && line[len - 2] == '\r') {
				line[len - 2] = '\0';
			}
		}

		if (line[0] == '\0') continue;

		mdpaths[*mdpathcnt] = malloc(strlen(line) + 1);
		if (mdpaths[*mdpathcnt] == NULL) {
			perror("malloc");
			exit(1);
		}
		strcpy(mdpaths[*mdpathcnt], line);
		(*mdpathcnt)++;
	}
}

char* expand_home(const char* path) {
	if (path[0] == '~' && (path[1] == '/' || path[1] == '\0')) {
		const char* home = getenv("HOME");
		if (!home) return NULL;
		size_t home_len = strlen(home);
		size_t rest_len = strlen(path + 1);
		char* full = malloc(home_len + rest_len + 1);
		if (full) {
			strcpy(full, home);
			strcat(full, path + 1);
		}
		return full;
	}
	return strdup(path);
}

int getlevel(char line[]) {
	int i = 0;
	for(;line[i] == '#';i++){}
	if(line[i] != ' '){ i = 0; }
	return i;
}

const char* get_title_text(const char* line) {
	const char* p = line;
	while (*p == '#') p++;
	while (*p == ' ' || *p == '\t') p++;
	return p;
}

char* rmenter(const char* line) {
    const char* title_start = get_title_text(line);
    size_t len = strlen(title_start);
    while (len > 0 && isspace((unsigned char)title_start[len - 1])) {
        len--;
    }
    char* result = malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, title_start, len);
    result[len] = '\0';
    return result;
}

int strcmp_nospace(const char* headline, const char* keyword) {
	const char* title = get_title_text(headline);
	size_t kw_len = strlen(keyword);
	size_t title_len = strlen(title);

	if (title_len < kw_len) { return -1; }

	if (strncmp(title, keyword, kw_len) != 0)
		return -1;

	for (size_t i = kw_len; i < title_len; i++) {
		if (!isspace((unsigned char)title[i]))
			return -1; 
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <heading1> [heading2] ...\n", argv[0]);
		return 1;
	}

	int istab = 0;
	int tablevel = 0;
	int downlevel = 1;
	if (strcmp(argv[argc - 1],"--t") == 0) {
		istab = 1;
		argc --;
	}else if (strcmp(argv[argc - 2],"--t") == 0) {
		istab = 1;
		downlevel = atoi(argv[argc - 1]);
		argc -= 2;
	}

	int  isfirst = 0;
	if (strcmp(argv[1],"--t") == 0) {
		isfirst = 1;
	}
	const char *confpaths[] = {"/etc/obrc", "~/.obrc"};
	char *mdpaths[MAX_PATHS] = {NULL}; 
	int mdpathcnt = 0;

	for (int i = 0; i < 2; i++) {
		char *expanded = expand_home(confpaths[i]);
		if (!expanded) continue;

		FILE *fp = fopen(expanded, "r");
		if (fp != NULL) {
			scanmdpath(fp, mdpaths, &mdpathcnt);
		fclose(fp);
		}
		free(expanded); 
	}
	//printf("ll,%d\n",mdpathcnt);
	//printf("%d%s\n",mdpathcnt,expand_home(mdpaths[1]));
	for (int i = 0; i < mdpathcnt; i++) {
		char *expanded = expand_home(mdpaths[i]);
		if(!expanded) continue;

		int startline = 0;
		int startlevel = 1;
		int isprintmod = 0;
		int nowline = -1;
		int nowlevel = 1;

		int j = 1;

		char line[LINE_LEN];
		FILE *fp = fopen(expanded, "r");
		if(fp ==NULL) { continue; }
		int isincodeblock = 0;
		int iscodeblockready = 0;
		int obxcount = 0;
		while (fgets(line, sizeof(line), fp)) {
			size_t len = strlen(line);

			if (line[0] == '\0') continue;

			if(iscodeblockready){
				iscodeblockready = 0;
				isincodeblock = 1;
			}

			if(strstr(line, "```") != NULL){
				if(isincodeblock){
					isincodeblock = !isincodeblock;		
				}else{
					iscodeblockready = 1;
				}
			}

			if(isfirst == 1 && !isincodeblock) {
				if(getlevel(line) == downlevel) {
					printf("%s\n",rmenter(line));
				}
			}
			if(isprintmod && !isincodeblock){
				if(getlevel(line) <= nowlevel && getlevel(line) != 0){
					goto cleanup;
					//isprintmod = 0;
					//printf("\n----oblivionis-notice--Next-session----\n\n");
				}
			}

			if(!isprintmod && !isincodeblock && getlevel(line) > 0 &&
			   strcmp_nospace(line, argv[j]) == 0){
				nowlevel = getlevel(line);
				if(j == argc - 1){
					if(istab) {
						tablevel = nowlevel + downlevel;
					}
					isprintmod = 1;
					
				}else{
					j++;
				}
			}

			if(isprintmod){
				if(istab)  {
					if(getlevel(line) == tablevel){
						printf("%s\n",rmenter(line));
					}
					continue;
				}
				if(isincodeblock){
					obxcount++;
					printf("obnum%d:obxtag\r\033[1;32m",obxcount);
				}
				printf("%s",line);
				if(isincodeblock){
					printf("\033[0m");
				}
			}

		}
		if(isprintmod) { goto cleanup;}
	}
	
cleanup:
	for (int i = 0; i < mdpathcnt; i++) {
		free(mdpaths[i]);
	}
	return 0;
}
