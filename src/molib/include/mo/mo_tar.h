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



#ifndef MO_TAR_H
#define	MO_TAR_H
#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_STREAM_H
#include	"mo_stream.h"
#endif
#ifndef MO_DIRECTORY_H
#include	"mo_directory.h"
#endif



namespace molib
{

#ifdef WIN32
MO_DLL_EXPORT_FUNC typedef	int		uid_t;
MO_DLL_EXPORT_FUNC typedef	int		gid_t;
#endif


class MO_DLL_EXPORT moTar : public moBase
{
public:
	// user accessible header
	class moTarHeader : public moBase
	{
	public:
				moTarHeader(void);
				moTarHeader(const moTarHeader& header);

		void		Reset(void);

		void		SetType(char type);
		void		SetName(const moWCString& name);
		void		SetLinkTo(const moWCString& link_to);
		void		SetUserName(const moWCString& user_name);
		void		SetGroupName(const moWCString& group_name);
		void		SetID(uid_t uid, gid_t gid);
		void		SetMode(mode_t mode);
		void		SetSize(size_t size);
		void		SetModifTime(time_t time);
		void		SetDevice(dev_t device);

		bool		GetType(char& type) const;
		bool		GetName(moWCString& name) const;
		bool		GetLinkTo(moWCString& link_to) const;
		bool		GetUserName(moWCString& user_name) const;
		bool		GetGroupName(moWCString& group_name) const;
		bool		GetID(uid_t& uid, gid_t& gid) const;
		bool		GetMode(mode_t& mode) const;
		bool		GetSize(size_t& size) const;
		bool		GetModifTime(time_t& time) const;
		bool		GetDevice(dev_t& device) const;

		bool		Define(const moWCString& filename, const moWCString& root);
		void		CopyMost(const moTarHeader& header);
		void		NoRoot(void);

	private:
		static const unsigned long	TAR_HEADER_TYPE      = 0x00000001;
		static const unsigned long	TAR_HEADER_NAME      = 0x00000002;
		static const unsigned long	TAR_HEADER_LINKTO    = 0x00000004;
		static const unsigned long	TAR_HEADER_USERNAME  = 0x00000008;
		static const unsigned long	TAR_HEADER_GROUPNAME = 0x00000010;
		static const unsigned long	TAR_HEADER_MODE      = 0x00000020;
		static const unsigned long	TAR_HEADER_ID        = 0x00000040;
		static const unsigned long	TAR_HEADER_SIZE      = 0x00000080;
		static const unsigned long	TAR_HEADER_MTIME     = 0x00000100;
		static const unsigned long	TAR_HEADER_DEVICE    = 0x00000200;

		unsigned long	f_flags;

		moWCString	f_name;
		moWCString	f_linkto;
		moWCString	f_username;
		moWCString	f_groupname;
		char		f_type;
		mode_t		f_mode;
		uid_t		f_uid;
		gid_t		f_gid;
		size_t		f_size;
		time_t		f_mtime;
		dev_t		f_device;
	};

protected:
	// the tar file header block definition
	struct tar_header_t {
		char		f_name[100];
		char		f_mode[8];
		char		f_uid[8];
		char		f_gid[8];
		char		f_size[12];
		char		f_mtime[12];
		char		f_chksum[8];
		char		f_typeflag;
		char		f_linkname[100];
		char		f_magic[6];
		char		f_version[2];
		char		f_uname[32];
		char		f_gname[32];
		char		f_devmajor[8];
		char		f_devminor[8];
		char		f_prefix[155];
		char		f_unused[12];

		bool		ConvertFrom(const moTarHeader& header);
		bool		ConvertTo(moTarHeader& header) const;
		unsigned long	CheckSum(void) const;
	};
};

// output (create) a tar file
class MO_DLL_EXPORT moOTar : public moTar
{
public:
				moOTar(void);
				moOTar(moOStream *output);
	virtual			~moOTar();

	void			SetOutput(moOStream *output);
	moOStreamSPtr		Output(void) const;

	bool			AppendHeader(const moTarHeader& header);
	bool			AppendBuffer(const moTarHeader& header, const void *data, size_t size);
	bool			AppendStream(const moTarHeader& header, moIStream& input);
	bool			AppendFile(const moTarHeader& header, const moWCString& filename);
	bool			AppendDirectory(const moTarHeader& header, const moWCString& path, const moWCString& root);
	bool			AppendDir(const moTarHeader& header, const moDirectory& dir, const moWCString& root);

protected:
	virtual bool		AcceptFile(const moTarHeader& header, const moWCString& filename);

private:
	void			Init(void);

	moOStreamSPtr		f_output;
};



};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_TAR_H

