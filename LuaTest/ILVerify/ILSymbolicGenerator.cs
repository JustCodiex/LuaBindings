using System.Reflection;
using System.Reflection.Emit;
using System.Text;

namespace LuaTest.ILVerify;

public sealed class ILSymbolicGenerator {

    readonly ILGenerator il;
    readonly StringBuilder sb;

    public ILSymbolicGenerator(ILGenerator generator) {
        this.il = generator;
        this.sb = new();
    }

    public LocalBuilder DeclareLocal(Type t) => il.DeclareLocal(t);

    public void Emit(OpCode code) {
        il.Emit(code);
        sb.AppendLine(code.ToString());
    }

    public void Emit(OpCode code, Type t) {
        il.Emit(code, t);
        sb.Append(code.ToString());
        sb.Append(' ');
        sb.AppendLine(t.Name);
    }

    public void Emit(OpCode code, int i) {
        il.Emit(code, i);
        sb.Append(code.ToString());
        sb.Append(' ');
        sb.AppendLine(i.ToString());
    }

    public void Emit(OpCode code, LocalBuilder l) {
        il.Emit(code, l);
        sb.Append(code.ToString());
        sb.Append(' ');
        sb.Append(l.LocalIndex);
        sb.Append(' ');
        sb.AppendLine(l.LocalType.Name);
    }

    public void EmitCall(OpCode code, MethodInfo method, Type[]? optionals) {
        il.EmitCall(code, method, optionals);
        sb.Append(code.ToString());
        sb.Append(' ');
        sb.AppendLine(method.ToString());
    }

    public override string ToString() {
        return this.sb.ToString();
    }

}
