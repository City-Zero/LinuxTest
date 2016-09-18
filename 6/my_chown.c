/*************************************************************************
	> File Name: my_chown.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月20日 星期三 21时46分34秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<errno.h>

int main(int argc ,char **argv)
{
    char user_name[20],group_name[20];
    int fd,i=0;
    char a[2];
    if(argc != 4 ) {
        printf("./my_chown <user name> <group name> <File name>");
        exit(0);
    } else {//search for gid
        fd = open("/etc/group",O_RDONLY);
        while(read(fd,a,1) != 0) {
            if(a[0] == argv[2][i]) {
                i++;
            } else {
                i=0;
                continue;
            }
            if(i != strlen(argv[2])) continue;
            if(i == strlen(argv[2])) {
                read(fd,a,1);
                if(a[0] == ':') {
                    break;
                } else {
                    i=0;
                    continue;
                }
            }
        }
            if(read(fd,a,1) == 0) {
                puts("no group");
                exit(0);
            }
        read(fd,group_name,1);
        read(fd,a,1);
        for(i=0;a[0] != ':';i++) {
            group_name[i] = a[0];
            read(fd,a,1);
        }
        group_name[i] = '\0';
		close(fd);
		//search for uid
		fd = open("/etc/passwd",O_RDONLY);
        while(read(fd,a,1) != 0) {
            if(a[0] == argv[1][i]) {
                i++;
            } else {
                i=0;
                continue;
            }
            if(i != strlen(argv[1])) continue;
            if(i == strlen(argv[1])) {
                read(fd,a,1);
                if(a[0] == ':') {
                    break;
                } else {
                    i=0;
                    continue;
                }
            }
        }
		if(read(fd,a,1) == 0) {
        	printf("no user");
            exit(0);
        }
        read(fd,user_name,1);
        read(fd,a,1);
        for(i=0;a[0] != ':';i++) {
            user_name[i] = a[0];
            read(fd,a,1);
        }
        user_name[i] = '\0';
		close(fd);
    }
    int user_id=0,group_id=0;
    for(i=(strlen(user_name)-1);i >= 0;i--) {
        user_id +=(user_name[i] - 48)*pow(10,strlen(user_name)-1-i);
    }
    for(i=(strlen(group_name)-1);i >= 0;i--) {
        group_id +=(group_name[i] - 48)*pow(10,strlen(group_name)-1-i);
    }
    if(chown(argv[3],user_id,group_id) == -1) {
        perror("chown:");
        exit(0);
    }
    printf("user:%d group:%d",user_id,group_id);
    return 0;
}
