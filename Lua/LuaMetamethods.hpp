#pragma once
#include "LuaException.hpp"

namespace Lua {

	/// <summary>
	/// Enum representing all available metamethods.
	/// </summary>
	public enum class LuaMetamethod {

		/// <summary>
		/// Provides overload to value indexing:
		/// <code>
		/// t[k];
		/// t.k;
		/// </code>
		/// </summary>
		Index, 

		/// <summary>
		/// Provides overload to assign a value:
		/// <code>
		/// t[k] = v;
		/// t.k = v;
		/// </code>
		/// </summary>
		NewIndex,

		/// <summary>
		/// Provides overload function to handle Lua garbage collection.
		/// </summary>
		GC, 

		/// <summary>
		/// Provides overload field to determine if a table is a weak table.
		/// </summary>
		Mode, 

		/// <summary>
		/// Provides overload to the value length operation.
		/// </summary>
		Length, 

		/// <summary>
		/// Provides overload to the equaility operation.
		/// </summary>
		Equal,

		/// <summary>
		/// Provides overload to the addition operation.
		/// </summary>
		Addition, 

		/// <summary>
		/// Provides overload to the subtraction operation.
		/// </summary>
		Subtraction, 

		/// <summary>
		/// Provides overload to the multiplication operation.
		/// </summary>
		Multiplication, 

		/// <summary>
		/// Provides overload to the modulo operation.
		/// </summary>
		Modulo, 

		/// <summary>
		/// Provides overload to the power operation.
		/// </summary>
		Power,

		/// <summary>
		/// Provides overload to the floating point division operation.
		/// </summary>
		Division, 

		/// <summary>
		/// Provides overload to the integer division operation.
		/// </summary>
		IntegerDivisiion,

		/// <summary>
		/// Provodies overload to the binary and operation.
		/// </summary>
		BinaryAnd,

		/// <summary>
		/// Provides overload t othe binary or operation.
		/// </summary>
		BinaryOr, 

		/// <summary>
		/// Provides overload to the binary xor operation.
		/// </summary>
		BinaryXor, 

		/// <summary>
		/// Provides overload to the shift left operation.
		/// </summary>
		ShiftLeft, 

		/// <summary>
		/// Provides overload to the shift right operation.
		/// </summary>
		ShiftRight,

		/// <summary>
		/// Provides overload to the unary minus operation.
		/// </summary>
		UnaryMinus,

		/// <summary>
		/// Provides overload to the binary not operation.
		/// </summary>
		BinaryNot,

		/// <summary>
		/// Provides overload to the less than operation.
		/// </summary>
		LessThan,

		/// <summary>
		/// Provides overload to the less equal operation.
		/// </summary>
		LessEqual,

		/// <summary>
		/// Provides overload to the concat operation.
		/// </summary>
		Concat,

		/// <summary>
		/// Provides overload to the call operation:
		/// <code>
		/// t(...);
		/// </code>
		/// </summary>
		Call,

		/// <summary>
		/// Provides overload to the close operation.
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

		/// <summary>
		/// Represents the constant string for the metatable "__index" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__index" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__index</b> takes two arguments:
		/// <c>
		/// __index = function(table, key) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_INDEX = "__index";

		/// <summary>
		/// Represents the constant string for the metatable "__newindex" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__newindex" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__newindex</b> takes two arguments:
		/// <c>
		/// __newindex = function(table, key, value) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_NEWINDEX = "__newindex";

		/// <summary>
		/// Represents the constant string for the metatable "__gc" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__gc" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__gc</b> takes two arguments:
		/// <c>
		/// __gc = function(object) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_GC = "__gc";
		
		/// <summary>
		/// Represents the constant string for the metatable field "__mode". Sets the weak state of a table's key set or value set by denoting 'k' for keys
		/// and 'v' for values.
		/// </summary>
		literal System::String^ METATABLE_MODE = "__mode";

		/// <summary>
		/// Represents the constant string for the metatable "__len" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__len" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__len</b> takes two arguments:
		/// <c>
		/// __len = function(object) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_LEN = "__len";

		/// <summary>
		/// Represents the constant string for the metatable "__eq" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__eq" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__eq</b> takes two arguments:
		/// <c>
		/// __eq = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_EQ = "__eq";

		/// <summary>
		/// Represents the constant string for the metatable "__mod" operation, overriding the <c>%</c> operator when used on value with a metatable defining a "__mod" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__mod</b> takes two arguments:
		/// <c>
		/// __mod = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_MOD = "__mod";

		/// <summary>
		/// Represents the constant string for the metatable "__pow" operation, overriding the <c>^</c> operator when used on value with a metatable defining a "__pow" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__pow</b> takes two arguments:
		/// <c>
		/// __pow = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_POW = "__pow";

		/// <summary>
		/// Represents the constant string for the metatable "__band" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__band" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__band</b> takes two arguments:
		/// <c>
		/// __band = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_BAND = "__band";

		/// <summary>
		/// Represents the constant string for the metatable "__bor" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__bor" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__bor</b> takes two arguments:
		/// <c>
		/// __bor = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_BOR = "__bor";

		/// <summary>
		/// Represents the constant string for the metatable "__bxor" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__bxor" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__bxor</b> takes two arguments:
		/// <c>
		/// __bxor = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_BXOR = "__bxor";

		/// <summary>
		/// Represents the constant string for the metatable "__shl" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__shl" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__shl</b> takes two arguments:
		/// <c>
		/// __shl = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_SHL = "__shl";

		/// <summary>
		/// Represents the constant string for the metatable "__shr" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__shr" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__shr</b> takes two arguments:
		/// <c>
		/// __shr = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_SHR = "__shr";

		/// <summary>
		/// Represents the constant string for the metatable "__unm" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__unm" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__unm</b> takes two arguments:
		/// <c>
		/// __unm = function(object) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_UNM = "__unm";

		/// <summary>
		/// Represents the constant string for the metatable "__bnot" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__bnot" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__bnot</b> takes two arguments:
		/// <c>
		/// __bnot = function(object) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_BNOT = "__bnot";

		/// <summary>
		/// Represents the constant string for the metatable "__lt" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__lt" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__lt</b> takes two arguments:
		/// <c>
		/// __lt = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_LT = "__lt";

		/// <summary>
		/// Represents the constant string for the metatable "__le" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__le" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__le</b> takes two arguments:
		/// <c>
		/// __le = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_LE = "__le";

		/// <summary>
		/// Represents the constant string for the metatable "__concat" operation, overriding the <c>..</c> operator when used on value with a metatable defining a "__concat" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__concat</b> takes two arguments:
		/// <c>
		/// __concat = function(left, right) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_CONCAT = "__concat";

		/// <summary>
		/// Represents the constant string for the metatable "__call" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__call" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__call</b> takes two arguments:
		/// <c>
		/// __call = function(object, ...) ... end
		/// </c>
		/// </remarks>
		literal System::String^ METATABLE_CALL = "__call";

		/// <summary>
		/// Represents the constant string for the metatable "__close" operation, overriding the <c>/</c> operator when used on value with a metatable defining a "__close" function.
		/// </summary>
		/// <remarks>
		/// A metatable function for <b>__close</b> takes two arguments:
		/// <c>
		/// __close = function(object) ... end
		/// </c>
		/// </remarks>
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
					throw gcnew LuaMetatableException(System::String::Format("Invalid metatable override '{0}'.", meta.ToString()));
			}
		}

	};

}
