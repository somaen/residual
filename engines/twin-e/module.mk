MODULE := engines/twin-e

MODULE_OBJS := \
	actors.o \
	actorScript.o \
	angle.o \
	angleTable.o \
	animAction.o \
	anim.o \
	body.o \
	comportementMenu.o \
	costume.o \
	cube.o \
	extra.o \
	fichePerso.o \
	fireEffect.o \
	fla.o \
	font.o \
	fullRedraw.o \
	hqr.o \
	images.o \
	inventory.o \
	script.o \
	main.o \
	mainLoop.o \
	mainMenu.o \
	mainSDL.o \
	moveActor.o \
	music.o \
	overlay.o \
	renderer.o \
	room.o \
	samples.o \
	save.o \
	shadow.o \
	streamReader.o \
	text.o \
	vox.o \
	zones.o


# This module can be built as a plugin
ifeq ($(ENABLE_TWINE), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

