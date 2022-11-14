#pragma once
#include "LuaMetamethods.hpp"

namespace Lua {

	/// <summary>
	/// Attribute marking a method as visible in the Lua environment.
	/// </summary>
	[System::AttributeUsageAttribute(System::AttributeTargets::Method, AllowMultiple = false)]
	public ref class LuaFunctionAttribute : public System::Attribute {
	public:
		/// <summary>
		/// Get or set the Lua name for the method. If <see langword="null"/> or empty, the method name is used.
		/// </summary>
		property System::String^ Name;
	};

	/// <summary>
	/// Attribute marking a property as visible in the Lua environment.
	/// </summary>
	[System::AttributeUsageAttribute(System::AttributeTargets::Property, AllowMultiple = false)]
	public ref class LuaFieldAttribute : public System::Attribute {
	public:
		/// <summary>
		/// Get or set the Lua name for the property. If <see langword="null"/> or empty, the property name is used.
		/// </summary>
		property System::String^ Name;
		/// <summary>
		/// Get or set if Lua can only read the value.
		/// </summary>
		property bool Readonly;
		/// <summary>
		/// Get or set if Lua can only write to the value.
		/// </summary>
		property bool Writonly;
	};

	/// <summary>
	/// Attribute marking a method as a special metamethod.
	/// </summary>
	[System::AttributeUsageAttribute(System::AttributeTargets::Method, AllowMultiple = false)]
	public ref class LuaMetamethodAttribute : public System::Attribute {
	public:
		/// <summary>
		/// Get the <see cref="LuaMetamethod"/> the method should define behaviour of.
		/// </summary>
		property LuaMetamethod Method {
			LuaMetamethod get() { return this->m_method; }
		}
	public:
		/// <summary>
		/// Initialise a new <see cref="LuaMetamethodAttribute"/> attribute with the specified <see cref="LuaMetamethod"/>.
		/// </summary>
		/// <param name="method">The metamethod to define behaviour for.</param>
		LuaMetamethodAttribute(LuaMetamethod method) {
			this->m_method = method;
		}
	private:
		LuaMetamethod m_method;
	};

}
