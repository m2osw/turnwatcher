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




#ifdef MO_PRAGMA_INTERFACE
#pragma implementation "mo/mo_fifo.h"
#endif

#include	"mo/mo_fifo.h"

#include	<memory>

namespace molib
{



/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Constructor - create an moFIFO object
	Copy operator - copy an moFIFO object
	Destructor - destroys an moFIFO object

SYNOPSIS

	moFIFO(void);
	moFIFO(unsigned long size);
	moFIFO(const moFIFO& fifo);
	virtual ~moFIFO();

PARAMETERS

	size - the size of this new FIFO in bytes
	fifo - a FIFO to be duplicated

DESCRIPTION

	Construct/destruct a FIFO.

	When constructed, a FIFO is automatically Reset().

	When no size is specified, the default is the size of the
	f_data field.

	The copy operator copies the size of the source FIFO but
	not the actual data. To get the data, one can use the
	Read() function with (a) a buffer as large as the FIFO
	and (b) setting the 'peek' flag to true; then write that
	data that back in the source and also once in the copy.

ERRORS

	These functions call the Init() function member which can
	generate a memory error long(MO_ERROR_MEMORY).

SEE ALSO

	Reset(), AllocBuffer()

*/
moFIFO::moFIFO(void)
{
	Init(sizeof(f_data));
}


moFIFO::moFIFO(unsigned long size)
{
	Init(size);
}


moFIFO::moFIFO(const moFIFO& fifo)
	: moBase(fifo)
{
	Init(fifo.f_max);
}


moFIFO::~moFIFO(void)
{
	if(f_fifo != f_data) {
		delete f_fifo;
	}
}




/************************************************************ DOC:

CLASS

	moFIFO

NAME

	private:
	Init - initialize a FIFO object

SYNOPSIS

	void Init(void);

DESCRIPTION

	This function is used internally to initialize a new FIFO
	object. It will allocate a buffer to hold the string data.

SEE ALSO

	AllocBuffer

*/
void moFIFO::Init(unsigned long size)
{
	f_max = 0;
	f_fifo = 0;
	AllocBuffer(size);
}



/************************************************************ DOC:

CLASS

	moFIFO

NAME

	protected:
	AllocBuffer - allocate a FIFO object

SYNOPSIS

	void AllocBuffer(unsigned long size);

DESCRIPTION

	The AllocBuffer() function is used to set the size of the
	buffer used by the FIFO.

	It will also call the Reset() function to clear the current
	position and length of the data available within the FIFO.
	It is important to notice that side effect since the data
	inside the FIFO could have been important. To make sure
	that the FIFO can safely have a new buffer, one can either
	wait for the FIFO to be empty (Flush()) or Read() until
	no data is available.

SEE ALSO

	AllocBuffer, Flush, Read, Reset

*/
void moFIFO::AllocBuffer(unsigned long size)
{
	Lock();

	if(size != f_max) {
		if(f_fifo != f_data) {
			delete f_fifo;
			f_fifo = 0;
		}
		if(size > sizeof(f_data)) {
			f_fifo = new unsigned char[size];
		}
		else {
			f_fifo = f_data;
		}
		f_max = size;
	}

	Reset();

	Unlock();
}



/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Reset - restart with an empty FIFO

SYNOPSIS

	virtual void Reset(void);

DESCRIPTION

	Reset the current state of the FIFO so it appears empty
	afterward.

ERRORS

	If someone waits on data (blocking Read() call) then
	the read fails and returns immediately.

SEE ALSO

	AllocBuffer, Flush, Read, Reset

*/
void moFIFO::Reset(void)
{
	Lock();
	f_size = 0;
	f_pos = 0;
	Unlock();
}




/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Size - number of bytes available for reading
	MaxSize - return the size of the FIFO in bytes

SYNOPSIS

	unsigned long MaxSize(void) const;
	unsigned long Size(void) const;
	unsigned long FreeSpace(void) const;

DESCRIPTION

	The MaxSize() function returns the size of the FIFO in bytes as defined
	at creation time or after a call to the AllocBuffer().

	The Size() function returns the number of bytes currently available
	in the FIFO for reading. If you have multiple threads read the same
	FIFO then this value is fairly meaning less. If you have multiple
	threads writing into the FIFO but only one which will read the
	data, then it represents the minimum number of bytes to be read.

	The FreeSpace() will usually be used by a writer to know how many
	bytes one can write in the FIFO without blocking or causing an
	error.

RETURN VALUE

	MaxSize()	The maximum number of bytes that the FIFO will accept

	Size()		The number of bytes currently available in the FIFO
			for reading

	FreeSpace()	The number of bytes currently available for writing

SEE ALSO

	AllocBuffer, Flush, Read, Reset

BUGS

	The Size() information can change in multi-thread environment
	if you don't first lock the FIFO. However, most of the time it is
	enough to know whether you can read or write data from/to the FIFO.

*/
unsigned long moFIFO::MaxSize(void) const
{
	return f_max;
}


unsigned long moFIFO::Size(void) const
{
	return f_size;
}


unsigned long moFIFO::FreeSpace(void) const
{
	return f_max - f_size;
}




/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Flush - wait for all the written data to be read
	WaitData - wait for data to be available for a read

SYNOPSIS

	virtual bool Flush(int64_t ustime = -1);
	virtual bool WaitData(int64_t ustime = -1,
			unsigned long size = static_cast<unsigned long>(-1)); 

PARAMETERS

	ustime - the number of microsecond to wait until the FIFO is empty
	size - the minimum number of bytes we want available before to return

DESCRIPTION

	The Flush() function will be called by the user sending data
	in the FIFO to wait until the FIFO is empty.

	This function is usually used when a process needs to know
	whether it can quit after writing data to a FIFO.

	If the Flush() function returns before the FIFO is empty, then
	it returns false.

	The WaitData() will be used to block for a given period of
	time, until some data can be read from the FIFO. The size
	parameter will be used to request at least that many bytes
	of data before to return.

NOTES

	The default Flush() function isn't expected to work in multi-threading
	mode since it doesn't itself implement a locking mechanism. Thus,
	it will always return false.

	The default WaitData() function isn't expected to work in
	multi-threading mode since it doesn't itself implement a locking
	mechanism. Thus, it will always return false.

RETURN VALUE

	true if the Flush() succeeded (the FIFO is now empty)
	false if either the Flush() is not implemented or the FIFO is not
	empty after ustime microseconds.

SEE ALSO

	Write, Read

BUGS

	The Size() information can change in multi-thread environment
	if you don't first lock the FIFO. However, most of the time it is
	enough to know whether you can read or write data from/to the FIFO.

*/
bool moFIFO::Flush(int64_t ustime)
{
	return false;
}


bool moFIFO::WaitData(int64_t ustime, unsigned long size)
{
	return false;
}






/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Write - writes to the FIFO
	Read - reads from the FIFO

SYNOPSIS

	virtual int Write(const void *buffer, unsigned long size);
	virtual int Read(void *buffer, unsigned long size, bool peek = false);

PARAMETERS

	buffer - a pointer to a user buffer
	size - the number of bytes to be transfered
	peek - whether the position pointer has to be changed

DESCRIPTION

	The Write() function will copy the user buffer bytes to the FIFO
	internal buffer. The size parameter specifies how many bytes
	will be written. When a multi-thread FIFO is used and more bytes
	are written to the FIFO than the FIFO can receive at once, the
	thread will block until the FIFO buffer premits more bytes to
	be written (NOTE: this can be implementation dependent).

	The Read() function will copy the FIFO internal buffer to the
	user buffer. The size parameter specifies how many bytes will
	be read. When a multi-thread FIFO is used, it is possible that
	the function will block until enough data is available (NOTE:
	this can be implementation dependent).

	When the Read() function is used with the peek parameters set to
	true, then the FIFO position is not changed. Note that at most
	MaxSize() characters (the total size of the FIFO in bytes) can be
	read with peek set to true.

	With multi-thread FIFOs, it is wise to have only one thread
	writing and one thread reading in the FIFO. If you need to
	call Write() multiple times in a row to write one block of
	data, then you will need to Lock() the FIFO first. This is
	very important in a multi-thread environment. Similarly,
	if multiple threads are to read the FIFO asynchroneously,
	then it will be necessary to lock the FIFO properly if one
	block can't be read at once.

RETURN VALUE

	both functions return the number of bytes read
	or -1 when an error occured

SEE ALSO

	Write, Read

*/
int moFIFO::Write(const void *buffer, unsigned long size)
{
	unsigned long	pos, l;

	if(!Lock()) {
		return -1;
	}
	if(size > f_max - f_size) {
		size = f_max - f_size;
	}

	if(size != 0) {
		pos = f_pos + f_size;
		if(pos >= f_max) {
			pos -= f_max;
			memcpy(f_fifo + pos, buffer, size);	/* Flawfinder: ignore */
		}
		else {
			l = f_max - pos;
			if(size > l) {
				memcpy(f_fifo + pos, buffer, l);	/* Flawfinder: ignore */
				memcpy(f_fifo, static_cast<const char *>(buffer) + l, size - l);	/* Flawfinder: ignore */
			}
			else {
				memcpy(f_fifo + pos, buffer, size);	/* Flawfinder: ignore */
			}
		}
		f_size += size;
	}
	Unlock();

	return size;
}


int moFIFO::Read(void *buffer, unsigned long size, bool peek)
{
	unsigned long	l;

	if(!Lock()) {
		return -1;
	}
	if(size > f_size) {
		size = f_size;
	}

	if(size != 0) {
		if(f_pos + size > f_max) {
			l = f_max - f_pos;
			memcpy(buffer, f_fifo + f_pos, l);	/* Flawfinder: ignore */
			memcpy(static_cast<char *>(buffer) + l, f_fifo, size - l);	/* Flawfinder: ignore */
		}
		else {
			memcpy(buffer, f_fifo + f_pos, size);	/* Flawfinder: ignore */
		}
		if(!peek) {
			f_size -= size;
			f_pos += size;
			if(f_pos >= f_max) {
				f_pos -= f_max;
			}
		}
	}
	Unlock();

	return size;
}






/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Lock - ensures only one thread can use  this FIFO
	Unlock - give the uni-thread usage back

SYNOPSIS

	virtual int Lock(bool block = true);
	virtual void Unlock(void);

PARAMETERS

	block - whether you want the function to wait until the lock is on

DESCRIPTION

	The Lock() function will attempt to lock the FIFO for the
	calling thread. The function returns true when the FIFO
	is properly locked and false otherwise.

	Note that an implementation (as the default one) which
	doesn't have any support for multi-threading will always
	return true.

	The Unlock() must be called once for each call to the
	Lock() function to ensure other threads will again be
	able to use the FIFO.

RETURN VALUE

	The Lock() function returns true when the FIFO is locked
	and false otherwise.

SEE ALSO

	Write, Read

*/
bool moFIFO::Lock(bool block)
{
	if(block) {
		f_mutex.Lock();
		return true;
	}

	return f_mutex.TryLock();
}


void moFIFO::Unlock(void)
{
	f_mutex.Unlock();
}

/************************************************************ DOC:

CLASS

	moFIFO

NAME

	Compare - compare two FIFO pointers

SYNOPSIS

	compare_t Compare(const moBase& object) const;

PARAMETERS

	object - another moFIFO

DESCRIPTION

	Compare two FIFO pointers so they can be found in a list
	of FIFO (see the moFIFOList for more information).

RETURN

	returns one of the following values:

		MO_BASE_COMPARE_SMALLER
		MO_BASE_COMPARE_GREATER
		MO_BASE_COMPARE_EQUAL

SEE ALSO

	moFIFOList object

*/
moBase::compare_t moFIFO::Compare(const moBase& object) const
{
	long diff = static_cast<long>(this - dynamic_cast<const moFIFO *>(&object));
	if(diff < 0) {
		return MO_BASE_COMPARE_GREATER;
	}
	if(diff > 0) {
		return MO_BASE_COMPARE_SMALLER;
	}
	return MO_BASE_COMPARE_EQUAL;
}





/************************************************************ DOC:

CLASS

	moFIFOList

NAME

	Constructor - create an moFIFOList object
	Copy constructor - duplicate a FIFO list
	Destructor - destroys an moFIFOList object

SYNOPSIS

	moFIFOList(void);
	moFIFOList(const moFIFOList& fifo_list);
	virtual ~moFIFOList();

PARAMETERS

	fifo_list - the FIFO list to be duplicated

DESCRIPTION

	Construct/destruct a FIFO list.

	When constructed, a FIFO list is empty. It is not necessary to
	insert other FIFOs in a FIFO list before to use it. In this
	special case, the FIFO list behaves just like a regular FIFO.

	It is not a good idea to add and remove FIFOs as you go since
	some of the data could otherwise be lost.

	When no size is specified, the default is the size of the
	f_data field.

	The copy constructor will duplicate each FIFO since FIFOs
	can't be shared. Note that all the data currently in the
	FIFOs won't be copied in the new FIFOs in the resulting list.

NOTES

	An moFIFOList is itself an moFIFO and thus an moFIFOList
	can be composed of moFIFO and moFIFOList objects.

ERRORS

	a memory error long(MO_ERROR_MEMORY) can be generated

SEE ALSO

	Reset(), AllocBuffer()

*/
moFIFOList::moFIFOList(void)
{
}


moFIFOList::moFIFOList(const moFIFOList& fifo_list)
	: moFIFO(fifo_list.MaxSize())
{
	moList::position_t	p, max;

	// duplicate the list of FIFOs from the source and
	// add them to our list in the same order
	max = fifo_list.f_fifos.Count();
	p = 0;
	while(p < max) {
		f_fifos += *new moFIFO(*fifo_list.f_fifos.Get(p));
		p++;
	}
}


moFIFOList::~moFIFOList()
{
}



/************************************************************ DOC:

CLASS

	moFIFOList

NAME

	InsertFIFO - insert a FIFO in the FIFO list
	RemoveFIFO - remove the specified FIFO
	RemoveAllFIFOs - remove all the FIFOs

SYNOPSIS

	bool InsertFIFO(const moFIFO *fifo,
			moList::position_t pos = moList::NO_POSITION);
	void RemoveFIFO(const moFIFO *fifo);
	void RemoveFIFO(position_t pos);
	void RemoveAllFIFOs(void);

PARAMETERS

	fifo - a FIFO pointer to insert or remove from the list
	pos - the position at which the FIFO is inserted or
		from which it is deleted from the list of FIFOs

DESCRIPTION

	The usefulness of a FIFO list object is to attach multiple
	FIFOs one after another. Thus, you can convert the input
	bytes to all sorts of different values.

	The InsertFIFO() will be used to insert FIFOs in the
	necessary order. By default, the position is set to
	NO_POSITION which means the new FIFO is appended to
	the list. FIFOs are called in the order they are
	inserted in the list.

	When a FIFO is inserted in the list, the function returns
	true. It can't be inserted twice in the same list.

	The RemoveFIFO() functions are used to get rid of a
	given FIFO from the list. The FIFO will simply be removed
	from the list whether or not some data was still in it.
	The system automacally release the FIFOs so you will need
	to keep them around in a smart pointer if you need them
	later.

	The RemoveAllFIFOs() is similar to calling the RemoveFIFO()
	until the list of FIFOs is empty.

NOTES

	The addition and removal of FIFOs should be done only
	when the FIFO list object is not actively in use since
	the state won't be properly preserved otherwise.

	The FIFOs will automatically have their reference counter
	incremented when inserted and not if not inserted. When
	a FIFO is being removed, it is released. If you need to
	keep a FIFO pointer, save it in a smart pointer.

RETURN

	The InsertFIFO() function returns true with the specified
	FIFO was inserted in this moFIFOList object.

	The RemoveFIFO() functions return the pointer of the FIFO
	being removed from the list unless it is being deleted
	in which case NULL is returned.

SEE ALSO

	Reset(), AllocBuffer()

*/
bool moFIFOList::InsertFIFO(const moFIFO *fifo, moList::position_t pos)
{
	moList::position_t	p;

	if(fifo == 0) {
		// nothing to add
		return false;
	}

#ifdef MO_DEBUG
	// we don't want to link a list inside itself
	// note that since lists can include other lists,
	// this test is not enough to avoid recursivity
	assert(this != fifo);
#endif

	if(f_fifos.IsEmpty()) {
		Reset();
	}

	p = f_fifos.Find(fifo);
	if(p != moList::NO_POSITION) {
		// already exists?!?
		return false;
	}

	// insert this new FIFO
	return f_fifos.Insert(fifo, pos);
}


void moFIFOList::RemoveFIFO(const moFIFO *fifo)
{
	moList::position_t	pos;

	if(fifo == 0) {
		// already removed?
		return;
	}

	// NOTE: pos may be set to NO_POSITION and that's fine!
	pos = f_fifos.Find(fifo);
	return RemoveFIFO(pos);
}


void moFIFOList::RemoveFIFO(moList::position_t pos)
{
	if(pos < f_fifos.Count()) {
		f_fifos.Delete(pos);
	}
}


void moFIFOList::RemoveAllFIFOs(void)
{
	f_fifos.Empty();
}



/************************************************************ DOC:

CLASS

	moFIFOList

NAME

	Reset -- make all the FIFOs in the list empty
	MaxSize -- the maximum size writable at once
	Size -- the current size of the output (last) FIFO
	FreeSpace -- how much data can be written without blocking
		or generating an error

	Flush -- flush the list of FIFOs
	Write -- write in the FIFOs

	WaitData -- wait until some data is readable
	Read -- read the available data

	Lock -- lock the FIFO list
	Unlock -- unlock the FIFO list

	Propagate -- this function is used to propagate a read or
		a write so all the FIFOs are being used properly

SYNOPSIS

	virtual void Reset(void);
	virtual unsigned long MaxSize(void) const;
	virtual unsigned long Size(void) const;
	virtual unsigned long FreeSpace(void) const;

	virtual bool Flush(int64_t ustime = -1);
	virtual int Write(const void *buffer, unsigned long size);

	virtual bool WaitData(int64_t ustime = -1, unsigned long size = static_cast<unsigned long>(-1));
	virtual int Read(void *buffer, unsigned long size, bool peek = false);

	virtual bool Lock(bool block = true);
	virtual void Unlock(void);

	private:
	bool Propage(bool start = true);

PARAMETERS

	ustime - time to wait until the function is done or block (-1)
	buffer - data to be written to the FIFO (write) or
		where the data read from the is copied (read)
	size - the size of the buffer
	peek - whether the read changes the FIFO pointer
	block - whether the Lock() will block until done
	start - whether to propagate from the start of the list

DESCRIPTION

	All of the overloaded functions are used by the moFIFOList
	to manage the list of FIFOs. For more information about these
	functions, please, read the corresponding documentation in
	the moFIFO object.

	The Propagate() function is automatically called by the
	Read() and Write() functions in order to ensure data
	propagation among the FIFOs.

RETURN

	MaxSize() -- returns the maximum size which can possibly
		be written (in the first FIFO)

	Size() -- returns the number of bytes available for reading
		(in the last FIFO)

	FreeSpace() -- returns the number of bytes which can be
		written (in the first FIFO -- we don't try to
		determine the total size which can be written in
		the entire list; after writing this number of
		bytes, it may be possible to write more data without
		reading anything; actually, there may not be anything
		to be read until all the FIFOs have received much
		more data than what this function will return)

	Flush() -- returns true when the Flush() occured, false if
		it timed out; all the FIFOs will be flushed

	Write() -- returns the number of bytes which were written
		to the FIFO (we write in the first FIFO, and then
		propagate the data right away without blocking)

	WaitData() -- returns true if the data arrived, false if
		it timed out (we wait on the last FIFO)

	Read() -- returns the number of bytes which were read
		from the FIFO (we try to read from the last
		FIFO, if something was read, then the read is
		propagated to see if any more data can be
		available in the last FIFO).

	Lock() -- returns true when the FIFO is properly locked
		and false if block was set to false and the
		FIFO can't be locked right away; you can call
		the Unlock() function ONLY if the Lock() function
		returns true; in all other cases, calling the
		Unlock() function will generate an undefined
		behavior

	Propagate() -- returns true when some propagation occured

SEE ALSO

	moFIFO corresponding functions

*/
void moFIFOList::Reset(void)
{
	moList::position_t	p;

	// reset all the FIFOs
	p = f_fifos.Count();
	while(p > 0UL) {
		p--;
		f_fifos.Get(p)->Reset();
	}

	// reset ourself also (just in case)
	moFIFO::Reset();
}


unsigned long moFIFOList::MaxSize(void) const
{
	if(f_fifos.IsEmpty()) {
		return moFIFO::MaxSize();
	}
	return f_fifos.GetFirst()->MaxSize();
}


unsigned long moFIFOList::Size(void) const
{
	if(f_fifos.IsEmpty()) {
		return moFIFO::Size();
	}
	return f_fifos.GetLast()->MaxSize();
}


unsigned long moFIFOList::FreeSpace(void) const
{
	if(f_fifos.IsEmpty()) {
		return moFIFO::FreeSpace();
	}
	return f_fifos.GetFirst()->FreeSpace();
}


bool moFIFOList::Flush(int64_t ustime)
{
	moList::position_t	p;

	// reset all the FIFOs
	p = f_fifos.Count();
	if(p == 0UL) {
		// flush ourself when we don't have any child FIFO
		return moFIFO::Flush(ustime);
	}

	while(p > 0UL) {
		p--;
		if(!f_fifos.Get(p)->Flush(ustime)) {
			return false;
		}
	}

	// Note that we don't need to flush ourself in this case
	return true;
}


int moFIFOList::Write(const void *buffer, unsigned long size)
{
	unsigned long	total, max;
	long		r;
	moFIFO		*fifo;

	if(f_fifos.IsEmpty()) {
		return moFIFO::Write(buffer, size);
	}

	fifo = f_fifos.GetFirst();

	total = 0;
	do {
		max = fifo->FreeSpace();
		if(max > size) {
			max = size;
		}

		r = fifo->Write(buffer, max);
		if(r < 0) {
			return r;
		}
		total += r;
		size -= r;
		buffer = reinterpret_cast<const char *>(buffer) + r;
	} while(Propagate(true) && size > 0);

	return total;
}


bool moFIFOList::WaitData(int64_t ustime, unsigned long size)
{
	moList::position_t	p;

	// reset all the FIFOs
	p = f_fifos.Count();
	if(p == 0UL) {
		// flush ourself when we don't have any child FIFO
		return moFIFO::WaitData(ustime, size);
	}
	while(p > 0UL) {
		p--;
		if(!f_fifos.Get(p)->WaitData(ustime, size)) {
			return false;
		}
	}

	// Note that we don't need to wait data on ourself in this case
	return true;
}


int moFIFOList::Read(void *buffer, unsigned long size, bool peek)
{
	unsigned long		total, max;
	long			r;
	moFIFO			*fifo;

	if(f_fifos.IsEmpty()) {
		return moFIFO::Read(buffer, size, peek);
	}

	fifo = f_fifos.GetLast();

	// we won't be able to propagate anything with a peek!
	if(peek) {
		return fifo->Read(buffer, size, true);
	}

	total = 0;
	do {
		max = fifo->Size();
		if(max > size) {
			max = size;
		}

		r = fifo->Read(buffer, max);
		if(r < 0) {
			return r;
		}
		total += r;
		size -= r;
		buffer = reinterpret_cast<char *>(buffer) + r;
	} while(Propagate(false) && size > 0);

	return total;
}


bool moFIFOList::Lock(bool block)
{
	moList::position_t	p, max;

	// reset all the FIFOs
	max = p = f_fifos.Count();
	while(p > 0UL) {
		p--;
		if(!f_fifos.Get(p)->Lock()) {
			// if one fails we need to unlock all those
			// we already have locked!
			do {
				f_fifos.Get(p)->Unlock();
				p++;
			} while(p < max);
			return false;
		}
	}

	// reset ourself also (just in case)
	if(!moFIFO::Lock()) {
		// if one fails we need to unlock all those
		// we already have locked!
		while(p < max) {
			f_fifos.Get(p)->Unlock();
			p++;
		}
		return false;
	}

	return true;
}


void moFIFOList::Unlock(void)
{
	moList::position_t	p;

	// reset all the FIFOs
	p = f_fifos.Count();
	while(p > 0UL) {
		p--;
		f_fifos.Get(p)->Unlock();
	}

	// reset ourself also (just in case)
	moFIFO::Unlock();
}


// propagate the read & write
bool moFIFOList::Propagate(bool start)
{
	bool			more, result;
	long			tmp_size, size, max_size, step;
	moList::position_t	max, p, end;
	moFIFO			*fifo, *next;
	std::auto_ptr<char>	tmp;

	// it is certainly an error to call this function when
	// no FIFOs are in the list
	if(f_fifos.IsEmpty()) {
		return false;
	}

	tmp_size = 0;
	result = false;
	max = f_fifos.Count();
	do {
		more = false;
		if(start) {
			p = 0;
			end = max - 1;	// all the pipes except the last
			step = 1;
		}
		else {
			p = max - 2;	// all the pipes except the last
			end = static_cast<moList::position_t>(-1);
			step = -1;
		}
		while(p != end) {
			fifo = f_fifos.Get(p);
			next = f_fifos.Get(p + 1);
			size = fifo->Size();
			max_size = next->FreeSpace();
			if(size > max_size) {
				size = max_size;
			}
			if(size > 0) {
				if(size > tmp_size) {
					tmp_size = (size + 1023) & -1024;
					tmp.reset(new char[tmp_size]);
				}
				max_size = fifo->Read(tmp.get(), size);
				if(max_size < 0) {
					goto exit;
				}
				// NOTE: max_size should always be equal
				// to size unless an error occurs but
				// just in case we use it to make sure
				// we write the correct amount of bytes
				if(next->Write(tmp.get(), max_size) < 0) {
					goto exit;
				}
				more = true;
				result = true;
			}
			p += step;
		}
	} while(more);

exit:
	return result;
}


// vim: ts=8
}			// namespace molib;

