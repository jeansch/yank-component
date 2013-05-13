/*
 * daycalc.h
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Huﬂmann <m.hussmann@home.ins.de>
 *
 * This program is free software; you  can redistribute it and/or modify it
 * under the terms of  the GNU General  Public License as published  by the
 * Free Software Foundation;  either version 2 of  the License, or (at your
 * option) any later version.
 *
 * This program  is  distributed in the  hope  that it will  be useful, but
 * WITHOUT  ANY   WARRANTY;  without   even   the  implied    warranty   of
 * MERCHANTABILITY or    FITNESS FOR A PARTICULAR   PURPOSE.   See  the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write to the  Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef DAYCALC_H
#define DAYCALC_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif  /* HAVE_CONFIG_H */

#include <gnome.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

char     *days2date(long int d_count);
long int date2days(char *date);
char     *current_date(void);
int      date2num(char *str, unsigned int *day, unsigned int *month,
		  unsigned int *year);
char     *num2date(unsigned int day, unsigned int month, unsigned int year);

int      isodate2num(char *str, unsigned int *day, unsigned int *month,
		     unsigned int *year);
long int isodate2days(char *date);
char*    num2isodate(unsigned int day, unsigned int month, unsigned int year);
char*    days2isodate(long int d_count);
char*    time2isodate(time_t *t);
time_t   isodate2time(char *str);

#endif    /* DAYCALC_H */
