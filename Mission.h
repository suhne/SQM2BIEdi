#pragma once //fuck you x-platformability, this is so much easier
#include <istream>
#include <ostream>
#include <vector>

#include "Intel.h"
#include "Group.h"

class Mission
{
	public:
		Mission();
		void DeserializeSQM(std::istream &in);
	private:
		Intel *m_intel;
		std::vector<Group*> m_groups;
};