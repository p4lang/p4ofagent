lib_LIBRARIES = libp4ofagent.a

libp4ofagent_a_CFLAGS = -I$(srcdir)/inc
libp4ofagent_a_CFLAGS += -I inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/indigo/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/submodules/loxigen-artifacts/loci/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/submodules/infra/modules/AIM/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/OFConnectionManager/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/SocketManager/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/OFStateManager/module/inc
libp4ofagent_a_CFLAGS += -I/usr/include/libnl3

libp4ofagent_a_SOURCES =           \
inc/p4ofagent/callbacks.h          \
inc/p4ofagent/openflow-spec1.3.0.h \
inc/p4ofagent/p4ofagent.h          \
inc/p4ofagent/parse.h              \
src/cpu_packet.h                   \
src/state.h                        \
src/callbacks.c                    \
src/cpu_packet.c                   \
src/ind_fwd.c                      \
src/ind_ports.c                    \
src/p4ofagent.c                    \
src/parse.c                        \
src/state.c

p4ofagent : all
	make -f indigo-lib.mk indigo-lib
	ar -M <libp4ofagent.mri
