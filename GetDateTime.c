/*
 * Date & Time Function Usage Sample 
 *
 * time_t time(time_t *t)
 * char *asctime(const struct tm * timeptr)
 * char *ctime(const time_t *timep)
 * struct tm *localtime(const time_t *timer)
 * struct tm *gmtime(const time_t *timer)
 * int gettimeofday(struct timeval *tv, struct timezone *tz);
 *
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

/* format: Fri Nov 25 21:06:21 2016 */
void GetDateTime1(time_t sec)
{
    printf("GetDateTime1:\t");
    printf ("%s", ctime(&sec));
}

/* format: Fri Nov 25 21:06:21 2016 */
void GetDateTime2(time_t sec)
{
    printf("GetDateTime2:\t");
    struct tm *dateTime = localtime(&sec);
    printf ("%s", asctime(dateTime));
}

/* format: 2016-11-25 19:11:37 */
void GetDateTime3(time_t sec)
{
    printf("GetDateTime3:\t");
    char timeStr[20] = {0};
    struct tm *dateTime = localtime(&sec);
    sprintf(timeStr, "%d-%02d-%02d %02d:%02d:%02d",
        dateTime->tm_year + 1900,
        dateTime->tm_mon + 1,
        dateTime->tm_mday,
        dateTime->tm_hour,
        dateTime->tm_min,
        dateTime->tm_sec);
    printf("%s\n", timeStr);
}

/* format: 2016-11-25 11:11:37 */
void GetDateTime4(time_t sec)
{
    printf("GetDateTime4:\t");
    char timeStr[20] = {0};
    time_t currentTime = time(NULL);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&sec));
    printf("%s\n", timeStr);
}

int main()
{
    time_t seconds;
    time(&seconds);        /*or use: seconds = time(NULL); */
    printf("Seconds since the Epoch: %ld\n\n", seconds);
    
    GetDateTime1(seconds);
    GetDateTime2(seconds);
    GetDateTime3(seconds);  /* Local time */
    GetDateTime4(seconds);  /* UTC time */
    
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    
    /* seconds == tv.tv_sec */
    printf("Seconds since the Epoch: %ld\n", tv.tv_sec);
/*
    printf("tv.tv_usec: %d\n",tv.tv_usec);
    printf("tz.tz_minuteswest: %d\n", tz.tz_minuteswest);
    printf("tz.tz_dsttime: %d\n", tz.tz_dsttime);
*/
}
