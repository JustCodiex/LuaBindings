#pragma once

// Convert a managed (C#) string into an unamanged C ANSII string.
#define __UnmanagedString(n,v,s) \
	System::IntPtr n = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(v); \
	const char* s = static_cast<const char*>(n.ToPointer())

// Free an unamanged C ANSII string that was retrived from managed (C#) space.
#define __UnmangedFreeString(n) System::Runtime::InteropServices::Marshal::FreeHGlobal(n)
