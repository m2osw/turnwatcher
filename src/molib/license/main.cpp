#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "license.h"

using namespace std;

bool process_file( const string& filename )
{
	ifstream project;
	project.open( filename.c_str() );
	if( !project.good() )
	{
		std::cerr << "Project: Cannot open '" << filename << "' for reading!" << std::endl;
		return false;
	}
    //
	vector<string> project_lines;
	vector<string> output_lines;

	bool first_time_comments = true;
    int line_num = -1;
	while( !project.eof() )
	{
        ++line_num;
        string line;
		getline( project, line );

		if( first_time_comments && (line.substr( 0, 2 ) == "//") )
		{
			// throw these away
		}
		else
		{
			first_time_comments = false;
			output_lines.push_back( line );
		}
	}
	project.close();

	// Overwrite the file, truncate it, and write out the modified file with the new license.
	//
	ofstream out_proj;
	out_proj.open( filename.c_str(), ios_base::out | ios_base::trunc );
	out_proj << license_gpl() << endl;
	for( const string& line : output_lines )
	{
		out_proj << line << endl;
	}
	out_proj.close();
}


int main( int argc, char* argv[] )
{
	if( argc != 2 )
	{
		cerr << "usage: " << argv[0] << " [filename]" << endl;
		return 1;
	}

	process_file( argv[1] );

	return 0;
}

