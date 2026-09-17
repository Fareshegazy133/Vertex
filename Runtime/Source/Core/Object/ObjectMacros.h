// Vertex

#pragma once

namespace VCore
{
#define VCLASS(...)
#define VSTRUCT(...)
#define VENUM(...)
#define VFUNCTION(...)
#define VPROPERTY(...)
#define VGENERATED_BODY(...)

#define VOBJECT_BODY(...)
#define VSTRUCT_BODY(...)

#define V_DECLARE_CLASS(ClassName, SuperClassName) \
public: \
    using ThisClass = ClassName; \
    using Super = SuperClassName; \
    static VClass* StaticClass(); \
    virtual VClass* GetClass() const override;

#define V_DECLARE_STRUCT(StructName, SuperStructName) \
public: \
    using ThisStruct = StructName; \
    using Super = SuperStructName; \
    static VStruct* StaticStruct();
}