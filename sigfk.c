// sigfk.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

static int *module_sig_enforce_ptr = NULL;
static struct file *log_file = NULL;
static const char *log_path = "/sdcard/sigfk_log.txt";

/*
 * Write log message to both kernel log and /sdcard log file
 */
static void write_log(const char *fmt, ...)
{
    va_list args;
    char log_buffer[256];
    int len;

    va_start(args, fmt);
    len = vsnprintf(log_buffer, sizeof(log_buffer), fmt, args);
    va_end(args);

    // Log to kernel messages (dmesg)
    pr_info("%s", log_buffer);

    // Log to file on /sdcard if available
    if (log_file) {
        kernel_write(log_file, log_buffer, len, &log_file->f_pos);
        kernel_write(log_file, "\n", 1, &log_file->f_pos);
    }
}

/*
 * Module Initialization
 * Attempts to locate the 'module_sig_enforce' symbol and disable module
 * signature verification by setting its value to 0.
 */
static int __init sigfk_init(void)
{
    mm_segment_t old_fs;

    // Open log file on /sdcard
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    log_file = filp_open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    set_fs(old_fs);

    if (IS_ERR(log_file)) {
        pr_err("sigfk: Unable to open log file %s\n", log_path);
        log_file = NULL;
    }

    write_log("sigfk: Initializing module to disable module signature verification");

    // Locate the address of 'module_sig_enforce' in the kernel
    module_sig_enforce_ptr = (int *)kallsyms_lookup_name("module_sig_enforce");

    if (module_sig_enforce_ptr) {
        write_log("sigfk: Located 'module_sig_enforce' at address %px", module_sig_enforce_ptr);

        // Disable module signature enforcement
        *module_sig_enforce_ptr = 0;
        write_log("sigfk: Module signature verification has been successfully disabled");
    } else {
        write_log("sigfk: Unable to locate 'module_sig_enforce'. Disabling signature verification is not possible");
        return -ENOSYS; // Return error if symbol not found
    }

    return 0; // Initialization successful
}

/*
 * Module Exit
 * Simply logs the module's exit and closes the log file.
 */
static void __exit sigfk_exit(void)
{
    write_log("sigfk: Exiting module.");

    // Close log file if it was opened
    if (log_file) {
        filp_close(log_file, NULL);
        log_file = NULL;
    }
}

module_init(sigfk_init);
module_exit(sigfk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Badmaneers");
MODULE_DESCRIPTION("sigfk: Disable module signature verification and log to /sdcard.");
MODULE_VERSION("1.0");
