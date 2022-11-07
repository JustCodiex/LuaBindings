#pragma once
#include "LuaState.h"
#include "LuaMarshal.h"

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
		static T DoString(LuaState^ state, System::String^ lStr) {
			if (!state->DoString(lStr)) {
				throw gcnew System::Exception(lStr);
			}
			return safe_cast<T>(LuaMarshal::MarshalStackValue(state->get_state(), -1));
			/*LuaType typ = state->Type();
			switch (typ) {
				case Lua::LuaType::Nil:
					return T();
				case Lua::LuaType::Boolean:
					return safe_cast<T>(state->GetBoolean());
				case Lua::LuaType::LightUserData:
					break;
				case Lua::LuaType::Number:
					return safe_cast<T>(state->GetNumber());
				case Lua::LuaType::String:
					return safe_cast<T>(state->GetString());
				case Lua::LuaType::Table:
					break;
				case Lua::LuaType::Function:
					break;
				case Lua::LuaType::UserData:
					break;
				case Lua::LuaType::Thread:
					break;
				default:
					break;
			}
			throw gcnew System::Exception(lStr);*/
		}

		generic <class T>
		[System::Runtime::CompilerServices::ExtensionAttribute]
		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="state"></param>
		/// <param name="name"></param>
		/// <returns></returns>
		static T GetGlobal(LuaState^ state, System::String^ name) {
			auto t = state->GetGlobal(name);
			return safe_cast<T>(LuaMarshal::MarshalStackValue(state->get_state(), t, -1));
			/*auto luaType = ;
			switch (luaType) {
				case Lua::LuaType::Nil:
					return T();
				case Lua::LuaType::Boolean:
					return safe_cast<T>(state->GetBoolean());
				case Lua::LuaType::LightUserData:
					break;
				case Lua::LuaType::Number:
					return safe_cast<T>(state->GetNumber());
				case Lua::LuaType::String:
					return safe_cast<T>(state->GetString());
				case Lua::LuaType::Table:
					break;
				case Lua::LuaType::Function:
					break;
				case Lua::LuaType::UserData:
					break;
				case Lua::LuaType::Thread:
					break;
				default:
					break;
			}
			throw gcnew System::Exception(name);*/
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
