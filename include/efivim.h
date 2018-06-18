#include <errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define USR_VIMRC_FILE "fs0:\\.vimrc"
#define USR_EXRC_FILE "fs0:\\.exrc"
#define VIMINFO_FILE "fs0:\\.viminfo"
#define FTOFF_FILE "ftoff.vim"
#define INDOFF_FILE "indoff.vim"
#define FILETYPE_FILE "filetype.vim"
#define FTPLUGIN_FILE "ftplugin.vim"
#define FTPLUGOF_FILE "ftplugof.vim"
#define INDENT_FILE "indent.vim"

#define SYNTAX_FNAME "fs0:\\vim\\vimfiles\\syntax\\%s.vim"
#define SYS_VIMRC_FILE "fs0:\\vim\\vimfiles\\vimrc"
#define VIM_DEFAULTS_FILE "fs0:\\vim\\vimfiles\\defaults.vim"

#define EVIM_FILE "evim.vim"
#define VIMRC_FILE ".vimrc"
#define EXRC_FILE ".exrc"

#define DFLT_BDIR "."
#define DFLT_VDIR "."
#define DFLT_DIR "."
#define DFLT_RUNTIMEPATH "."
#define DFLT_HELPFILE "notexist"
#define DFLT_MAXMEM (2*1024)
#define DFLT_MAXMEMTOT (10*1024)

#define DFLT_ERRORFILE "errors.err"

#define BASENAMELEN 128

#define TEMPNAME "efi_tmp.XXXXXX"
#define TEMPNAMELEN 128

#define mch_rmdir(x) rmdir((char *)(x))
#define vim_mkdir(x, y) mkdir((char *)(x), y)

/* vim mch fns */
void mch_early_init(void);
int mch_init(void);
void mch_exit(int);
void mch_suspend(void);
void mch_breakcheck(int);
void mch_hide(unsigned char *name);

int mch_get_shellsize(void);
void mch_set_shellsize(void);
void mch_new_shellsize(void);

void mch_get_host_name(unsigned char *s, int len);
long mch_get_pid(void);

unsigned char* mch_getenv(unsigned char *name);

int mch_has_wildcard(unsigned char *p);
int mch_has_exp_wildcard(unsigned char *p);

int mch_expandpath(void *gap /* garray_T* */, unsigned char *pat, int flags);

long mch_getperm(unsigned char *name);
int mch_setperm(unsigned char *name, long perm);

int mch_nodetype(unsigned char *name);

int mch_screenmode(unsigned char *arg);
int mch_check_win(int argc, char **argv);
void mch_settitle(unsigned char *title, unsigned char *icon);
void mch_restore_title(int which);
int mch_can_restore_icon(void);
int mch_can_restore_title(void);
void mch_setmouse(int on);

void mch_delay(long msec, int ignoreinput);
int mch_inchar(unsigned char *buf, int maxlen, long wtime, int tb_change_cnt);
void mch_write(unsigned char *s, int len);
int mch_char_avail(void);

int mch_rename(const char *src, const char *dest);
int mch_remove(unsigned char *name);
int mch_isdir(unsigned char *name);
int mch_dirname(unsigned char *buf, int len);
int mch_chdir(char *path);
long mch_getperm(unsigned char *name);
int mch_isFullName(unsigned char *fname);
int mch_FullName(unsigned char *fname, unsigned char *buf, int len, int force);

int mch_input_isatty(void);
void mch_settmode(int tmode);

int mch_can_exe(unsigned char *name, unsigned char **path, int use_path);
int mch_call_shell(unsigned char *cmd, int options);
int mch_get_user_name(unsigned char *s, int len);

/* misc stuff */
int putenv(const char *string);

void slash_adjust(unsigned char *p);

