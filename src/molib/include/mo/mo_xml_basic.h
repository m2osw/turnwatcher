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



#ifndef MO_XML_BASIC_H
#define	MO_XML_BASIC_H

#ifdef MO_PRAGMA_INTERFACE
#pragma interface
#endif

#ifndef MO_XML_H
#include	"mo_xml.h"
#endif

#ifndef MO_WORDS_H
#include	"mo_words.h"
#endif


namespace molib
{

/*
 * The moXML_Basic object encapsulates all the basic event handlers
 * to make the XML parser worth using!
 * This includes the variable management and basic include, expressions
 * loops, conditions, etc.
 */
class MO_DLL_EXPORT moXML_Basic : public moBase
{
public:
	class MO_DLL_EXPORT moXML_Basic_Set : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Set(void);
		virtual			~moXML_Basic_Set();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);
	};

	class MO_DLL_EXPORT moXML_Basic_Unset : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Unset(void);
		virtual			~moXML_Basic_Unset();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);
	};

	class MO_DLL_EXPORT moXML_Basic_Replace : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Replace(void);
		virtual			~moXML_Basic_Replace();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);
	};

	class MO_DLL_EXPORT moXML_Basic_Include : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Include(void);
		virtual			~moXML_Basic_Include();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

		void			AddIncludePath(const moWCString& path);
		void			AppendIncludePath(const moListBase& path);

	private:
		moListUniqueOfWCStrings	f_include_path;
	};

	class MO_DLL_EXPORT moXML_Basic_Insert : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Insert(void);
		virtual			~moXML_Basic_Insert();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);
	};

	class MO_DLL_EXPORT moXML_Basic_If : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_If(void);
		virtual			~moXML_Basic_If();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

		bool			GetCond(moXMLParser& parser, moXMLParser::moXMLType& type, bool& result);
		bool			SkipFalse(moXMLParser& parser, moXMLParser::moXMLTag& tag, bool maybe = false);

	private:
		unsigned long		f_state_count;
	};

	class MO_DLL_EXPORT moXML_Basic_Else : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Else(moXML_Basic_If& basic_if);
		virtual			~moXML_Basic_Else();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

	private:
		moXML_Basic_If *	f_basic_if;
	};

	class MO_DLL_EXPORT moXML_Basic_Elsif : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Elsif(moXML_Basic_If& basic_if);
		virtual			~moXML_Basic_Elsif();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

	private:
		moXML_Basic_If *	f_basic_if;
	};

	class MO_DLL_EXPORT moXML_Basic_Elif : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Elif(moXML_Basic_If& basic_if);
		virtual			~moXML_Basic_Elif();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

	private:
		moXML_Basic_If *	f_basic_if;
	};

	class MO_DLL_EXPORT moXML_Basic_Loop : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_Loop(void);
		virtual			~moXML_Basic_Loop();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

	private:
		class MO_DLL_EXPORT Counter
		{
		public:
			enum counter_type_t {
				COUNTER_UNDEFINED,
				COUNTER_INTEGER,
				COUNTER_FLOAT
			};

						Counter(void);
			void			LoadCounter(moXML_Basic::moXML_Basic_Loop *loop, moXMLParser& parser, moXMLParser::moXMLTag& tag, const moWCString& name);
			void			ForceInt(void);
			void			ForceFloat(void);
			void			SetToOne(const Counter& counter);
			void			Positive(void);
			void			Negative(void);
			moWCString		ToString(void) const;
			bool			IsUndefined(void) const
						{
							return f_type == COUNTER_UNDEFINED;
						}
			bool			IsInteger(void) const
						{
							return f_type == COUNTER_INTEGER;
						}
			bool			IsFloat(void) const
						{
							return f_type == COUNTER_FLOAT;
						}

			bool			operator < (long value) const;
			bool			operator < (const Counter& value) const;
			bool			operator == (long value) const;
			Counter&		operator -- (int);
			Counter&		operator += (const Counter& value);

		private:
			counter_type_t		f_type;
			long			f_int;
			double			f_float;
		};

		class MO_DLL_EXPORT LoopData : public moBase
		{
		public:
			Counter			f_from;
			Counter			f_to;
			Counter			f_step;
			Counter			f_repeat;
			long			f_count;
			moWords			f_for_each;
			moWCString		f_while;
			moWCString		f_until;
			moWCString		f_name;
			moWCString		f_data;
			bool			f_has_words;
			bool			f_interpret_variables;
		};
		typedef moSmartPtr<LoopData> LoopDataSPtr;

		bool			EvaluateLoopExpr(moXMLParser& parser, moXMLParser::moXMLTag *tag, LoopData *data, bool test_until);
		void			EvaluateNextLoop(moXMLParser& parser, LoopData *data);

		static const unsigned long STACK_MAX_COUNT = 5;
		unsigned long		f_count;
		LoopDataSPtr		f_data[STACK_MAX_COUNT];
		moList			f_stack;
	};

	class MO_DLL_EXPORT moXML_Basic_UseDTD : public moXMLParser::moXMLEvent
	{
	public:
					moXML_Basic_UseDTD(void);
		virtual			~moXML_Basic_UseDTD();

		virtual bool		OnEvent(moXMLParser& parser, moXMLParser::moXMLType& type);

	private:
		moArray			f_restore_usedtd;
	};

				moXML_Basic(void);
	virtual			~moXML_Basic();

	void			Attach(moXMLParser& xml);
	void			AddIncludePath(const moWCString& path);
	void			AppendIncludePath(const moListBase& path);

private:
	moXML_Basic_Set		f_set;
	moXML_Basic_Unset	f_unset;
	moXML_Basic_Replace	f_replace;
	moXML_Basic_Include	f_include;
	moXML_Basic_Insert	f_insert;
	moXML_Basic_If		f_if;
	moXML_Basic_Elif	f_elif;
	moXML_Basic_Elsif	f_elsif;
	moXML_Basic_Else	f_else;
	moXML_Basic_Loop	f_loop;
	moXML_Basic_UseDTD	f_usedtd;
};


};			// namespace molib;

// vim: ts=8 sw=8
#endif		// #ifndef MO_XML_BASIC_H

