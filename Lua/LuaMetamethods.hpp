#pragma once

namespace Lua {

	/// <summary>
	/// Enum representing all available metamethods.
	/// </summary>
	public enum class LuaMetamethod {

		/// <summary>
		/// 
		/// </summary>
		Index, 

		/// <summary>
		/// 
		/// </summary>
		NewIndex,

		/// <summary>
		/// 
		/// </summary>
		GC, 

		/// <summary>
		/// 
		/// </summary>
		Mode, 

		/// <summary>
		/// 
		/// </summary>
		Length, 

		/// <summary>
		/// 
		/// </summary>
		Equal,

		/// <summary>
		/// 
		/// </summary>
		Addition, 

		/// <summary>
		/// 
		/// </summary>
		Subtraction, 

		/// <summary>
		/// 
		/// </summary>
		Multiplication, 

		/// <summary>
		/// 
		/// </summary>
		Modulo, 

		/// <summary>
		/// 
		/// </summary>
		Power,

		/// <summary>
		/// 
		/// </summary>
		Division, 

		/// <summary>
		/// 
		/// </summary>
		IntegerDivisiion,

		/// <summary>
		/// 
		/// </summary>
		BinaryAnd,

		/// <summary>
		/// 
		/// </summary>
		BinaryOr, 

		/// <summary>
		/// 
		/// </summary>
		BinaryXor, 

		/// <summary>
		/// 
		/// </summary>
		ShiftLeft, 

		/// <summary>
		/// 
		/// </summary>
		ShiftRight,

		/// <summary>
		/// 
		/// </summary>
		UnaryMinus,

		/// <summary>
		/// 
		/// </summary>
		BinaryNot,

		/// <summary>
		/// 
		/// </summary>
		LessThan,

		/// <summary>
		/// 
		/// </summary>
		LessEqual,

		/// <summary>
		/// 
		/// </summary>
		Concat,

		/// <summary>
		/// 
		/// </summary>
		Call,

		/// <summary>
		/// 
		/// </summary>
		Close

	};

	[System::Runtime::CompilerServices::ExtensionAttribute]
	public ref class LuaMetamethods abstract sealed {
	public:

		/// <summary>
		/// Represents the constant string for the metatable "__add" operation, overriding the <c>+</c> operator when used on value with a metatable defining a "__add" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__add</b> takes two arguments:
		/// <c>
		/// __add = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_ADD = "__add";

		/// <summary>
		/// Represents the constant string for the metatable "__sub" operation, overriding the <c>-</c> operator when used on value with a metatable defining a "__sub" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__sub</b> takes two arguments:
		/// <c>
		/// __sub = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_SUB = "__sub";

		/// <summary>
		/// Represents the constant string for the metatable "__mul" operation, overriding the <c>*</c> operator when used on value with a metatable defining a "__mul" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__mul</b> takes two arguments:
		/// <c>
		/// __mul = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_MUL = "__mul";

		/// <summary>
		/// Represents the constant string for the metatable "__div" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__div" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__div</b> takes two arguments:
		/// <c>
		/// __div = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_DIV = "__div";

		/// <summary>
		/// Represents the constant string for the metatable "__idiv" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__idiv" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__idiv</b> takes two arguments:
		/// <c>
		/// __idiv = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_IDIV = "__idiv";

		literal System::String^ METATABLE_INDEX = "__index";
		literal System::String^ METATABLE_NEWINDEX = "__newindex";
		literal System::String^ METATABLE_GC = "__gc";
		
		/// <summary>
		/// Represents the constant string for the metatable field "__mode". Sets the weak state of a table's key set or value set by denoting 'k' for keys
		/// and 'v' for values.
		/// </summary>
		literal System::String^ METATABLE_MODE = "__mode";

		literal System::String^ METATABLE_LEN = "__len";
		literal System::String^ METATABLE_EQ = "__eq";
		literal System::String^ METATABLE_MOD = "__mod";
		literal System::String^ METATABLE_POW = "__pow";
		literal System::String^ METATABLE_BAND = "__band";
		literal System::String^ METATABLE_BOR = "__bor";
		literal System::String^ METATABLE_BXOR = "__bxor";
		literal System::String^ METATABLE_SHL = "__shl";
		literal System::String^ METATABLE_SHR = "__shr";
		literal System::String^ METATABLE_UNM = "__unm";
		literal System::String^ METATABLE_BNOT = "__bnot";
		literal System::String^ METATABLE_LT = "__lt";
		literal System::String^ METATABLE_LE = "__le";
		literal System::String^ METATABLE_CONCAT = "__concat";
		literal System::String^ METATABLE_CALL = "__call";
		literal System::String^ METATABLE_CLOSE = "__close";

	public:

		[System::Runtime::CompilerServices::ExtensionAttribute]
		static System::String^ ToString(LuaMetamethod meta) {
			switch (meta)
			{
				case Lua::LuaMetamethod::Index:
					return METATABLE_INDEX;
				case Lua::LuaMetamethod::NewIndex:
					return METATABLE_NEWINDEX;
				case Lua::LuaMetamethod::GC:
					return METATABLE_GC;
				case Lua::LuaMetamethod::Mode:
					return METATABLE_MODE;
				case Lua::LuaMetamethod::Length:
					return METATABLE_LEN;
				case Lua::LuaMetamethod::Equal:
					return METATABLE_EQ;
				case Lua::LuaMetamethod::Addition:
					return METATABLE_ADD;
				case Lua::LuaMetamethod::Subtraction:
					return METATABLE_SUB;
				case Lua::LuaMetamethod::Multiplication:
					return METATABLE_MUL;
				case Lua::LuaMetamethod::Modulo:
					return METATABLE_MOD;
				case Lua::LuaMetamethod::Power:
					return METATABLE_POW;
				case Lua::LuaMetamethod::Division:
					return METATABLE_DIV;
				case Lua::LuaMetamethod::IntegerDivisiion:
					return METATABLE_IDIV;
				case Lua::LuaMetamethod::BinaryAnd:
					return METATABLE_BAND;
				case Lua::LuaMetamethod::BinaryOr:
					return METATABLE_BOR;
				case Lua::LuaMetamethod::BinaryXor:
					return METATABLE_BXOR;
				case Lua::LuaMetamethod::ShiftLeft:
					return METATABLE_SHL;
				case Lua::LuaMetamethod::ShiftRight:
					return METATABLE_SHR;
				case Lua::LuaMetamethod::UnaryMinus:
					return METATABLE_UNM;
				case Lua::LuaMetamethod::BinaryNot:
					return METATABLE_BNOT;
				case Lua::LuaMetamethod::LessThan:
					return METATABLE_LT;
				case Lua::LuaMetamethod::LessEqual:
					return METATABLE_LE;
				case Lua::LuaMetamethod::Concat:
					return METATABLE_CONCAT;
				case Lua::LuaMetamethod::Call:
					return METATABLE_CALL;
				case Lua::LuaMetamethod::Close:
					return METATABLE_CLOSE;
				default:
					throw gcnew System::NotImplementedException();
			}
		}

	internal:

		/*const char* to_name(LuaMetamethod meta) {
			return static_cast<const char*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ToMetaname(meta)).ToPointer());
		}*/

	};

}
