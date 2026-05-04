//===============================================================================
// File:	src/tw-setup.c
// Object:	Turn Watcher Gtk/Pango setup
//
// Copyright:	Copyright (c) 2006-2008 Made to Order Software Corp.
//
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
//===============================================================================


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<windows.h>


 


typedef struct pipe_handleTAG
{
	HANDLE			f_in;
	HANDLE			f_out;
	char *			f_cmd;
	PROCESS_INFORMATION	f_proc_info;
} pipe_handle;


FILE *mo_popen(const char *command, const char *flags)
{ 
	SECURITY_ATTRIBUTES	attr;
	STARTUPINFO		start_info;
	BOOL			func;
	int			len;
	pipe_handle		handle, *pipe;

// Create a pipe for the child process's STDOUT. 
// TODO: test flags to know how to do manage the pipes...

// Set the bInheritHandle flag so pipe handles are inherited. 
	memset(&attr, 0, sizeof(attr));
	attr.nLength = sizeof(attr); 
	attr.bInheritHandle = TRUE; 

	if(!CreatePipe(&handle.f_in, &handle.f_out, &attr, 0)) {
		return 0;
	}

// Ensure the read handle to the pipe for STDOUT is not inherited.
	SetHandleInformation(handle.f_in, HANDLE_FLAG_INHERIT, 0);

// Create a pipe for the child process's STDIN.
//	if (! CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) 
//		ErrorExit("Stdin pipe creation failed\n"); 

// Ensure the write handle to the pipe for STDIN is not inherited. 
//	SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0);


// Now create the child process. 
// Set up members of the PROCESS_INFORMATION structure.
	memset(&handle.f_proc_info, 0, sizeof(handle.f_proc_info));

// Set up members of the STARTUPINFO structure. 
	memset(&start_info, 0, sizeof(start_info));
	start_info.cb = sizeof(start_info); 
	start_info.hStdError  = handle.f_out;
	start_info.hStdOutput = handle.f_out;
	start_info.dwFlags    = STARTF_USESTDHANDLES;

// Create the child process. 
	len = strlen(command);
	handle.f_cmd = malloc(len + 256);
	strcpy(handle.f_cmd, command);
	if(!CreateProcess(
			NULL,
			handle.f_cmd,	// command line
			NULL,		// process security attributes
			NULL,		// primary thread security attributes
			TRUE,		// handles are inherited
			0,		// creation flags
			NULL,		// use parent's environment
			NULL,		// use parent's current directory
			&start_info,	// STARTUPINFO pointer
			&handle.f_proc_info)) {	// receives PROCESS_INFORMATION
		return 0;
	}
	WaitForInputIdle(handle.f_proc_info.hProcess, 2000);
	//CloseHandle(handle.f_proc_info.hProcess);
	//CloseHandle(handle.f_proc_info.hThread);

	pipe = malloc(sizeof(pipe_handle));
	memcpy(pipe, &handle, sizeof(handle));

	return (FILE *) pipe;
}

#if 0
VOID WriteToPipe(VOID) 
{ 
	DWORD dwRead, dwWritten; 
	CHAR chBuf[BUFSIZE]; 

	// Read from a file and write its contents to a pipe. 

	for (;;) 
	{ 
		if (! ReadFile(hInputFile, chBuf, BUFSIZE, &dwRead, NULL) || 
				dwRead == 0) break; 
		if (! WriteFile(hChildStdinWr, chBuf, dwRead, 
					&dwWritten, NULL)) break; 
	} 

	// Close the pipe handle so the child process stops reading. 

	if (! CloseHandle(hChildStdinWr)) 
		ErrorExit("Close pipe failed\n"); 
} 
#endif
 

int mo_fread(void *buf, int nelem, int size, FILE *f) 
{
	DWORD		total, size_read;
	pipe_handle	*pipe = (pipe_handle *) f;
	int		max;

// We HAVE to close this handle for the pipe to work (go figure)
	CloseHandle(pipe->f_out);
	pipe->f_out = 0;

// Read output from the child process, and write to parent's STDOUT. 
	total = 0;
	max = nelem * size;
	while(max > 0) {
		if(!ReadFile(pipe->f_in, buf, max, &size_read, NULL)) {
			if(GetLastError() == ERROR_BROKEN_PIPE) {
				// we're done!
				return total == 0 ? -1 : total;
			}
			if(GetLastError() != ERROR_MORE_DATA) {
				return -1;
			}
			Sleep(100);
		}
		else if(size_read == 0) {
			Sleep(100);
		}
		max -= size_read;
		total += size_read;
		buf = (char *) buf + size_read;
	}

	return total / nelem;
} 


int mo_pclose(FILE *f)
{
	pipe_handle	*pipe = (pipe_handle *) f;

	//WaitForInputIdle(pipe->f_proc_info.hProcess, 2000);
	//CloseHandle(pipe->f_proc_info.hProcess);

	if(pipe->f_cmd) {
		free(pipe->f_cmd);
	}
	if(pipe->f_in) {
		CloseHandle(pipe->f_in);
	}
	if(pipe->f_out) {
		CloseHandle(pipe->f_out);
	}

	return 0;
}
 



/* Ensure that gtk & pango can find all of their modules */
int WINAPI WinMain(HINSTANCE h, HINSTANCE unused, LPSTR name, int nshow)
{
	static const char *in_out[] = {
		"pango-querymodules.exe", "pango\\pango.modules",
		"gtk-query-immodules-2.0.exe", "gtk-2.0\\gtk.immodules",
		"gdk-pixbuf-query-loaders.exe", "gtk-2.0\\gdk-pixbuf.loaders"
	};
	char	*command_line, c;
	char	cmd[1024];
	char	out[1024];
	char	*buf;
	int	idx, size, verbose;
	FILE	*f, *o;

	command_line = GetCommandLine();

	// Skip argv[0] (command name)
	if(*command_line == '\"') {
		command_line++;
		while(*command_line != 0 && *command_line != '\"') {
			command_line++;
		}
		if(*command_line == '\"') {
			command_line++;
		}
	}
	else {
		while(*command_line != 0 && *command_line != ' ') {
			command_line++;
		}
	}
	while(*command_line == ' ') {
		command_line++;
	}


	if(command_line[0] == '-' && command_line[1] == 'v' && command_line[2] == ' ') {
		command_line += 3;
		verbose = 1;
	}
	else {
		verbose = 0;
	}

	cmd[sizeof(cmd) - 1] = '\0';
	out[sizeof(out) - 1] = '\0';
	buf = malloc(1024 * 128);		// 128Kb should be enough!

	for(idx = 0; idx < 6; idx += 2) {
		snprintf(cmd, sizeof(cmd) - 1, "%s\\bin\\%s",
				command_line,
				in_out[idx + 0]);
		f = fopen(cmd, "rb+");
		if(f == 0) {
			continue;
		}
		fseek(f, 0xDC, SEEK_SET);
		c = 2;
		fwrite(&c, 1, 1, f);
		fclose(f);
		f = mo_popen(cmd, "r");
		if(verbose) {
			printf("Running [%s] (%p)\n", cmd, f);
		}
		if(f != NULL) {
			snprintf(out, sizeof(out) - 1, "%s\\etc\\%s",
					command_line,
					in_out[idx + 1]);
			o = fopen(out, "wb");
			if(verbose) {
				printf("Saving in [%s] (%p)\n", out, o);
			}
			if(o != NULL) {
				// read as much as possible at once
				for(;;) {
					size = mo_fread(buf, 1, 1024 * 128, f);
					if(size < 0) {
						break;
					}
					fwrite(buf, 1, size, o);
				}
				fclose(o);
			}
			mo_pclose(f);
		}
	}

	exit(0);
}

