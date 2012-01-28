MODULE := engines/fitd

MODULE_OBJS := \
	actor_list.o \
	aitd_box.o \
	anim_action.o \
	anim.o \
	cos_table.o \
	debug_font.o \
	debugger.o \
	detection.o \
	eval_var.o \
	file_access.o \
	fitd.o \
	floor.o \
	font.o \
	hqr.o \
	input.o \
	inventory.o \
	life.o \
	life_macro_table.o \
	lines.o \
	main_loop.o \
	main.o \
	music.o \
	object.o \
	osystemSDL.o \
	pak.o \
	polys.o \
	renderer.o \
	room.o \
	save.o \
	screen.o \
	sequence.o \
	startup_menu.o \
	system_menu.o \
	tatou.o \
	thread_code.o \
	time.o \
	track.o \
	unpack.o \
	vars.o \
	version.o \
	video_mode.o \
	zv.o

# This module can be built as a plugin
ifeq ($(ENABLE_FITD), DYNAMIC_PLUGIN)
	PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
  
