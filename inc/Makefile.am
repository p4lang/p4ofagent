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
loci/loci.h

indigo_inc = \
indigo/indigo.h \
indigo/error.h  \
indigo/fi.h     \
indigo/types.h  \
indigo/port_manager.h

nobase_include_HEADERS = \
$(p4ofagent_includes)    \
$(of_cxn_manager_inc)    \
$(of_state_manager_inc)  \
$(of_socket_manager_inc) \
$(loci_inc)              \
$(indigo_inc)            
