#pragma once

#include <string>
#include <sstream>

std::string license_gpl()
{
	std::stringstream ss;
	ss << "//===============================================================================" << std::endl;
	ss << "// Copyright (c) 2005-2013 by Made to Order Software Corporation" << std::endl;
	ss << "// " << std::endl;
	ss << "// All Rights Reserved." << std::endl;
	ss << "// " << std::endl;
	ss << "// The source code in this file (\"Source Code\") is provided by Made to Order Software Corporation" << std::endl;
	ss << "// to you under the terms of the GNU General Public License, version 2.0" << std::endl;
	ss << "// (\"GPL\").  Terms of the GPL can be found in doc/GPL-license.txt in this distribution." << std::endl;
	ss << "// " << std::endl;
	ss << "// By copying, modifying or distributing this software, you acknowledge" << std::endl;
	ss << "// that you have read and understood your obligations described above," << std::endl;
	ss << "// and agree to abide by those obligations." << std::endl;
	ss << "// " << std::endl;
	ss << "// ALL SOURCE CODE IN THIS DISTRIBUTION IS PROVIDED \"AS IS.\" THE AUTHOR MAKES NO" << std::endl;
	ss << "// WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY," << std::endl;
	ss << "// COMPLETENESS OR PERFORMANCE." << std::endl;
	ss << "//===============================================================================" << std::endl;
	ss << "" << std::endl;
	ss << "" << std::endl;
	return ss.str();
}
