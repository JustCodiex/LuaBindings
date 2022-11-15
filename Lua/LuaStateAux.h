#pragma once
#include "LuaState.h"
#include "LuaMarshal.h"
#include "LuaException.hpp"

namespace Lua {

	[System::Runtime::CompilerServices::ExtensionAttribute]
	/// <summary>
	/// Static extension class that adds additional functions to the <see cref="LuaState"/> class.
	/// </summary>
	public ref class LuaStateAux abstract sealed {
	
	public:

		generic<class T>
		[System::Runtime::CompilerServices::ExtensionAttribute]
		/// <summary>
		/// Load and run a string of Lua code and retrieve the .NET result.
		/// </summary>
		/// <typeparam name="T">The .NET type to retrieve from the result.</typeparam>
		/// <param name="state">The state to retrieve run code with.</param>
		/// <param name="lStr">The lua string to load and run.</param>
		/// <returns>The resulting .NET type.</returns>
		/// <exception cref="LuaRuntimeException"/>
		static T DoString(LuaState^ state, System::String^ lStr) {
			if (state->DoString(lStr) != CallResult::Ok) {
				const char* pErrStr = lua_tostring(state->get_state(), -1);
				System::String^ str = System::Runtime::InteropServices::Marshal::PtrToStringAnsi(static_cast<System::IntPtr>(const_cast<char*>(pErrStr)));
				throw gcnew LuaRuntimeException(str);
			}
			try {
				return safe_cast<T>(LuaMarshal::MarshalStackValue(state->get_state(), -1));
			} catch (System::InvalidCastException^ icex) {
				System::String^ err = System::String::Format("Lua return value type {0} cannot be cast to managed type {1}.", state->Type(), T::typeid->FullName);
				throw gcnew LuaRuntimeException(err, icex);
			}
			return T();
		}

		generic <class T>
		[System::Runtime::CompilerServices::ExtensionAttribute]
		/// <summary>
		/// Get the .NET value of the global Lua variable.
		/// </summary>
		/// <typeparam name="T">The .NET type to convert Lua type into.</typeparam>
		/// <param name="state">The lua state to get global variable from.</param>
		/// <param name="name">The name of the global variable.</param>
		/// <returns>A .NET instance representing the global variable.</returns>
		/// <exception cref="LuaRuntimeException"/>
		static T GetGlobal(LuaState^ state, System::String^ name) {
			auto t = state->GetGlobal(name);
			try {
				return safe_cast<T>(LuaMarshal::MarshalStackValue(state->get_state(), t, -1));
			} catch (System::InvalidCastException^ icex) {
				System::String^ err = System::String::Format("Global lua type {0} cannot be cast to managed type {1}.", t, T::typeid->FullName);
				throw gcnew LuaRuntimeException(err, icex);
			}
			return T();
		}

		[System::Runtime::CompilerServices::ExtensionAttribute]
		/// <summary>
		/// Push a C# function on to the Lua stack and give it a global name.
		/// </summary>
		/// <param name="state">The lua state to push the function into.</param>
		/// <param name="name">The global name of the function.</param>
		/// <param name="function">The C# function to push into the Lua domain.</param>
		static void PushCSharpFunction(LuaState^ state, System::String^ name, Lua::LuaFunctionDelegate^ function) {
			state->PushCSharpFunction(function);
			state->SetGlobal(name);
		}

	};

}
