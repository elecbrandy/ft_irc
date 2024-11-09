#ifndef SERVERCODE_HPP
#define SERVERCODE_HPP

/* COUT LOG */
#define LOG_SERVER_INIT			"IRC Server INIT! : port "
#define LOG_NEW_CONNECTED		"new client connected : fd "
#define LOG_END_CONNECTED		"client connection closed : fd "
#define LOG_CLINET_INPUT		"client input received : "
#define LOG_CONNECTION_CHECK	"checking client connections..."
#define LOG_CONNECTION_TIMEOUT	"client connection timed out"
#define LOG_SERVER_SHUTDOWN		"server Closed"

/* !ERROR! */
#define	ERR_ARG_COUNT			"invalid argument count. (only 3)"
#define	ERR_PORT_NULL			"port number is NULL or empty."
#define	ERR_PORT_DIGIT			"port number can only contain numbers."
#define	ERR_PORT_RANGE			"port number is out of valid range."
#define	ERR_PASSWORD_NULL		"password is NULL or empty."
#define ERR_PASSWORD_SIZE		"password must be less than 10 characters."
#define	ERR_PASSWORD_ALNUM		"password can only contain alphabet or number."

#define	ERR_SOCKET_CREATION	    "socket creation error."
#define	ERR_SOCKET_OPTIONS		"socket options setting error."
#define	ERR_SOCKET_BIND			"socket binding error."
#define	ERR_SOCKET_LISTEN		"socket listening error."
#define	ERR_SET_NONBLOCKING		"failed to set socket to non-blocking mode."
#define	ERR_ACCEPT_CLIENT		"client acceptance error."
#define	ERR_CLIENT_NONBLOCKING	"failed to set client socket to non-blocking mode."
#define	ERR_DATA_RECEIVE		"data reception error."
#define	ERR_SEND				"send() error."
#define	ERR_POLL				"poll() error."
#define ERR_RECV				"recv() error."
#define	ERR_ETC					"unknown error occurred."

#endif