#pragma once

namespace corgi
{
	// I know this is 64bits, but in might not always
	// be the case on every platform, so, if this ever happens, I'd just
	// have to put a ifdef depending on the current platform used, and change only this
	// thing, instead of changing it everywhere
	using int_64 = long long;
}
