/*
 *                           0BSD 
 * 
 *                    BSD Zero Clause License
 * 
 *  Copyright (c) 2019 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "xfilepicker.h"
#ifdef _WIN32 //Includes
#include <windows.h>
#endif


static inline int fp_compare_fun (const void *p1, const void *p2) {
#ifdef _WIN32
    if(strstr(*(const char**)p1, PATH_SEPARATOR) && strstr(*(const char**)p2, PATH_SEPARATOR))
        return strcasecmp(*(const char**) p1, *(const char**) p2);
    if(strstr(*(const char**)p1, PATH_SEPARATOR)) return -1;
    if(strstr(*(const char**)p2, PATH_SEPARATOR)) return 1;
#endif
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline int fp_compare_hidden_dirs_fun (const void *p1, const void *p2) {
    if(strstr(*(const char**)p1, PATH_SEPARATOR".") && strstr(*(const char**)p2, PATH_SEPARATOR".")) return 0;
    if(strstr(*(const char**)p1, PATH_SEPARATOR".")) return 1;
    if(strstr(*(const char**)p2, PATH_SEPARATOR".")) return -1;
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline int fp_compare_hidden_files_fun (const void *p1, const void *p2) {
    if(strncmp(*(const char**)p1, ".",1)==0 && strncmp(*(const char**)p2, ".",1)==0) return 0;
    if(strncmp(*(const char**)p1, ".",1)==0 ) return 1;
    if(strncmp(*(const char**)p2, ".",1)==0 ) return -1;
    return strcasecmp(*(const char**) p1, *(const char**) p2);
}

static inline bool fp_show_hidden_files(FilePicker *filepicker, char* file) {
    return (filepicker->show_hidden) ? strcmp(file,".")!=0 : (file[0] != '.');
}

static inline bool fp_show_filter_files(FilePicker *filepicker, char* file) {
    if (!filepicker->use_filter) {
        return true;
    } else {
        if(strstr(filepicker->filter,".")) {
            char *ms =strdup(filepicker->filter);
            char * p = strtok (ms, "|");
            while (p) {
               if (strstr(file, p)) {
                    free(ms);
                    return true;
                }
                p = strtok (NULL, "|");
            }
            free(ms);
            return false;
        }
#ifdef __XDG_MIME_H__
        return strstr(xdg_mime_get_mime_type_from_file_name(file), filepicker->filter);
#else
        return true;
#endif
    }
}

static void fp_sort_buffers(FilePicker *filepicker, int get_dirs) {
    if (filepicker->dir_counter>1 && get_dirs) {
        qsort (filepicker->dir_names, filepicker->dir_counter,
          sizeof filepicker->dir_names[0], (filepicker->show_hidden) ?
          fp_compare_hidden_dirs_fun : fp_compare_fun);
    }
    if (filepicker->file_counter>1) {
        qsort (filepicker->file_names, filepicker->file_counter,
          sizeof filepicker->file_names[0], (filepicker->show_hidden) ?
          fp_compare_hidden_files_fun : fp_compare_fun);
    }
}

static void fp_clear_filebuffer(FilePicker *filepicker) {
    unsigned int j = 0;
    for(; j<filepicker->file_counter;j++) {
        free(filepicker->file_names[j]);
        filepicker->file_names[j] = NULL;
    }
    if(filepicker->file_counter) {
        free(filepicker->file_names);
        filepicker->file_names = NULL;
        filepicker->file_counter=0;
    }
}

static void fp_clear_dirbuffer(FilePicker *filepicker) {
    unsigned int j = 0;
    for(; j<filepicker->dir_counter;j++) {
        free(filepicker->dir_names[j]);
        filepicker->dir_names[j] = NULL;
    }
    if(filepicker->dir_counter) {
        free(filepicker->dir_names);
        filepicker->dir_names = NULL;
        filepicker->dir_counter=0;
    }
}

#ifdef _WIN32 //file/drive functions
bool is_root_directory(char *path) {
   return (((strlen(path)==3) && path[1] == ':' && path[2] == '\\')
          ||((strlen(path)==1) && path[0] == '\\'));
}

void add_root_directory(FilePicker *filepicker, char *path) {
   /* DWORD drives = GetLogicalDrives();
    int i;
    for (i=0; i<='Z'-'A'; i++) {
      if ((drives & (1 << i)) != 0) {
        filepicker->dir_counter += 1;
        filepicker->dir_names = (char **)realloc(filepicker->dir_names,
          (filepicker->dir_counter) * sizeof(char *));
        asprintf(&filepicker->dir_names[filepicker->dir_counter-1], "%c:\\", 'A'+i);
      }
    }*/
    filepicker->dir_names = (char **)realloc(filepicker->dir_names,
      (filepicker->dir_counter + 1) * sizeof(char *));
    assert(filepicker->dir_names != NULL);
    asprintf(&filepicker->dir_names[filepicker->dir_counter++], "%s", path);
    assert(&filepicker->dir_names[filepicker->dir_counter] != NULL);
    
}

bool is_file(DIR *dirp, struct dirent *dp) {
    return ((dirp->dd_dta.attrib & _A_SUBDIR)==0);
}

bool is_directory(DIR *dirp, struct dirent *dp) {
    return ((dirp->dd_dta.attrib & _A_SUBDIR) != 0);
}

#else // _OS_UNIX_
bool is_root_directory(char *path) {
   return (strcmp (path, PATH_SEPARATOR) == 0);
}

void add_root_directory(FilePicker *filepicker, char *path) {
    filepicker->dir_names = (char **)realloc(filepicker->dir_names,
      (filepicker->dir_counter + 1) * sizeof(char *));
    assert(filepicker->dir_names != NULL);
    asprintf(&filepicker->dir_names[filepicker->dir_counter++], "%s",path);
    assert(&filepicker->dir_names[filepicker->dir_counter] != NULL);
}

bool is_file(DIR *dirp, struct dirent *dp) {
    return (dp-> d_type != DT_DIR && dp->d_type != DT_UNKNOWN);
}

bool is_directory(DIR *dirp, struct dirent *dp) {
    return (dp -> d_type == DT_DIR);
}

#endif

static inline int fp_prefill_dirbuffer(FilePicker *filepicker, char *path) {
    int ret = 0;
    if (is_root_directory(path)) {
        add_root_directory(filepicker, path);
    } else {
        char *ho;
        asprintf(&ho, "%s",path);
        assert(ho != NULL);
        while (!is_root_directory (ho)) {
            filepicker->dir_names = (char **)realloc(filepicker->dir_names,
              (filepicker->dir_counter + 1) * sizeof(char *));
            assert(filepicker->dir_names != NULL);
            asprintf(&filepicker->dir_names[filepicker->dir_counter++], "%s",dirname(ho));
            assert(&filepicker->dir_names[filepicker->dir_counter-1] != NULL);
            ret++;
        }
        if (!is_root_directory (path)) {
            filepicker->dir_names = (char **)realloc(filepicker->dir_names,
              (filepicker->dir_counter + 1) * sizeof(char *));
            assert(filepicker->dir_names != NULL);
            asprintf(&filepicker->dir_names[filepicker->dir_counter++], "%s",path);
            assert(&filepicker->dir_names[filepicker->dir_counter-1] != NULL);
        }
        free(ho);
    }
    return ret;   
}

int fp_check_link(char *path, struct dirent *dp) {
#ifdef _OS_UNIX_ //not supported on win32
    if(dp -> d_type == DT_LNK) {
        char s[256];
        snprintf(s, 256, (strcmp(path, PATH_SEPARATOR) != 0) ?
              "%s" PATH_SEPARATOR "%s" : "%s%s" , path,dp->d_name);
        struct stat sb;
        if (stat(s, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return 1;
        }
    }
#endif
    return 0;
}

int fp_check_dir(char *path, struct dirent *dp) {
#ifdef _OS_UNIX_ //not supported on win32
    if (dp->d_type == DT_UNKNOWN) {
        char s[256];
        snprintf(s, 256, (strcmp(path, PATH_SEPARATOR) != 0) ?
              "%s" PATH_SEPARATOR "%s" : "%s%s" , path,dp->d_name);
        struct stat sb;
        if (stat(s, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return 1;
        } else {
            return 2;
        }
    }
#endif
    return 0;
}

int fp_is_link(struct dirent *dp) {
#ifdef _WIN32 //not supported on win32
    return 0;
#else
    return dp -> d_type == DT_LNK;
#endif
}

int fp_get_files(FilePicker *filepicker, char *path, int get_dirs, int get_files) {
    int ret = 0;
    fp_clear_filebuffer(filepicker);

    DIR *dirp;
    struct dirent *dp;
    if((dirp = opendir(path)) == NULL) {
        path =(char*)PATH_SEPARATOR;
        dirp = opendir(PATH_SEPARATOR);
        assert(dirp);
    }

    if(get_dirs) {
        fp_clear_dirbuffer(filepicker);
        ret = fp_prefill_dirbuffer(filepicker, path);
    }

    while ((dp = readdir(dirp)) != NULL) {

        if((get_files && (is_file(dirp, dp) || (fp_check_dir(path, dp) == 2)) && strlen(dp->d_name)!=0
          && strcmp(dp->d_name,"..")!=0 && fp_show_hidden_files(filepicker, dp->d_name)
          && fp_show_filter_files(filepicker, dp->d_name) && !fp_check_link(path, dp)) ) {

            filepicker->file_names = (char **)realloc(filepicker->file_names,
              (filepicker->file_counter + 1) * sizeof(char *));
            assert(filepicker->file_names != NULL);
            asprintf(&filepicker->file_names[filepicker->file_counter++],"%s",dp->d_name);
            assert(&filepicker->file_names[filepicker->file_counter-1] != NULL);

        } else if(get_dirs && (is_directory(dirp, dp) || fp_is_link(dp) || (fp_check_dir(path, dp) == 1)) 
          && strlen(dp->d_name)!=0 && strcmp(dp->d_name,"..")!=0 && fp_show_hidden_files(filepicker, dp->d_name)) {

            if (fp_is_link(dp)) {
                if (!fp_check_link(path, dp)) continue;
            }
            filepicker->file_names = (char **)realloc(filepicker->file_names,
              (filepicker->file_counter + 1) * sizeof(char *));
            assert(filepicker->file_names != NULL);
            asprintf(&filepicker->file_names[filepicker->file_counter++], is_root_directory(path) ?
              "%s%s" : "%s" PATH_SEPARATOR "%s"  , path,dp->d_name);
            //asprintf(&filepicker->file_names[filepicker->file_counter++],"%s%s" , path,dp->d_name);
            assert(&filepicker->file_names[filepicker->file_counter-1] != NULL);
        }
    }
    closedir(dirp);
    fp_sort_buffers(filepicker, get_dirs);
    return ret;
}

void fp_free(FilePicker *filepicker) {
    fp_clear_filebuffer(filepicker);
    fp_clear_dirbuffer(filepicker);
    free(filepicker->selected_file);
    free(filepicker->path);
    free(filepicker->filter);
}

void fp_init(FilePicker *filepicker, const char *path) {
    filepicker->file_counter=0;
    filepicker->dir_counter=0;
    filepicker->use_filter = 0;
    filepicker->show_hidden = false;
    filepicker->file_names = NULL;
    filepicker->dir_names = NULL;
    filepicker->selected_file = NULL;
    filepicker->path = NULL;
    filepicker->filter = NULL;
#ifdef _WIN32 //file/drive functions
    bool is_dir = false;
    WIN32_FIND_DATA data;
    HANDLE hFile = FindFirstFile(path, &data);
    if ( hFile == INVALID_HANDLE_VALUE ) {
        is_dir = false;
    } else {
        FindClose(hFile);
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            is_dir = true;
        is_dir = false;
    }
    if (is_dir) {
        asprintf(&filepicker->path, "%s", path);
    } else {
        asprintf(&filepicker->path, "%s\\", getenv("SYSTEMDRIVE"));
    }
#else
    asprintf(&filepicker->path, "%s", path);
#endif
    assert(filepicker->path != NULL);
}
