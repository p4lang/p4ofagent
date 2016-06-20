ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

p4ofagent_includes = \
p4ofagent/callbacks.h          \
p4ofagent/openflow-spec1.3.0.h \
p4ofagent/p4ofagent.h          \
p4ofagent/parse.h              

p4ofagent_headers = \
$(p4ofagent_includes)

# extra includes from indigo
of_cxn_manager_inc = \
OFConnectionManager/ofconnectionmanager.h

of_state_manager_inc = \
OFStateManager/ofstatemanager.h

of_socket_manager_inc = \
SocketManager/socketmanager.h

loci_inc = \
loci/loci.h         \
loci/loci_base.h    \
loci/loci_idents.h  \
loci/of_message.h   \
loci/of_buffer.h    \
loci/of_match.h     \
loci/of_object.h    \
loci/of_wire_buf.h  \
loci/loci_classes.h \
loci/loci_class_metadata.h

indigo_inc = \
indigo/indigo.h                \
indigo/error.h                 \
indigo/fi.h                    \
indigo/types.h                 \
indigo/port_manager.h          \
indigo/memory.h                \
indigo/time.h                  \
indigo/assert.h                \
indigo/of_state_manager.h      \
indigo/of_connection_manager.h \
indigo/forwarding.h

aim_inc = \
AIM/aim.h            \
AIM/aim_config.h     \
AIM/aim_porting.h    \
AIM/aim_pvs.h        \
AIM/aim_valist.h     \
AIM/aim_object.h     \
AIM/aim_log_util.h   \
AIM/aim_map.h        \
AIM/aim_pvs_file.h   \
AIM/aim_pvs_buffer.h \
AIM/aim_utils.h      \
AIM/aim_string.h     \
AIM/aim_valgrind.h   \
AIM/aim_datatypes.h  \
AIM/aim_list.h       \
AIM/aim_printf.h     \
AIM/aim_sparse.h     \
AIM/aim_error.h      \
AIM/aim_compiler.h   \
AIM/aim_bitmap.h     \
AIM/aim_daemon.h     \
AIM/aim_memory.h

nobase_include_HEADERS = \
$(p4ofagent_includes)    \
$(of_cxn_manager_inc)    \
$(of_state_manager_inc)  \
$(of_socket_manager_inc) \
$(loci_inc)              \
$(indigo_inc)            \
$(aim_inc)
