//===============================================================================
// Copyright (c) 2005-2013 by Made to Order Software Corporation
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



#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_socket.h"
#endif

#include	"mo/mo_socket.h"

// Win32 does not define this flag
#ifndef MSG_NOSIGNAL
#define	MSG_NOSIGNAL 0
#endif
#ifdef WIN32
#define	RECV_BUFFER_TYPE	char*
#define	SEND_BUFFER_TYPE	const char*
#else
#define	RECV_BUFFER_TYPE	void*
#define	SEND_BUFFER_TYPE	const void*
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#ifdef MO_WIN32
#include <io.h>
#endif


namespace molib
{



// documented in base.c++
const char *moSocket::moGetClassName() const
{
	return "molib::moBase::moSocket";
}
const char *moSocketConnection::moGetClassName() const
{
	return "molib::moBase::moSocket::moSocketConnection";
}
const char *moServer::moGetClassName() const
{
	return "molib::moBase::moSocket::moSocketConnection::moServer";
}




/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Init - initialize a socket object

SYNOPSIS

	void Init(const moWCString& filename, int port);

DESCRIPTION

	This function is called by the socket constructor to initialize
	the different parameters present in that object.

SEE ALSO

	Constructor

*/
void moSocket::Init(const moWCString& filename, int port)
{
	f_type = SOCK_STREAM;			// type of socket (such as SOCK_STREAM)
	f_protocol = IPPROTO_TCP;		// protocol of the socket
	memset(&f_host, 0, sizeof(f_host));	// ensure the sin_pad/zero is clear
	f_host.sin_port = htons(port);		// a port number
	f_host.sin_family = AF_INET;		// the socket family (such as AF_INET)
	f_host.sin_addr.s_addr = 0;		// the main address, extra addresses are saved in the following list
	f_length = 0;				// no host defined
	f_params.Load(filename);		// load the user parameters
	f_socket = SOCKET_UNDEFINED;		// no socket open yet

#ifdef WIN32
	WSADATA ignore;
	WSAStartup(0x0202, &ignore);
	// TODO: add a throw on failure
#endif
}



/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Type - determine the type of a socket

SYNOPSIS

	void Type(void);

DESCRIPTION

	This function is called by the constructor to determine the
	type which will be used when the socket is created.

	The type will usually be defined in the list of parameters
	as TYPE=<value> where <value> is any of the names as found
	after a #define SOCK_...

	The default is:

		TYPE=STREAM

	The currently understood types are:

		STREAM
		DGRAM
		RAW
		RDM
		SEQPACKET
		PACKET

	and there integer counter part.

ERRORS

	MO_ERROR_INVALID	an unknown type was specified

SEE ALSO

	Constructor

*/
void moSocket::Type(void)
{
	moVariable	*v;
	long		r;

	v = f_params.Get("TYPE");
	if(v == NULL) {
		// keep the default
		return;
	}

	if(moWCString(*v) == "STREAM") {
		f_type = SOCK_STREAM;
		return;
	}

	if(moWCString(*v) == "DGRAM") {
		f_type = SOCK_DGRAM;
		return;
	}

	if(moWCString(*v) == "RAW") {
		f_type = SOCK_RAW;
		return;
	}

	if(moWCString(*v) == "RDM") {
		f_type = SOCK_RDM;
		return;
	}

	if(moWCString(*v) == "SEQPACKET") {
		f_type = SOCK_SEQPACKET;
		return;
	}

#ifdef LINUX
	if(moWCString(*v) == "PACKET") {
		f_type = SOCK_PACKET;
		return;
	}
#endif

	r = v->Integer();
	if(errno != 0) {
		r = -1;
	}

	switch(r) {
	case SOCK_STREAM:
	case SOCK_DGRAM:
	case SOCK_RAW:
	case SOCK_RDM:
	case SOCK_SEQPACKET:
#ifdef LINUX
	case SOCK_PACKET:
#endif
		f_type = r;
		return;

	default:
		throw moError(MO_ERROR_INVALID, "type not understood by socket");

	}
}


/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Protocol - determine the protocol of a socket

SYNOPSIS

	void Protocol(void);

DESCRIPTION

	This function is called by the constructor to determine the
	protocol which will be used when the socket is created.

	The protocol will usually be defined in the list of parameters
	as PROTOCOL=<value> where <value> is any of the names as found
	after a #define IPPROTO_...

	The default is:

		PROTOCOL=TCP

	The currently understood types are:

		TCP
		UDP
		ICMP

	and there integer counter part plus the integer values of many
	other protocols understood by Linux.

ERRORS

	MO_ERROR_INVALID	an unknown protocol was specified

SEE ALSO

	Constructor

*/
void moSocket::Protocol(void)
{
	moVariable	*v;
	long		r;

	v = f_params.Get("PROTOCOL");
	if(v == NULL) {
		// keep the current protocol
		return;
	}

	if(moWCString(*v) == "TCP") {
		f_protocol = IPPROTO_TCP;
		return;
	}

	if(moWCString(*v) == "UDP") {
		f_protocol = IPPROTO_UDP;
		return;
	}

	if(moWCString(*v) == "ICMP") {
		f_protocol = IPPROTO_ICMP;
		return;
	}

	r = v->Integer();
	if(errno != 0) {
		r = -1;		/* an invalid protocol */
	}

	switch(r) {
	case IPPROTO_IP:
	case IPPROTO_ICMP:
	case IPPROTO_IGMP:
	case IPPROTO_TCP:
	case IPPROTO_PUP:
	case IPPROTO_UDP:
	case IPPROTO_IDP:
#ifndef _MSC_VER
	case IPPROTO_IPV6:
	case IPPROTO_ESP:
	case IPPROTO_AH:
#endif
	case IPPROTO_RAW:
#ifndef WIN32
	case IPPROTO_IPIP:
	case IPPROTO_EGP:
	case IPPROTO_RSVP:
	case IPPROTO_GRE:
	case IPPROTO_PIM:
#endif
#ifdef LINUX
	case IPPROTO_COMP:
#endif
	//case SOL_ICMPV6:
	//case SOL_IPX:
	//case SOL_AX25:
	//case SOL_ATALK:
	//case SOL_NETROM:
	//case SOL_ROSE:
	//case SOL_DECNET:
	//case SOL_X25:
	//case SOL_PACKET:
	//case SOL_ATM:
	//case SOL_AAL:
	//case SOL_IRDA:
		f_protocol = r;
		return;

	default:
		throw moError(MO_ERROR_INVALID, "protocol not understood by socket");

	}
}


/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Port - determine the port of a socket

SYNOPSIS

	void Port(void);

DESCRIPTION

	This function is called by the constructor to determine the
	port which will be used when the socket is created.

	The port will either be defined in the list of parameters
	as PORT=<value> or in the call to the constructor.

	There is no default port. If the current port value is not
	zero, then this function won't modify it.

ERRORS

	MO_ERROR_INVALID	an invalid port number was specified

SEE ALSO

	Constructor

*/
void moSocket::Port(void)
{
	moVariable	*v;
	long		r;

	if(f_host.sin_port != 0) {
		return;
	}

	v = f_params.Get("PORT");
	if(v != NULL) {
#if WIN32
		f_host.sin_port = (u_short) -1;
#else
		f_host.sin_port = (in_port_t) -1;
#endif
		r = v->Integer();
		if(errno != 0 || r < 0 || r > 65535) {
			throw moError(MO_ERROR_INVALID, "invalid port number");
		}
		f_host.sin_port = htons(static_cast<u_short>(r));
	}
}



/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Family - determine the family of the address

SYNOPSIS

	void Family(void);

DESCRIPTION

	This function is called by the constructor to determine the
	family of the address used to connect.

	The family will either be defined in the list of parameters
	as FAMILY=<value> or when the host is looked up.

	The default family is AF_INET since it is the most commonly
	used.

ERRORS

	MO_ERROR_INVALID	an invalid port number was specified

SEE ALSO

	Constructor

*/
void moSocket::Family(void)
{
	moVariable	*v;
	long		r;

	v = f_params.Get("FAMILY");
	if(v == NULL) {
		return;
	}

	if(moWCString(*v) == "INET") {
		f_host.sin_family = AF_INET;
		return;
	}

	r = v->Integer();
	if(errno != 0) {
		r = -1;
	}

	switch(r) {
	case AF_INET:
		f_host.sin_family = AF_INET;
		return;

	default:
		throw moError(MO_ERROR_INVALID, "family not understood by socket");

	}
}



/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Host - determine the host address to connect to

SYNOPSIS

	void Host(const moWCString& host);

DESCRIPTION

	This function will determine the host address to connect to.

	If the address is an IP address in dot notation, then the
	family of the host is forced to AF_INET.

ERRORS

	MO_ERROR_INVALID	an invalid host name was specified
				or it can't be found at this time

SEE ALSO

	Constructor

*/
void moSocket::Host(const moWCString& host)
{
	const moWCString	*v;
	char			hn[256], *hostname;
	struct hostent		*h;

	if(host.IsEmpty()) {
		v = f_params.Get("HOST");
		if(v == NULL) {
			throw moError(MO_ERROR_INVALID, "no host specified");
		}
	}
	else {
		v = &host;
	}
	host.MBData(hn, sizeof(hn));
	hostname = hn;

	/* just in case */
	while(isspace(*hostname)) {
		hostname++;
	}

/* special case for the "any address" */
	if(hostname[0] == '*') {
		f_length = sizeof(struct in_addr);
		f_host.sin_addr.s_addr = htonl(INADDR_ANY);
		f_host.sin_family = AF_INET;
		return;
	}

/* special case host (inet_addr() would return -1 for this one) */
	if(strncmp(hostname, "255.255.255.255", 15) == 0) {
		f_length = sizeof(struct in_addr);
		f_host.sin_addr.s_addr = (unsigned int) -1;
		f_host.sin_family = AF_INET;
		return;
	}

/* check if the host is in dot notation */
	f_host.sin_addr.s_addr = inet_addr(hostname);
	if(f_host.sin_addr.s_addr != (unsigned int) -1) {
		f_length = sizeof(struct in_addr);
		f_host.sin_family = AF_INET;
		return;
	}

/* host lookup */
	h = gethostbyname(hostname);

/* verify the result of the gethostbyname() */
	if(h == NULL			/* anything at all? */
	|| h->h_addr_list == NULL	/* any address? */
	|| *h->h_addr_list == NULL) {	/* not even one address! */
		throw moError(MO_ERROR_INVALID, "host not found");
	}

	/* we don't support to small or large sizes */
	if(h->h_length < 4 || h->h_length > 12) {
		throw moError(MO_ERROR_INVALID, "unsupported host length");
	}

	f_length = h->h_length;
	memcpy(&f_host.sin_addr, *h->h_addr_list, f_length);
}




/************************************************************ DOC:

CLASS

	moSocket

NAME

	private:
	Socket - create a socket and set it up as expected

SYNOPSIS

	void Socket(void);

DESCRIPTION

	This function creates a socket and setup different parameters
	on it such as the IPTOS, non-blocking effect and keep-alive.

	The user can setup some flags in order to turn ON or OFF these
	different features. The flags currently available are:

		KEEPALIVE	set to TRUE or FALSE
		IPTOS		set to THROUGHPUT
		NONBLOCK	set to TRUE or FALSE

ERRORS

	MO_ERROR_INVALID	an invalid host name was specified
				or it can't be found at this time

SEE ALSO

	Constructor

*/
void moSocket::Socket(void)
{
	moVariable	*v;
#ifdef WIN32
	char		value;
#else
	long		value;
#endif

/* create the socket we will use for the connection */
	f_socket = socket(f_host.sin_family, f_type, f_protocol);
	if(f_socket == SOCKET_UNDEFINED) {
		throw moError(MO_ERROR_IO, "can't create the socket I/O handle");
	}

/* when possible and if requested by the user, change the IP transfer method */
#if defined(IP_TOS) && !defined(WIN32)
	v = f_params.Get("IPTOS");
	if(v != NULL) {
		if(moWCString(*v) == "THROUGHPUT") {
			value = IPTOS_THROUGHPUT;
		}
		else {
			throw moError(MO_ERROR_INVALID, "IPTOS value not understood by socket");
		}
		setsockopt(f_socket, IPPROTO_IP, IP_TOS, &value, sizeof(value));
	}
#endif

/* when possible and if requested by the user, send a keepalive message on line */
#ifdef SO_KEEPALIVE
	v = f_params.Get("KEEPALIVE");
	if(v != NULL) {
		if(v->CaseCompare("TRUE") == MO_BASE_COMPARE_EQUAL) {
			value = 1;
		}
		else {
			value = 0;
		}
		setsockopt(f_socket, SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(value));
	}
#endif

/* if the user asked, try to make it a non blocking file */
	v = f_params.Get("NONBLOCK");
	if(v != NULL) {
#ifdef WIN32
		u_long		flags;

		if(v->CaseCompare("TRUE") == MO_BASE_COMPARE_EQUAL) {
			flags = 1;
		}
		else {
			flags = 0;
		}
		if(ioctlsocket(f_socket, FIONBIO, &flags) != 0) {
			return;
		}
#else
		int		flags;

		flags = fcntl(f_socket, F_GETFL);
		if(flags == -1) {
			return;
		}
		if(v->CaseCompare("TRUE") == MO_BASE_COMPARE_EQUAL) {
			flags |= FNONBLOCK;
		}
		else {
			flags &= ~FNONBLOCK;
		}
		if(fcntl(f_socket, F_SETFL, flags) == -1) {
			return;
		}
#endif
	}
}



/************************************************************ DOC:

CLASS

	moSocket

NAME

	Constructor - initialize a socket object

SYNOPSIS

	moSocket(const moWCString& filename, const moWCString& host, int port);

DESCRIPTION

	A socket object is created and initialized using the named
	file as a file of variables (as defined for the moVariable.Load()
	function).

PARAMETERS

	filename -	name of a variable file which declares the socket
			parameters

	host -		the name of the host; it can be in dotted numbers or
			a name to resolve with a DNS as gethostbyname() does

	port -		the port number to use for this socket; if 0, use
			either the port available in the socket parameters
			or a default port when possible

ERRORS

	One of several throw may occur:

	MO_ERROR_MEMORY		when a string or a file can't be allocated
				in memory

	MO_ERROR_INVALID	some necessary parameters couldn't be
				defined

SEE ALSO

	Type, Protocol, Port

*/
moSocket::moSocket(const moWCString& filename, const moWCString& host, int port)
	: f_params("SOCKET")
{
 	Init(filename, port);
	Type();
	Protocol();
	Port();
	Family();
	Host(host);
	Socket();
}


/************************************************************ DOC:

CLASS

	moSocket

NAME

	Destructor - ends a socket object

SYNOPSIS

	~moSocket();

DESCRIPTION

	Clean up a socket object before total destruction.

SEE ALSO

	Constructor

*/
moSocket::~moSocket()
{
	// close the socket if need be
	if(f_socket != SOCKET_UNDEFINED) {
		close(static_cast<int>(f_socket));
	}
}





/** \brief Wait on the socket.
 *
 * Waits on an event on the socket.
 *
 * If the waiting times (sec & usec) are (0, 0), then
 * the wait returns only when a connection is available
 * (moServer) or data arrived (moConnection).
 *
 * Otherwise, it returns after that amount of time was
 * elapsed. If the times are set to a negative value
 * like (-1), then this function can be used to just check
 * whether there are some activity pending on the given
 * socket object.
 *
 * \bug
 * Note that under MS-Windows, this function does not work properly
 * if the socket is in blocking mode.
 *
 * \param[in] sec   The maximum number of second to wait
 * \param[in] usec  The maximum number of micro-second to wait
 *
 * \return < 0 when an error occured; 0 when no connections are
 *	pending; > 0 one or more connections are pending
 */
int moSocket::Wait(long sec, long usec) const
{
	fd_set		set_read, set_write, set_exception;
	struct timeval	elapse, *e;
	int		r;

	FD_ZERO(&set_read);
	FD_ZERO(&set_write);
	FD_ZERO(&set_exception);

	FD_SET(f_socket, &set_read);
	FD_SET(f_socket, &set_write);
	FD_SET(f_socket, &set_exception);

	if(sec == 0 && usec == 0) {
		e = NULL;
	}
	else if(sec < 0 || usec < 0) {
		e = &elapse;
		elapse.tv_sec = 0;
		elapse.tv_usec = 0;
	}
	else {
		e = &elapse;
		elapse.tv_sec = sec;
		elapse.tv_usec = usec;
	}

	r = select(static_cast<int>(f_socket + 1), &set_read, NULL, NULL, e);
	if(r < 0) {
		perror("select");
		return -1;
	}

	return r == 0 ? 0 : 1;
}














/************************************************************ DOC:

CLASS

	moServer

NAME

	private:
	InitServer - initialize a server object variable members

SYNOPSIS

	void InitServer(void);

DESCRIPTION

	This function initializes the object variable members so
	the socket can be used as a server.

SEE ALSO

	Constructor

*/
void moServer::InitServer(void)
{
	f_connection = SOCKET_UNDEFINED;
	// f_addr_connect ... unused if length == 0
	f_addr_length = 0;
	f_count = MO_SERVER_MAX_CHILDREN;
	f_free_child = 0;
	f_children = 0;
	f_fork = true;
}




/************************************************************ DOC:

CLASS

	moServer

NAME

	private:
	Children - initialize a server children array

SYNOPSIS

	void Children(void);

DESCRIPTION

	This function will look for the variable named MAXCONNECTIONS and
	use the value as the maximum number of children that the server can
	have at any given time.

	If the variable is not defined, then the default of
	MO_SERVER_MAX_CHILDREN is used.

ERRORS

	One of several throw may occur:

	MO_ERROR_MEMORY		if we can't allocate the array of children

	MO_ERROR_INVALID	the MAXCONNECTIONS variable value is invalid

SEE ALSO

	Constructor

*/
void moServer::Children(void)
{
	moVariable	*v;

	// the default maximum number of connection
	f_count = MO_SERVER_MAX_CHILDREN;

	// check for a user number of connection
	v = f_params.Get("MAXCONNECTION");
	if(v != NULL) {
		f_count = v->Integer();
		if(errno != 0) {
			throw moError(MO_ERROR_INVALID, "MAXCONNECTION must be set to a valid value");
		}
		// limit the number of connections (children)
		if(f_count < 1) {
			f_count = 1;
		}
		if(f_count > 250) {
			f_count = 250;
		}
	}

	// allocate a table of children used to run connections in the background
	f_children = new int[f_count];
	if(f_children == 0) {
		throw moError(MO_ERROR_MEMORY, "can't allocate the children array");
	}

	// clear this table
	memset(f_children, 0, sizeof(int) * f_count);
}




/************************************************************ DOC:

CLASS

	moServer

NAME

	private:
	Flags - defines different flags used by the servers

SYNOPSIS

	void Flags(void);

DESCRIPTION

	This function looks for different flags that the server uses to
	change behavior.
	
	1. FORK

	By default a server forks on every new connection before to call
	the Start() function with the new connection file descriptor.

	It is possible to set FORK to FALSE in order to run the Start() function
	as the parent process. In this case the user can do a call to Fork()
	or not if he wants to. It is adviced to use the Fork() member function
	as this function ensures that the process identifier is saved in
	ther server object.

	A server being deleted will wait for all of its children to die
	first (though, no new connection will be accepted in the mean time).

ERRORS

	One of several throw may occur:

	MO_ERROR_INVALID	one of the variable value is invalid

SEE ALSO

	Constructor

*/
void moServer::Flags(void)
{
	moVariable	*v;

	v = f_params.Get("FORK");
	if(v != NULL) {
		if(v->CaseCompare("FALSE") == MO_BASE_COMPARE_EQUAL) {
			f_fork = false;
		}
	}
}




/************************************************************ DOC:

CLASS

	moServer

NAME

	private:
	Listen - start listening on the socket of this server

SYNOPSIS

	void Listen(void);

DESCRIPTION

	This function calls the listen(2) function to start listening for
	incoming connections.

ERRORS

	One of several throw may occur:

	MO_ERROR_INVALID	one of the variable value is invalid

SEE ALSO

	Constructor

*/
void moServer::Listen(void)
{
	int		r;

#if 0
printf("Listening on host: %d.%d.%d.%d:%d\n",
			(f_host.sin_addr.s_addr >>  0) & 255,
			(f_host.sin_addr.s_addr >>  8) & 255,
			(f_host.sin_addr.s_addr >> 16) & 255,
			(f_host.sin_addr.s_addr >> 24) & 255,
			htons(f_host.sin_port));
#endif

	/*
	memset(&f_host.sin_addr, 0, sizeof(f_host.sin_addr));
	memset(&f_host.sin_port, 0, sizeof(f_host.sin_port));
	*/

	r = bind(f_socket, (const sockaddr *) &f_host, sizeof(f_host));
	if(r < 0) {
		perror("bind(2)");
		throw moError(MO_ERROR_MEMORY, "can't bind socket to the specified host address");
	}

	if(f_type == SOCK_STREAM || f_type == SOCK_SEQPACKET) {
		// other types can't call the listen instruction
		r = listen(f_socket, 5);
		if(r < 0) {
			throw moError(MO_ERROR_MEMORY, "can't listen to socket");
		}
	}
}



/************************************************************ DOC:

CLASS

	moServer

NAME

	Constructor - initialize a server object

SYNOPSIS

	moServer(const moWCString& filename, const moWCString& host, int port = 0);

DESCRIPTION

	A server object is created and initialized using the named
	file as a file of variables (as defined for the moVariable.Load()
	function).

	The server will have a socket and will automatically run a listen
	on the socket.

	It is up to the user to call the Wait() or Accept() functions once
	the server was starter.

	If the server is created as a NONBLOCK server (NONBLOCK=TRUE) then
	the Wait() function will not block but return right away with zero.

	The Accept() function will accept a connection if there is one
	pending. When the server is not setup with the parameter FORK=FALSE,
	then accepted connections are kept only is a forked child. The
	parent process will have to somehow check on the death of the child.
	The child (or main process if FORK=FALSE) then calls the Start()
	function with the given file descriptor.

PARAMETERS

	filename -	name of a variable file which declares the socket
			parameters

	host -		the name of the host; it can be in dotted numbers or
			a name to resolve with a DNS, NIS... as gethostbyname()
			does

	port -		the port number to use for this socket; if 0, use
			either the port available in the socket parameters
			or a default port when possible

ERRORS

	One of several throw may occur:

	MO_ERROR_MEMORY		when a string or a file can't be allocated
				in memory

	MO_ERROR_INVALID	some necessary parameters couldn't be
				defined

SEE ALSO

	Type, Protocol, Port

*/
moServer::moServer(const moWCString& filename, const moWCString& host, int port)
	: moSocketConnection(filename, host, port, true)
{
	InitServer();
	Children();
	Flags();
	Listen();
}


/************************************************************ DOC:

CLASS

	moServer

NAME

	Destructor - ends a server object

SYNOPSIS

	~moServer();

DESCRIPTION

	Clean up a server object before total destruction.

SEE ALSO

	Constructor

*/
moServer::~moServer()
{
	// TODO: wait on the children which are still alive!
	delete f_children;
}






/************************************************************ DOC:

CLASS

	moServer

NAME

	Accept - accept the next connection

SYNOPSIS

	int Accept(void);

DESCRIPTION

	Waits and pop out the next connection on this server.

	The resulting connection is temporarilly saved in
	the f_addr_length and f_addr_connect parameters of
	the server. These will change at any time and should
	not be rely only but on entry of the Start() function.

	Not that the Start() function will be called BEFORE
	this Accept() function returns. The Start() command
	may be called within a child process if the FORK
	parameter was not set FALSE.

	If you want to make sure that this call never blocks,
	make sure the socket is created with the NONBLOCK
	parameter set to TRUE.

NOTE

	To test whether some connections are pending, please,
	use the Wait() function instead.

RETURNED VALUE

	-1	an error occured
	0	no error occured - possibly a connection was successfully made

	The error is found in errno and can be any error that accept(2),
	fork(2), the Start() virtual function return, or ENOENT when too
	many children are already running (too many connections are already
	running...)

SEE ALSO

	Constructor

*/
int moServer::Accept(void)
{
	int		r;
	pid_t		pid;

	r = FreeChild();
	if(r < 0) {
		return r;
	}

	f_addr_length = sizeof(f_addr_connect);
	f_connection = accept(f_socket, (struct sockaddr *) &f_addr_connect, &f_addr_length);
	if(static_cast<int>(f_connection) < 0) {
		if(errno == EAGAIN) {
			// non-blocking error when no connections are present
			return 0;
		}
		return -1;
	}

	f_disconnected = false;
	if(f_fork) {
#ifdef WIN32
		pid = -1;
#else
		pid = fork();
#endif
		if(pid == 0) {
			// I'm the child!
			f_socket = f_connection;
			r = Start(static_cast<int>(f_connection));
			close(static_cast<int>(f_connection));
			f_connection = SOCKET_UNDEFINED;
			f_disconnected = true;
			exit(r);
		}
		// whatever happens, we don't need that file descriptor
		// in the parent process...
		close(static_cast<int>(f_connection));
		f_connection = SOCKET_UNDEFINED;
		f_disconnected = true;
		if(pid < 0) {
			return -1;
		}
		// save children PID, we wait on them later...
		f_children[f_free_child] = pid;
	}
	else {
		r = Start(static_cast<int>(f_connection));
		close(static_cast<int>(f_connection));
		f_connection = SOCKET_UNDEFINED;
		f_disconnected = true;
		if(r != 0) {
			return -1;
		}
	}

	return 0;
}




/************************************************************ DOC:

CLASS

	moServer

NAME

	private:
	FreeChild - look for a free child entry in our table

SYNOPSIS

	int FreeChild(void);

DESCRIPTION

	This function looks for a free entry in the list of children
	PID to save a PID returned by Fork().

RETURN VALUE

	When an error occurs -1 is returned. Otherwise 0 is returned.

ERROR

	When no children are available, then errno is set to ENOENT.

SEE ALSO

	Accept, Fork

*/
int moServer::FreeChild(void)
{
#ifndef WIN32
	unsigned long	i;
	pid_t		pid, died;
	bool		found;

	found = false;
	i = f_count;
	while(i != 0) {
		i--;
		pid = f_children[i];
		if(pid != (pid_t) 0) {
			died = waitpid(pid, 0, WNOHANG);
			if(died == pid) {
				// this child died!
				f_children[i] = (pid_t) 0;
				found = true;
				f_free_child = i;
			}
		}
		else {
			found = true;
			f_free_child = i;
		}
	}

	if(found) {
		return 0;
	}
#endif

	errno = ENOENT;
	return -1;
}


/************************************************************ DOC:

CLASS

	moServer

NAME

	private:
	FreeChild - look for a free child entry in our table

SYNOPSIS

	int FreeChild(void);

DESCRIPTION

	This function creates a child process.

	It is intended to the users who want to create a child
	process on their own. They should first set the FORK flag
	to FALSE, then within their Start() function, they should
	call this Fork() function.

	The Fork() function is used to save the process id's of the
	children created with the fork(2) function. If no children
	process identifier should be saved within the parent (server)
	process, then this function should not be called.

	The f_socket descriptor of the child is also set to the
	f_connection descriptor of the parent. Thus, the child can
	be viewed as an moSocketConnection object.

RETURN VALUE

	When an error occurs -1 is returned. Otherwise 0 is returned.

ERROR

	When no children are available, then errno is set to ENOENT.

SEE ALSO

	Accept, Fork

*/
pid_t moServer::Fork(void)
{
	int		r;
	pid_t		pid;

	r = FreeChild();
	if(r < 0) {
		return (pid_t) -1;
	}

#ifdef WIN32
	pid = -1;
#else
	pid = fork();
#endif
	if(pid > 0) {
		f_children[f_free_child] = pid;
	}
	else if(pid == 0) {
		f_socket = f_connection;
	}

	return pid;
}




/************************************************************ DOC:

CLASS

	moServer

NAME

	Loop - loop forever waiting for connections

SYNOPSIS

	void Loop(void);

DESCRIPTION

	This function waits for a connection on this server socket
	and connects. This generates calls to the Start() function.

	Unless an error occurs, this function won't return.

SEE ALSO

	Wait, Accept

*/
void moServer::Loop(void)
{
	int		r;

	for(;;) {
		// we use the Wait() function because the Accept() won't
		// block if the socket was made NONBLOCK and we won't
		// change that over the user...
		r = Wait();
		if(r < 0) {
			printf("Bad wait?\n");
			break;
		}
		r = Accept();
		if(r < 0) {
			printf("Bad accept?\n");
			break;
		}
	}
}















/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	private:
	InitConnection - initialize a connection object

SYNOPSIS

	void InitConnection(void);

DESCRIPTION

	This function initializes the connection object.

ERRORS

	One of several throw may occur:

	MO_ERROR_INVALID	one of the variable value is invalid

SEE ALSO

	Constructor

*/
void moSocketConnection::InitConnection(void)
{
	f_disconnected = true;
	f_crypt = 0;
	f_input_pos = 0;
	f_output_pos = 0;
	//f_input -- auto-init
	//f_output -- auto-init
	f_decoded_size = 0;
	f_decoded_pos = 0;
	f_decoded = 0;
}




/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	private:
	Connect - try to connect to the given address & port

SYNOPSIS

	void Connect(void);

DESCRIPTION

	This function calls the connect(2) function to start a connection for
	data transfers.

ERRORS

	One of several throw may occur:

	MO_ERROR_INVALID	one of the variable value is invalid

SEE ALSO

	Constructor

*/
void moSocketConnection::Connect(void)
{
	int		r;

#if 0
printf("CONNECT TO Host = %d.%d.%d.%d:%d\n",
			(f_host.sin_addr.s_addr >>  0) & 255,
			(f_host.sin_addr.s_addr >>  8) & 255,
			(f_host.sin_addr.s_addr >> 16) & 255,
			(f_host.sin_addr.s_addr >> 24) & 255,
			htons(f_host.sin_port));
#endif

	if(f_type == SOCK_STREAM || f_type == SOCK_SEQPACKET) {
		// other types can't call the connect instruction
		r = connect(f_socket, (struct sockaddr *) &f_host, sizeof(f_host));
		if(r < 0) {
			throw moError(MO_ERROR_MEMORY, "can't connect to specified host & port");
		}
	}

	f_disconnected = false;
}



/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	Constructor - initialize a connection object

SYNOPSIS

	moSocketConnection(const moWCString& filename, const moWCString& host, int port = 0);

PARAMETERS

	filename -	name of a variable file which declares the socket
			parameters

	host -		the name of the host; it can be in dotted numbers or
			a name to resolve with a DNS, NIS... as gethostbyname()
			does

	port -		the port number to use for this socket; if 0, use
			either the port available in the socket parameters
			or a default port when possible

DESCRIPTION

	A connection object is created and initialized using the named
	file as a file of variables (as defined for the moVariable.Load()
	function).

	The connection will have a socket and will automatically beconnected.

	It is up to the user to call the Wait(), Receive() and Send() functions
	if the creation of the socket succeeded.

	If the connection is created as a NONBLOCK server (NONBLOCK=TRUE) then
	the Wait() function will not block but return right away with zero.

	The Send() function can be used to write data in the connection feed.
	The data is sent to the other side of the connection.

	The Receive() function can be used to read data from the connection.
	Usually, only STREAM connections will expect to reveice data as in a
	pipe. The other connections receive data as block messages.

NOTE

	The constructor has one more entry which is used to know whether a
	server is being created. When a server is created, the object doesn't
	try to connect since the server will accept connections instead
	(whenever possible).

ERRORS

	One of several throw may occur:

	MO_ERROR_MEMORY		when a string or a file can't be allocated
				in memory

	MO_ERROR_INVALID	some necessary parameters couldn't be
				defined

SEE ALSO

	Type, Protocol, Port

*/
moSocketConnection::moSocketConnection(const moWCString& filename, const moWCString& host, int port, bool server)
	: moSocket(filename, host, port)
{
	InitConnection();
	if(!server) {
		Connect();
	}
}


/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	Destructor - ends a connection object

SYNOPSIS

	~moSocketConnection();

DESCRIPTION

	Clean up a connection object before total destruction.

SEE ALSO

	Constructor

*/
moSocketConnection::~moSocketConnection()
{
	//f_crypt -- this is the caller responsability (it may not even be an allocated object!)
	//f_input -- auto-delete
	//f_output -- auto-delete
	delete f_decoded;
}





/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	SetEncryption - change the encryption mechanism

SYNOPSIS

	void SetEncryption(moCrypt *crypt, unsigned long input_size,
					unsigned long output_size);

PARAMETERS

	crypt		a list of encryption/decryption objects

	input_size	maximum number of bytes necessary before the
			data can be decrypted properly

	output_size	exact number of bytes necessary before the
			data can be encrypted properly

DESCRIPTION

	This function can be used to setup an encryption scheme over
	a connection.

	The result is that any data sent will be encrypted and any
	data received is decrypted with the given encryption tables.

	The encryption object will be created and populated with a
	list of moCryptBase objects before it is attached to a
	connection object.

	These tables shouldn't be attached to a server, but to the
	connections which results only.

	It is possible to suppress the effect of a previous encryption
	setup simply by calling this function with 0 in all the parameters.

	It is an error (MO_ERROR_INVALID) to set the 'crypt' parameter
	to a non-null pointer, but have the input_size of output_size
	set to zero.

ERRORS

	This function may throw an error of the following types:

	MO_ERROR_INVALID	one of the input parameter is
				not a valid value

	MO_ERROR_MEMORY		the allocation of one of the
				buffer (input or output) failed

BUGS

	The input and output buffers are currently lost whenever
	the encryption tables are changed.

SEE ALSO

	Receive, Send

*/
void moSocketConnection::SetEncryption(moCryptBase *crypt, unsigned long input_size, unsigned long output_size)
{
	f_crypt = crypt;

	// redefine the in/out buffers
	f_input.Empty();
	f_output.Empty();

	f_input_pos = 0;
	f_output_pos = 0;

	if(crypt != 0) {
		if(input_size == 0 || output_size == 0) {
			throw moError(MO_ERROR_INVALID, "When an encryption is defined, the input/output sizes have to be defined.\n");
		}
		f_input.SetSize(input_size);
		f_output.SetSize(output_size);
	}
}






/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	Receive - read incoming data

SYNOPSIS

	long Receive(void *buffer, unsigned long size);

PARAMETERS

	buffer		a buffer pointer which is at least 'size'
			bytes

	size		number of bytes to read from the connection
			and write in buffer

DESCRIPTION

	Reads incoming data in the user specified buffer for a number
	of bytes as specified by size.

	If an error occurs, less bytes than 'size' may be returned.

	When the file was made non-blocking, then the function will
	return 0 if not data is available or the call was to block
	to wait for more data. Note that 0 is also returned when the
	connection is dropped. In order to make the difference between
	both you can check the f_disconnected flag. If true, the
	connection was dropped.

	When an encryption object was supplied (see the SetEncryption()
	function), it is used here to Decode() the incoming data.
	Note that means the function will have to wait (or be called
	repeatively) until enough data for a decoding is received.

	However, once data was decoded, there may be more than you
	wanted to read. Thus, the following calls may return right
	away with data copied from an intenal buffer.

RETURNED VALUE

	this function returns the number of bytes read or -1 when an
	error occurs;

	non-blocking connections will possibly return a size of zero;

	connections which expect messages of a specific size will always
	return that size, 0 or -1;

SEE ALSO

	Constructor

*/
long moSocketConnection::Receive(void *buffer, unsigned long size)
{
	long		l, p, decode_size;
	unsigned char	*in;
	unsigned long	input_size;

	if(f_disconnected || size == 0) {
		return 0;
	}

	if(buffer == 0) {
		throw moError(MO_ERROR_INVALID, "Receive() expects a non-null buffer pointer");
	}

	if(f_crypt == 0) {
		l = recv(f_socket, reinterpret_cast<RECV_BUFFER_TYPE>(buffer), size, MSG_NOSIGNAL);
		if(l == 0 || (l < 0 && errno == EPIPE)) {
			f_disconnected = true;
			/* it usually is a good idea to close the connection on our end too! */
			close(static_cast<int>(f_socket));
			f_socket = SOCKET_UNDEFINED;
		}
		else if(l < 0 && errno == EAGAIN) {
			/* don't consider EAGAIN as an error! */
			l = 0;
		}
		return l;
	}

	if(f_input.IsEmpty()) {
		throw moError(MO_ERROR_INVALID, "the f_input field is zero and can't be used to read data");
	}

	// get a bare pointer to the data (Hmmm)
	f_input.GetPointer(in, input_size);

	// p is used to know the cumulative amount of data being read
	p = 0;
	for(;;) {
		l = f_decoded_size - f_decoded_pos;
		if(l > 0) {
			// we still have some data we can send to the end user!
			if((unsigned long) l >= size) {
				memcpy(buffer, f_decoded + f_decoded_pos, size);
				f_decoded_pos += size;
				if(f_decoded_pos >= f_decoded_size) {
					delete f_decoded;
					f_decoded = 0;
					f_decoded_size = 0;
					f_decoded_pos = 0;
				}
				return p + size;
			}
			memcpy(buffer, f_decoded + f_decoded_pos, l);
			delete f_decoded;
			f_decoded = 0;
			f_decoded_size = 0;
			f_decoded_pos = 0;
			buffer = (unsigned char *) buffer + l;
			size -= l;
			p += l;
		}

		// receive a message (some data) from the connection
		l = recv(f_socket, reinterpret_cast<RECV_BUFFER_TYPE>(in + f_input_pos), input_size - f_input_pos, 0);

		// disconnected?
		if(l == 0) {
			f_disconnected = true;
			close(static_cast<int>(f_socket));
			f_socket = SOCKET_UNDEFINED;
			return p;
		}

		// an error?
		if(l < 0) {
			if(errno == EAGAIN) {
				// in case we receive an EAGAIN, the user asked for a NON-BLOCKING
				// file and therefore we have to return now
				return p;
			}
			// an error occured... but we still want to return p if it isn't zero
			return p > 0 ? p : -1;
		}

		// we read some data, can we decode it?
		f_input_pos += l;
		decode_size = f_crypt->DecodeSize(in, f_input_pos);

//printf("Input size [%ld], decode size [%ld]\n", f_input_pos, decode_size);

		if(decode_size > 0 && (unsigned long) decode_size <= f_input_pos) {
			// we can decode whatever we received!
			// it is automatically saved in a buffer in the f_decoded field
			f_crypt->Decode(in, f_input_pos, f_decoded, f_decoded_size);
		}
	}
	/*NOTREACHED*/
}







/************************************************************ DOC:

CLASS

	moSocketConnection

NAME

	Send - send outgoing data

SYNOPSIS

	long Send(const void *buffer, unsigned long size);

PARAMETERS

	buffer		a buffer pointer which is at least 'size'
			bytes

	size		number of bytes to write to the connection
			and read from buffer

DESCRIPTION

	Writes outgoing data from the user specified buffer for a number
	of bytes as specified by size.

RETURNED VALUE

	this function returns the number of bytes written or -1 when an
	error occurs;

	connections which expect messages of a specific size will always
	specify that size or an error will most probably result;

SEE ALSO

	Constructor

*/
long moSocketConnection::Send(const void *buffer, unsigned long size)
{
	long		l, p;
	unsigned long	encoded_size;
	unsigned char	*encoded;
	unsigned char	*out;
	unsigned long	output_size;

	if(f_disconnected || size == 0) {
		return 0;
	}

	if(buffer == 0) {
		throw moError(MO_ERROR_INVALID, "Send() expects a non-null buffer pointer");
	}

	if(f_crypt == 0) {
		l = send(f_socket, reinterpret_cast<SEND_BUFFER_TYPE>(buffer), size, MSG_NOSIGNAL);
		if(l < 0 && errno == EPIPE) {
			/* pipe turned off */
			f_disconnected = true;
			/* it usually is a good idea to close the connection on our end too! */
			close(static_cast<int>(f_socket));
			f_socket = SOCKET_UNDEFINED;
			l = 0;
		}
		return l;
	}

	if(f_output.IsEmpty()) {
		throw moError(MO_ERROR_INVALID, "the f_output field is zero and can't be used to write data");
	}

	f_output.GetPointer(out, output_size);

	// p is used to know the cumulative amount of data being written
	p = 0;
	do {
		l = output_size - f_output_pos;
		if((unsigned long) l > size) {
			l = size;
		}
		memcpy(out + f_output_pos, buffer, l);
		size -= l;
		f_output_pos += l;
		if(f_output_pos >= output_size) {
			// we have enough data to encode and send data
			f_crypt->Encode(out, output_size, encoded, encoded_size);
			p += output_size;

//printf("Encoded (sending) size = %lu\n", encoded_size);

			l = send(f_socket, reinterpret_cast<SEND_BUFFER_TYPE>(encoded), encoded_size, MSG_NOSIGNAL);
			delete encoded;
			if(l < 0 && errno == EPIPE) {
				// pipe turned off
				f_disconnected = true;
				// it usually is a good idea to close the connection on our end too!
				close(static_cast<int>(f_socket));
				f_socket = SOCKET_UNDEFINED;
				// we won't be able to send anything more... so return
				return p;
			}
			if(l < 0) {
				return -1;
			}
		}
	} while(size > 0);

	return p;
}







};			// namespace molib;

// vim: ts=8

