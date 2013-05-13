/*
 * daycalc.c
 */

/*
 * yank  -  yet another NoteKeeper
 * Copyright (C) 1999, 2000, 2001 Michael Hußmann <m.hussmann@home.ins.de>
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

#include "daycalc.h"

static char *monthlist[] =
{
    N_("Jan"),
    N_("Feb"),
    N_("Mar"),
    N_("Apr"),
    N_("May"),
    N_("Jun"),
    N_("Jul"),
    N_("Aug"),
    N_("Sep"),
    N_("Oct"),
    N_("Nov"),
    N_("Dec")
};

/* -------------------------------------------------------- */
/*
 * returns TRUE if 'year' is a leapyear
 */

static int
leapyear(unsigned int year)
{
    if ( (( year % 4 ==0) && (year % 100 != 0)) || (year % 400 == 0) )
    {
        return (1);
    }
    return (0);
}

/* -------------------------------------------------------- */
/*
 * returns the daynr for yyyy-mm-ddd
 */

static unsigned int
daynr(unsigned int day, unsigned int month, unsigned int year)
{
    int d, e;

    d = (month + 10) / 13;
    e = day + (611 * (month + 2)) / 20 - 2 * d - 91;
    return  (e + leapyear(year) * d);
}

/* -------------------------------------------------------- */
/*
 * returns month for daynr in year
 */

static unsigned int
month_in_year(unsigned int year, int n)
{
    int a;

    a = leapyear(year);
    if ( n > 59+a )
    {
	n += 2-a;
    }
    n += 91;
    return ((20*n)/611 - 2);
}

/* -------------------------------------------------------- */
/*
 * returns weekday for daynr in year
 */

static unsigned int
day_in_year(unsigned int year, int n)
{
    int a, m;
    a = leapyear(year);
    if ( n > 59+a )
    {
	n += 2-a;
    }
    n += 91;
    m = (20*n)/611;
    return n - (611*m)/20;
}

/* -------------------------------------------------------- */
/*
 * get the number of days since 1.1.1970 and return
 * a string 'dd month yyyy'
 */

char *
days2date(long int d_count)
{
    int days;
    int year = 1970;
    static char ptr[] = "";

    if (d_count < 0)
    {
        return (ptr);
    }
    
    d_count++;
    while (d_count > (days = leapyear(year) ? 366 : 365))
    {
	d_count -= days;
	year++;
    }
    return (num2date(day_in_year(year, d_count),
                     (month_in_year(year, d_count) -1) ,year));
}

/* -------------------------------------------------------- */
/*
 * get the number of days since 1.1.1970 and return 
 * a string 'yyyy-mm-dd'
 */

char *
days2isodate(long int d_count)
{
    int days;
    int year = 1970;
    static char ptr[] = "";

    if (d_count < 0)
    {
        return (ptr);
    }
    
    d_count++;
    while (d_count > (days = leapyear(year) ? 366 : 365))
    {
	d_count -= days;
	year++;
    }
    return (num2isodate(day_in_year(year, d_count),
                        (month_in_year(year, d_count)) ,year));
}

/* -------------------------------------------------------- */
/*
 * get a string 'dd month yyyy' and return  * the number of
 * days since 1.1.1970
 */

long int
date2days(char *date)
{
    int      t_year = 1970;
    int      d_days = 0;
    int      c_days = 0;
    long int ret;
    unsigned int      day;
    unsigned int      month;
    unsigned int      year;
    char     *p;

    if (strlen(date))
    {
        p = date;
        ret = date2num(p, &day, &month, &year);
        if (ret == 0)
        {
            long int day_nr;
            while ((t_year < year) && (d_days = leapyear(t_year) ? 366 : 365))
            {
                c_days = c_days + d_days;
                t_year++;
            }
            day_nr = daynr(day, (month+1), year);
            day_nr = day_nr + c_days;
            ret = day_nr - 1;
        }
    }
    else
    {
        ret = -1;
    }
    
    return (ret);
}

/* -------------------------------------------------------- */
/*
 * get YYYY MM DD and return the number of days since 1.1.1970
 */

long int
isodate2days(char *date)
{
    int      t_year = 1970;
    int      d_days = 0;
    int      c_days = 0;
    long int ret;
    long int day_nr;
    unsigned int      day;
    unsigned int      month;
    unsigned int      year;
    char     *p;

    if (strlen(date))
    {        
        p = date;
        ret = isodate2num(p, &day, &month, &year);
        if (ret == 0)
        {
            while ((t_year < year) && (d_days = leapyear(t_year) ? 366 : 365))
            {
                c_days = c_days + d_days;
                t_year++;
            }
            day_nr = daynr(day, (month+1), year);
            day_nr = day_nr + c_days;
            ret = day_nr - 1;
        }
    }
    else
    {
        ret = -1;
    }
    
    return (ret);
}

/* -------------------------------------------------------- */
/*
 * return a nice formatted string with the current date
 * like '31 Jan 2001
 */

char *
current_date(void)
{
    struct tm *stm;
    time_t    xtt;
    
    xtt = time(&xtt);
    stm = localtime((const time_t *) &xtt);

    return (num2date(stm->tm_mday, stm->tm_mon, (1900+stm->tm_year)));
}

/* -------------------------------------------------------- */
/*
 * converts a string "dd month year" into integers
 * returns 0 upon success
 */

int
date2num(char *str, unsigned int *day, unsigned int *month, unsigned int *year)
{
    char buf[4];
    int  ret = 0;
    int  found = 0;
    
    if (str && sscanf(str, "%u %3s %u", day, buf, year) == 3)
    {
        *month = 0;
	/* search month */
	while ((!found) && (*month < 12))
        {
	    if (!strcmp(buf, _(monthlist[*month])))
            {
		found = 1;
	    }
            else
            {
		(*month)++;
	    }
	}
	if (!found)
	  ret = -1;
    }
    else
      ret = -1;
    
    return ret;
}

/* -------------------------------------------------------- */
/*
 * converts integers to a string "dd month year"
 */

char*
num2date(unsigned int day, unsigned int month, unsigned int year)
{
    static char t_buffer[20];
    
    snprintf(t_buffer, (19), "%02u %s %u", day, _(monthlist[month]), year);
    t_buffer[19] = '\0';
    
    return (t_buffer);
}

/* ------------------------------------------------------ */
/*
 * converts integer to a string "year-mm-dd"
 */

char*
num2isodate(unsigned int day, unsigned int month, unsigned int year)
{
    static char t_buffer[20];

    snprintf(t_buffer, (19), "%04u-%02u-%02u", year, month, day);
    t_buffer[19] = '\0';

    return(t_buffer);
}

/* ------------------------------------------------------ */
/*
 * return yyyy, (month - 1), day from YYYY-MM-DD
 */

int
isodate2num(char *str, unsigned int *day, unsigned int *month,
	    unsigned int *year)
{
    if (str && sscanf(str, "%4u-%2u-%2u", year, month, day) == 3)
    {
	*month -= 1;
	return 0;
    }
    return -1;
}

/* ------------------------------------------------------ */
/*
 * return a nice formatted string with date& time
 */

char*
time2isodate(time_t *t)
{
    static struct tm *tms;
    static char      t_string[30];

    tms = localtime(t);
    strftime(t_string, 29, "%Y-%m-%d %X", tms);
    
    return (t_string);
}

/* ------------------------------------------------------ */
/*
 * transform "2000-02-29 11:45:32" into time_t
 */

time_t
isodate2time(char *str)
{
    time_t        t_val;
    static struct tm p_t;
    char           *pos;

    t_val = -1;

    if (str == NULL)
    {
        return (t_val);
        /* notreached */
    }
    
    pos = strptime(str, "%Y-%m-%d %X", &p_t);
    if (pos != NULL)
    {
        t_val = mktime(&p_t);
    }
    
    return (t_val);
}

/* ------------------------------------------------------ */
