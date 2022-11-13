using Lua;

using System.Diagnostics;
using System.Reflection;
using System.Reflection.Emit;

namespace LuaTest.ILVerify;

/// <summary>
/// Test class for generating valid code output
/// </summary>
public class ILCodeGenVerifier {

    private void LoadArgument(ILSymbolicGenerator il, int i, LocalBuilder l) {

        il.Emit(OpCodes.Ldarg, 0); // Load LuaState^
        il.Emit(OpCodes.Ldc_I4, -i); // Load lua stack index onto the stack
        il.EmitCall(OpCodes.Call, LuaMarshal.__netStackMarshal, null); // Call lua marshal

        // Unbox from Object type if value type
        if (l.LocalType.IsValueType) {
            il.Emit(OpCodes.Unbox_Any, l.LocalType);
        } else {
            il.Emit(OpCodes.Castclass, l.LocalType);
        }

        il.Emit(OpCodes.Stloc, l); // Store in local

    }

    public LuaFunctionDelegate CreateDelegate(MethodInfo method) {

        // Grab static flag
        var isStatic = method.IsStatic;

        // Get dynamic method ctor args
        var returnId = typeof(int);
        var dynamicName = String.Format("lua_{0}", method.Name.ToLowerInvariant());

        // Grab argument count
        var methodParams = method.GetParameters();

        // Create the dynamic method
        var dMethod = new DynamicMethod(dynamicName, returnId, new Type[] { typeof(LuaState) });
        var body = new ILSymbolicGenerator(dMethod.GetILGenerator());
        body.Emit(OpCodes.Nop);

        // Prepare var for argument offset
        int argCount = methodParams.Length + (isStatic ? 0 : 1);

        // Prepare locals
        var locals = new LocalBuilder[argCount];

        // Demarshal stack values
        for (int i = 0; i < argCount; i++) {

            // Hande local
            int pi = isStatic ? i : (i - 1);
            var locTy = (i == 0 && !isStatic) ? method.DeclaringType : methodParams[pi].ParameterType;
            locals[i] = body.DeclareLocal(locTy!);

            // Load the argument
            LoadArgument(body, argCount - i, locals[i]);

        }

        // Arg offset tracker
        int locOffset = 0;

        // Emit caller (null if static)
        if (!isStatic) {
            body.Emit(OpCodes.Ldloc, locals[0]);
            locOffset++;
        }

        // Push args
        for (int i = 0; i < methodParams.Length; i++) {
            body.Emit(OpCodes.Ldloc, locals[locOffset + i]);
        }

        // Emit call to method
        body.EmitCall(OpCodes.Callvirt, method, null);

        // If void, push 0
        if (method.ReturnType == typeof(void)) {
            body.Emit(OpCodes.Ldc_I4_0);
        } else { // TODO: Add support to tuple types

            // Store it in a local
            var returnValLoc = body.DeclareLocal(typeof(object));
            if (method.ReturnType.IsValueType) {
                body.Emit(OpCodes.Box, method.ReturnType);
            }
            body.Emit(OpCodes.Stloc, returnValLoc);

            // Load L
            body.Emit(OpCodes.Ldarg_0);

            // Load result
            body.Emit(OpCodes.Ldloc, returnValLoc);

            // Call marshal
            body.EmitCall(OpCodes.Call, LuaMarshal.__netToStackMarshal, null);

            // Push one onto the stack as return value
            body.Emit(OpCodes.Ldc_I4_1);

        }

        // Push return instruction
        body.Emit(OpCodes.Ret);

        Trace.WriteLine(body);

        // Return delegate
        return (LuaFunctionDelegate)(dMethod.CreateDelegate(typeof(LuaFunctionDelegate)));

    }

    class Obj {
        public double Call(double n) => n + 1;
    }

    [Test]
    public void CanCompileDynamic() {

        MethodInfo m = typeof(Obj).GetMethod(nameof(Obj.Call))!;

        // Get delegate
        var del = CreateDelegate(m);
        Assert.That(del, Is.Not.Null);

        LuaState L = LuaState.NewState();
        L.PushCSharpFunction("Test", del);

        L.PushLightUserdata(new Obj());
        L.PushNumber(5.0);

        // Try invoke
        int c = del.Invoke(L);
        Assert.That(L.GetNumber(), Is.EqualTo(6.0));

    }

}
