######################################################
#lib dll exe
TARGET_TYPE = lib
# Order is important
SUB_TARGETS =
OUTDIR      = ./tmp

INCLUDES_WINDOWS_RELEASE =
INCLUDES_WINDOWS_DEBUG   =

LIBS_WINDOWS_RELEASE =
LIBS_WINDOWS_DEBUG   =

DEFINES_WINDOWS_RELEASE = -DUNICODE -DNDEBUG -D_USE_MATH_DEFINES
DEFINES_WINDOWS_DEBUG   = -DUNICODE -DDEBUG  -D_USE_MATH_DEFINES

INCLUDES_UNIX_RELEASE =
INCLUDES_UNIX_DEBUG   = $(INCLUDES_UNIX_RELEASE)

LIBS_UNIX_RELEASE =
LIBS_UNIX_DEBUG   = $(LIBS_UNIX_RELEASE)

DEFINES_UNIX_RELEASE = -DNDEBUG
DEFINES_UNIX_DEBUG   = -DDEBUG

######################################################

COMMON_VARS = _TARGET_TYPE="$(TARGET_TYPE)" _SUB_TARGETS="$(SUB_TARGETS)" _OUTDIR="$(OUTDIR)"

######################################################

all: $(OS)_$(CONFIG)

windows_release:
	@$(MAKE) -nologo -f ../Makefile.$(OS) $(COMMON_VARS) \
		INCLUDES="$(INCLUDES_WINDOWS_RELEASE)" LIBS="$(LIBS_WINDOWS_RELEASE)" DEFINES="$(DEFINES_WINDOWS_RELEASE)"

windows_debug:
	@$(MAKE) -nologo -f ../Makefile.$(OS) $(COMMON_VARS) \
		INCLUDES="$(INCLUDES_WINDOWS_DEBUG)"   LIBS="$(LIBS_WINDOWS_DEBUG)"   DEFINES="$(DEFINES_WINDOWS_DEBUG)"

unix_release:
	@$(MAKE) --no-print-directory -f ../Makefile.$(OS) $(COMMON_VARS) \
		INCLUDES="$(INCLUDES_UNIX_RELEASE)"    LIBS="$(LIBS_UNIX_RELEASE)"    DEFINES="$(DEFINES_UNIX_RELEASE)"

unix_debug:
	@$(MAKE) --no-print-directory -f ../Makefile.$(OS) $(COMMON_VARS) \
		INCLUDES="$(INCLUDES_UNIX_DEBUG)"      LIBS="$(LIBS_UNIX_DEBUG)"      DEFINES="$(DEFINES_UNIX_DEBUG)"
