#include "root.h"

#include "./NodeModuleModule.h"

#include "ImportMetaObject.h"
#include "JavaScriptCore/JSBoundFunction.h"

using namespace Zig;
using namespace JSC;

JSC_DEFINE_HOST_FUNCTION(jsFunctionNodeModuleCreateRequire,
                         (JSC::JSGlobalObject * globalObject,
                          JSC::CallFrame *callFrame)) {
  JSC::VM &vm = globalObject->vm();
  auto scope = DECLARE_THROW_SCOPE(vm);
  if (callFrame->argumentCount() < 1) {
    throwTypeError(globalObject, scope,
                   "createRequire() requires at least one argument"_s);
    return JSC::JSValue::encode(JSC::jsUndefined());
  }

  Zig::ImportMetaObject *importMetaObject = Zig::ImportMetaObject::create(
      globalObject, callFrame->uncheckedArgument(0));
  auto clientData = WebCore::clientData(vm);

  RETURN_IF_EXCEPTION(scope, {});

  if (!importMetaObject) {
    throwTypeError(globalObject, scope, "Invalid path"_s);
    return JSC::JSValue::encode(JSC::jsUndefined());
  }

  auto requireFunctionValue = importMetaObject->get(
      globalObject, clientData->builtinNames().requirePublicName());
  RETURN_IF_EXCEPTION(scope, {});

  JSC::JSBoundFunction *boundRequireFunction = JSC::JSBoundFunction::create(
      vm, globalObject, requireFunctionValue.getObject(), importMetaObject,
      nullptr, 1, jsString(vm, String("require"_s)));
  RETURN_IF_EXCEPTION(scope, {});
  auto resolveFunction = importMetaObject->get(
      globalObject, clientData->builtinNames().resolveSyncPublicName());

  JSC::JSBoundFunction *boundResolveFunction = JSC::JSBoundFunction::create(
      vm, globalObject, resolveFunction.getObject(), importMetaObject, nullptr,
      1, jsString(vm, String("resolve"_s)));
  boundRequireFunction->putDirect(
      vm, clientData->builtinNames().resolvePublicName(), boundResolveFunction,
      JSC::PropertyAttribute::Function | 0);

  RELEASE_AND_RETURN(scope, JSC::JSValue::encode(boundRequireFunction));
}
JSC_DEFINE_HOST_FUNCTION(jsFunctionNodeModulePaths,
                         (JSC::JSGlobalObject * globalObject,
                          JSC::CallFrame *callFrame)) {
  return JSC::JSValue::encode(JSC::JSArray::create(globalObject->vm(), 0));
}

JSC_DEFINE_HOST_FUNCTION(jsFunctionFindSourceMap,
                         (JSGlobalObject * globalObject,
                          CallFrame *callFrame)) {
  auto &vm = globalObject->vm();
  auto scope = DECLARE_THROW_SCOPE(vm);
  throwException(globalObject, scope,
                 createError(globalObject, "Not implemented"_s));
  return JSValue::encode(jsUndefined());
}

JSC_DEFINE_HOST_FUNCTION(jsFunctionSyncBuiltinExports,
                         (JSGlobalObject * globalObject,
                          CallFrame *callFrame)) {
  return JSValue::encode(jsUndefined());
}

JSC_DEFINE_HOST_FUNCTION(jsFunctionSourceMap, (JSGlobalObject * globalObject,
                                               CallFrame *callFrame)) {
  auto &vm = globalObject->vm();
  auto scope = DECLARE_THROW_SCOPE(vm);
  throwException(globalObject, scope,
                 createError(globalObject, "Not implemented"_s));
  return JSValue::encode(jsUndefined());
}

namespace Zig {

void generateNodeModuleModule(JSC::JSGlobalObject *globalObject,
                              JSC::Identifier moduleKey,
                              Vector<JSC::Identifier, 4> &exportNames,
                              JSC::MarkedArgumentBuffer &exportValues) {
  JSC::VM &vm = globalObject->vm();

  exportValues.append(JSFunction::create(
      vm, globalObject, 1, String("createRequire"_s),
      jsFunctionNodeModuleCreateRequire, ImplementationVisibility::Public));
  exportValues.append(JSFunction::create(vm, globalObject, 1, String("paths"_s),
                                         jsFunctionNodeModulePaths,
                                         ImplementationVisibility::Public));
  exportValues.append(JSFunction::create(
      vm, globalObject, 1, String("findSourceMap"_s), jsFunctionFindSourceMap,
      ImplementationVisibility::Public));
  exportValues.append(JSFunction::create(
      vm, globalObject, 0, String("syncBuiltinExports"_s),
      jsFunctionSyncBuiltinExports, ImplementationVisibility::Public));
  exportValues.append(
      JSFunction::create(vm, globalObject, 1, String("SourceMap"_s),
                         jsFunctionSourceMap, ImplementationVisibility::Public,
                         NoIntrinsic, jsFunctionSourceMap, nullptr));

  exportNames.append(JSC::Identifier::fromString(vm, "createRequire"_s));
  exportNames.append(JSC::Identifier::fromString(vm, "paths"_s));
  exportNames.append(JSC::Identifier::fromString(vm, "findSourceMap"_s));
  exportNames.append(JSC::Identifier::fromString(vm, "syncBuiltinExports"_s));
  exportNames.append(JSC::Identifier::fromString(vm, "SourceMap"_s));

  exportNames.append(JSC::Identifier::fromString(vm, "builtinModules"_s));

  JSC::JSArray *builtinModules = JSC::JSArray::create(
      vm,
      globalObject->arrayStructureForIndexingTypeDuringAllocation(
          ArrayWithContiguous),
      7);
  builtinModules->putDirectIndex(globalObject, 0,
                                 JSC::jsString(vm, String("node:assert"_s)));
  builtinModules->putDirectIndex(globalObject, 1,
                                 JSC::jsString(vm, String("node:buffer"_s)));
  builtinModules->putDirectIndex(globalObject, 2,
                                 JSC::jsString(vm, String("node:events"_s)));
  builtinModules->putDirectIndex(globalObject, 3,
                                 JSC::jsString(vm, String("node:util"_s)));
  builtinModules->putDirectIndex(globalObject, 4,
                                 JSC::jsString(vm, String("node:path"_s)));
  builtinModules->putDirectIndex(globalObject, 5,
                                 JSC::jsString(vm, String("bun:ffi"_s)));
  builtinModules->putDirectIndex(globalObject, 6,
                                 JSC::jsString(vm, String("bun:sqlite"_s)));
  exportValues.append(builtinModules);
}
} // namespace Zig