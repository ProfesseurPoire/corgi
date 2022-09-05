#pragma once

namespace corgi
{
	namespace TiledImporter
	{
		/*!
		 * @brief Read a file generated by Tiled into a comprehensible format
		 *
		 * @param input  : Path to the input tiled file
		 * @param output : Path where the importer should write the result of the import process
		 */
		void import(const char * input, const char * output);
	};
}