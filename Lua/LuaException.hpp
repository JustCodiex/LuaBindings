#pragma once
#include "LuaType.h"

namespace Lua {

	/// <summary>
	/// Represents errors that occur during Lua execution.
	/// </summary>
	public ref class LuaException : public System::Exception {
	public:
		/// <summary>
		/// Initialise a new instance of the <see cref="LuaException"/> class with a default message. 
		/// </summary>
		LuaException() : System::Exception("Fatal Lua runtime exception occured") {}
		/// <summary>
		/// Initialise a new instance of the <see cref="LuaException"/> class with a specified message.
		/// </summary>
		/// <param name="message">The message clarifying the exception.</param>
		LuaException(System::String^ message) : System::Exception(message) {}
		/// <summary>
		/// Initialise a new instance of the <see cref="LuaException"/> class with a specified message and a reference to the inner <see cref="System::Exception"/> that
		/// caused the exception.
		/// </summary>
		/// <param name="message">The message clarifying the exception.</param>
		/// <param name="innerException">The inner <see cref="System::Exception"/> that triggered this <see cref="LuaException"/>.</param>
		LuaException(System::String^ message, System::Exception^ innerException) : System::Exception(message, innerException) {}
	};

	/// <summary>
	/// Represents errors that occur during the Lua runtime.
	/// </summary>
	public ref class LuaRuntimeException : public LuaException {
	public:
		/// <summary>
		/// Initialise a new instance of the <see cref="LuaRuntimeException"/> class with a default message. 
		/// </summary>
		LuaRuntimeException() : LuaException("Fatal Lua runtime exception occured") {}
		/// <summary>
		/// Initialise a new instance of the <see cref="LuaRuntimeException"/> class with a specified message.
		/// </summary>
		/// <param name="message">The message clarifying the exception.</param>
		LuaRuntimeException(System::String^ message) : LuaException(message) {}
		/// <summary>
		/// Initialise a new instance of the <see cref="LuaRuntimeException"/> class with a specified message and a reference to the inner <see cref="System::Exception"/> that
		/// caused the exception.
		/// </summary>
		/// <param name="message">The message clarifying the exception.</param>
		/// <param name="innerException">The inner <see cref="System::Exception"/> that triggered this <see cref="LuaException"/>.</param>
		LuaRuntimeException(System::String^ message, System::Exception^ innerException) : LuaException(message, innerException) {}
	};

	/// <summary>
	/// Represents errors that occure during Lua execution where one type was expected but another was found.
	/// </summary>
	public ref class LuaTypeExpectedException : public LuaException {
	private:
		LuaType ltFound;
		LuaType ltExpected;
	public:
		/// <summary>
		/// Get the expected type.
		/// </summary>
		property LuaType Expected {
			LuaType get() { return this->ltExpected; }
		}
		/// <summary>
		/// Get the found type that caused the <see cref="LuaTypeExpectedException"/>.
		/// </summary>
		property LuaType Found {
			LuaType get() { return this->ltFound; }
		}
	public:
		/// <summary>
		/// 
		/// </summary>
		LuaTypeExpectedException() : LuaException("Lua runtime found a different type than expected") {}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="found"></param>
		/// <param name="expected"></param>
		LuaTypeExpectedException(LuaType found, LuaType expected) 
			: LuaException(System::String::Format("Lua runtime found {0} when expecteing a {1}", found, expected)) {
			this->ltFound = found;
			this->ltExpected = expected;
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="message">The message clarifying the exception.</param>
		LuaTypeExpectedException(System::String^ message) : LuaException(message) {}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="found"></param>
		/// <param name="expected"></param>
		/// <param name="message">The message clarifying the exception.</param>
		LuaTypeExpectedException(LuaType found, LuaType expected, System::String^ message) : LuaException(message) {
			this->ltFound = found;
			this->ltExpected = expected;
		}
	};

}
