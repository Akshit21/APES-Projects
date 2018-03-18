#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x7109deb1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xc91c5c6b, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xee626e98, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x5a5ae27, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xf9b3f2c5, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xc9ee7957, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x85a5abae, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x3142a067, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x60dc330a, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xbc477a2, __VMLINUX_SYMBOL_STR(irq_set_irq_type) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0xd3b401ac, __VMLINUX_SYMBOL_STR(fasync_helper) },
	{ 0xa25652e0, __VMLINUX_SYMBOL_STR(kill_fasync) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

