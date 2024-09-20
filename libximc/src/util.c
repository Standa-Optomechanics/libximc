#include "common.h"
#include <stdarg.h>
#include <locale.h>

#include "ximc.h"
#include "util.h"
#include "metadata.h"
#include "platform.h"

#define ERRORBUFLEN 2000

logging_callback_t g_logging_callback = NULL;
void* g_logging_user_data = NULL;

/* Strip from the string str all last occurrences of any character from the delim */
void wcsrstrip (wchar_t* str, wchar_t* delim)
{
	size_t i = wcslen( str ), k;
	while (i-- > 0)
	{
		for (k = 0; k < wcslen( delim ); ++k)
		{
			if (str[i] == delim[k])
			{
				str[i] = 0;
				break;
			}
		}
	}
}

wchar_t* str_to_widestr (const char* str)
{
	wchar_t *result;
	mbstate_t mbs;
	size_t len;
	memset(&mbs, 0, sizeof(mbs));
	len = mbsrtowcs( NULL, &str, 0, &mbs );
	if (len == (size_t)(-1))
		return NULL;
	result = (wchar_t*)malloc(sizeof(wchar_t)*(len+1));
	if (result && mbsrtowcs( result, &str, len+1, &mbs ) != len)
	{
		free(result);
		return NULL;
	}
	return result;
}

char* widestr_to_str (const wchar_t* str)
{
	char *result;
	mbstate_t mbs;
	size_t len;
	memset(&mbs, 0, sizeof(mbs));
	len = wcsrtombs( NULL, &str, 0, &mbs );
	if (len == (size_t)(-1))
		return NULL;
	result = (char*)malloc(sizeof(char)*(len+1));
	if (result && wcsrtombs( result, &str, len+1, &mbs ) != len)
	{
		free(result);
		return NULL;
	}
	return result;
}

uint16_t get_crc(const byte *pbuf, size_t n)
{
	uint16_t crc, carry_flag, a;
	size_t i,j;
	crc = 0xffff;
	for ( i = 0; i < n; i++ ){
		crc = crc ^ pbuf[i];
		for ( j = 0; j < 8; j++ ){
			a = crc;
			carry_flag = a & 0x0001;
			crc = crc >> 1;
			if ( carry_flag == 1 ) crc = crc ^ 0xA001;
		};
	}
	return crc;
}

size_t ximc_min(size_t a, size_t b)
{
	return a < b ? a : b;
}

size_t ximc_max(size_t a, size_t b)
{
	return a > b ? a : b;
}

int powi(int x, int y)
{
	int temp;
	if (y < 0)
	{
		log_error( L"error in function powi" );
		abort();
	}
	if (y == 0)
		return 1;
	temp = powi(x, y/2);
	if (y%2 == 0)
		return temp*temp;
	else
		return x*temp*temp;
}


int is_numeric (const char* s)
{
	if (!*s)
		return 0;
	for (;*s;++s)
		if (!isdigit( *s ))
				return 0;
	return 1;
}

int is_hex (const char* s)
{
	for (;*s;++s)
		if (!(isdigit( *s ) || (tolower( *s ) >= 'a' && tolower( *s ) <= 'f')))
				return 0;
	return 1;
}

void dump_bytes (const byte* data, size_t amount)
{
	size_t i;
	wchar_t buf[1300], buf1[32]; // 1300==5*256+20
	size_t MAXBUF=sizeof(buf)/sizeof(wchar_t);
	size_t MAXBUF1=sizeof(buf1)/sizeof(wchar_t);

	wcsncpy( buf, L"[", MAXBUF-1 );
	for (i = 0; i < amount; ++i)
	{
		swprintf( buf1, MAXBUF1, L"%02X ", data[i] );
		wcsncat( buf, buf1, MAXBUF-1 );
	}
	wcsncat( buf, L" (", MAXBUF-1 );
	for (i = 0; i < amount; ++i)
	{
		if (data[i] >= 32)
		{
			swprintf( buf1, MAXBUF1, L"%hc ", data[i] );
			wcsncat( buf, buf1, MAXBUF-1 );
		}
		else
			wcsncat( buf, L"* ", MAXBUF-1 );
	}
	wcsncat( buf, L" ) ]", MAXBUF-1 );
	log_debug( buf );
}


// logging

void log_generic_varg (int loglevel, const wchar_t* format, va_list arglst)
{
	wchar_t s[ERRORBUFLEN];
	wchar_t format_with_tid[ERRORBUFLEN];
	if (g_logging_callback)
	{
		/* hey, we do not have snwprintf! */
		swprintf( format_with_tid, ERRORBUFLEN-1, L"[%llx] %ls", get_thread_id(), format );
		vswprintf( s, ERRORBUFLEN-1, format_with_tid, arglst);
		g_logging_callback( loglevel, s, g_logging_user_data );
	}
}

void log_generic (int loglevel, const wchar_t* format, ...)
{
	va_list arglst;
	va_start( arglst, format );
	log_generic_varg( loglevel, format, arglst );
	va_end( arglst );
}

void log_debug (const wchar_t* format, ...)
{
	va_list arglst;
	va_start( arglst, format );
	log_generic_varg( LOGLEVEL_DEBUG, format, arglst );
	va_end( arglst );
}

void log_info (const wchar_t* format, ...)
{
	va_list arglst;
	va_start( arglst, format );
	log_generic_varg( LOGLEVEL_INFO, format, arglst );
	va_end( arglst );
}

void log_warning (const wchar_t* format, ...)
{
	va_list arglst;
	va_start( arglst, format );
	log_generic_varg( LOGLEVEL_WARNING, format, arglst );
	va_end( arglst );
}

void log_error (const wchar_t* format, ...)
{
	va_list arglst;
	va_start( arglst, format );
	log_generic_varg( LOGLEVEL_ERROR, format, arglst );
	va_end( arglst );
}

void log_system_error (const wchar_t* format, ...)
{
	unsigned int code = get_system_error_code();
	va_list arglst;
	wchar_t s[ERRORBUFLEN];
	wchar_t *error_str;
	wchar_t default_message[] = L"Unknown error";

	va_start( arglst, format );
	vswprintf( s, ERRORBUFLEN-1, format, arglst);
	va_end( arglst );
	error_str = get_system_error_str( code );

	log_generic( LOGLEVEL_ERROR, L"%ls [%lu] %ls",
			s,
			code,
			error_str ? error_str : default_message
		);

	free_system_error_str(error_str);
}

const wchar_t* loglevel_string_w (int loglevel)
{
	switch (loglevel)
	{
		case LOGLEVEL_ERROR: 	return L"ERROR";
		case LOGLEVEL_WARNING:	return L"WARN";
		case LOGLEVEL_INFO:		return L"INFO";
		case LOGLEVEL_DEBUG:	return L"DEBUG";
		default:				return L"UNKNOWN";
	}
}


const int default_max_loglevel = LOGLEVEL_WARNING;

int parse_path_and_query(
		const char *ppath,
		char *path, size_t path_len,
		char *paramname, size_t paramname_len,
		char *paramvalue, size_t paramvalue_len)
{
	const char *p = ppath, *pn;

	// check is there a param
	pn = strstr(p, "?");

	if (pn)
	{
		// write path
		if ((size_t)(pn + 1 - p) > path_len)
			return 1;
		memcpy(path, p, pn-p);
		path[pn-p] = 0;

		// parse param
		p = pn;
		pn = strstr(p, "=");
		if (pn)
		{
			if (paramname && paramvalue)
			{
				// param name
				if ((size_t)(pn + 1 - p) > paramname_len)
					return 1;
				memcpy(paramname, p+1, pn-p-1);
				paramname[pn-p-1] = 0;
				// param value
				if (strlen(pn)+1 > paramvalue_len)
					return 1;
				strcpy(paramvalue, pn+1);
				paramvalue[strlen(pn)] = 0;
			}
		}
	}
	else
	{
		// path without param
		if (strlen(p)+1 > path_len)
			return 1;
		strcpy(path, p);
		path[strlen(p)] = 0;
	}
	return 0;
}

/* Parse an uri
 * Returns 0 on success
 */
int parse_uri(const char *name,
		char *scheme, size_t scheme_len,
		char *host, size_t host_len,
		char *path, size_t path_len,
		char *paramname, size_t paramname_len,
		char *paramvalue, size_t paramvalue_len)
{
	const char *p, *pn;
	if (!name || !scheme || !host || !path ||
			!scheme_len || !host_len || !path_len)
		return 1;
	*host = *path = *scheme = 0;

	if (paramname && paramname_len)
		*paramname = 0;
	if (paramvalue && paramvalue_len)
		*paramvalue = 0;

	p = name + strspn(name, " \t");
	if (!p)
		return 1;

	// parse scheme
	pn = strchr(p, ':');
	if (!pn)
		return 1;
	// copy scheme
	if ((size_t)(pn + 1 - p) > scheme_len)
		return 1;
	memcpy(scheme, p, pn-p);
	scheme[pn-p] = 0;

	// move after colon
	p = pn+1;

	// parse host
	if (strstr(p, "//") == p)
	{
		// there is a host part (i.e. scheme://host or scheme://host/path)
		p += 2;
		if (!*p)
			return 1;
		// p now points to first character after //
		pn = strchr(p, '/');
		if (pn == p)
		{
			// no host part, only path (scheme:///path)
			if (strcmp(scheme, "xi-emu") == 0)
			{
				/* A little kludge! In case of "xi-emu:///path" on Windows, we demand absolute path: "xi-emu:///C:\xxxx" #85091#note-28.
				 *
				 * Add platform-dependent path validation. We demand absolute path! For windows it must start with partition letter followed
				 * by ":\\" or ":/". Yes, let it be POSIX-slash-style compatible.
				 * Purpose of the following is to block allowed in fopen relative paths. User must be restricted!
				 */
				#ifdef _WIN32
				if (!isalpha(p[1]) || p[2] != ':' || p[3] != '\\' && p[3] != '/') // In case of wrong absolute path
					return 1;
				#endif
			}

			if (parse_path_and_query(p+1,
						path, path_len,
						paramname, paramname_len,
						paramvalue, paramvalue_len))
				return 1;
		}
		else if (pn)
		{
			// path part (scheme://host/path)
			if ((size_t)(pn + 1 - p) > host_len)
				return 1;
			memcpy(host, p, pn-p);
			host[pn-p] = 0;

			if (parse_path_and_query(pn+1,
						path, path_len,
						paramname, paramname_len,
						paramvalue, paramvalue_len))
				return 1;
		}
		else // pn == NULL
		{
			// no path (scheme://host)
			if (strlen(p)+1 > host_len)
				return 1;
			strcpy(host, p);
			host[strlen(p)] = 0;
		}
	}
	else
	{
		if (*p)
		{
			// only path (scheme:path)
			if (strlen(p)+1 > path_len)
				return 1;
			strcpy(path, p);
			path[strlen(p)] = 0;
		}
	}
	return 0;
}


/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return (char)(isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10);
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *uri_encode(const char *str) {
  static char hex[] = "0123456789ABCDEF";
  const char *pstr = str;
  char *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~' || *pstr == '/') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
	{
      *pbuf++ = '%';
	  *pbuf++ = hex[(*pstr >> 4) % 15];
	  *pbuf++ = hex[(*pstr & 15) % 15];
	}
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *uri_decode(const char *str) {
  const char *pstr = str;
  char *buf = malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') { 
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* Returns a copy of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *uri_copy(const char *str) {
	size_t len = strlen(str);
	char *buf = malloc(len + 1);
	strncpy(buf, str, len);
	buf[len] = '\0';
	return buf;
}

/*
 * Exported functions begins
 */

#if defined(__cplusplus)
extern "C" {
#endif

void XIMC_API logging_callback_stderr_wide(int loglevel, const wchar_t* message, void* user_data)
{
	XIMC_UNUSED(user_data);
	if (loglevel > default_max_loglevel)
		return;
	fwprintf( stderr, L"%ls: %ls\n", loglevel_string_w( loglevel ), message );
}

void XIMC_API logging_callback_stderr_narrow(int loglevel, const wchar_t* message, void* user_data)
{
	XIMC_UNUSED(user_data);
	if (loglevel > default_max_loglevel)
		return;
	fprintf( stderr, "%ls: %ls\n", loglevel_string_w( loglevel ), message );
}

void XIMC_API set_logging_callback(logging_callback_t logging_callback, void* user_data)
{
	if (logging_callback)
		g_logging_callback = logging_callback;
	else
		g_logging_callback = NULL;
	g_logging_user_data = user_data;
}


#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

void XIMC_API ximc_version(char* version)
{
	if (version)
	{
		strncpy( version, STR(LIBXIMC_VERSION), 32 );
	}
}

#if defined(__cplusplus)
};
#endif

// vim: syntax=c tabstop=4 shiftwidth=4
