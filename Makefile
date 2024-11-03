# Define the platform and root directories
MTK_PLATFORM := $(subst ",,$(CONFIG_MTK_PLATFORM))
SIGFK_ROOT_DIR := $(src)
SIGFK_COMMON_DIR := $(wildcard $(SIGFK_ROOT_DIR)/common)

# Define the platform-specific directory if available
ifneq ($(MTK_PLATFORM),)
    SIGFK_PLF_DIR := $(wildcard $(SIGFK_ROOT_DIR)/$(MTK_PLATFORM))
else
    SIGFK_PLF_DIR :=
endif

ifeq ($(CONFIG_MODULES),y)

# Check if ftrace and tracing are enabled
ifeq ($(CONFIG_FTRACE),y)
    ifeq ($(CONFIG_TRACING),y)
        FTRACE_READY := y
    endif
endif

# Check if the module should be built
ifeq ($(CONFIG_SIGFK),m)
    SIGFK_BUILD_KO := y
endif

$(info ******** Start to build sigfk for $(MTK_PLATFORM) ********)

# Conditional checks for building the module
ifneq ($(SIGFK_PLF_DIR),)
    ifeq ($(FTRACE_READY),y)
        ifeq ($(SIGFK_BUILD_KO),y)
            include $(SIGFK_COMMON_DIR)/Kbuild
        else
            $(warning Not building sigfk.ko due to CONFIG_SIGFK not set to m)
        endif
    else
        $(warning Not building sigfk.ko due to CONFIG_FTRACE/CONFIG_TRACING not set)
    endif
else
    $(warning not support "$(MTK_PLATFORM)")
endif

else # CONFIG_MODULES = n
 #   $(warning Not building sigfk.ko due to CONFIG_MODULES not set)
endif
