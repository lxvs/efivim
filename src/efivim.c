#include <Uefi.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include <sys/select.h>
#include <sys/termios.h>

#include "efivim.h"
#include "vim.h"

#define where(format, ...) do{DEBUG((EFI_D_INFO, "\n%s> " format "\n", __func__, ##__VA_ARGS__));}while(0)

EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *sto;
UINTN	cols;
UINTN	rows;

// current index in fgmap
unsigned char fgcol;
// current index in bgmap
unsigned char bgcol;

// index is ansi code, value is uefi code
UINTN fgmap[] = {
	// black
	0x0,
	// red
	0x4,
	// green
	0x2,
	// yellow
	0xe,
	// blue
	0x1,
	// magenta
	0x5,
	// cyan
	0x3,
	// white
	0xf,
};

UINTN bgmap[] = {
	// black
	0x00,
	// red
	0x40,
	// green
	0x20,
	// yellow - uefi lacks yellow bg so this is actually the brown color code.
	0x60,
	// blue
	0x10,
	// magenta
	0x50,
	// cyan
	0x30,
	// white - uefi lacks white bg so this is actually light gray.
	0x70,
};

static EFI_STATUS
setattr(void)
{
	EFI_STATUS	status;

	status = sto->SetAttribute(sto, fgmap[fgcol]|bgmap[bgcol]);
	where("setattr fg %#x bg %#x -> %x", fgcol, bgcol, status);
	return status;
}

static EFI_STATUS
setfg(unsigned char col)
{
	fgcol = col;
	return setattr();
}

static EFI_STATUS
setbg(unsigned char col)
{
	bgcol = col;
	return setattr();
}

void
mch_early_init(void)
{
	// kill wdt
	gBS->SetWatchdogTimer (0, 0, 0, NULL);

	// don't buffer debug output
	setbuf(stderr, NULL);
}

int
mch_init(void)
{
	int fd;
	EFI_STATUS status;

	// init the console
	status = gBS->HandleProtocol(gST->ConsoleOutHandle, &gEfiSimpleTextOutProtocolGuid, (VOID**)&sto);
	if(EFI_ERROR(status)){
		where("console init failed: %#x", status);
		exit(1);
	}

	fgcol = 7;
	bgcol = 0;
	setattr();

	mch_get_shellsize();

	//sto->Reset(sto, TRUE);
	sto->EnableCursor(sto, TRUE);
	sto->ClearScreen(sto);

	// we'll set nonblock on stdin so reading does not block
	fd = STDIN_FILENO;
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	return -1;
}

void
mch_exit(int code)
{
	where("exit %d", code);
	exit(code);
}

void
mch_suspend(void)
{
	where();
}

void
mch_breakcheck(int force)
{
}

void
mch_hide(unsigned char *name)
{
}

int
mch_get_shellsize(void)
{
	EFI_STATUS status;

	status = sto->QueryMode(sto, sto->Mode->Mode, &cols, &rows);
	if(EFI_ERROR(status)){
		where("getting console size failed: %#x", status);
		exit(1);
	}

	Rows = rows;
	Columns = cols;

	return OK;
}

void
mch_set_shellsize(void)
{
}

void
mch_new_shellsize(void)
{
}

void
mch_get_host_name(unsigned char *s, int len)
{
	strncpy((char*)s, "efi", len-1);
	//gethostname((char*)s, (size_t)len);
}

long
mch_get_pid(void)
{
	return (long)getpid();
}

unsigned char*
mch_getenv(unsigned char *name)
{
	return NULL;
}

int
mch_has_wildcard(unsigned char *p)
{
	where("p %s", p);
	for(; *p; MB_PTR_ADV(p)){
		if(*p == '\\' && p[1] != NUL){
			++p;
		} else if(vim_strchr((char_u *)"*?[{`'$", *p) != NULL ||
			(*p == '~' && p[1] != NUL)){
			return TRUE;
		}
	}
	return FALSE;
}

int
mch_has_exp_wildcard(unsigned char *p)
{
	where("p %s", p);
	for (; *p;  MB_PTR_ADV(p)){
		if (*p == '\\' && p[1] != NUL)
			++p;
		else if (vim_strchr((char_u *) "*?[{'", *p) != NULL)
			return TRUE;
	}
	return FALSE;
}

int
mch_expandpath(void *gap /* garray_T* */, unsigned char *pat, int flags)
{
	where("pat %s flags %#x", pat, flags);
	garray_T *ga = gap;
	(void)ga;
	return FAIL;
}

long
mch_getperm(unsigned char *name)
{
	struct stat st;
	if (stat((char*)name, &st) < 0) {
		return -1;
	}
	where("file `%s` mode %o", name, st.st_mode);

	// mask off uefi bits
	return st.st_mode & (S_IFMT | ALLPERMS);
}

int
mch_setperm(unsigned char *name, long perm)
{
	where("name `%s` mode %o", name, perm);
	return chmod((char*)name, (mode_t)perm) == 0 ? OK : FAIL;
}

int
mch_nodetype(unsigned char *name)
{
	struct stat st;
	if (stat((char*)name, &st) < 0) {
		return NODE_NORMAL;
	}
	if (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode)) {
		return NODE_NORMAL;
	}
	return NODE_WRITABLE;
}

int
mch_screenmode(unsigned char *arg)
{
	where();
	//EMSG(_(e_screenmode));
	return FAIL;
}

int
mch_check_win(int argc, char **argv)
{
	return OK;
}

void
mch_settitle(unsigned char *title, unsigned char *icon)
{
}

void
mch_restore_title(int which)
{
}

int
mch_can_restore_icon(void)
{
	return FAIL;
}

int
mch_can_restore_title(void)
{
	return FAIL;
}

void
mch_setmouse(int on)
{
}

static int
RealWaitForChar(int fd, long msec, int *_unused)
{
	UINTN		index;
	EFI_STATUS	status;
	EFI_EVENT	te;
	EFI_EVENT	evs[2];

	// just try uefi wait event
	if(msec < 0){
		gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
		return 1;
	}

	gBS->CreateEvent(EVT_TIMER, 0, NULL, NULL, &te);
	gBS->SetTimer(te, TimerRelative, msec * 10000);

	evs[0] = gST->ConIn->WaitForKey;
	evs[1] = te;

	status = gBS->WaitForEvent(2, evs, &index);
	if(EFI_ERROR(status))
		return -1;

	// timeout?
	if(index == 1)
		return 0;

	return 1;
}

// -1 == forever
// 0 == no block
// n == timeout
static int
WaitForChar(long msec)
{
	return RealWaitForChar(STDIN_FILENO, msec, NULL);
}

void
mch_delay(long msec, int ignoreinput)
{
	if(ignoreinput)
		usleep(msec*1000);
	else
		RealWaitForChar(STDIN_FILENO, msec, NULL);
}

int
mch_inchar(unsigned char *buf, int maxlen, long wtime, int tb_change_cnt)
{
	int len;

	where("wtime %d", wtime);

	if(wtime >= 0){
		if(WaitForChar(wtime) == 0)
			return 0;
	}

	// -1, wait forever
	for(;;){
		if(WaitForChar(-1) == 0)
			return 0;
		len = read(STDIN_FILENO, buf, maxlen/*maxlen*/);
		where("read `%.*s`", len, *buf);
		if(len > 0)
			return len;
	}
}

/* Read a number and return bytes consumed. */
static int
scr_escape_number(char *p, int len, int *n)
{
	int num;
	int chlen;

	if(len == 0){
		return -1;
	}

	num = 0;
	chlen = 0;
	while(len && isdigit(*p)){
		num = num * 10 + (*p - '0');
		p++;
		len--;
		chlen++;
	}

	*n = num;
	return chlen;
}

static void
fixops(int *operand)
{
	if(operand[0] < 1)
		operand[0] = 1;
}

static void
curset(int col, int row)
{
	/* escape codes use 1-based cursor, uefi uses 0 */
	where("curset %d %d", col-1, row-1);
	sto->SetCursorPosition(sto, col-1, row-1);
}

static void
curdelta(int col, int row)
{
	UINTN newcol, newrow;

	newcol = sto->Mode->CursorColumn + col;
	newrow = sto->Mode->CursorRow + row;
	where("curdelta %d %d", newcol, newrow);
	sto->SetCursorPosition(sto, newcol, newrow);
}

/* Handle escape sequence and return number of bytes consumed. */
static int
scr_escape_sequence(char *p, int len)
{
	UINTN scol, srow, color;
	int operand[10];
	int noperand;
	int i;
	int chlen;

	if(len == 0){
		return 0;
	}

	chlen = 0;


	memset(operand, 0, sizeof(operand));
	i = scr_escape_number(p, len, &operand[0]);
	noperand = 1;
	p += i;
	len -= i;
	chlen += i;
	//where("\noperand[0] = %d operand[1] = %d operand[2] = %d look = %c",operand[0],operand[1],operand[2], *p);
	while(*p == ';' || *p == '?'){
		p += 1;
		len -= 1;
		chlen += 1;
		if(noperand >= 10){
			where("too many escape operands");
			exit(1);
		}

		/* +1 to skip ;/? */
		i = scr_escape_number(p, len, &operand[noperand]);
		noperand++;
		p += i;
		len -= i;
		chlen += i;
	}

	chlen += 1;
	switch(*p){
	case 'J': /* clear screen */
		sto->ClearScreen(sto);
		break;

	case 'C': /* cursor forward */
		fixops(operand);
		curdelta(operand[0], 0);
		break;

	case 'H': /* cursor motion */
		fixops(operand+1);
		curset(operand[1], operand[0]);
		break;

	case 'K': /* Clear in Line */
		scol = sto->Mode->CursorColumn, srow = sto->Mode->CursorRow;
		switch(operand[0]){
		case 2: /* whole line */
			sto->SetCursorPosition(sto, 0, sto->Mode->CursorRow);
			for(i = 0; i < cols; i++)
				write(STDOUT_FILENO, " ", 1);
			break;
		case 1: /* start of line to pos */
			sto->SetCursorPosition(sto, 0, sto->Mode->CursorRow);
			for(i = 0; i < scol; i++)
				write(STDOUT_FILENO, " ", 1);
			break;
		default: /* pos to eol */
			for(i = 0; i < cols-scol-1; i++)
				write(STDOUT_FILENO, " ", 1);
			break;
		}
		sto->SetCursorPosition(sto, scol, srow);
		break;

	case 'm': /* mode */
		i = operand[0];
		if(i == 0){
			setfg(7);
			setbg(0);
		} else if(i == 7){
			/* swap fg/bg */
			color = bgcol;
			bgcol = fgcol;
			fgcol = bgcol;
			setattr();
		} else if(i >= 30 && i <= 37){
			/* fg color */
			setfg(i-30);
		} else if(i >= 40 && i <= 47){
			/* bg color */
			setbg(i-40);
		} else {
			where("scr_escape_sequence: unhandles mode sequence (p=%d)", i);
		}
		break;

	//case 'R': /* scroll region */
/*
		where("fix R");
		scrollregion.min.y = n1;
		scrollregion.max.y = n2 + 1;
*/
		break;

	//case 'V': /* scroll region vertical */
/*
		where("fix V");
		scrollregion.min.x = n1;
		scrollregion.max.x = n2 + 1;
*/
		break;

	//case 'K': /* clear to end of line */
/*
		where("fix K");
		draw(screen, Rect(screen->clipr.min.x + curcol * fontsize.x,
					screen->clipr.min.y + currow * fontsize.y,
					screen->clipr.max.x,
					screen->clipr.min.y + (currow + 1) * fontsize.y),
				bgcolor, nil, ZP);
		break;
*/

	case 'L': /* add new blank line */
		p++;
		int nlines = 1;
		while(len >= 3 && p[0] == '\x1b' && p[1] == '[' && p[2] == 'L'){
			nlines++;
			len -= 3;
			p += 3;
			chlen += 3;
		}
		for(i = 0; i < nlines; i++){
			// bleg, \n side effects.
			mch_write((unsigned char*)"\n", 1);
		}
		break;

	default:
		/* TODO */
		where("scr_escape_sequence: unhandled sequence (p=%c)", *p);
		exit(1);
	}

	return chlen;
}

void
mch_write(unsigned char *_p, int _len)
{
	char *p, buf[4096];
	size_t len, n;

	if(_len > sizeof(buf)-1){
		where("too much crap to write");
		exit(1);
	}

	len = snprintf(buf, sizeof(buf), "%.*s", _len, _p);
	p = buf;

	where("output `%.*s`\n", len, p);

	while(len > 0){
/*
		fprintf(stdout, "%#x ", *p);
		if(isprint(*p))
			fprintf(stdout, "%c\n", *p);
		else
			fprintf(stdout, "%o\n", *p);
		len--;
		p++;
// */
/**/
		n = strcspn((char*)p, "\a\b\033");
		if(n != 0){
			where("write `%.*s`\n", n, p);
			write(STDOUT_FILENO, p, n);
			len -= n;
			p += n;
			continue;
		}

		switch(*p){
		case '\a':
			len--, p++;
			break;
		case '\b':
			len--, p++;
			curdelta(-1, 0);
			write(STDOUT_FILENO, " ", 1);
			curdelta(-1, 0);
			break;
		case '\n':
			len--, p++;
			write(STDOUT_FILENO, p, 1);
			break;
		case '\r':
			len--, p++;
			write(STDOUT_FILENO, p, 1);
			break;
		case '\033':
			if(len < 3 || p[1] != '['){
				where("bad escape data: %s", p);
				exit(1);
			}

			n = 2;
			n += scr_escape_sequence((char*)p+2, len-2);
			where("escape `%.*s`\n", n, p);
			len -= n;
			p += n;
			break;
		default:
			where("whack attack %#x", *p);
			exit(1);
		}
// */
	}
}

int
mch_char_avail(void)
{
	return WaitForChar(0);
}

int
mch_rename(const char *src, const char *dest)
{
	struct stat st;
	int rv;

	where("src %s dest %s", src, dest);

	rv = stat(dest, &st);
	if(rv >= 0){
		where("file `%s` exists", dest);
		return -1;
	}

	rv = rename(src, dest);
	if(rv != 0){
		where("rename `%s` -> `%s` failed: %d %s", src, dest, errno, strerror(errno));
		return -1;
	}

	return 0;
}

int
mch_remove(unsigned char *name)
{
	return remove((char*)name);
}

int
mch_isdir(unsigned char *name)
{
	struct stat st;
	if (stat((char*)name, &st) != 0) {
		return FALSE;
	}
	return S_ISDIR(st.st_mode) ? TRUE : FALSE;
}

int
mch_dirname(unsigned char *buf, int len)
{
	return (getcwd((char*)buf, len) ? OK : FAIL);
}

int
mch_chdir(char *path)
{
	return chdir(path);
}

int
mch_isFullName(unsigned char *fname)
{
	// e.g. fs0:\foo.txt
	if (strstr((char*)fname, ":\\") != NULL)
		return TRUE;

	return FALSE;
}

int
mch_FullName(unsigned char *fname, unsigned char *buf, int len, int force)
{
	char path[PATH_MAX];

	where("fname `%s`", fname);

	if(realpath((char*)fname, path) == NULL)
		return FAIL;

	where("expand `%s`", path);

	strncpy((char*)buf, (char*)path, len-1);
	if(len > 0)
		buf[len-1] = '\0';

	return OK;
}

int
mch_input_isatty(void)
{
	return isatty(STDIN_FILENO);
}

void
mch_settmode(int mode)
{
	static int first = TRUE;
	static struct termios told;
	struct termios tnew;

	if(first){
		first = FALSE;
		tcgetattr(STDIN_FILENO, &told);
	}

	tnew = told;

	where("mode %d", mode);

	switch(mode){
	case TMODE_COOK:
		break;
	case TMODE_SLEEP:
		tnew.c_lflag &= ~(ECHO);
		break;
	case TMODE_RAW:
		tnew.c_iflag &= ~ICRNL;
		tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE | IEXTEN);
		//tnew.c_oflag &= ~ONLCR;
		tnew.c_oflag |= ONLRET;
		tnew.c_cc[VMIN] = 1;
		tnew.c_cc[VTIME] = 0;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &tnew);
}

int
mch_can_exe(unsigned char *name, unsigned char **path, int use_path)
{
	return -1;
}

int
mch_call_shell(unsigned char *cmd, int options)
{
	return -1;
}

int
mch_get_user_name(unsigned char *s, int len)
{
	vim_strncpy(s, (unsigned char*)"none", len-1);
	return OK;
}

char **environ = NULL;

int
putenv(const char *string)
{
	errno = ENOSYS;
	return -1;
}

void
slash_adjust(unsigned char *p)
{
	if(path_with_url(p))
		return;
	while(*p){
		if(*p == psepcN)
			*p = psepc;
		MB_PTR_ADV(p);
	}
}

/* pathdef.c */
#include "vim.h"
char_u *default_vim_dir = (char_u *)"fs0:\\vim";
char_u *default_vimruntime_dir = (char_u *)"fs0:\\vim\\vimfiles";
char_u *all_cflags = (char_u *)"you don't want to know";
char_u *all_lflags = (char_u *)"you don't want to know";
char_u *compiled_user = (char_u *)"none";
char_u *compiled_sys = (char_u *)"none";

