cmake_minimum_required(VERSION 2.6)

project(jk2sp)

set(cpu ${CMAKE_SYSTEM_PROCESSOR})
if (cpu MATCHES "i.86")
	set(cpu "x86")
elseif(cpu STREQUAL "x86_64")
	set(cpu "amd64")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_definitions(-pipe)
	add_definitions(-Wall)
	add_definitions(-g)
	add_definitions(-O2)
	add_definitions(-Wno-unknown-pragmas)
	add_definitions(-Wno-write-strings)
	add_definitions(-Wno-missing-braces)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-Wno-pragmas)
	add_definitions(-fpermissive)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_definitions(-Wno-parentheses-equality)
endif()

add_definitions( -D_M_IX86=1 )	# tested to mean little endian...
add_definitions( -D_IMMERSION_DISABLE )
add_definitions( -DNDEBUG )
add_definitions( -DFINAL_BUILD )

include_directories(/usr/X11R6/include/)
link_directories(/usr/X11R6/lib)

include_directories(/usr/local/include/)
link_directories(/usr/local/lib)

set(src_client
	client/cl_cgame.cpp
	client/cl_cin.cpp
	client/cl_console.cpp
	client/cl_input.cpp
	client/cl_keys.cpp
	client/cl_main.cpp
	client/cl_mp3.cpp
	client/cl_parse.cpp
	client/cl_scrn.cpp
	client/cl_ui.cpp
	client/snd_ambient.cpp
	client/snd_dma.cpp
	client/snd_mem.cpp
	client/snd_mix.cpp
	client/snd_music.cpp
	client/vmachine.cpp
)

set(src_encryption
	encryption/buffer.cpp
	encryption/cpp_interface.cpp
	encryption/sockets.cpp
)

set(src_ff
	ff/cl_ff.cpp
	ff/ff.cpp
	ff/ff_ChannelSet.cpp
	ff/ff_ConfigParser.cpp
	ff/ff_HandleTable.cpp
	ff/ff_MultiCompound.cpp
	ff/ff_MultiEffect.cpp
	ff/ff_MultiSet.cpp
	ff/ff_ffset.cpp
	ff/ff_snd.cpp
	ff/ff_system.cpp
	ff/ff_utils.cpp
)

set(src_ghoul2
	ghoul2/G2_API.cpp
	ghoul2/G2_bolts.cpp
	ghoul2/G2_bones.cpp
	ghoul2/G2_misc.cpp
	ghoul2/G2_surfaces.cpp
)

set(src_mp3code
	mp3code/cdct.c
	mp3code/csbt.c
	mp3code/csbtL3.c
	mp3code/csbtb.c
	mp3code/cup.c
	mp3code/cupL1.c
	mp3code/cupini.c
	mp3code/cupl3.c
	mp3code/cwin.c
	mp3code/cwinb.c
	mp3code/cwinm.c
	mp3code/hwin.c
	mp3code/l3dq.c
	mp3code/l3init.c
	mp3code/mdct.c
	mp3code/mhead.c
	mp3code/msis.c
	mp3code/towave.c
	mp3code/uph.c
	mp3code/upsf.c
	mp3code/wavep.c
)

set(src_qcommon
	qcommon/cm_load.cpp
	qcommon/cm_patch.cpp
	qcommon/cm_polylib.cpp
	qcommon/cm_test.cpp
	qcommon/cm_trace.cpp
	qcommon/cmd.cpp
	qcommon/common.cpp
	qcommon/cvar.cpp
	qcommon/files.cpp
	qcommon/hstring.cpp
	qcommon/md4.cpp
	qcommon/msg.cpp
	qcommon/net_chan.cpp
	qcommon/strip.cpp
	qcommon/unzip.cpp
)

set(src_renderer
	renderer/matcomp.c
	renderer/tr_worldeffects.cpp
	renderer/tr_animation.cpp
	renderer/tr_backend.cpp
	renderer/tr_bsp.cpp
	renderer/tr_cmds.cpp
	renderer/tr_curve.cpp
	renderer/tr_draw.cpp
	renderer/tr_font.cpp
	renderer/tr_ghoul2.cpp
	renderer/tr_image.cpp
	renderer/tr_init.cpp
	renderer/tr_jpeg_interface.cpp
	renderer/tr_light.cpp
	renderer/tr_main.cpp
	renderer/tr_marks.cpp
	renderer/tr_mesh.cpp
	renderer/tr_model.cpp
	renderer/tr_noise.cpp
	renderer/tr_quicksprite.cpp
	renderer/tr_scene.cpp
	renderer/tr_shade.cpp
	renderer/tr_shade_calc.cpp
	renderer/tr_shader.cpp
	renderer/tr_shadows.cpp
	renderer/tr_sky.cpp
	renderer/tr_stl.cpp
	renderer/tr_surface.cpp
	renderer/tr_surfacesprites.cpp
	renderer/tr_world.cpp
)

set(src_server
	server/exe_headers.cpp
	server/sv_ccmds.cpp
	server/sv_client.cpp
	server/sv_game.cpp
	server/sv_init.cpp
	server/sv_main.cpp
	server/sv_savegame.cpp
	server/sv_snapshot.cpp
	server/sv_world.cpp
)

set(src_smartheap
	smartheap/HEAPAGNT.H
	smartheap/SMRTHEAP.C
	smartheap/SMRTHEAP.H
)

set(src_ui
	ui/ui_atoms.cpp
	ui/ui_connect.cpp
	ui/ui_debug.cpp
	ui/ui_main.cpp
	ui/ui_shared.cpp
	ui/ui_syscalls.cpp
)

set(src_win32
	win32/FeelIt/fffx.cpp
	win32/FeelIt/fffx_feel.cpp
	win32/win_gamma.cpp
	win32/win_glimp.cpp
	win32/win_input.cpp
	win32/win_main.cpp
	win32/win_qgl.cpp
	win32/win_shared.cpp
	win32/win_snd.cpp
	win32/win_syscon.cpp
	win32/win_video.cpp
	win32/win_wndproc.cpp
)

set(src_main_game
	game/q_math.cpp
	game/q_shared.cpp
	game/genericparser2.cpp
)

set(src_jpeg
	jpeg-6/jcapimin.cpp
	jpeg-6/jccoefct.cpp
	jpeg-6/jccolor.cpp
	jpeg-6/jcdctmgr.cpp
	jpeg-6/jchuff.cpp
	jpeg-6/jcinit.cpp
	jpeg-6/jcmainct.cpp
	jpeg-6/jcmarker.cpp
	jpeg-6/jcmaster.cpp
	jpeg-6/jcomapi.cpp
	jpeg-6/jcparam.cpp
	jpeg-6/jcphuff.cpp
	jpeg-6/jcprepct.cpp
	jpeg-6/jcsample.cpp
	jpeg-6/jctrans.cpp
	jpeg-6/jdapimin.cpp
	jpeg-6/jdapistd.cpp
	jpeg-6/jdatadst.cpp
	jpeg-6/jdatasrc.cpp
	jpeg-6/jdcoefct.cpp
	jpeg-6/jdcolor.cpp
	jpeg-6/jddctmgr.cpp
	jpeg-6/jdhuff.cpp
	jpeg-6/jdinput.cpp
	jpeg-6/jdmainct.cpp
	jpeg-6/jdmarker.cpp
	jpeg-6/jdmaster.cpp
	jpeg-6/jdpostct.cpp
	jpeg-6/jdsample.cpp
	jpeg-6/jdtrans.cpp
	jpeg-6/jerror.cpp
	jpeg-6/jfdctflt.cpp
	jpeg-6/jidctflt.cpp
	jpeg-6/jmemmgr.cpp
	jpeg-6/jmemnobs.cpp
	jpeg-6/jutils.cpp
)

set(src_unix
	unix/unix_main.cpp
	unix/unix_shared.cpp
	unix/linux_glimp.cpp
	unix/linux_qgl.cpp
	null/null_snddma.cpp
)

set(src_starwars
	${src_ff}
	${src_client}
	${src_encryption}
	${src_ghoul2}
	${src_mp3code}
	${src_qcommon}
	${src_renderer}
	${src_server}
	${src_ui}
	${src_main_game}
	${src_jpeg}
	${src_unix}
)

add_executable(jk2sp
	${src_starwars}
)

set_target_properties(jk2sp PROPERTIES COMPILE_DEFINITIONS "_JK2EXE;_FF_DISABLE")

target_link_libraries(jk2sp
	m pthread
	X11 Xxf86vm Xxf86dga
	openal
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(jk2sp dl)
endif()

set(src_icarus
	icarus/BlockStream.cpp
	icarus/Instance.cpp
	icarus/Sequence.cpp
	icarus/Sequencer.cpp
	icarus/TaskManager.cpp
)

set(src_cgame
	cgame/FX_ATSTMain.cpp
	cgame/FX_Blaster.cpp
	cgame/FX_Bowcaster.cpp
	cgame/FX_BryarPistol.cpp
	cgame/FX_DEMP2.cpp
	cgame/FX_Disruptor.cpp
	cgame/FX_Emplaced.cpp
	cgame/FX_Flechette.cpp
	cgame/FX_HeavyRepeater.cpp
	cgame/FX_RocketLauncher.cpp
	cgame/FxParsing.cpp
	cgame/FxPrimitives.cpp
	cgame/FxScheduler.cpp
	cgame/FxSystem.cpp
	cgame/FxTemplate.cpp
	cgame/FxUtil.cpp
	cgame/cg_camera.cpp
	cgame/cg_consolecmds.cpp
	cgame/cg_credits.cpp
	cgame/cg_draw.cpp
	cgame/cg_drawtools.cpp
	cgame/cg_effects.cpp
	cgame/cg_ents.cpp
	cgame/cg_event.cpp
	cgame/cg_headers.cpp
	cgame/cg_info.cpp
	cgame/cg_lights.cpp
	cgame/cg_localents.cpp
	cgame/cg_main.cpp
	cgame/cg_marks.cpp
	cgame/cg_players.cpp
	cgame/cg_playerstate.cpp
	cgame/cg_predict.cpp
	cgame/cg_scoreboard.cpp
	cgame/cg_servercmds.cpp
	cgame/cg_snapshot.cpp
	cgame/cg_syscalls.cpp
	cgame/cg_text.cpp
	cgame/cg_view.cpp
	cgame/cg_weapons.cpp
)

set(src_game 
	game/AI_Atst.cpp
	game/AI_Default.cpp
	game/AI_Droid.cpp
	game/AI_GalakMech.cpp
	game/AI_Grenadier.cpp
	game/AI_Howler.cpp
	game/AI_ImperialProbe.cpp
	game/AI_Interrogator.cpp
	game/AI_Jedi.cpp
	game/AI_Mark1.cpp
	game/AI_Mark2.cpp
	game/AI_MineMonster.cpp
	game/AI_Remote.cpp
	game/AI_Seeker.cpp
	game/AI_Sentry.cpp
	game/AI_Sniper.cpp
	game/AI_Stormtrooper.cpp
	game/AI_Utils.cpp
	game/G_Timer.cpp
	game/NPC.cpp
	game/NPC_behavior.cpp
	game/NPC_combat.cpp
	game/NPC_goal.cpp
	game/NPC_misc.cpp
	game/NPC_move.cpp
	game/NPC_reactions.cpp
	game/NPC_senses.cpp
	game/NPC_sounds.cpp
	game/NPC_spawn.cpp
	game/NPC_stats.cpp
	game/NPC_utils.cpp
	game/Q3_Interface.cpp
	game/Q3_Registers.cpp
	game/bg_lib.cpp
	game/bg_misc.cpp
	game/bg_pangles.cpp
	game/bg_panimate.cpp
	game/bg_pmove.cpp
	game/bg_slidemove.cpp
	game/g_ICARUS.cpp
	game/g_active.cpp
	game/g_breakable.cpp
	game/g_camera.cpp
	game/g_client.cpp
	game/g_cmds.cpp
	game/g_combat.cpp
	game/g_functions.cpp
	game/g_fx.cpp
	game/g_headers.cpp
	game/g_inventory.cpp
	game/g_itemLoad.cpp
	game/g_items.cpp
	game/g_main.cpp
	game/g_mem.cpp
	game/g_misc.cpp
	game/g_misc_model.cpp
	game/g_missile.cpp
	game/g_mover.cpp
	game/g_nav.cpp
	game/g_navigator.cpp
	game/g_navnew.cpp
	game/g_object.cpp
	game/g_objectives.cpp
	game/g_ref.cpp
	game/g_roff.cpp
	game/g_savegame.cpp
	game/g_session.cpp
	game/g_spawn.cpp
	game/g_svcmds.cpp
	game/g_target.cpp
	game/g_trigger.cpp
	game/g_turret.cpp
	game/g_usable.cpp
	game/g_utils.cpp
	game/g_weapon.cpp
	game/g_weaponLoad.cpp
	game/game.def
	game/genericparser2.cpp
	game/q_math.cpp
	game/q_shared.cpp
	game/wp_saber.cpp
	qcommon/tri_coll_test.cpp
	smartheap/SMRTHEAP.C
	ui/gameinfo.cpp
)

add_library(jk2game${cpu} SHARED
	${src_icarus}
	${src_cgame}
	${src_game}
)

SET_TARGET_PROPERTIES(jk2game${cpu} PROPERTIES PREFIX "")