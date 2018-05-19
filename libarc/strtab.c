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
#include "libarc/mblock.h"
#include "strtab.h"

#ifdef HAVE_SAFE_MALLOC
extern void *safe_malloc(size_t count);
#define malloc safe_malloc
#endif /* HAVE_SAFE_MALLOC */


void init_string_table(StringTable *stab)
{
    memset(stab, 0, sizeof(StringTable));
}

StringTableNode *put_string_table(StringTable *stab, char *str, int len)
{
    StringTableNode *p;

    p = new_segment(&stab->pool, sizeof(StringTableNode) + len + 1);
    if(p == NULL)
	return NULL;
    p->next = NULL;
    if(str != NULL)
	memcpy(p->string, str, len + 1);
    if(stab->head == NULL)
    {
	stab->head = stab->tail = p;
	stab->nstring = 1;
    }
    else
    {
	stab->nstring++;
	stab->tail = stab->tail->next = p;
    }
    return p;
}

char **make_string_array(StringTable *stab)
{
    char **table, *u;
    int i, n, s;
    StringTableNode *p;

    n = stab->nstring;
    if(n == 0)
	return NULL;
    if((table = (char **)malloc((n + 1) * sizeof(char *))) == NULL)
	return NULL;

    s = 0;
    for(p = stab->head; p; p = p->next)
	s += strlen(p->string) + 1;

    if((u = (char *)malloc(s)) == NULL)
    {
	free(table);
	return NULL;
    }

    for(i = 0, p = stab->head; p; i++, p = p->next)
    {
	int len;

	len = strlen(p->string) + 1;
	table[i] = u;
	memcpy(u, p->string, len);
	u += len;
    }
    table[i] = NULL;
    delete_string_table(stab);
    return table;
}

void delete_string_table(StringTable *stab)
{
    reuse_mblock(&stab->pool);
    init_string_table(stab);
}
