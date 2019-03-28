#ifndef NMT_STATE_H
#define NMT_STATE_H

typedef enum {
	NMT_state_initialisation = 0,
	NMT_state_perOperational = 127,
	NMT_state_oprational = 5,
	NMT_state_stopped = 4
} NMT_state;

typedef enum {
	NMT_command_resetNode = 129,
	NMT_command_resetComm = 130,
	NMT_command_enterPreOp = 128,
	NMT_command_stopRemoteNode = 2,
	NMT_command_startRemoteNode = 1
} NMT_command;

#endif
