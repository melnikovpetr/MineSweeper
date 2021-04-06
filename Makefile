##############################################################
# Defaults
OS      = windows
CONFIG  = release

##############################################################

help:
	@echo For running specify command and set target, os and config, please.
	@echo Usage: MAKE COMMAND TARGET="Target" OS="Operating system" CONFIG="config"
	@echo ---
	@echo Available commands are:
	@echo 1} build
	@echo 2} clean
	@echo ---
	@echo Target - project name { sub-directory in the current directory }
	@echo ---
	@echo Available OSs are:
	@echo 1} windows {default, MS Windows and Visual C++}
	@echo 2} unix {Unix-like and GNU GCC}
	@echo ---
	@echo Available configs are:
	@echo 1} release {default}
	@echo 2} debug

RECIPE = \
	@cd "./$(TARGET)" && \
	echo --- && echo Executing of '$@' on '$(TARGET)' started... && echo --- && \
	$(MAKE) COMMAND="$@" TARGET="$(TARGET)" OS="$(OS)" CONFIG="$(CONFIG)" && \
	echo --- && echo Executing of '$@' on '$(TARGET)' has finished. && echo --- && \
	cd ..

build:
	$(RECIPE)

clean:
	$(RECIPE)
