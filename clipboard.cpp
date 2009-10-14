/*
	stdin -> clipboard
	clipboard -> stdout

	by opa
*/

#include <string>
#include <cstdio>
#include <cio>
#include <cfcntl>

#include <windows>

#define PGM					"clipboard"
#define PGM_DEBUG			PGM ": "
#define PGM_INFO			PGM ": "
#define PGM_WARN			PGM " warning: "
#define PGM_ERR				PGM " error: "
#define VERSTR				"1.00"

#define CREDIT2009			"Copyright (c) 2009 by opa"

typedef signed char schar;
typedef unsigned char uchar;
typedef signed int sint;
typedef unsigned int uint;
typedef signed long slong;
typedef unsigned long ulong;

using namespace std;

char
	credit[] = PGM " version " VERSTR " " CREDIT2009;
bool
	error = false;
sint
	rcode = 0;

////////////////////////////////////////////////////////////////////////

void SetClipboardText(const string &s)
{
	HANDLE
		h;
	char
		*sc;

	if((h = ::GlobalAlloc(GHND, s.size()+1)) != NULL){
		if((sc = (char *)::GlobalLock(h)) != NULL){
			memcpy(sc, s.c_str(), s.size());
			sc[s.size()] = '\0';
			if(::OpenClipboard(NULL) != 0){
				::EmptyClipboard();
				::SetClipboardData(CF_TEXT, h);
				::CloseClipboard();
			}
		}
		::GlobalUnlock(h);
	}
}

void stdin_to_clipboard()
{
	FILE
		*f = stdin;
	string
		cb;
	int
		c;

	fflush(f);
	setmode(fileno(f), O_BINARY);

	while((c = fgetc(f)) != EOF)
		cb += (char)c;

	fflush(f);
	setmode(fileno(f), O_TEXT);

	SetClipboardText(cb);
}

////////////////////////////////////////////////////////////////////////

string GetClipboardText()
{
	string
		r;
	HANDLE
		h;

	if(::OpenClipboard(NULL) == 0)
		return string();

	if((h = ::GetClipboardData(CF_TEXT)) != NULL){
		r.assign((char *)::GlobalLock(h));
		::GlobalUnlock(h);
	}

	if(::CloseClipboard() == 0)
		return string();

	return r;
}

void clipboard_to_stdout()
{
	FILE
		*f = stdout;
	string
		cb = GetClipboardText();

	fflush(f);
	setmode(fileno(f), O_BINARY);

	fputs(cb.c_str(), f);

	fflush(f);
	setmode(fileno(f), O_TEXT);
}

////////////////////////////////////////////////////////////////////////

sint wmain(sint, wchar_t *[])
{
	if(!isatty(fileno(stdin)))
		stdin_to_clipboard();
	else
		clipboard_to_stdout();

	return rcode;
}
