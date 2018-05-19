#ifndef ___EXPLODE_H_
#define ___EXPLODE_H_
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

typedef struct _ExplodeHandler *ExplodeHandler;

enum explode_method_t
{
    EXPLODE_LIT8,
    EXPLODE_LIT4,
    EXPLODE_NOLIT8,
    EXPLODE_NOLIT4
};

extern ExplodeHandler open_explode_handler(
	long (* read_func)(char *buf, long size, void *user_val),
	int method,
	long compsize, long origsize,
	void *user_val);

extern long explode(ExplodeHandler decoder,
		    char *decode_buff,
		    long decode_buff_size);

extern void close_explode_handler(ExplodeHandler decoder);


#endif /* ___EXPLODE_H_ */
