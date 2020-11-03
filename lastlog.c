
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LASTLOG "/var/adm/lastlog"
#define RECORD_LEN (sizeof(struct lastlog))

int main(int argc, char *argv[]) {

    const char *path = (argc > 1 ? argv[1] : LASTLOG);
    int fd = open(path, O_RDONLY|O_LARGEFILE);
    if(-1 == fd) {
        perror("lastlog: open()");
        return 2;
    }

    struct lastlog record;
    struct lastlog null_record;
    memset(&null_record, 0, RECORD_LEN);

    uid_t uid = 0;
    ssize_t bytes_read = 0;
    while(RECORD_LEN == (bytes_read = read(fd, (void *)&record, RECORD_LEN))) {
        if(memcmp((void *)&record, (void *)&null_record, RECORD_LEN)) {
            char time[26];
            time_t timestamp = record.ll_time; /* Struct has time32_t */
            if(!ctime_r(&timestamp, time)) {
                perror("lastlog: ctime()");
                return 1;
            }
            time[24] = '\0'; /* Erase newline */
            char *name = "";
            struct passwd *pw = getpwuid((uid_t) uid);
            if(pw) {
                name = pw->pw_name;
                name[8] = '\0'; /* Truncate to 8 characters */
            }
            printf("%-10u  %-8s  %s  %-8s  %s\n", uid, name, time, record.ll_line, record.ll_host);
            uid++;
        } else {
            off_t seeked_to = llseek(fd, (off_t)(uid+1)*RECORD_LEN, SEEK_DATA);
            if(-1 == seeked_to) {
                perror("lastlog: llseek()");
                return 1;
            }
            off_t overran_by = seeked_to % RECORD_LEN;
            if(overran_by) {
                if(-1 == llseek(fd, -overran_by, SEEK_CUR)) {
                    perror("lastlog: llseek()");
                    return 1;
                }
            }
            uid = seeked_to / RECORD_LEN;
        }
    }

    if(-1 == bytes_read) {
        perror("lastlog: read()");
        return 1;
    } else if(0 == bytes_read) {
        (void)close(fd); /* Don't care */
        return 0;
    } else {
        (void)fprintf(stderr, "lastlog: read(): got only %ld/%ld bytes\n", bytes_read, RECORD_LEN);
        return 1;
    }

}
