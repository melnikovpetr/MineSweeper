######################################################
#lib dll exe
TARGET_TYPE = exe
# Order is important
SUB_TARGETS = MineSweeper Common
OUTDIR      = ./tmp

WINDOWS_QT_DIR = "E:/Qt/5.15.0/msvc2015_64"

INCLUDES_WINDOWS_RELEASE = -I$(WINDOWS_QT_DIR)"/include"
INCLUDES_WINDOWS_DEBUG   = -I$(WINDOWS_QT_DIR)"/include"

LIBS_WINDOWS_RELEASE = -libpath:$(WINDOWS_QT_DIR)"/lib" shell32.lib qtmain.lib Qt5Core.lib Qt5Gui.lib Qt5Widgets.lib
LIBS_WINDOWS_DEBUG   = -libpath:$(WINDOWS_QT_DIR)"/lib" shell32.lib qtmaind.lib Qt5Cored.lib Qt5Guid.lib Qt5Widgetsd.lib

DEFINES_WINDOWS_RELEASE = -DUNICODE -DNDEBUG -D_USE_MATH_DEFINES
DEFINES_WINDOWS_DEBUG   = -DUNICODE -DDEBUG  -D_USE_MATH_DEFINES

INCLUDES_UNIX_RELEASE = -isystem"/usr/include/x86_64-linux-gnu/qt5"
INCLUDES_UNIX_DEBUG   = $(INCLUDES_UNIX_RELEASE)

LIBS_UNIX_RELEASE = -L/usr/lib/x86_64-linux-gnu/ -lQt5Widgets -lQt5Gui -lQt5Core -lpthread
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
