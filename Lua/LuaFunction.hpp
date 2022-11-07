#pragma once

struct lua_State;

namespace Lua {

	ref class LuaState;

	/// <summary>
	/// Struct representing a lua function stored on the stack.
	/// </summary>
	public value class LuaFunction {
	
	public:

		System::Object^ Invoke(int resultValueCount, ... array<System::Object^>^ args);

		generic<class T>
		T Invoke(... array<System::Object^>^ args);

		bool InvokeProtected(int resultValueCount, [System::Runtime::InteropServices::OutAttribute] System::Object^% result, ... array<System::Object^>^ args);

		generic<class T>
		bool InvokeProtected([System::Runtime::InteropServices::OutAttribute] T% result, ... array<System::Object^>^ args);

		void Call(int argc, int retc);

		bool ProtectedCall(int argc, int retc);

		property bool IsCallable {
			bool get();
		}

		property int StackIndex {
			int get() { return this->iStackOffset; }
		}

	public:

		/// <summary>
		/// Create a <see cref="LuaFunction"/> instance from the current top stack value.
		/// </summary>
		/// <param name="L">The state to get function from.</param>
		/// <returns>The <see cref="LuaFunction"/> currently at the top of the stack.</returns>
		static LuaFunction FromTop(LuaState^ L);

	private:

		LuaFunction(lua_State* L, int offset) {
			this->L = L;
			this->iStackOffset = offset;
		}

	private:

		lua_State* L;
		int iStackOffset;

	internal:

		static LuaFunction from_top(lua_State* L, int idx);

	};

}
