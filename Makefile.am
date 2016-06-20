lib_LIBRARIES = libp4ofagent.a

SUBDIRS = inc

libp4ofagent_a_CFLAGS = -I$(srcdir)/inc
libp4ofagent_a_CFLAGS += -I inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/indigo/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/submodules/loxigen-artifacts/loci/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/submodules/infra/modules/AIM/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/OFConnectionManager/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/SocketManager/module/inc
libp4ofagent_a_CFLAGS += -Isubmodules/indigo/modules/OFStateManager/module/inc
libp4ofagent_a_CFLAGS += -I/usr/include/libnl3

libp4ofagent_a_CFLAGS += -lrt

p4ofagent_includes = \
inc/p4ofagent/callbacks.h          \
inc/p4ofagent/openflow-spec1.3.0.h \
inc/p4ofagent/p4ofagent.h          \
inc/p4ofagent/parse.h              

p4ofagent_headers = \
src/state.h                        \
src/cpu_packet.h                   \
$(p4ofagent_includes)

p4ofagent_sources = \
src/callbacks.c                    \
src/cpu_packet.c                   \
src/ind_fwd.c                      \
src/ind_ports.c                    \
src/p4ofagent.c                    \
src/parse.c                        \
src/state.c                        

libp4ofagent_a_SOURCES = $(p4ofagent_headers) $(p4ofagent_sources)

nobase_include_HEADERS = dependmodules.x

indigo-lib:
	make -f indigo-lib.mk indigo-lib
	find submodules -name inc -exec sh -c "cp -r {}/* inc" \;

p4ofagent: indigo-lib all 
	ar -M <libp4ofagent.mri
