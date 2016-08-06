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



#ifndef MO_FIFO_H
#define	MO_FIFO_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_MUTEX_H
#include	"mo_mutex.h"
#endif
#ifndef MO_LIST_H
#include	"mo_list.h"
#endif

namespace molib
{



class MO_DLL_EXPORT moFIFO : public moBase
{
public:
	static const unsigned int	MO_FIFO_DEFAULT_SIZE = 64;

				moFIFO(void);
				moFIFO(unsigned long size);
				moFIFO(const moFIFO& fifo);
	virtual			~moFIFO();

	virtual void		Reset(void);
	virtual unsigned long	MaxSize(void) const;
	virtual unsigned long	Size(void) const;
	virtual unsigned long	FreeSpace(void) const;

	virtual bool		Flush(int64_t ustime = -1);
	virtual int		Write(const void *buffer, unsigned long size);

	virtual bool		WaitData(int64_t ustime = -1, unsigned long size = static_cast<unsigned long>(-1));
	virtual int		Read(void *buffer, unsigned long size, bool peek = false);

	virtual bool		Lock(bool block = true);
	virtual void		Unlock(void);

	virtual compare_t	Compare(const moBase& object) const;

protected:
	void			AllocBuffer(unsigned long size);

private:
	void			Init(unsigned long size);

	unsigned long		f_max;		// total size of the FIFO buffer
	unsigned char *		f_fifo;		// a dynamically allocated buffer if larger than f_data

	// f_data is used whenever the FIFO doesn't require a larger buffer
	unsigned char		f_data[MO_FIFO_DEFAULT_SIZE];

	unsigned long		f_size;		// size of valid (written) data
	unsigned long		f_pos;		// position within the f_fifo buffer

	moMutex			f_mutex;	// for the lock/unlock
};

typedef moSmartPtr<moFIFO>		moFIFOSPtr;
typedef moTmplList<moFIFO, moList>	moListOfFIFO;


class MO_DLL_EXPORT moFIFOList : public moFIFO
{
public:
				moFIFOList(void);
				moFIFOList(const moFIFOList& fifo_list);
				~moFIFOList();

	bool			InsertFIFO(const moFIFO *fifo, moList::position_t pos = moList::NO_POSITION);
	void			RemoveFIFO(const moFIFO *fifo);
	void			RemoveFIFO(moList::position_t pos);
	void			RemoveAllFIFOs(void);

	virtual void		Reset(void);
	virtual unsigned long	MaxSize(void) const;
	virtual unsigned long	Size(void) const;
	virtual unsigned long	FreeSpace(void) const;

	virtual bool		Flush(int64_t ustime = -1);
	virtual int		Write(const void *buffer, unsigned long size);

	virtual bool		WaitData(int64_t ustime = -1, unsigned long size = static_cast<unsigned long>(-1));
	virtual int		Read(void *buffer, unsigned long size, bool peek = false);

	virtual bool		Lock(bool block = true);
	virtual void		Unlock(void);

private:
	bool			Propagate(bool start);

	moListOfFIFO		f_fifos;
};

typedef moSmartPtr<moFIFOList>	moFIFOListSPtr;



};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_FIFO_H

