ifdef ENABLE_GRIM
DEFINES += -DENABLE_GRIM=$(ENABLE_GRIM)
MODULES += engines/grim
endif

ifdef ENABLE_MYST3
DEFINES += -DENABLE_MYST3=$(ENABLE_MYST3)
MODULES += engines/myst3
endif

ifdef ENABLE_FITD
DEFINES += -DENABLE_FITD=$(ENABLE_FITD)
MODULES += engines/fitd
endif
