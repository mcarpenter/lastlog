
/*
 *  lastlog.c
 *
 *  Copyright 2009 Martin Carpenter, mcarpenter@free.fr
 *
 *  Read, parse and output the contents of /var/adm/lastlog. This file
 *  is a binary table, with each record (struct lastlog) keyed off the
 *  UID. The lastlog only contains the hostname, date and (p)tty of the
 *  user's last login. Users that have never logged in are not
 *  displayed.
 *
 *  Sample output:
 *  0      root      Mon Oct 12 11:50:21 2009  pts/10   saturn
 *  10001  felipe    Wed Sep 17 14:15:02 2008  pts/8    122.106.27.6
 *  10013  boris     Mon Sep 21 12:23:54 2009  pts/18   jupiter
 *
 */

#include <time.h>
#include <lastlog.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>

#define LASTLOG "/var/adm/lastlog"
#define RECORD_LEN (sizeof(struct lastlog))

int main(int argc, char *argv[]) {

    FILE *fp = NULL;
    struct lastlog record;
    struct lastlog null_record;
    char *name = NULL;
    char *time = NULL;
    struct passwd *pw = NULL;
    int uid = 0;

    if(NULL == (fp = fopen(LASTLOG, "r"))) {
        perror("lastlog");
        return 2;
    }

    memset(&null_record, 0, RECORD_LEN);
    while(1 == fread((void *)&record, RECORD_LEN, (size_t) 1, fp)) {
        if(memcmp((void *)&record, (void *)&null_record, RECORD_LEN)) {
            time = ctime(&record.ll_time);
            time[24] = '\0'; /* Erase newline, ew */
            if((pw = getpwuid((uid_t) uid))) {
                name = pw->pw_name;
                name[8] = '\0'; /* Truncate to 8 characters */
            } else {
                name = "";
            }
            printf("%-5i  %-8s  %s  %-8s  %s\n", uid, name, time, record.ll_line, record.ll_host);
        }
        uid++;
    }

    if(ferror(fp)) {
        fprintf(stderr, "lastlog: read error\n");
        return 1;
    } else {
        (void) fclose(fp); /* Don't care */
        return 0;
    }

}

