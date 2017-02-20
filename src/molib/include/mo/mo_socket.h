//===============================================================================
// Copyright (c) 2005-2017 by Made to Order Software Corporation
// 
// All Rights Reserved.
// 
// The source code in this file ("Source Code") is provided by Made to Order Software Corporation
// to you under the terms of the GNU General Public License, version 2.0
// ("GPL").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution.
// 
// By copying, modifying or distributing this software, you acknowledge
// that you have read and understood your obligations described above,
// and agree to abide by those obligations.
// 
// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED "AS IS." THE AUTHOR MAKES NO
// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
// COMPLETENESS OR PERFORMANCE.
//===============================================================================



#ifndef MO_SOCKET_H
#define	MO_SOCKET_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_VARIABLE_H
#include	"mo_variable.h"
#endif
#ifndef MO_CRYPT_H
#include	"mo_crypt.h"
#endif
#ifndef MO_BUFFER_H
#include	"mo_buffer.h"
#endif
#if defined(_MSC_VER)
//#include	<winsock2.h>
typedef int	socklen_t;
#else
#if defined(WIN32)
//#include	<winsock2.h> -- ws2tcpip includes winsock2.h
#include	<ws2tcpip.h>
#endif
#endif


namespace molib
{

// The socket information are saved in a file of variables
// which can be read with the moVariable.Load() function.

class MO_DLL_EXPORT moSocket : public moBase
{
public:
#ifdef WIN32
	typedef SOCKET		socket_t;
#else
	typedef int		socket_t;
#endif
	static const socket_t	SOCKET_UNDEFINED = (socket_t) -1;

				moSocket(const moWCString& filename, const moWCString& host, int port);
	virtual			~moSocket();
	virtual const char *	moGetClassName(void) const;

	int			FileDescriptor(void) const;
	int			Wait(long sec = 0, long usec = 0) const;

protected:
	moVariableList		f_params;	// parameters read from the specified filename on creation
	int			f_type;		// type of socket (such as SOCK_STREAM)
	int			f_protocol;	// protocol of the socket
	struct sockaddr_in	f_host;		// the host address (note: when multiple addresses are
						// available then one socket needs to be created per
						// address)
	int			f_length;	// size of the address (usually 4 at this time)
	socket_t		f_socket;	// socket number when the constructor succeeded

private:
	void			Init(const moWCString& filename, int port);
	void			Type(void);
	void			Protocol(void);
	void			Family(void);
	void			Port(void);
	void			Host(const moWCString& host);
	void			Socket(void);
};





/** \brief Return the socket file descriptor
 *
 * This function returns the file descriptor of the socket defined
 * in the moSocket object.
 *
 * This file descriptor can be used for calls to select() or poll().
 *
 * \note
 * The object creation fails if the socket cannot be created and
 * initialized properly. Thus, this function always returns a
 * valid file descriptor.
 *
 * \return The socket file descriptor
 */
inline int moSocket::FileDescriptor(void) const
{
	return static_cast<int>(f_socket);
}








class MO_DLL_EXPORT moSocketConnection : public moSocket
{
public:
				moSocketConnection(const moWCString& filename, const moWCString& host, int port = 0, bool server = false);
	virtual			~moSocketConnection();
	virtual const char *	moGetClassName(void) const;

	void			SetEncryption(moCryptBase *crypt, unsigned long input_size, unsigned long output_size);

	long			Receive(void *buffer, unsigned long size);
	long			Send(const void *buffer, unsigned long size);

protected:
	bool			f_disconnected;
	moCryptBaseSPtr		f_crypt;	// an object used to encrypt/decrypt data going through this connection
	unsigned long		f_input_pos;	// current position in input buffer
	unsigned long		f_output_pos;	// current position in output buffer
	moBuffer		f_input;	// the input buffer
	moBuffer		f_output;	// the output buffer
	unsigned long		f_decoded_size;	// number of bytes decoded from the last input
	unsigned long		f_decoded_pos;	// position within the decoded 
	unsigned char *		f_decoded;	// buffer of decoded bytes from last input

private:
	void			InitConnection(void);
	void			Connect(void);
};








#define	MO_SERVER_MAX_CHILDREN		20	// default maximum number of children (connection) accepted on this server

class MO_DLL_EXPORT moServer : public moSocketConnection
{
public:
				moServer(const moWCString& filename, const moWCString& host, int port = 0);
	virtual			~moServer();
	virtual const char *	moGetClassName(void) const;

	int			Accept(void);
	void			Loop(void);

protected:
	virtual int		Start(int fd) = 0;
	pid_t			Fork(void);

	moSocket::socket_t	f_connection;	// the connection socket (file descriptor)
	struct sockaddr_in	f_addr_connect;	// connection address
	socklen_t		f_addr_length;	// length of the address

private:
	void			InitServer(void);
	void			Children(void);
	void			Flags(void);
	void			Listen(void);
	int			FreeChild(void);

	unsigned long		f_count;	// the number of children (can be defined by the end user)
	unsigned long		f_free_child;	// index to check for a free child entry
	pid_t *			f_children;	// an array of connected children
	bool			f_fork;		// do fork whenever a connection is accepted
};




/************************************************************ DOC:

CLASS

	moServer

NAME

	protected:
	Start - called whenever a connection is made

SYNOPSIS

	virtual int Start(int fd) = 0;

PARAMETERS

	fd - the file descriptor which represents the connection

DESCRIPTION

	This function is a pure virtual function which is called each
	time a new connection is made.

	New connections are made in the Accept() function.

	The fd parameter is the file descriptor connection. It is a
	read/write socket which has no specific setup other than being
	connected and of the same type as the server socket (same
	protocol, family, etc.).

	The address of the connection can be checked within the
	f_addr_connect. The length of the address is saved in
	the f_addr_length variable member. These variables are the
	responsability of the Start() function. It should be copied
	if it is to be used in order to avoid any problems.
	
	When a child process is started, then the connection descriptor
	is copied in the f_socket field of the child. Thus, the child
	can be seen as an moSocketConnection object (i.e. the Receive()
	and Send() functions can be called safely).

	The socket will be closed by the moServer object whenever the
	Start() function returns. It should never be closed by the
	user.

	The returned value is used in the exit() call (that's when the
	Start() was called by a child process). In any event, if it
	isn't 0 it represents an error. When no child process is created
	then the Start() function is expected to set the errno variable
	to a valid error if it is not to return 0.

RETURNED VALUE

	-1	an error occured
	0	no error occured - possibly a connection was successfully made

	The error is found in errno and can be any error that accept(2),
	fork(2), the Start() virtual function return, or ENOENT when too
	many children are already running (too many connections are already
	running...)

SEE ALSO

	Accept

*/





};			// namespace molib;

// vim: ts=8
#endif	// #ifndef MO_SOCKET_H

