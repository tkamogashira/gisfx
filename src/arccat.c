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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libarc/arc.h>

extern struct URL_module URL_module_file;
extern struct URL_module URL_module_dir;
extern struct URL_module URL_module_http;
extern struct URL_module URL_module_ftp;
extern struct URL_module URL_module_news;
extern struct URL_module URL_module_newsgroup;

#ifndef __W32__
extern struct URL_module URL_module_pipe;
#endif /* __W32__ */

int main(int argc, char** argv)
{
    int i;
    URL url;
    int nfiles;
    char **files;

    url_add_modules(
	&URL_module_file,
	&URL_module_dir,
	&URL_module_http,
	&URL_module_ftp,
	&URL_module_news,
	&URL_module_newsgroup,
#ifndef __W32__
	&URL_module_pipe,
#endif /* __W32__ */
	NULL);

    nfiles = argc - 1;
    files  = argv + 1;

    /* Expand archive file names. */
    files = expand_archive_names(&nfiles, files);
    if(files == NULL)
	return 1;

    for(i = 0; i < nfiles; i++) {
	if(strchr(files[i], '#') != NULL) {
	    /* archive file */
	    url = url_arc_open(files[i]);
	} else {
	    /* normal file */
	    url = url_open(files[i]);
	}

	if(url == NULL)
	{
	    fprintf(stderr, "Can't open: %s\n", files[i]);
	    continue;
	}

	if(url->type != URL_dir_t && url->type != URL_newsgroup_t)
	{
	    int c;
	    while((c = url_getc(url)) != EOF)
		putchar(c);
	}
	else
	{
	    char buff[BUFSIZ];
	    while(url_gets(url, buff, sizeof(buff)) != NULL)
		puts(buff);
	}
	url_close(url);
    }

    arc_list_free(files);

    /* free internal caches. */
    free_archive_files();

    return 0;
}
