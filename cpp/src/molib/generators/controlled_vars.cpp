//
// File:	generators/controlled_vars.c++
// Object:	Generates the mo_controlled.h header file
//
// Copyright:	Copyright (c) 2005-2008 Made to Order Software Corp.
//		All Rights Reserved.
//
//		This software and its associated documentation contains
//		proprietary, confidential and trade secret information
//		of Made to Order Software Corp. and except as provided by
//		written agreement with Made to Order Software Corp.
//
//		a) no part may be disclosed, distributed, reproduced,
//		   transmitted, transcribed, stored in a retrieval system,
//		   adapted or translated in any form or by any means
//		   electronic, mechanical, magnetic, optical, chemical,
//		   manual or otherwise,
//
//		and
//
//		b) the recipient is not entitled to discover through reverse
//		   engineering or reverse compiling or other such techniques
//		   or processes the trade secrets contained therein or in the
//		   documentation.
//
// Usage:
//
// This tool can be compiled with a very simple make controller_vars
// Then run it to get the mo_controlled.h file created as in:
//
// 	controlled_vars >include/mo_controlled.h
//

#include	<stdlib.h>
#include	<stdio.h>
#ifndef _MSC_VER
#include	<unistd.h>
#endif
#include	<string.h>

struct TYPES {
	const char	*name;
	const char	*short_name;
	const char	*condition;
};
typedef struct TYPES	types_t;

const types_t g_types[] =
{
	{ "bool", "bool", 0 }, /* this generates quite many problems as operator input */
	{ "char", "char", 0 },
	{ "signed char", "schar", 0 },
	{ "unsigned char", "uchar", 0 },
	{ "int16_t", "int16", 0 },
	{ "uint16_t", "uint16", 0 },
	{ "int32_t", "int32", 0 },
	{ "uint32_t", "uint32", 0 },
	{ "long", "plain_long", "#if UINT_MAX == ULONG_MAX" },
	{ "unsigned long", "plain_ulong", "#if UINT_MAX == ULONG_MAX" },
	{ "int64_t", "int64", 0 },
	{ "uint64_t", "uint64", 0 },
	{ "float", "float", 0 },
	{ "double", "double", 0 },
#ifndef MAC
	{ "long double", "longdouble" },
#endif
	{ "size_t", "size" },
	{ "time_t", "time" }
};
#define	TYPES_ALL	(sizeof(g_types) / sizeof(g_types[0]))

#ifdef MAC
/*
 * We have to make an exception for Mac OS/X because the types size_t and time_t are defined
 * as __darwin_size_t and __darwin_time_t respectively, and these are intrisic types. Otherwise,
 * you get ambiguity errors.
 */
#	define	TYPES_MAX	(TYPES_ALL)
#else
#	define	TYPES_MAX	(TYPES_ALL - 2)
#endif



#define	FLAG_HAS_VOID		0x00000001
#define	FLAG_HAS_DOINIT		0x00000002
#define	FLAG_HAS_INITFLG	0x00000004
#define	FLAG_HAS_DEFAULT	0x00000008

#define	FLAG_HAS_RETURN_T	0x00010000
#define	FLAG_HAS_RETURN_BOOL	0x00020000
#define	FLAG_HAS_NOINIT		0x00040000


struct OP_T {
	const char *name;
	long flags;
};
typedef struct OP_T	op_t;

const op_t generic_operators[] = {
	{ "=", FLAG_HAS_NOINIT },
	{ "*=", 0 },
	{ "/=", 0 },
	{ "%=", 0 },
	{ "+=", 0 },
	{ "-=", 0 },
	{ "<<=", 0 },
	{ ">>=", 0 },
	{ "&=", 0 },
	{ "|=", 0 },
	{ "^=", 0 },
	{ "*", FLAG_HAS_RETURN_T },
	{ "/", FLAG_HAS_RETURN_T },
	{ "%", FLAG_HAS_RETURN_T },
	{ "+", FLAG_HAS_RETURN_T },
	{ "-", FLAG_HAS_RETURN_T },
	{ "<<", FLAG_HAS_RETURN_T },
	{ ">>", FLAG_HAS_RETURN_T },
	{ "&", FLAG_HAS_RETURN_T },
	{ "|", FLAG_HAS_RETURN_T },
	{ "^", FLAG_HAS_RETURN_T },
	{ "==", FLAG_HAS_RETURN_BOOL },
	{ "!=", FLAG_HAS_RETURN_BOOL },
	{ "<", FLAG_HAS_RETURN_BOOL },
	{ "<=", FLAG_HAS_RETURN_BOOL },
	{ ">", FLAG_HAS_RETURN_BOOL },
	{ ">=", FLAG_HAS_RETURN_BOOL }
};
#define	GENERIC_OPERATORS_MAX	(sizeof(generic_operators) / sizeof(generic_operators[0]))



void create_operator(const char *name, const char *op, const char *type, long flags)
{
	const char *right;
	int direct;

	printf("\t");
	if((flags & FLAG_HAS_RETURN_BOOL) != 0) {
		printf("bool");
		direct = 1;
	}
	else if((flags & FLAG_HAS_RETURN_T) != 0) {
		printf("T");
		direct = 1;
	}
	else {
		printf("%s_init&", name);
		direct = 0;
	}
	printf(" operator %s (", op);
	if(type == 0) {
		printf("const %s_init& n", name);
		right = "n.f_value";
	}
	else {
		printf("%s value", type);
		right = "value";
	}
	printf(")\n");
	printf("\t{\n");
	if((flags & FLAG_HAS_INITFLG) != 0) {
		if((flags & FLAG_HAS_NOINIT) == 0) {
			printf("\t\tassert(f_initialized);\n");
		}
		else {
			printf("\t\tf_initialized = true;\n");
		}
		if(type == 0) {
			printf("\t\tassert(n.f_initialized);\n");
		}
	}
	if(direct) {
		printf("\t\treturn f_value %s %s;\n", op, right);
	}
	else {
		printf("\t\tf_value %s %s;\n", op, right);
		printf("\t\treturn *this;\n");
	}
	printf("\t}\n");
}

void create_all_operators(const char *name, long flags)
{
	const op_t	*op;
	unsigned long	o, t, f;

	for(o = 0; o < GENERIC_OPERATORS_MAX; ++o) {
		op = generic_operators + o;
		f = flags | op->flags;
		create_operator(name, op->name, 0, f);
		/* IMPORTANT:
		 *	here we skip the types:
		 *	bool
		 *	size_t
		 *	time_t
		 *
		 * ...except on MAC where size_t and time_t are typedeffed as
		 * __darwin_size_t and __darwin_time_t, respectively.
		 *
		 */
		for(t = 1; t < TYPES_MAX; ++t) {
			if(g_types[t].condition) {
				printf("%s\n", g_types[t].condition);
			}
			create_operator(name, op->name, g_types[t].name, f);
			if(g_types[t].condition) {
				printf("#endif\n");
			}
		}
	}
}


void create_unary_operators(const char *name, long flags)
{
	int		i;
	const char	*s;

	// NOTE: max i can be either 2 or 4
	//	 at this time, we don't want to have the T * operators
	//	 instead we'll have a set of Ptr() functions
	for(i = 0; i < 2; ++i) {
		printf("\toperator T%s ()%s\n",
				i & 2 ? " *" : "",
				i & 1 ? "" : " const");
		printf("\t{\n");
		// NOTE: we want to change the following test for T *
		//	 but it requires a reference!!!
		//	 (also, we use Ptr() instead for now)
		if((flags & FLAG_HAS_INITFLG) != 0) {
			printf("\t\tassert(f_initialized);\n");
		}
		printf("\t\treturn %sf_value;\n", i & 2 ? "&" : "");
		printf("\t}\n");
	}

	for(i = 0; i < 2; ++i) {
		s = i & 1 ? "" : "const";
		printf("\t%s T * Ptr() %s\n", s, s);
		printf("\t{\n");
		// NOTE: we want to change the following test for T *
		//	 but it requires a reference!!!
		if((flags & FLAG_HAS_INITFLG) != 0) {
			printf("\t\tassert(f_initialized);\n");
		}
		printf("\t\treturn &f_value;\n");
		printf("\t}\n");
	}

	printf("\tbool operator ! () const\n");
	printf("\t{\n");
	if((flags & FLAG_HAS_INITFLG) != 0) {
		printf("\t\tassert(f_initialized);\n");
	}
	printf("\t\treturn !f_value;\n");
	printf("\t}\n");

	static const char *op = "~+-";
	for(i = 0; i < 3; ++i) {
		printf("\tT operator %c () const\n", op[i]);
		printf("\t{\n");
		if((flags & FLAG_HAS_INITFLG) != 0) {
			printf("\t\tassert(f_initialized);\n");
		}
		printf("\t\treturn %cf_value;\n", op[i]);
		printf("\t}\n");
	}

	// NOTE: operator ++/-- () -> ++/--var
	//	 operator ++/-- (int)  -> var++/--
	for(i = 0; i < 4; ++i) {
		printf("\t%s_init%s operator %s (%s)\n",
			name,
			i & 1 ? "" : "&",
			i & 2 ? "--" : "++",
			i & 1 ? "int" : "void");
		printf("\t{\n");
		if((flags & FLAG_HAS_INITFLG) != 0) {
			printf("\t\tassert(f_initialized);\n");
		}
		if(i & 1) {
			printf("\t\t%s_init<T> result(*this);\n", name);
		}
		printf("\t\t%sf_value;\n", i & 2 ? "--" : "++");
		if(i & 1) {
			printf("\t\treturn result;\n");
		}
		else {
			printf("\t\treturn *this;\n");
		}
		printf("\t}\n");
	}
}


void create_typedef(const char *name, const char *short_name, long flags)
{
	const char	*t;
	unsigned int	idx;

	// here we include the size_t and time_t types
	for(idx = 0; idx < TYPES_ALL; ++idx) {
		t = g_types[idx].name;
		if((strcmp(t, "float") == 0 || strcmp(t, "double") == 0 || strcmp(t, "long double") == 0)) {
			if(strcmp(name, "auto") == 0) {
				continue;
			}
		}
		else {
			if(strcmp(name, "fauto") == 0) {
				continue;
			}
		}
		if(g_types[idx].condition) {
			printf("%s\n", g_types[idx].condition);
		}
		printf("typedef %s_init<%s>\t%s%s_t;\n", name, t, short_name, g_types[idx].short_name);
		if(g_types[idx].condition) {
			printf("#endif\n");
		}
	}
}


void create_class(const char *name, const char *short_name, long flags)
{
	unsigned int	idx;
	const char 	*init;

	if((flags & FLAG_HAS_DEFAULT) != 0) {
		printf("template<class T, T init_value = 0>\n");
		init = "\t\tf_value = init_value;\n";
	}
	else {
		printf("template<class T>\n");
		init = "\t\tf_value = 0.0;\n";
	}
	printf("class %s_init\n", name);
	printf("{\n");
	printf("public:\n");
	printf("\ttypedef T\tprimary_type_t;\n");

	// Constructors
	if((flags & FLAG_HAS_VOID) != 0) {
		printf("\t%s_init()\n\t{\n%s%s\t}\n", name,
			(flags & FLAG_HAS_DOINIT) != 0 ? init : "",
			(flags & FLAG_HAS_INITFLG) != 0 ? "\t\tf_initialized = false;\n" : "");
	}
	// we don't want the bool type in the constructors...
	// it creates some problems
	// here we exclude the bool, size_t and time_t types
	for(idx = 1; idx < TYPES_MAX; ++idx) {
		if(g_types[idx].condition) {
			printf("%s\n", g_types[idx].condition);
		}
		printf("\t%s_init(%s value)\n", name, g_types[idx].name);
		printf("\t{\n");
		if((flags & FLAG_HAS_INITFLG) != 0) {
			printf("\t\tf_initialized = true;\n");
		}
		// The static cast is nice to have with cl which otherwise generates
		// warnings about values being truncated all over the place.
		printf("\t\tf_value = static_cast<primary_type_t>(value);\n");
		printf("\t}\n");
		if(g_types[idx].condition) {
			printf("#endif\n");
		}
	}

	// Unary operators
	create_unary_operators(name, flags);

	// Binary Operators
	create_all_operators(name, flags);

	printf("\n");
	printf("#ifdef MO_DEBUG\n");
	printf("\tbool is_initialized() const\n");
	printf("\t{\n");
	if((flags & FLAG_HAS_INITFLG) != 0) {
		printf("\t\treturn f_initialized;\n");
	}
	else {
		printf("\t\treturn true;\n");
	}
	printf("\t}\n");
	printf("#endif\n");

	printf("\n");
	printf("private:\n");
	if((flags & FLAG_HAS_INITFLG) != 0) {
		printf("\tbool f_initialized;\n");
	}
	printf("\tT f_value;\n");
	printf("};\n");
	printf("\n");

	create_typedef(name, short_name, flags);
}


void create_direct_typedef(const char *name, const char *short_name)
{
	unsigned int	idx;

	// here we include the bool, size_t and time_t types
	for(idx = 0; idx < TYPES_ALL; ++idx) {
		if(g_types[idx].condition) {
			printf("%s\n", g_types[idx].condition);
		}
		printf("typedef %s\t%s%s_t;\n", g_types[idx].name, short_name, g_types[idx].short_name);
		if(g_types[idx].condition) {
			printf("#endif\n");
		}
	}
}


int main(int argc, char *argv[])
{
	printf("// WARNING: do not edit; this is an auto-generated\n");
	printf("// WARNING: file; please, use the generator named\n");
	printf("// WARNING: controlled_vars.c to re-generate\n");
	printf("//\n");
	printf("//\n");
	printf("#include <limits.h>\n");
	printf("#include \"mo_export.h\"\n");
	printf("#ifdef _MSC_VER\n");
	printf("#pragma warning(push)\n");
	printf("#pragma warning(disable: 4800 4244 4018)\n");
	printf("#endif\n");
	printf("#include <sys/types.h>\n");

	create_class("auto", "z", FLAG_HAS_VOID | FLAG_HAS_DOINIT | FLAG_HAS_DEFAULT);
	printf("\n");
	create_class("fauto", "z", FLAG_HAS_VOID | FLAG_HAS_DOINIT);
	printf("\n");
	create_class("need", "m", 0);
	printf("\n");
	printf("#ifdef MO_DEBUG\n");
	create_class("no", "r", FLAG_HAS_VOID | FLAG_HAS_INITFLG);
	printf("#else\n");
	create_direct_typedef("no", "r");
	printf("#endif\n");
	printf("#ifdef _MSC_VER\n");
	printf("#pragma warning(pop)\n");
	printf("#endif\n");
	printf("\n");

	return 0;
}


