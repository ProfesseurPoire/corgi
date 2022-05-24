#pragma once

namespace corgi
{
	struct Resource
	{
		virtual ~Resource(){}

		virtual int memory_usage()
		{
			return 0;
		}
	};
}
