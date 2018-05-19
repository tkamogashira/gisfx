/*
    Copyright (C) 2000 Masanao Izumo <mo@goice.co.jp>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include "libarc/url.h"

typedef struct _URL_cgiesc
{
    char common[sizeof(struct _URL)];
    URL reader;
    long rpos;
    int beg, end, eod, eof;
    unsigned char cnvbuf[1024];
    int decode;
    int autoclose;
} URL_cgiesc;

static long url_cgiesc_read(URL url, void *buff, long n);
static int  url_cgiesc_fgetc(URL url);
static long url_cgiesc_tell(URL url);
static void url_cgiesc_close(URL url);

static URL url_cgiesc_open(URL reader, int type, int autoclose)
{
    URL_cgiesc *url;

    url = (URL_cgiesc *)alloc_url(sizeof(URL_cgiesc));
    if(url == NULL)
    {
	if(autoclose)
	    url_close(reader);
	url_errno = errno;
	return NULL;
    }

    /* common members */
    URLm(url, type)      = type;
    URLm(url, url_read)  = url_cgiesc_read;
    URLm(url, url_gets)  = NULL;
    URLm(url, url_fgetc) = url_cgiesc_fgetc;
    URLm(url, url_seek)  = NULL;
    URLm(url, url_tell)  = url_cgiesc_tell;
    URLm(url, url_close) = url_cgiesc_close;

    /* private members */
    url->reader = reader;
    url->rpos = 0;
    url->beg = 0;
    url->end = 0;
    url->eod = 0;
    url->eof = 0;
    memset(url->cnvbuf, 0, sizeof(url->cnvbuf));
    url->decode = (type == URL_cgi_unescape_t);
    url->autoclose = autoclose;

    return (URL)url;
}

URL url_cgi_escape_open(URL reader, int autoclose)
{
    return url_cgiesc_open(reader, URL_cgi_escape_t, autoclose);
}

URL url_cgi_unescape_open(URL reader, int autoclose)
{
    return url_cgiesc_open(reader, URL_cgi_unescape_t, autoclose);
}

static int fillbuff(URL_cgiesc *urlp)
{
    URL reader;
    int c, n;
    unsigned char *p = urlp->cnvbuf;
    static char *cgi_encode_table =
	"000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F"
	"\000\0002122232425262728292A2B2C\000\000\000\0002F\000\000\000\000"
	"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	"3A3B3C3D3E3F40\000\000\000\000\000\000\000\000\000\000\000\000\000"
	"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	"\000\000\000\000\000\000\0005B5C5D5E\000\00060\000\000\000\000\000"
	"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	"\000\000\000\000\000\000\000\000\000\000\000\000\000\000\0007B7C"
	"7D7E7F808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C"
	"9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBC"
	"BDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDC"
	"DDDEDFE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFC"
	"FDFEFF";
    static char hex_decode[256] =
    {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12,
	13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15
    };

    if(urlp->eod)
    {
	urlp->eof = 1;
	return 1;
    }

    reader = urlp->reader;
    n = 0;

    if(urlp->decode)
    {
	while(n < sizeof(urlp->cnvbuf))
	{
	    if((c = url_getc(reader)) == EOF)
	    {
		urlp->eod = 1;
		break;
	    }
	    if(c == '+')
		c = ' ';
	    else if(c == '%')
	    {
		int c1, c2;

		if((c1 = url_getc(reader)) == EOF)
		{
		    urlp->eod = 1;
		    break;
		}
		if((c2 = url_getc(reader)) == EOF)
		{
		    urlp->eod = 1;
		    break;
		}
		c1 = hex_decode[c1];
		c2 = hex_decode[c2];
		c = (c1 << 4 | c2);
	    }
	    p[n++] = c;
	}
    }
    else
    {
	char *esp;
	while(n < sizeof(urlp->cnvbuf) - 2)
	{
	    if((c = url_getc(reader)) == EOF)
	    {
		urlp->eod = 1;
		break;
	    }
	    if(c == ' ')
		p[n++] = '+';
	    else
	    {
		esp = cgi_encode_table + 2 * c;
		if(esp[0] == '\0')
		    p[n++] = c;
		else
		{
		    p[n++] = '%';
		    p[n++] = esp[0];
		    p[n++] = esp[1];
		}
	    }
	}
    }

    urlp->rpos += urlp->beg;
    urlp->beg = 0;
    urlp->end = n;

    if(n == 0)
    {
	urlp->eof = 1;
	return 1;
    }
    return 0;
}

static long url_cgiesc_read(URL url, void *buff, long size)
{
    URL_cgiesc *urlp = (URL_cgiesc *)url;
    unsigned char *p = (unsigned char *)buff;
    long n;

    if(urlp->eof)
	return 0;

    n = 0;
    while(n < size)
    {
	int i;

	if(urlp->beg == urlp->end)
	    if(fillbuff(urlp))
		break;
	i = urlp->end - urlp->beg;
	if(i > size - n)
	    i = size - n;
	memcpy(p + n, urlp->cnvbuf + urlp->beg, i);
	n += i;
	urlp->beg += i;
    }
    return n;
}

static int url_cgiesc_fgetc(URL url)
{
    URL_cgiesc *urlp = (URL_cgiesc *)url;

    if(urlp->eof)
	return EOF;
    if(urlp->beg == urlp->end)
	if(fillbuff(urlp))
	    return EOF;

    return (int)urlp->cnvbuf[urlp->beg++];
}

static long url_cgiesc_tell(URL url)
{
    URL_cgiesc *urlp = (URL_cgiesc *)url;

    return urlp->rpos + urlp->beg;
}

static void url_cgiesc_close(URL url)
{
    URL_cgiesc *urlp = (URL_cgiesc *)url;

    if(urlp->autoclose)
	url_close(urlp->reader);
    free(url);
}

#ifdef CGIESC_MAIN
void main(int argc, char** argv)
{
    URL url;
    char buff[256], *filename;
    int c, decode;

    if(argc != 2)
    {
	fprintf(stderr, "Usage: %s filename\n", argv[0]);
	exit(1);
    }

    if(strstr(argv[0], "unescape"))
	decode = 0;
    else
	decode = 1;
    filename = argv[1];

    if((url = url_file_open(filename)) == NULL)
    {
	perror(argv[1]);
	exit(1);
    }

    if(decode)
	url = url_cgi_unescape_open(url, 1);
    else
	url = url_cgi_escape_open(url, 1);

#if CGIESC_MAIN
    while((c = url_getc(url)) != EOF)
	putchar(c);
#else
    while((c = url_read(url, buff, sizeof(buff))) > 0)
	write(1, buff, c);
#endif
    url_close(url);
    exit(0);
}

#endif /* CGIESC_MAIN */
