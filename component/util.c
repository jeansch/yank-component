/*
 * util.c
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

#include "util.h"

/* ------------------------------------------------------ */
/*
 * stolen from pan
 */

char*
get_host_name(void)
{
    char *ptr;
    char hostname[256] = { '\0' };
    
    if ((gethostname(hostname, sizeof(hostname))) == -1) {
        if ((ptr = getenv("HOST")) != NULL)
            strncpy(hostname, ptr, MAXHOSTNAMELEN);
        else if ((ptr = getenv("HOSTNAME")) != NULL)
            strncpy(hostname, ptr, MAXHOSTNAMELEN);
        else
            hostname[0] = '\0';
    }
    hostname[255] = '\0';
    ptr = strtok (hostname, ".");
    
    return g_strdup (ptr);
}

/* ------------------------------------------------------ */
/*
 * stolen from pan
 * Returns the fully qualified domain name 
 */

char*
get_fqdn(const char * host)
{
    char name[512] = { '\0' };
    char line[1025];
    char fqdn[1024] = { '\0' };
    char * cp;
    char * domain = NULL;
    FILE * inf;
    struct hostent * hp;
    struct in_addr in;
    
    if (host) {
        if (strchr(host, '.'))
            return g_strdup(host);
        strncpy(name, host, MAXHOSTNAMELEN);
    }
    else {
        if (gethostname(name, MAXHOSTNAMELEN))
            return NULL;
    }
    
    if (isdigit((int)*name)) {
        in.s_addr = inet_addr (name);
        if ((hp = gethostbyaddr((char *) &in.s_addr, 4, AF_INET)))
            in.s_addr = (*hp->h_addr);
        return g_strdup((hp && strchr(hp->h_name, '.') ? hp->h_name : (char *) inet_ntoa(in)));
    }
    if ((hp = gethostbyname(name)) && !strchr(hp->h_name, '.'))
        if ((hp = gethostbyaddr(hp->h_addr, hp->h_length, hp->h_addrtype
            )))
            in.s_addr = (*hp->h_addr);
    
    sprintf(fqdn, "%s", hp ? strchr(hp->h_name, '.')
            ? hp->h_name : (char *) inet_ntoa(in)
            : "");
    if (!*fqdn || (fqdn[strlen(fqdn) - 1] <= '9')) {
        *fqdn = '\0';
        inf = fopen("/etc/resolv.conf", "r");
        if (inf) {
            while (fgets(line, 1024, inf)) {
                line[1024] = '\0';
                g_strchug(line);
                g_strchomp(line);
                if (strncmp(line, "domain ", 7) == 0) {
                    domain = line + 7;
                    break;
                }
                if (strncmp(line, "search ", 7) == 0) {
                    domain = line + 7;
                    cp = strchr(domain, ' ');
                    if (cp)
                        *cp = '\0';
                    break;
                }
            }
            if (domain)
                sprintf(fqdn, "%s.%s", name, domain);
            fclose(inf);
        }
    }
    return g_strdup (fqdn);
}

/* ------------------------------------------------------ */
/*
 * stolen from pan
 */

gchar*
create_note_id(void)
{
    char * tmp;
    GString * msgid = g_string_sized_new (256);
    
    /* add local part to message-id */
    {
        char buf[32];
        const time_t now = time(NULL);
        struct tm local;
        localtime_r (&now, &local);
        strftime (buf, sizeof(buf), "%Y%m%d.%H%M%S", &local);
        g_string_append (msgid, buf);
        g_string_sprintfa (msgid, ".%d.%lu", rand(), (gulong)getpid());
    }
    /* delimit */
    g_string_append_c (msgid, '@');
    
    /* add fqdn to message-id */
    {
        gchar * host = get_host_name ();
        gchar * fqdn = get_fqdn (host);
        if (is_nonempty_string(fqdn))
        {
            g_string_append (msgid, fqdn);
        }
        g_free (host);
        g_free (fqdn);
    }
    
    /* return */
    tmp = msgid->str;
    g_string_free (msgid, FALSE);
    return tmp;
}

/* ------------------------------------------------------ */
