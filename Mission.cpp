#include "Mission.h"
#include "StringFuncs.h"
#include <iostream>

Mission::Mission() : m_intel(NULL)
{
}

void Mission::DeserializeSQM(std::istream &in)
{
	SeekToPhrase("class Mission", in);
	SeekToPhrase("{", in);
	while(!in.eof())
	{
		std::string strLine;
		std::getline(in, strLine);
		strLine = StringReplace(strLine, "\x09", "");
		strLine = StringReplace(strLine, "\r", "");
		strLine = StringReplace(strLine, "\n", "");
		//prepare for the madness
		if(strLine == "addOns[]=")
		{
			while(RemoveWhitespace(strLine) != "};") //skip it for now
			{
				std::getline(in, strLine);
			}
		}
		else if(strLine == "addOnsAuto[]=")
		{
			while(RemoveWhitespace(strLine) != "};") //same thing
			{
				std::getline(in, strLine);
			}
		}
		else if(strLine == "class Intel")
		{
			if(m_intel != NULL)
			{
				std::cout << "WARNING: DUPLICATE INTEL";
			}
			else
			{
				m_intel = new Intel;
			}
			m_intel->DeserializeSQM(in);
		}
		else if(strLine == "class Groups")
		{
			//parser-ception
			std::getline(in, strLine);//{
			std::getline(in, strLine);
			strLine = StringReplace(strLine, "\x09", "");
			strLine = StringReplace(strLine, "items=", "");
			strLine = StringReplace(strLine, ";", "");
			m_groups.reserve(atoi(strLine.c_str()));
			unsigned int scope = 1;
			while(scope != 0)
			{
				std::getline(in, strLine);
				scope += CharCount(strLine, '{');
				scope -= CharCount(strLine, '}');
				strLine = StringReplace(strLine, "\x09", "");
				if(strLine.find("class Item") != std::string::npos)
				{
					m_groups.resize(m_groups.size()+1);
					static unsigned short id = 0;
					Group *group = new Group(id);
					id++;
					group->DeserializeSQM(in);
					m_groups[m_groups.size()-1] = group;
				}
			}
		}
		else if(strLine == "class Vehicles")
		{
			std::getline(in, strLine);//{
			std::getline(in, strLine);
			strLine = StringReplace(strLine, "\x09", "");
			strLine = StringReplace(strLine, "items=", "");
			strLine = StringReplace(strLine, ";", "");
			m_vehicles.reserve(atoi(strLine.c_str()));
			unsigned int scope = 1;
			while(scope != 0)
			{
				std::getline(in, strLine);
				scope += CharCount(strLine, '{');
				scope -= CharCount(strLine, '}');
				strLine = StringReplace(strLine, "\x09", "");
				if(strLine.find("class Item") != std::string::npos)
				{
					m_vehicles.resize(m_vehicles.size()+1);
					static unsigned short id = 0;
					Vehicle *vehicle = new Vehicle(id);
					id++;
					vehicle->DeserializeSQM(in);
					m_vehicles[m_vehicles.size()-1] = vehicle;
				}
			}
		}
		else if(strLine == "class Markers")
		{
			std::getline(in, strLine);//{
			std::getline(in, strLine);
			strLine = StringReplace(strLine, "\x09", "");
			strLine = StringReplace(strLine, "items=", "");
			strLine = StringReplace(strLine, ";", "");
			m_markers.reserve(atoi(strLine.c_str()));
			unsigned int scope = 1;
			while(scope != 0)
			{
				std::getline(in, strLine);
				scope += CharCount(strLine, '{');
				scope -= CharCount(strLine, '}');
				strLine = StringReplace(strLine, "\x09", "");
				if(strLine.find("class Item") != std::string::npos)
				{
					m_markers.resize(m_markers.size()+1);
					static unsigned short id = 0;
					Marker *marker = new Marker(id);
					id++;
					marker->DeserializeSQM(in);
					m_markers[m_markers.size()-1] = marker;
				}
			}
		}
	}
}

void Mission::SerializeBiEdi(std::ostream &out)
{
	out << "class _prefix_0" << std::endl << "{" << std::endl << "	objectType=\"prefix\";"
		<< std::endl << "	class Arguments" << std::endl << "	{" << std::endl
		<< "	};" << std::endl << "};" << std::endl;

	if(m_intel != NULL)
	{
		m_intel->SerializeBiEdi(out);
	}

	for(size_t i = 0; i < Center::GetCenters().size(); i++)
	{
		out << "class _center_" << i << std::endl
			<< "{" << std::endl
			<< "	objectType=\"center\";" << std::endl
			<< "	class Arguments" << std::endl
			<< "	{" << std::endl
			<< "		SIDE=\"" << Center::GetCenters()[i]->GetSide() << "\";" << std::endl
			<< "	};" << std::endl
			<< "};" << std::endl;
	}

	for(size_t i = 0; i < m_groups.size(); i++)
	{
		m_groups[i]->SerializeBiEdi(out);
	}

	for(size_t i = 0; i < m_vehicles.size(); i++)
	{
		m_vehicles[i]->SerializeBiEdi(out);
	}

	for(size_t i = 0; i < m_markers.size(); i++)
	{
		m_markers[i]->SerializeBiEdi(out);
	}

	out << "class _postfix_0" << std::endl
		<< "{" << std::endl
		<< "	objectType=\"postfix\";" << std::endl
		<< "	class Arguments" << std::endl
		<< "	{" << std::endl
		<< "	};" << std::endl
		<< "};" << std::endl;
}