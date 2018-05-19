#ifndef ___LZH_H_
#define ___LZH_H_
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

typedef struct _UNLZHHandler *UNLZHHandler;

extern UNLZHHandler open_unlzh_handler(long (* read_func)(char*,long,void*),
				       const char *method,
				       long compsize, long origsize,
				       void *user_val);
extern long unlzh(UNLZHHandler decoder, char *buff, long buff_size);
extern void close_unlzh_handler(UNLZHHandler decoder);

extern char *lzh_methods[];

#endif /* ___LZH_H_ */
