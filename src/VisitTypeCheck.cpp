#include "VisitTypeCheck.h"

#include <functional>
#include <iostream>
#include <unordered_set>
#include <utility>

#include "Context.h"

namespace Stella {
void VisitTypeCheck::visitProgram(Program* p) {}                    // abstract class
void VisitTypeCheck::visitLanguageDecl(LanguageDecl* p) {}          // abstract class
void VisitTypeCheck::visitExtension(Extension* p) {}                // abstract class
void VisitTypeCheck::visitDecl(Decl* p) {}                          // abstract class
void VisitTypeCheck::visitLocalDecl(LocalDecl* p) {}                // abstract class
void VisitTypeCheck::visitAnnotation(Annotation* p) {}              // abstract class
void VisitTypeCheck::visitParamDecl(ParamDecl* p) {}                // abstract class
void VisitTypeCheck::visitReturnType(ReturnType* p) {}              // abstract class
void VisitTypeCheck::visitThrowType(ThrowType* p) {}                // abstract class
void VisitTypeCheck::visitType(Type* p) {}                          // abstract class
void VisitTypeCheck::visitMatchCase(MatchCase* p) {}                // abstract class
void VisitTypeCheck::visitOptionalTyping(OptionalTyping* p) {}      // abstract class
void VisitTypeCheck::visitPatternData(PatternData* p) {}            // abstract class
void VisitTypeCheck::visitExprData(ExprData* p) {}                  // abstract class
void VisitTypeCheck::visitPattern(Pattern* p) {}                    // abstract class
void VisitTypeCheck::visitLabelledPattern(LabelledPattern* p) {}    // abstract class
void VisitTypeCheck::visitBinding(Binding* p) {}                    // abstract class
void VisitTypeCheck::visitExpr(Expr* p) {}                          // abstract class
void VisitTypeCheck::visitPatternBinding(PatternBinding* p) {}      // abstract class
void VisitTypeCheck::visitVariantFieldType(VariantFieldType* p) {}  // abstract class
void VisitTypeCheck::visitRecordFieldType(RecordFieldType* p) {}    // abstract class
void VisitTypeCheck::visitTyping(Typing* p) {}                      // abstract class
void VisitTypeCheck::visitPatternCastAs(PatternCastAs* p) {}        // abstract class
void VisitTypeCheck::visitTryCastAs(TryCastAs* p) {}                // abstract class
void VisitTypeCheck::visitPatternAsc(PatternAsc* p) {}              // abstract class
void VisitTypeCheck::visitTypeAuto(TypeAuto* p) {}                  // abstract class

void VisitTypeCheck::visitAProgram(AProgram* a_program) {
    env_ = std::make_shared<Context>();
    env_->addScope();

    if (a_program->languagedecl_) a_program->languagedecl_->accept(this);
    if (a_program->listextension_) a_program->listextension_->accept(this);

    if (a_program->listdecl_) {
        for (auto decl : *a_program->listdecl_) {
            if (auto f = dynamic_cast<DeclFun*>(decl)) {
                const std::string name = f->stellaident_;
                if (!f->listparamdecl_ || f->listparamdecl_->size() != 1) {
                    typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Only fuctions with 1 param are supported");
                }
                auto param = dynamic_cast<AParamDecl*>(f->listparamdecl_->front());

                if (!param || !param->type_) {
                    typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "parameter must have explicit type");
                }

                param->type_->accept(this);
                auto paramType = currentType_;

                auto srt = dynamic_cast<SomeReturnType*>(f->returntype_);

                if (!srt || !srt->type_) {
                    typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing return type");
                }

                srt->accept(this);
                auto returnType = currentType_;
                auto listTypePtr = std::make_shared<ListType>();
                listTypePtr->push_back(paramType->clone());
                auto funcType = std::make_shared<TypeFun>(listTypePtr->clone(), returnType->clone());

                env_->bindNew(name, funcType);
            }
        }

        for (auto decl : *a_program->listdecl_) {
            if (!dynamic_cast<DeclFun*>(decl)) {
                decl->accept(this);
            }
        }

        for (auto decl : *a_program->listdecl_) {
            if (auto f = dynamic_cast<DeclFun*>(decl)) {
                env_->addScope();
                if (f->listparamdecl_) f->listparamdecl_->accept(this);

                if (f->expr_) {
                    auto savedExpected = expectedType_;
                    auto maybeFunc = env_->lookup(f->stellaident_);
                    TypeFun* tf = maybeFunc ? dynamic_cast<TypeFun*>(maybeFunc.get()) : nullptr;
                    std::shared_ptr<Type> returnType;
                    if (tf && tf->type_) returnType = std::shared_ptr<Type>(tf->type_->clone());
                    expectedType_ = returnType;
                    f->expr_->accept(this);
                    expectedType_ = savedExpected;

                    auto bodyType = currentType_;
                    if (!bodyType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid function body type");

                    if (typeEquals(bodyType.get(), returnType.get())) {
                    } else if (dynamic_cast<TypeBottom*>(bodyType.get())) {
                        if (dynamic_cast<TypeFun*>(returnType.get())) {
                            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "function body type mismatch");
                        }
                    } else {
                        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "function body type mismatch");
                    }
                }
                env_->popScope();
            }
        }
    }

    if (!env_->lookup("main")) {
        typeError("ERROR_MISSING_MAIN", "program has no main");
    }
}

void VisitTypeCheck::visitLanguageCore(LanguageCore* language_core) { /* Code For LanguageCore Goes Here */ }

void VisitTypeCheck::visitAnExtension(AnExtension* an_extension) {
    /* Code For AnExtension Goes Here */

    if (an_extension->listextensionname_) an_extension->listextensionname_->accept(this);
}

void VisitTypeCheck::visitDeclFun(DeclFun* decl_fun) {
    env_->addScope();

    if (decl_fun->listannotation_) {
        decl_fun->listannotation_->accept(this);
    }

    visitStellaIdent(decl_fun->stellaident_);
    if (decl_fun->listparamdecl_) {
        decl_fun->listparamdecl_->accept(this);
    }
    if (decl_fun->returntype_) decl_fun->returntype_->accept(this);
    if (decl_fun->throwtype_) decl_fun->throwtype_->accept(this);
    if (decl_fun->listdecl_) decl_fun->listdecl_->accept(this);
    if (decl_fun->expr_) decl_fun->expr_->accept(this);

    env_->popScope();
}

void VisitTypeCheck::visitDeclTypeAlias(DeclTypeAlias* decl_type_alias) {
    visitStellaIdent(decl_type_alias->stellaident_);
    if (decl_type_alias->type_) decl_type_alias->type_->accept(this);
}

void VisitTypeCheck::visitDeclExceptionType(DeclExceptionType* decl_exception_type) {
    if (decl_exception_type->type_) {
        decl_exception_type->type_->accept(this);
        if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid exception type");
        env_->bindNew("__exception_type", std::shared_ptr<Type>(currentType_->clone()));
    } else {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing exception type");
    }
}

void VisitTypeCheck::visitDeclExceptionVariant(DeclExceptionVariant* decl_exception_variant) {
    visitStellaIdent(decl_exception_variant->stellaident_);
    if (decl_exception_variant->type_) decl_exception_variant->type_->accept(this);
}

void VisitTypeCheck::visitAssign(Assign* assign) {
    if (!assign || !assign->expr_1 || !assign->expr_2)
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid assignment");

    assign->expr_1->accept(this);
    auto targetRef = currentType_ ? dynamic_cast<TypeRef*>(currentType_.get()) : nullptr;
    if (!targetRef) typeError("ERROR_NOT_A_REFERENCE", "assignment target must be a reference");
    if (!targetRef->type_)
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "assignment target has unknown inner type");

    auto savedExpected = expectedType_;
    expectedType_ = std::shared_ptr<Type>(targetRef->type_->clone());
    assign->expr_2->accept(this);
    expectedType_ = savedExpected;

    if (!currentType_ || !typeEquals(currentType_.get(), targetRef->type_)) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_PARAMETER", "assignment value type mismatch");
    }

    currentType_ = std::make_shared<TypeUnit>();
}

void VisitTypeCheck::visitRef(Ref* ref) {
    if (!ref || !ref->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid reference creation");

    auto savedExpected = expectedType_;
    if (expectedType_) {
        if (auto etr = dynamic_cast<TypeRef*>(expectedType_.get())) {
            if (etr->type_)
                expectedType_ = std::shared_ptr<Type>(etr->type_->clone());
            else
                expectedType_.reset();
        } else {
            expectedType_.reset();
        }
    }

    ref->expr_->accept(this);
    expectedType_ = savedExpected;

    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid new(...) expression");
    currentType_ = std::make_shared<TypeRef>(currentType_->clone());
}

void VisitTypeCheck::visitDeref(Deref* deref) {
    if (!deref || !deref->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid deref expression");

    auto savedExpected = expectedType_;
    if (expectedType_) {
        if (!dynamic_cast<TypeRef*>(expectedType_.get())) {
            expectedType_ = std::make_shared<TypeRef>(expectedType_->clone());
        }
    }

    deref->expr_->accept(this);
    expectedType_ = savedExpected;

    auto refTy = currentType_ ? dynamic_cast<TypeRef*>(currentType_.get()) : nullptr;
    if (!refTy) typeError("ERROR_NOT_A_REFERENCE", "deref expects a reference");
    if (!refTy->type_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "referenced inner type is unknown");

    currentType_ = std::shared_ptr<Type>(refTy->type_->clone());
}

void VisitTypeCheck::visitPanic(Panic* panic) {
    if (!panic) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid panic node");

    if (!expectedType_) {
        typeError("ERROR_AMBIGUOUS_PANIC_TYPE", "panic expression requires expected type");
    }

    currentType_ = std::shared_ptr<Type>(expectedType_->clone());
}

void VisitTypeCheck::visitThrow(Throw* throw_) {
    auto exTy = env_->lookup("__exception_type");
    if (!exTy) typeError("ERROR_EXCEPTION_TYPE_NOT_DECLARED", "throw used without exception type declaration");

    if (!throw_ || !throw_->expr_) {
        typeError("ERROR_AMBIGUOUS_THROW_TYPE", "cannot infer type for throw expression without payload");
    }

    auto savedExpected = expectedType_;
    expectedType_ = std::shared_ptr<Type>(exTy->clone());
    throw_->expr_->accept(this);
    expectedType_ = savedExpected;

    if (!currentType_) {
        typeError("ERROR_AMBIGUOUS_THROW_TYPE", "cannot infer type for throw expression");
    }

    if (!typeEquals(currentType_.get(), exTy.get())) {
        typeError("ERROR_AMBIGUOUS_THROW_TYPE", "throw expression type mismatch with declared exception type");
    }

    currentType_ = std::make_shared<TypeBottom>();
}

void VisitTypeCheck::visitTryCatch(TryCatch* try_catch) {
    auto exTy = env_->lookup("__exception_type");
    if (!exTy) typeError("ERROR_EXCEPTION_TYPE_NOT_DECLARED", "try/catch used without exception type declaration");

    if (!try_catch->expr_1) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "try missing expression");
    try_catch->expr_1->accept(this);
    auto tryType = currentType_;
    if (!tryType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid try block type");

    env_->addScope();

    if (try_catch->pattern_) {
        if (auto pv = dynamic_cast<PatternVar*>(try_catch->pattern_)) {
            env_->bindNew(pv->stellaident_, std::shared_ptr<Type>(exTy->clone()));
        } else {
            auto savedExpected = expectedType_;
            expectedType_ = std::shared_ptr<Type>(exTy->clone());
            try_catch->pattern_->accept(this);
            expectedType_ = savedExpected;
        }
    }

    auto savedExpected = expectedType_;
    expectedType_ = tryType;
    if (!try_catch->expr_2) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "catch handler missing expression");
    try_catch->expr_2->accept(this);
    expectedType_ = savedExpected;

    auto handlerType = currentType_;

    env_->popScope();

    if (!handlerType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid catch handler type");

    bool compatible = typeEquals(tryType.get(), handlerType.get()) || dynamic_cast<TypeBottom*>(tryType.get()) ||
                      dynamic_cast<TypeBottom*>(handlerType.get());
    if (!compatible) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "try and catch must have same type");
    }

    if (dynamic_cast<TypeBottom*>(tryType.get()))
        currentType_ = handlerType;
    else
        currentType_ = tryType;
}

void VisitTypeCheck::visitTryWith(TryWith* try_with) {
    auto exTy = env_->lookup("__exception_type");
    if (!exTy) typeError("ERROR_EXCEPTION_TYPE_NOT_DECLARED", "try/with used without exception type declaration");

    if (!try_with->expr_1) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "try missing expression");
    try_with->expr_1->accept(this);
    auto tryType = currentType_;
    if (!tryType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid try block type");

    auto savedExpected = expectedType_;
    expectedType_ = tryType;
    if (!try_with->expr_2) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "with handler missing expression");
    try_with->expr_2->accept(this);
    expectedType_ = savedExpected;

    auto handlerType = currentType_;
    if (!handlerType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid with handler type");

    bool compatible = typeEquals(tryType.get(), handlerType.get()) || dynamic_cast<TypeBottom*>(tryType.get()) ||
                      dynamic_cast<TypeBottom*>(handlerType.get());
    if (!compatible) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "try and with must have same type");
    }

    if (dynamic_cast<TypeBottom*>(tryType.get()))
        currentType_ = handlerType;
    else
        currentType_ = tryType;
}

void VisitTypeCheck::visitALocalDecl(ALocalDecl* a_local_decl) {
    /* Code For ALocalDecl Goes Here */

    if (a_local_decl->decl_) a_local_decl->decl_->accept(this);
}

void VisitTypeCheck::visitInlineAnnotation(InlineAnnotation* inline_annotation) {
    /* Code For InlineAnnotation Goes Here */
}

void VisitTypeCheck::visitAParamDecl(AParamDecl* a_param_decl) {
    visitStellaIdent(a_param_decl->stellaident_);
    if (a_param_decl->type_) a_param_decl->type_->accept(this);

    auto paramType = currentType_;
    env_->bindNew(a_param_decl->stellaident_, paramType);
}

void VisitTypeCheck::visitNoReturnType(NoReturnType* no_return_type) { /* Code For NoReturnType Goes Here */ }

void VisitTypeCheck::visitSomeReturnType(SomeReturnType* some_return_type) {
    if (!some_return_type->type_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing return type");
    some_return_type->type_->accept(this);
    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid return type");
    currentType_ = std::shared_ptr<Type>(currentType_->clone());
}

void VisitTypeCheck::visitNoThrowType(NoThrowType* no_throw_type) { /* Code For NoThrowType Goes Here */ }

void VisitTypeCheck::visitSomeThrowType(SomeThrowType* some_throw_type) {
    /* Code For SomeThrowType Goes Here */

    if (some_throw_type->listtype_) some_throw_type->listtype_->accept(this);
}

void VisitTypeCheck::visitTypeFun(TypeFun* type_fun) {
    auto listTypePtr = std::make_shared<ListType>();
    if (type_fun->listtype_) {
        for (auto t : *type_fun->listtype_) {
            t->accept(this);
            if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid parameter type");
            listTypePtr->push_back(currentType_->clone());
        }
    }

    if (!type_fun->type_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing return type");

    type_fun->type_->accept(this);

    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid return type");
    currentType_ = std::make_shared<TypeFun>(listTypePtr->clone(), currentType_->clone());
}

void VisitTypeCheck::visitTypeRec(TypeRec* type_rec) {
    /* Code For TypeRec Goes Here */

    visitStellaIdent(type_rec->stellaident_);
    if (type_rec->type_) type_rec->type_->accept(this);
}

void VisitTypeCheck::visitTypeSum(TypeSum* type_sum) {
    if (!type_sum || !type_sum->type_1 || !type_sum->type_2)
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Sum type requires two component types");

    type_sum->type_1->accept(this);
    auto left = std::shared_ptr<Type>(currentType_->clone());

    type_sum->type_2->accept(this);
    auto right = std::shared_ptr<Type>(currentType_->clone());

    currentType_ = std::make_shared<TypeSum>(left->clone(), right->clone());
}

void VisitTypeCheck::visitTypeTuple(TypeTuple* type_tuple) {
    if (!type_tuple || !type_tuple->listtype_ || type_tuple->listtype_->size() != 2) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Tuple must have exactly two elements");
    }

    auto listTypePtr = std::make_shared<ListType>();
    for (auto t : *type_tuple->listtype_) {
        if (!t) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid tuple component");
        t->accept(this);
        if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid tuple component type");
        listTypePtr->push_back(currentType_->clone());
    }

    currentType_ = std::make_shared<TypeTuple>(listTypePtr->clone());

    if (expectedType_) {
        if (!typeEquals(currentType_.get(), expectedType_.get())) {
            typeError("ERROR_UNEXPECTED_TUPLE", "Tuple type doesn't match expected type");
        }
    }
}

void VisitTypeCheck::visitTypeRecord(TypeRecord* type_record) {
    if (!type_record) return;

    auto fields = std::make_shared<ListRecordFieldType>();

    if (type_record->listrecordfieldtype_) {
        for (auto rf : *type_record->listrecordfieldtype_) {
            auto arft = dynamic_cast<ARecordFieldType*>(rf);
            if (!arft) {
                typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid record field type");
            }

            if (!arft->type_) {
                typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "record field missing type");
            }

            arft->type_->accept(this);

            if (!currentType_) {
                typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid record field type");
            }

            RecordFieldType* newField = new ARecordFieldType(arft->stellaident_, currentType_->clone());
            fields->push_back(newField);
        }
    }
    currentType_ = std::make_shared<TypeRecord>(fields->clone());
}

void VisitTypeCheck::visitTypeVariant(TypeVariant* type_variant) {
    if (!type_variant) return;

    if (type_variant->listvariantfieldtype_) {
        for (auto vf : *type_variant->listvariantfieldtype_) {
            auto av = dynamic_cast<AVariantFieldType*>(vf);
            if (!av) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid variant field type");
            if (av->optionaltyping_) {
                auto savedExpected = expectedType_;
                expectedType_.reset();
                av->optionaltyping_->accept(this);
                expectedType_ = savedExpected;
                if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid variant field type");
            }
        }
        currentType_ = std::make_shared<TypeVariant>(type_variant->listvariantfieldtype_->clone());
    } else {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "variant must have fields");
    }
}

void VisitTypeCheck::visitTypeList(TypeList* type_list) {
    if (!type_list) return;
    if (!type_list->type_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing list element type");

    type_list->type_->accept(this);
    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid list element type");

    TypeList* tl = new TypeList(currentType_->clone());
    currentType_ = std::shared_ptr<Type>(tl);
}

void VisitTypeCheck::visitTypeBool(TypeBool* type_bool) { currentType_ = std::make_shared<TypeBool>(); }

void VisitTypeCheck::visitTypeNat(TypeNat* type_nat) { currentType_ = std::make_shared<TypeNat>(); }

void VisitTypeCheck::visitTypeUnit(TypeUnit* type_unit) { currentType_ = std::make_shared<TypeUnit>(); }

void VisitTypeCheck::visitTypeTop(TypeTop* type_top) { /* Code For TypeTop Goes Here */ }

void VisitTypeCheck::visitTypeBottom(TypeBottom* type_bottom) { /* Code For TypeBottom Goes Here */ }

void VisitTypeCheck::visitTypeRef(TypeRef* type_ref) {
    if (!type_ref) return;
    if (!type_ref->type_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing referenced type");
    type_ref->type_->accept(this);
    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid referenced type");
    currentType_ = std::make_shared<TypeRef>(currentType_->clone());
}

void VisitTypeCheck::visitTypeVar(TypeVar* type_var) {
    /* Code For TypeVar Goes Here */

    visitStellaIdent(type_var->stellaident_);
}

void VisitTypeCheck::visitAMatchCase(AMatchCase* a_match_case) {
    env_->addScope();

    if (a_match_case->pattern_) a_match_case->pattern_->accept(this);
    if (a_match_case->expr_) a_match_case->expr_->accept(this);
    env_->popScope();
}

void VisitTypeCheck::visitNoTyping(NoTyping* no_typing) { /* Code For NoTyping Goes Here */ }

void VisitTypeCheck::visitSomeTyping(SomeTyping* some_typing) {
    /* Code For SomeTyping Goes Here */

    if (some_typing->type_) some_typing->type_->accept(this);
}

void VisitTypeCheck::visitNoPatternData(NoPatternData* no_pattern_data) { /* Code For NoPatternData Goes Here */ }

void VisitTypeCheck::visitSomePatternData(SomePatternData* some_pattern_data) {
    if (!expectedType_) {
        typeError("ERROR_UNEXPECTED_PATTERN_FOR_TYPE");
    }
    if (some_pattern_data->pattern_) some_pattern_data->pattern_->accept(this);
}

void VisitTypeCheck::visitNoExprData(NoExprData* no_expr_data) { /* Code For NoExprData Goes Here */ }

void VisitTypeCheck::visitSomeExprData(SomeExprData* some_expr_data) {
    /* Code For SomeExprData Goes Here */

    if (some_expr_data->expr_) some_expr_data->expr_->accept(this);
}

void VisitTypeCheck::visitPatternVariant(PatternVariant* pattern_variant) {
    if (!pattern_variant) return;

    if (!expectedType_) {
        typeError("ERROR_AMBIGUOUS_VARIANT_TYPE", "pattern variant requires expected variant type");
    }

    auto expVar = dynamic_cast<TypeVariant*>(expectedType_.get());
    if (!expVar) {
        typeError("ERROR_UNEXPECTED_VARIANT", "variant pattern where non-variant expected");
    }

    AVariantFieldType* found = nullptr;
    if (!expVar->listvariantfieldtype_) {
        typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "expected variant type has no labels");
    }
    for (auto& vf : *expVar->listvariantfieldtype_) {
        auto av = dynamic_cast<AVariantFieldType*>(vf);
        if (av && av->stellaident_ == pattern_variant->stellaident_) {
            found = av;
            break;
        }
    }

    if (!found) {
        typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "variant label not present in expected type");
    }

    std::shared_ptr<Type> fieldType;
    if (found->optionaltyping_) {
        auto savedCurrent = currentType_;
        auto savedExpected = expectedType_;
        expectedType_.reset();
        found->optionaltyping_->accept(this);
        fieldType = currentType_;
        currentType_ = savedCurrent;
        expectedType_ = savedExpected;
        if (!fieldType) typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "invalid variant field typing");
    } else {
        fieldType = std::make_shared<TypeUnit>();
    }

    if (pattern_variant->patterndata_) {
        Pattern* innerPattern = nullptr;
        if (auto spd = dynamic_cast<SomePatternData*>(pattern_variant->patterndata_)) {
            innerPattern = spd->pattern_;
        }

        std::string nameToBind;
        bool haveName = false;

        if (innerPattern) {
            if (auto pv = dynamic_cast<PatternVar*>(innerPattern)) {
                nameToBind = pv->stellaident_;
                haveName = true;
            } else if (auto alp = dynamic_cast<ALabelledPattern*>(innerPattern)) {
                if (alp->pattern_) {
                    if (auto innerPv = dynamic_cast<PatternVar*>(alp->pattern_)) {
                        nameToBind = innerPv->stellaident_;
                        haveName = true;
                    } else {
                        haveName = false;
                    }
                } else {
                    nameToBind = alp->stellaident_;
                    haveName = true;
                }
            }
        }

        if (haveName) {
            if (!fieldType) typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "invalid variant field typing");
            env_->bindNew(nameToBind, std::shared_ptr<Type>(fieldType->clone()));
        } else {
            auto saved = expectedType_;
            expectedType_ = std::shared_ptr<Type>(fieldType->clone());
            pattern_variant->patterndata_->accept(this);
            expectedType_ = saved;
        }
    } else {
        if (!dynamic_cast<TypeUnit*>(fieldType.get())) {
            typeError("ERROR_UNEXPECTED_VARIANT", "variant pattern missing payload for non-Unit label");
        }
    }
}

void VisitTypeCheck::visitPatternInl(PatternInl* pattern_inl) {
    std::shared_ptr<Type> bindTy;
    if (expectedType_) {
        if (auto sum = dynamic_cast<TypeSum*>(expectedType_.get())) {
            bindTy = std::shared_ptr<Type>(sum->type_1->clone());
        } else {
            bindTy = std::shared_ptr<Type>(expectedType_->clone());
        }
    }

    if (pattern_inl->pattern_) {
        if (auto pv = dynamic_cast<PatternVar*>(pattern_inl->pattern_)) {
            if (!bindTy) typeError("ERROR_AMBIGUOUS_SUM_TYPE");
            env_->bindNew(pv->stellaident_, bindTy);
        } else {
            auto saved = expectedType_;
            expectedType_ = bindTy;
            pattern_inl->pattern_->accept(this);
            expectedType_ = saved;
        }
    }

    patternLeftRight_.first = true;
    currentType_.reset();
}

void VisitTypeCheck::visitPatternInr(PatternInr* pattern_inr) {
    std::shared_ptr<Type> bindTy;
    if (expectedType_) {
        if (auto sum = dynamic_cast<TypeSum*>(expectedType_.get())) {
            bindTy = std::shared_ptr<Type>(sum->type_2->clone());
        } else {
            bindTy = std::shared_ptr<Type>(expectedType_->clone());
        }
    }

    if (pattern_inr->pattern_) {
        if (auto pv = dynamic_cast<PatternVar*>(pattern_inr->pattern_)) {
            if (!bindTy) typeError("ERROR_AMBIGUOUS_SUM_TYPE");
            env_->bindNew(pv->stellaident_, bindTy);
        } else {
            auto saved = expectedType_;
            expectedType_ = bindTy;
            pattern_inr->pattern_->accept(this);
            expectedType_ = saved;
        }
    }

    patternLeftRight_.second = true;
    currentType_.reset();
}

void VisitTypeCheck::visitPatternTuple(PatternTuple* pattern_tuple) {
    /* Code For PatternTuple Goes Here */

    if (pattern_tuple->listpattern_) pattern_tuple->listpattern_->accept(this);
}

void VisitTypeCheck::visitPatternRecord(PatternRecord* pattern_record) {
    /* Code For PatternRecord Goes Here */

    if (pattern_record->listlabelledpattern_) pattern_record->listlabelledpattern_->accept(this);
}

void VisitTypeCheck::visitPatternList(PatternList* pattern_list) {
    /* Code For PatternList Goes Here */

    if (pattern_list->listpattern_) pattern_list->listpattern_->accept(this);
}

void VisitTypeCheck::visitPatternCons(PatternCons* pattern_cons) {
    /* Code For PatternCons Goes Here */

    if (pattern_cons->pattern_1) pattern_cons->pattern_1->accept(this);
    if (pattern_cons->pattern_2) pattern_cons->pattern_2->accept(this);
}

void VisitTypeCheck::visitPatternFalse(PatternFalse* pattern_false) { /* Code For PatternFalse Goes Here */ }

void VisitTypeCheck::visitPatternTrue(PatternTrue* pattern_true) { /* Code For PatternTrue Goes Here */ }

void VisitTypeCheck::visitPatternUnit(PatternUnit* pattern_unit) { /* Code For PatternUnit Goes Here */ }

void VisitTypeCheck::visitPatternInt(PatternInt* pattern_int) {
    /* Code For PatternInt Goes Here */

    visitInteger(pattern_int->integer_);
}

void VisitTypeCheck::visitPatternSucc(PatternSucc* pattern_succ) {
    /* Code For PatternSucc Goes Here */

    if (pattern_succ->pattern_) pattern_succ->pattern_->accept(this);
}

void VisitTypeCheck::visitPatternVar(PatternVar* pattern_var) {
    /* Code For PatternVar Goes Here */
    visitStellaIdent(pattern_var->stellaident_);
}

void VisitTypeCheck::visitALabelledPattern(ALabelledPattern* a_labelled_pattern) {
    /* Code For ALabelledPattern Goes Here */

    visitStellaIdent(a_labelled_pattern->stellaident_);
    if (a_labelled_pattern->pattern_) a_labelled_pattern->pattern_->accept(this);
}

void VisitTypeCheck::visitABinding(ABinding* a_binding) {
    /* Code For ABinding Goes Here */

    visitStellaIdent(a_binding->stellaident_);
    if (a_binding->expr_) a_binding->expr_->accept(this);
}

void VisitTypeCheck::visitSequence(Sequence* sequence) {
    if (!sequence) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid Sequence node");
    if (!sequence->expr_1 || !sequence->expr_2)
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "sequence requires two expressions");

    auto savedExpected = expectedType_;
    expectedType_ = std::make_shared<TypeUnit>();
    sequence->expr_1->accept(this);
    expectedType_ = savedExpected;

    if (!dynamic_cast<TypeUnit*>(currentType_.get()))
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "left operand of sequencing must be Unit");

    sequence->expr_2->accept(this);
    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid second expression in sequence");
}

void VisitTypeCheck::visitIf(If* if_) {
    if (if_->expr_1) {
        auto savedExpected = expectedType_;
        expectedType_ = std::make_shared<TypeBool>();
        if_->expr_1->accept(this);
        expectedType_ = savedExpected;

        if (!dynamic_cast<TypeBool*>(currentType_.get())) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "if condition must be Bool");
        }
    }

    if (!if_->expr_2 || !if_->expr_3) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "if must have both branches");
    }

    if_->expr_2->accept(this);
    auto thenType = std::move(currentType_);
    if_->expr_3->accept(this);
    auto elseType = std::move(currentType_);

    if (!typeEquals(thenType.get(), elseType.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "then/else branch type mismatch");
    }

    currentType_ = thenType;
}

void VisitTypeCheck::visitLet(Let* let) {
    if (!let) return;

    env_->addScope();
    if (let->listpatternbinding_) {
        auto lpb = dynamic_cast<ListPatternBinding*>(let->listpatternbinding_);
        for (auto& i : *lpb) {
            auto apb = dynamic_cast<APatternBinding*>(i);
            if (!apb->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "let binding missing rhs");

            auto savedExpected = expectedType_;
            expectedType_.reset();
            apb->expr_->accept(this);
            expectedType_ = savedExpected;

            auto name = dynamic_cast<PatternVar*>(apb->pattern_)->stellaident_;
            env_->bindNew(name, currentType_);
        }
    }
    if (!let->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "let missing body");
    let->expr_->accept(this);
    env_->popScope();
}

void VisitTypeCheck::visitLetRec(LetRec* let_rec) {
    /* Code For LetRec Goes Here */

    if (let_rec->listpatternbinding_) let_rec->listpatternbinding_->accept(this);
    if (let_rec->expr_) let_rec->expr_->accept(this);
}

void VisitTypeCheck::visitLessThan(LessThan* less_than) {
    /* Code For LessThan Goes Here */

    if (less_than->expr_1) less_than->expr_1->accept(this);
    if (less_than->expr_2) less_than->expr_2->accept(this);
}

void VisitTypeCheck::visitLessThanOrEqual(LessThanOrEqual* less_than_or_equal) {
    /* Code For LessThanOrEqual Goes Here */

    if (less_than_or_equal->expr_1) less_than_or_equal->expr_1->accept(this);
    if (less_than_or_equal->expr_2) less_than_or_equal->expr_2->accept(this);
}

void VisitTypeCheck::visitGreaterThan(GreaterThan* greater_than) {
    /* Code For GreaterThan Goes Here */

    if (greater_than->expr_1) greater_than->expr_1->accept(this);
    if (greater_than->expr_2) greater_than->expr_2->accept(this);
}

void VisitTypeCheck::visitGreaterThanOrEqual(GreaterThanOrEqual* greater_than_or_equal) {
    /* Code For GreaterThanOrEqual Goes Here */

    if (greater_than_or_equal->expr_1) greater_than_or_equal->expr_1->accept(this);
    if (greater_than_or_equal->expr_2) greater_than_or_equal->expr_2->accept(this);
}

void VisitTypeCheck::visitEqual(Equal* equal) {
    if (!equal) return;

    if (equal->expr_1) equal->expr_1->accept(this);
    auto left = currentType_ ? std::shared_ptr<Type>(currentType_->clone()) : nullptr;

    if (equal->expr_2) equal->expr_2->accept(this);
    auto right = currentType_ ? std::shared_ptr<Type>(currentType_->clone()) : nullptr;

    if (!left || !right) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid equality operands");
    }

    if (!typeEquals(left.get(), right.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "equality operand types differ");
    }

    currentType_ = std::make_shared<TypeBool>();
}

void VisitTypeCheck::visitNotEqual(NotEqual* not_equal) {
    if (!not_equal) return;

    if (not_equal->expr_1) not_equal->expr_1->accept(this);
    auto left = currentType_ ? std::shared_ptr<Type>(currentType_->clone()) : nullptr;

    if (not_equal->expr_2) not_equal->expr_2->accept(this);
    auto right = currentType_ ? std::shared_ptr<Type>(currentType_->clone()) : nullptr;

    if (!left || !right) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid inequality operands");
    }

    if (!typeEquals(left.get(), right.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "inequality operand types differ");
    }

    currentType_ = std::make_shared<TypeBool>();
}

void VisitTypeCheck::visitTypeAsc(TypeAsc* type_asc) {
    if (!type_asc) return;
    if (!type_asc->type_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing ascription type");

    type_asc->type_->accept(this);
    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid ascription type");
    auto ascribed = std::shared_ptr<Type>(currentType_->clone());

    if (!type_asc->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing expression for type ascription");

    auto savedExpected = expectedType_;
    expectedType_ = std::shared_ptr<Type>(ascribed->clone());
    type_asc->expr_->accept(this);
    expectedType_ = savedExpected;

    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid expression in type ascription");
    if (!typeEquals(currentType_.get(), ascribed.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "expression does not match ascribed type");
    }

    currentType_ = ascribed;
}

void VisitTypeCheck::visitTypeCast(TypeCast* type_cast) {
    /* Code For TypeCast Goes Here */

    if (type_cast->expr_) type_cast->expr_->accept(this);
    if (type_cast->type_) type_cast->type_->accept(this);
}

void VisitTypeCheck::visitAbstraction(Abstraction* abstraction) {
    auto listTypePtr = std::make_shared<ListType>();
    auto savedExpected = expectedType_;

    env_->addScope();

    if (!abstraction->listparamdecl_ || abstraction->listparamdecl_->size() != 1) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Only fuctions with 1 param are supported");
    }

    if (abstraction->listparamdecl_) {
        for (auto i : *abstraction->listparamdecl_) {
            auto pd = dynamic_cast<AParamDecl*>(i);
            if (!pd || !pd->type_) {
                typeError("ERROR_UNEXPECTED_TYPE_FOR_PARAMETER", "parameter must have explicit type");
            }

            pd->accept(this);
            if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_PARAMETER", "invalid parameter type");
            listTypePtr->push_back(currentType_->clone());
        }
    } else {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_PARAMETER", "function must have a parameter");
    }

    std::shared_ptr<Type> bodyExpected;
    if (savedExpected) {
        if (auto ef = dynamic_cast<TypeFun*>(savedExpected.get())) {
            if (ef->type_) bodyExpected = std::shared_ptr<Type>(ef->type_->clone());
        }
    }

    if (!abstraction->expr_) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing function body");
    }

    auto savedForBody = expectedType_;
    expectedType_ = bodyExpected;
    abstraction->expr_->accept(this);
    expectedType_ = savedForBody;

    auto bodyType = currentType_;
    env_->popScope();

    if (!bodyType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid function body type");

    currentType_ = std::make_shared<TypeFun>(listTypePtr->clone(), bodyType->clone());
    expectedType_ = savedExpected;
}

void VisitTypeCheck::visitVariant(Variant* variant) {
    if (!variant) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid variant node");
    visitStellaIdent(variant->stellaident_);

    if (!expectedType_) typeError("ERROR_AMBIGUOUS_VARIANT_TYPE", "variant expression requires expected variant type");
    auto expVar = dynamic_cast<TypeVariant*>(expectedType_.get());
    if (!expVar) typeError("ERROR_UNEXPECTED_VARIANT", "variant found where non-variant expected");

    if (!expVar->listvariantfieldtype_)
        typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "expected variant type has no labels");
    AVariantFieldType* found = nullptr;
    for (auto& vf : *expVar->listvariantfieldtype_) {
        if (auto av = dynamic_cast<AVariantFieldType*>(vf); av && av->stellaident_ == variant->stellaident_) {
            found = av;
            break;
        }
    }
    if (!found) typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "variant label not present in expected type");

    std::shared_ptr<Type> fieldType;
    if (found->optionaltyping_) {
        auto saveCur = currentType_;
        auto saveExp = expectedType_;
        expectedType_.reset();
        found->optionaltyping_->accept(this);
        fieldType = currentType_;
        currentType_ = saveCur;
        expectedType_ = saveExp;
        if (!fieldType) typeError("ERROR_UNEXPECTED_VARIANT_LABEL", "invalid variant field typing");
    } else {
        fieldType = std::make_shared<TypeUnit>();
    }

    if (variant->exprdata_) {
        auto saveExp = expectedType_;
        expectedType_ = std::shared_ptr<Type>(fieldType->clone());
        variant->exprdata_->accept(this);
        expectedType_ = saveExp;
        if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid variant payload");
        if (!typeEquals(currentType_.get(), fieldType.get()))
            typeError("ERROR_UNEXPECTED_SUBTYPE", "variant payload type mismatch");
    } else {
        if (!dynamic_cast<TypeUnit*>(fieldType.get()))
            typeError("ERROR_UNEXPECTED_VARIANT", "variant missing payload for non-Unit label");
    }

    currentType_ = std::shared_ptr<Type>(expVar->clone());
}

void VisitTypeCheck::visitMatch(Match* match) {
    if (!match->listmatchcase_ || match->listmatchcase_->empty()) {
        typeError("ERROR_ILLEGAL_EMPTY_MATCHING");
    }

    if (match->expr_) match->expr_->accept(this);

    auto matchType = currentType_;
    if (!matchType) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION");

    bool isSum = dynamic_cast<TypeSum*>(matchType.get()) != nullptr;
    bool isVariant = dynamic_cast<TypeVariant*>(matchType.get()) != nullptr;
    if (!isSum && !isVariant) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION");
    }

    patternLeftRight_ = {false, false};
    std::shared_ptr<Type> branchType;

    std::unordered_set<std::string> expectedLabels;
    if (isVariant) {
        auto vv = dynamic_cast<TypeVariant*>(matchType.get());
        if (!vv->listvariantfieldtype_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION");
        for (auto& vf : *vv->listvariantfieldtype_) {
            auto av = dynamic_cast<AVariantFieldType*>(vf);
            if (av)
                expectedLabels.insert(av->stellaident_);
            else
                typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION");
        }
    }

    std::unordered_set<std::string> coveredLabels;

    for (auto i : *match->listmatchcase_) {
        auto savedExpected = expectedType_;
        expectedType_ = matchType;

        if (isVariant) {
            if (auto amc = dynamic_cast<AMatchCase*>(i)) {
                if (amc->pattern_) {
                    if (auto pv = dynamic_cast<PatternVariant*>(amc->pattern_)) {
                        coveredLabels.insert(pv->stellaident_);
                    }
                }
            }
        }

        i->accept(this);
        expectedType_ = savedExpected;

        if (!currentType_) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid match branch expression");
        }

        if (!branchType) {
            branchType = std::shared_ptr<Type>(currentType_->clone());
        } else {
            if (!typeEquals(branchType.get(), currentType_.get())) {
                typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "match branches must have the same type");
            }
        }
    }

    if (isSum) {
        if (!patternLeftRight_.first || !patternLeftRight_.second) {
            typeError("ERROR_NONEXHAUSTIVE_MATCH_PATTERNS", "non-exhaustive match patterns");
        }
    } else if (isVariant) {
        if (coveredLabels.size() != expectedLabels.size()) {
            typeError("ERROR_NONEXHAUSTIVE_MATCH_PATTERNS", "non-exhaustive match patterns");
        }
        for (auto& lbl : expectedLabels) {
            if (coveredLabels.find(lbl) == coveredLabels.end()) {
                typeError("ERROR_NONEXHAUSTIVE_MATCH_PATTERNS", "non-exhaustive match patterns");
            }
        }
    }

    currentType_ = branchType;
}

void VisitTypeCheck::visitList(List* list) {
    if (!list) return;

    TypeList* expectedTL = expectedType_ ? dynamic_cast<TypeList*>(expectedType_.get()) : nullptr;

    if (!list->listexpr_ || list->listexpr_->empty()) {
        if (!expectedTL) typeError("ERROR_AMBIGUOUS_LIST_TYPE");
        currentType_ = std::shared_ptr<Type>(expectedTL->clone());
        return;
    }

    if (expectedTL) {
        if (!expectedTL->type_) typeError("ERROR_AMBIGUOUS_LIST_TYPE", "list element type is unknown");
        auto elemType = std::shared_ptr<Type>(expectedTL->type_->clone());
        for (auto& e : *list->listexpr_) {
            auto saved = expectedType_;
            expectedType_ = std::shared_ptr<Type>(elemType->clone());
            e->accept(this);
            expectedType_ = saved;
            if (!currentType_ || !typeEquals(currentType_.get(), elemType.get()))
                typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "list element type mismatch");
        }
        currentType_ = std::shared_ptr<Type>(expectedTL->clone());
        return;
    }

    list->listexpr_->front()->accept(this);
    if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid list element");
    auto inferred = std::shared_ptr<Type>(currentType_->clone());

    for (size_t i = 1; i < list->listexpr_->size(); ++i) {
        auto saved = expectedType_;
        expectedType_ = std::shared_ptr<Type>(inferred->clone());
        (*list->listexpr_)[i]->accept(this);
        expectedType_ = saved;
        if (!currentType_ || !typeEquals(currentType_.get(), inferred.get()))
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "list element type mismatch");
    }

    TypeList* tl = new TypeList(inferred->clone());
    currentType_ = std::shared_ptr<Type>(tl);
}

void VisitTypeCheck::visitAdd(Add* add) {
    /* Code For Add Goes Here */

    if (add->expr_1) add->expr_1->accept(this);
    if (add->expr_2) add->expr_2->accept(this);
}

void VisitTypeCheck::visitSubtract(Subtract* subtract) {
    /* Code For Subtract Goes Here */

    if (subtract->expr_1) subtract->expr_1->accept(this);
    if (subtract->expr_2) subtract->expr_2->accept(this);
}

void VisitTypeCheck::visitLogicOr(LogicOr* logic_or) {
    /* Code For LogicOr Goes Here */

    if (logic_or->expr_1) logic_or->expr_1->accept(this);
    if (logic_or->expr_2) logic_or->expr_2->accept(this);
}

void VisitTypeCheck::visitMultiply(Multiply* multiply) {
    /* Code For Multiply Goes Here */

    if (multiply->expr_1) multiply->expr_1->accept(this);
    if (multiply->expr_2) multiply->expr_2->accept(this);
}

void VisitTypeCheck::visitDivide(Divide* divide) {
    /* Code For Divide Goes Here */

    if (divide->expr_1) divide->expr_1->accept(this);
    if (divide->expr_2) divide->expr_2->accept(this);
}

void VisitTypeCheck::visitLogicAnd(LogicAnd* logic_and) {
    /* Code For LogicAnd Goes Here */

    if (logic_and->expr_1) logic_and->expr_1->accept(this);
    if (logic_and->expr_2) logic_and->expr_2->accept(this);
}

void VisitTypeCheck::visitApplication(Application* application) {
    auto saved = expectedType_;

    expectedType_ = saved;
    if (application->expr_) {
        application->expr_->accept(this);
    } else {
        typeError("ERROR_NOT_A_FUNCTION", "Missing function in application");
    }
    expectedType_ = saved;

    auto funcType = currentType_;
    auto tf = dynamic_cast<TypeFun*>(funcType.get());
    if (!tf) {
        typeError("ERROR_NOT_A_FUNCTION", "Function was expected");
    }
    if (!tf->listtype_ || tf->listtype_->size() != 1) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Function must have exactly one parameter type");
    }
    if (!application->listexpr_ || application->listexpr_->size() != 1) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_PARAMETER", "Function must have exactly one argument");
    }

    auto saved2 = expectedType_;
    auto paramType = (*tf->listtype_)[0];
    expectedType_ = std::shared_ptr<Type>(paramType->clone());
    application->listexpr_->front()->accept(this);
    auto argType = currentType_;
    expectedType_ = saved2;

    if (!typeEquals(paramType, argType.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_PARAMETER", "Argument type mismatch");
    }

    currentType_ = std::shared_ptr<Type>(tf->type_->clone());
    expectedType_ = saved;
}

void VisitTypeCheck::visitDotRecord(DotRecord* dot_record) {
    if (!dot_record || !dot_record->expr_) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing record expression for field access");
    }

    const std::string fieldName = dot_record->stellaident_;
    auto savedExpected = expectedType_;

    expectedType_.reset();

    dot_record->expr_->accept(this);

    expectedType_ = savedExpected;

    auto recType = currentType_ ? dynamic_cast<TypeRecord*>(currentType_.get()) : nullptr;
    if (!recType) {
        typeError("ERROR_NOT_A_RECORD", "expected a record for field access");
    }
    if (!recType->listrecordfieldtype_) {
        typeError("ERROR_UNEXPECTED_RECORD_FIELDS", "record has no fields");
    }

    ARecordFieldType* found = nullptr;
    for (auto& rf : *recType->listrecordfieldtype_) {
        auto ar = dynamic_cast<ARecordFieldType*>(rf);
        if (ar) {
            if (ar->stellaident_ == fieldName) {
                found = ar;
                break;
            }
        }
    }

    if (!found || !found->type_) {
        typeError("ERROR_UNEXPECTED_FIELD_ACCESS", "field '" + fieldName + "' not present in record");
    }

    currentType_ = std::shared_ptr<Type>(found->type_->clone());
}

void VisitTypeCheck::visitDotTuple(DotTuple* dot_tuple) {
    if (!dot_tuple) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid DotTuple node");
    if (!dot_tuple->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing tuple expression");

    dot_tuple->expr_->accept(this);

    auto tupleTy = dynamic_cast<TypeTuple*>(currentType_.get());

    if (!tupleTy) {
        typeError("ERROR_NOT_A_TUPLE", "expected a tuple");
    }

    int idx = static_cast<int>(dot_tuple->integer_);
    if (idx < 1 || idx > 2) {
        typeError("ERROR_UNEXPECTED_TUPLE", "tuple index must be 1 or 2");
    }
    if (!tupleTy->listtype_ || tupleTy->listtype_->size() != 2)
        typeError("ERROR_UNEXPECTED_TUPLE", "tuple must have exactly two components");

    Type* targetComp = (*tupleTy->listtype_)[idx - 1];
    if (!targetComp) typeError("ERROR_UNEXPECTED_TUPLE", "tuple component missing");
    currentType_ = std::shared_ptr<Type>(targetComp->clone());
}

void VisitTypeCheck::visitTuple(Tuple* tuple) {
    if (!tuple->listexpr_ || tuple->listexpr_->size() != 2) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Tuple must have exactly two elements");
    }

    TypeTuple* expectedTT = nullptr;
    if (expectedType_) expectedTT = dynamic_cast<TypeTuple*>(expectedType_.get());

    auto listTypePtr = std::make_shared<ListType>();
    for (size_t i = 0; i < tuple->listexpr_->size(); ++i) {
        auto expr = (*tuple->listexpr_)[i];

        auto saved = expectedType_;
        if (expectedTT && expectedTT->listtype_ && i < expectedTT->listtype_->size()) {
            expectedType_ = std::shared_ptr<Type>((*expectedTT->listtype_)[i]->clone());
        } else {
            expectedType_.reset();
        }

        expr->accept(this);

        expectedType_ = saved;

        if (!currentType_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid tuple component type");
        listTypePtr->push_back(currentType_->clone());
    }

    currentType_ = std::make_shared<TypeTuple>(listTypePtr->clone());

    if (expectedType_) {
        if (dynamic_cast<TypeTuple*>(expectedType_.get())) {
            if (!typeEquals(currentType_.get(), expectedType_.get())) {
                typeError("ERROR_UNEXPECTED_TUPLE", "Tuple type doesn't match expected type");
            }
        }
    }
}

void VisitTypeCheck::visitRecord(Record* record) {
    if (!record || !record->listbinding_) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid record node");
    }

    TypeRecord* expectedRec = expectedType_ ? dynamic_cast<TypeRecord*>(expectedType_.get()) : nullptr;
    bool expectedAllNamed = true;
    std::vector<std::pair<std::string, Type*>> expectedList;
    if (expectedRec && expectedRec->listrecordfieldtype_) {
        for (auto& ef : *expectedRec->listrecordfieldtype_) {
            if (auto aef = dynamic_cast<ARecordFieldType*>(ef)) {
                expectedList.emplace_back(aef->stellaident_, aef->type_);
            } else {
                expectedAllNamed = false;
                break;
            }
        }
    } else {
        expectedAllNamed = false;
    }

    std::unordered_map<std::string, Type*> expectedMap;
    if (expectedAllNamed) {
        for (auto& p : expectedList) expectedMap[p.first] = p.second;
    }

    auto fields = std::make_shared<ListRecordFieldType>();
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> actualList;
    std::unordered_map<std::string, std::shared_ptr<Type>> actualMap;

    size_t idx = 0;
    for (auto& b : *record->listbinding_) {
        auto ab = dynamic_cast<ABinding*>(b);
        if (!ab) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid record binding");
        }
        if (!ab->expr_) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "record field missing expression");
        }

        std::shared_ptr<Type> savedExpected = expectedType_;
        if (!expectedRec) {
            expectedType_.reset();
        } else {
            if (expectedAllNamed) {
                auto it = expectedMap.find(ab->stellaident_);
                if (it != expectedMap.end() && it->second) {
                    expectedType_ = std::shared_ptr<Type>(it->second->clone());
                } else {
                    expectedType_.reset();
                }
            } else {
                if (expectedRec->listrecordfieldtype_ &&
                    expectedRec->listrecordfieldtype_->size() == record->listbinding_->size()) {
                    if (auto aef = dynamic_cast<ARecordFieldType*>((*expectedRec->listrecordfieldtype_)[idx])) {
                        expectedType_ = std::shared_ptr<Type>(aef->type_->clone());
                    } else {
                        expectedType_.reset();
                    }
                } else {
                    expectedType_.reset();
                }
            }
        }

        ab->expr_->accept(this);
        expectedType_ = savedExpected;

        if (!currentType_) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid record field expression");
        }

        ARecordFieldType* rft = new ARecordFieldType(ab->stellaident_, currentType_->clone());
        fields->push_back(rft);
        actualList.emplace_back(ab->stellaident_, std::shared_ptr<Type>(currentType_->clone()));
        actualMap[ab->stellaident_] = std::shared_ptr<Type>(currentType_->clone());
        ++idx;
    }

    currentType_ = std::make_shared<TypeRecord>(fields->clone());

    if (expectedRec) {
        bool deferredMismatch = false;

        if (expectedAllNamed) {
            std::unordered_map<std::string, Type*> expMap;
            for (auto& p : expectedList) expMap[p.first] = p.second;

            for (auto& kv : expMap) {
                if (actualMap.find(kv.first) == actualMap.end()) {
                    deferredMismatch = true;
                }
            }
            for (auto& kv : actualMap) {
                if (expMap.find(kv.first) == expMap.end()) {
                    deferredMismatch = true;
                }
            }
            for (auto& kv : expMap) {
                auto actIt = actualMap.find(kv.first);
                if (actIt != actualMap.end()) {
                    if (!typeEquals(kv.second, actIt->second.get())) {
                        deferredMismatch = true;
                    }
                }
            }
        } else {
            size_t expN = expectedRec->listrecordfieldtype_ ? expectedRec->listrecordfieldtype_->size() : 0;
            if (expN != actualList.size()) {
                deferredMismatch = true;
            }
            for (size_t i = 0; i < expN && i < actualList.size(); ++i) {
                Type* expectedFieldType = nullptr;
                if (auto aef = dynamic_cast<ARecordFieldType*>((*expectedRec->listrecordfieldtype_)[i])) {
                    expectedFieldType = aef->type_;
                } else {
                    deferredMismatch = true;
                }
                auto& actPair = actualList[i];
                if (expectedFieldType && !typeEquals(expectedFieldType, actPair.second.get())) {
                    deferredMismatch = true;
                }
            }
        }

        if (deferredMismatch) {
            return;
        }
    }
}

void VisitTypeCheck::visitConsList(ConsList* cons_list) {
    if (!cons_list) return;
    if (!cons_list->expr_1 || !cons_list->expr_2)
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "cons requires head and tail");

    TypeList* expectedTL = expectedType_ ? dynamic_cast<TypeList*>(expectedType_.get()) : nullptr;
    if (expectedTL) {
        if (!expectedTL->type_) typeError("ERROR_AMBIGUOUS_LIST_TYPE", "list element type is unknown");
        auto elemType = std::shared_ptr<Type>(expectedTL->type_->clone());

        {
            auto saved = expectedType_;
            expectedType_ = std::shared_ptr<Type>(elemType->clone());
            cons_list->expr_1->accept(this);
            expectedType_ = saved;
        }
        if (!currentType_ || !typeEquals(currentType_.get(), elemType.get()))
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "cons head type mismatch");

        {
            auto saved = expectedType_;
            expectedType_ = std::shared_ptr<Type>(expectedTL->clone());
            cons_list->expr_2->accept(this);
            expectedType_ = saved;
        }
        if (!dynamic_cast<TypeList*>(currentType_.get())) typeError("ERROR_NOT_A_LIST", "cons tail must be a list");

        currentType_ = std::shared_ptr<Type>(expectedTL->clone());
        return;
    }

    cons_list->expr_2->accept(this);
    auto tailTL = currentType_ ? dynamic_cast<TypeList*>(currentType_.get()) : nullptr;
    if (!tailTL) typeError("ERROR_NOT_A_LIST", "cons tail must be a list");
    if (!tailTL->type_) typeError("ERROR_AMBIGUOUS_LIST_TYPE", "list element type is unknown");

    auto elemType = std::shared_ptr<Type>(tailTL->type_->clone());
    {
        auto saved = expectedType_;
        expectedType_ = std::shared_ptr<Type>(elemType->clone());
        cons_list->expr_1->accept(this);
        expectedType_ = saved;
    }
    if (!currentType_ || !typeEquals(currentType_.get(), elemType.get()))
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "cons head type mismatch");

    currentType_ = std::shared_ptr<Type>(tailTL->clone());
}

void VisitTypeCheck::visitHead(Head* head) {
    if (!head || !head->expr_) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "head requires an argument");
    }

    head->expr_->accept(this);

    auto tl = currentType_ ? dynamic_cast<TypeList*>(currentType_.get()) : nullptr;
    if (!tl) {
        typeError("ERROR_NOT_A_LIST", "List::head expects a list");
    }

    if (!tl->type_) {
        typeError("ERROR_AMBIGUOUS_LIST_TYPE", "list element type is unknown");
    }

    currentType_ = std::shared_ptr<Type>(tl->type_->clone());
}

void VisitTypeCheck::visitIsEmpty(IsEmpty* is_empty) {
    if (is_empty->expr_) is_empty->expr_->accept(this);

    if (!dynamic_cast<TypeList*>(currentType_.get())) {
        typeError("ERROR_NOT_A_LIST", "List::isEmpty expects a list");
    }

    if (!dynamic_cast<TypeList*>(currentType_.get())->type_) {
        typeError("ERROR_AMBIGUOUS_LIST_TYPE", "list element type is unknown");
    }

    currentType_ = std::make_shared<TypeBool>();
}

void VisitTypeCheck::visitTail(Tail* tail) {
    if (!tail || !tail->expr_) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "tail requires an argument");
    }

    tail->expr_->accept(this);

    auto tl = currentType_ ? dynamic_cast<TypeList*>(currentType_.get()) : nullptr;
    if (!tl) {
        typeError("ERROR_NOT_A_LIST", "List::tail expects a list");
    }

    if (!tl->type_) {
        typeError("ERROR_AMBIGUOUS_LIST_TYPE", "list element type is unknown");
    }

    currentType_ = std::shared_ptr<Type>(tl->clone());
}

void VisitTypeCheck::visitInl(Inl* inl) {
    if (!inl || !inl->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "inl requires an expression");

    if (!expectedType_ || !dynamic_cast<TypeSum*>(expectedType_.get())) {
        typeError("ERROR_AMBIGUOUS_SUM_TYPE");
    }
    auto sum = dynamic_cast<TypeSum*>(expectedType_.get());
    if (!sum) typeError("ERROR_AMBIGUOUS_SUM_TYPE", "expected a sum type for inl");

    auto saved = expectedType_;
    expectedType_ = std::shared_ptr<Type>(sum->type_1->clone());
    inl->expr_->accept(this);
    expectedType_ = saved;

    if (!currentType_ || !typeEquals(currentType_.get(), sum->type_1)) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "inl inner expression type mismatch");
    }

    currentType_ = std::shared_ptr<Type>(sum->clone());
}

void VisitTypeCheck::visitInr(Inr* inr) {
    if (!inr || !inr->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "inr requires an expression");

    if (!expectedType_ || !dynamic_cast<TypeSum*>(expectedType_.get())) {
        typeError("ERROR_AMBIGUOUS_SUM_TYPE");
    }
    auto sum = dynamic_cast<TypeSum*>(expectedType_.get());
    if (!sum) typeError("ERROR_AMBIGUOUS_SUM_TYPE", "expected a sum type for inr");

    auto saved = expectedType_;
    expectedType_ = std::shared_ptr<Type>(sum->type_2->clone());
    inr->expr_->accept(this);
    expectedType_ = saved;

    if (!currentType_ || !typeEquals(currentType_.get(), sum->type_2)) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "inr inner expression type mismatch");
    }

    currentType_ = std::shared_ptr<Type>(sum->clone());
}

void VisitTypeCheck::visitSucc(Succ* succ) {
    /* Code For Succ Goes Here */

    if (succ->expr_) {
        succ->expr_->accept(this);
        if (!dynamic_cast<TypeNat*>(currentType_.get()))
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Succ expects Nat");
        currentType_ = std::make_shared<TypeNat>();
    }
}

void VisitTypeCheck::visitLogicNot(LogicNot* logic_not) {
    /* Code For LogicNot Goes Here */

    if (logic_not->expr_) logic_not->expr_->accept(this);
}

void VisitTypeCheck::visitPred(Pred* pred) {
    /* Code For Pred Goes Here */

    if (pred->expr_) pred->expr_->accept(this);

    if (!dynamic_cast<TypeNat*>(currentType_.get()))
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Pred expects Nat");
    currentType_ = std::make_shared<TypeNat>();
}

void VisitTypeCheck::visitIsZero(IsZero* is_zero) {
    if (is_zero->expr_) {
        is_zero->expr_->accept(this);
        if (!dynamic_cast<TypeNat*>(currentType_.get()))
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "IsZero expects Nat");
        currentType_ = std::make_shared<TypeBool>();
    }
}

void VisitTypeCheck::visitFix(Fix* fix) {
    if (!fix || !fix->expr_) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid fix node");

    fix->expr_->accept(this);
    auto tf = currentType_ ? dynamic_cast<TypeFun*>(currentType_.get()) : nullptr;
    if (!tf) {
        typeError("ERROR_NOT_A_FUNCTION", "fix argument must be a function");
    }
    if (!tf->listtype_ || tf->listtype_->size() != 1) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "fix function must take exactly one parameter");
    }

    Type* param = (*tf->listtype_)[0];
    Type* ret = tf->type_;
    if (!param || !ret) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid function type for fix");
    }

    if (!typeEquals(param, ret)) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "fix expects a function of type A -> A (parameter and result must match)");
    }
    currentType_ = std::shared_ptr<Type>(ret->clone());
}

void VisitTypeCheck::visitNatRec(NatRec* nat_rec) {
    if (!nat_rec->expr_1 || !nat_rec->expr_2 || !nat_rec->expr_3)
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Nat::rec requires 3 arguments");
    nat_rec->expr_1->accept(this);
    if (!dynamic_cast<TypeNat*>(currentType_.get()))
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Nat::rec first arg must be Nat");

    nat_rec->expr_2->accept(this);
    auto baseType = currentType_;

    {
        auto outerList = std::make_shared<ListType>();
        outerList->push_back(new TypeNat());
        auto innerList = std::make_shared<ListType>();
        innerList->push_back(baseType->clone());
        auto innerFun = std::make_shared<TypeFun>(innerList->clone(), baseType->clone());
        auto expectedStep = std::make_shared<TypeFun>(outerList->clone(), innerFun->clone());

        auto savedExpected = expectedType_;
        expectedType_ = expectedStep;
        nat_rec->expr_3->accept(this);
        expectedType_ = savedExpected;
    }

    auto stepType = currentType_;

    if (!dynamic_cast<TypeFun*>(stepType.get())) {
        typeError("ERROR_NOT_A_FUNCTION", "Nat::rec third arg must be a function");
    }

    auto stepFuncType = dynamic_cast<TypeFun*>(stepType.get());

    if (stepFuncType->listtype_->size() != 1) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Nat::rec step function must take exactly one parameter");
    }

    auto stepParamType = (*stepFuncType->listtype_)[0];
    if (!dynamic_cast<TypeNat*>(stepParamType)) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Nat::rec step function parameter must be Nat");
    }

    auto stepReturnType = stepFuncType->type_;
    if (!dynamic_cast<TypeFun*>(stepReturnType)) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Nat::rec step function must return a function");
    }

    auto innerFuncType = dynamic_cast<TypeFun*>(stepReturnType);

    if (innerFuncType->listtype_->size() != 1) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Inner function must take exactly one parameter");
    }

    auto innerParamType = (*innerFuncType->listtype_)[0];
    if (!typeEquals(innerParamType, baseType.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Inner function parameter type must match base type");
    }

    auto innerReturnType = innerFuncType->type_;
    if (!typeEquals(innerReturnType, baseType.get())) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Inner function return type must match base type");
    }

    currentType_ = baseType;
}

void VisitTypeCheck::visitFold(Fold* fold) {
    /* Code For Fold Goes Here */

    if (fold->type_) fold->type_->accept(this);
    if (fold->expr_) fold->expr_->accept(this);
}

void VisitTypeCheck::visitUnfold(Unfold* unfold) {
    /* Code For Unfold Goes Here */

    if (unfold->type_) unfold->type_->accept(this);
    if (unfold->expr_) unfold->expr_->accept(this);
}

void VisitTypeCheck::visitConstTrue(ConstTrue* const_true) { currentType_ = std::make_shared<TypeBool>(); }

void VisitTypeCheck::visitConstFalse(ConstFalse* const_false) { currentType_ = std::make_shared<TypeBool>(); }

void VisitTypeCheck::visitConstUnit(ConstUnit* const_unit) { currentType_ = std::make_shared<TypeUnit>(); }

void VisitTypeCheck::visitConstInt(ConstInt* const_int) {
    if (!const_int) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid integer literal");
    }
    if (const_int->integer_ < 0) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "negative integer literal");
    }
    currentType_ = std::make_shared<TypeNat>();
}

void VisitTypeCheck::visitConstMemory(ConstMemory* const_memory) {
    if (!const_memory) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "invalid memory literal");

    if (!expectedType_) {
        typeError("ERROR_AMBIGUOUS_MEMORY_ADDRESS", "memory address literal requires expected reference type");
    }

    if (!dynamic_cast<TypeRef*>(expectedType_.get())) {
        typeError("ERROR_UNEXPECTED_MEMORY_ADDRESS", "memory address literal requires reference expected type");
    }

    currentType_ = std::shared_ptr<Type>(expectedType_->clone());
}

void VisitTypeCheck::visitVar(Var* var) {
    auto ty = env_->lookup(var->stellaident_);

    if (!ty) {
        typeError("ERROR_UNDEFINED_VARIABLE", "Variable " + var->stellaident_ + " does not exist");
    }

    currentType_ = ty;
}

void VisitTypeCheck::visitAPatternBinding(APatternBinding* a_pattern_binding) {
    /* Code For APatternBinding Goes Here */

    if (a_pattern_binding->pattern_) a_pattern_binding->pattern_->accept(this);
    if (a_pattern_binding->expr_) a_pattern_binding->expr_->accept(this);
}

void VisitTypeCheck::visitAVariantFieldType(AVariantFieldType* a_variant_field_type) {
    visitStellaIdent(a_variant_field_type->stellaident_);
    if (a_variant_field_type->optionaltyping_) a_variant_field_type->optionaltyping_->accept(this);
}

void VisitTypeCheck::visitARecordFieldType(ARecordFieldType* a_record_field_type) {
    if (!a_record_field_type) return;
    visitStellaIdent(a_record_field_type->stellaident_);
    if (a_record_field_type->type_) {
        a_record_field_type->type_->accept(this);
    }
}

void VisitTypeCheck::visitListRecordFieldType(ListRecordFieldType* list_record_field_type) {
    if (!list_record_field_type) return;
    for (const auto& i : *list_record_field_type) {
        if (i) i->accept(this);
    }
}

void VisitTypeCheck::visitATyping(ATyping* a_typing) {
    /* Code For ATyping Goes Here */

    if (a_typing->expr_) a_typing->expr_->accept(this);
    if (a_typing->type_) a_typing->type_->accept(this);
}

void VisitTypeCheck::visitListStellaIdent(ListStellaIdent* list_stella_ident) {
    for (const auto& i : *list_stella_ident) {
        visitStellaIdent(i);
    }
}

void VisitTypeCheck::visitListExtensionName(ListExtensionName* list_extension_name) {
    for (const auto& i : *list_extension_name) {
        visitExtensionName(i);
    }
}

void VisitTypeCheck::visitListExtension(ListExtension* list_extension) {
    for (const auto& i : *list_extension) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListDecl(ListDecl* list_decl) {
    for (const auto& i : *list_decl) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListLocalDecl(ListLocalDecl* list_local_decl) {
    for (const auto& i : *list_local_decl) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListAnnotation(ListAnnotation* list_annotation) {
    for (const auto& i : *list_annotation) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListParamDecl(ListParamDecl* list_param_decl) {
    for (const auto& i : *list_param_decl) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListType(ListType* list_type) {
    for (const auto& i : *list_type) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListMatchCase(ListMatchCase* list_match_case) {
    for (const auto& i : *list_match_case) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListPattern(ListPattern* list_pattern) {
    for (const auto& i : *list_pattern) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListLabelledPattern(ListLabelledPattern* list_labelled_pattern) {
    for (const auto& i : *list_labelled_pattern) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListBinding(ListBinding* list_binding) {
    for (const auto& i : *list_binding) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListExpr(ListExpr* list_expr) {
    for (const auto& i : *list_expr) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListPatternBinding(ListPatternBinding* list_pattern_binding) {
    for (const auto& i : *list_pattern_binding) {
        i->accept(this);
    }
}

void VisitTypeCheck::visitListVariantFieldType(ListVariantFieldType* list_variant_field_type) {
    if (!list_variant_field_type) return;
    for (const auto& i : *list_variant_field_type) {
        if (i) i->accept(this);
    }
}

void VisitTypeCheck::visitInteger(Integer x) {
    if (x < 0) {
        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "negative integer literal");
    }
}

void VisitTypeCheck::visitChar(Char x) { /* Code for Char Goes Here */ }

void VisitTypeCheck::visitDouble(Double x) { /* Code for Double Goes Here */ }

void VisitTypeCheck::visitString(String x) { /* Code for String Goes Here */ }

void VisitTypeCheck::visitIdent(Ident x) { /* Code for Ident Goes Here */ }

void VisitTypeCheck::visitStellaIdent(StellaIdent x) { /* Code for StellaIdent Goes Here */ }

void VisitTypeCheck::visitExtensionName(ExtensionName x) { /* Code for ExtensionName Goes Here */ }

void VisitTypeCheck::visitMemoryAddress(MemoryAddress x) { /* Code for MemoryAddress Goes Here */ }

void VisitTypeCheck::visitDeclFunGeneric(DeclFunGeneric* decl_fun_generic) {
    /* Code For DeclFunGeneric Goes Here */

    if (decl_fun_generic->listannotation_) decl_fun_generic->listannotation_->accept(this);
    visitStellaIdent(decl_fun_generic->stellaident_);
    if (decl_fun_generic->liststellaident_) decl_fun_generic->liststellaident_->accept(this);
    if (decl_fun_generic->listparamdecl_) decl_fun_generic->listparamdecl_->accept(this);
    if (decl_fun_generic->returntype_) decl_fun_generic->returntype_->accept(this);
    if (decl_fun_generic->throwtype_) decl_fun_generic->throwtype_->accept(this);
    if (decl_fun_generic->listdecl_) decl_fun_generic->listdecl_->accept(this);
    if (decl_fun_generic->expr_) decl_fun_generic->expr_->accept(this);
}

void VisitTypeCheck::visitTypeAbstraction(TypeAbstraction* type_abstraction) {
    /* Code For TypeAbstraction Goes Here */

    if (type_abstraction->liststellaident_) type_abstraction->liststellaident_->accept(this);
    if (type_abstraction->expr_) type_abstraction->expr_->accept(this);
}

void VisitTypeCheck::visitTypeApplication(TypeApplication* type_application) {
    /* Code For TypeApplication Goes Here */

    if (type_application->expr_) type_application->expr_->accept(this);
    if (type_application->listtype_) type_application->listtype_->accept(this);
}

void VisitTypeCheck::visitTypeForAll(TypeForAll* type_for_all) {
    /* Code For TypeForAll Goes Here */

    if (type_for_all->liststellaident_) type_for_all->liststellaident_->accept(this);
    if (type_for_all->type_) type_for_all->type_->accept(this);
}

//---------------------------------------------------------------------------

[[noreturn]] void VisitTypeCheck::typeError(const char* tag, const std::string& msg) {
    if (msg.empty()) {
        std::cerr << tag << '\n';
    } else {
        std::cerr << tag << ": " << msg << '\n';
    }
    std::exit(1);
}

bool VisitTypeCheck::typeEquals(Type* a, Type* b) {
    if (a == b) return true;
    if (!a || !b) return false;

    if (dynamic_cast<TypeBool*>(a) && dynamic_cast<TypeBool*>(b)) return true;
    if (dynamic_cast<TypeNat*>(a) && dynamic_cast<TypeNat*>(b)) return true;
    if (dynamic_cast<TypeUnit*>(a) && dynamic_cast<TypeUnit*>(b)) return true;

    if (auto af = dynamic_cast<TypeFun*>(a)) {
        auto bf = dynamic_cast<TypeFun*>(b);
        if (!bf) return false;
        if (!af->listtype_ || !bf->listtype_) return false;
        if (af->listtype_->size() != bf->listtype_->size()) return false;
        for (size_t i = 0; i < af->listtype_->size(); ++i) {
            if (!typeEquals((*af->listtype_)[i], (*bf->listtype_)[i])) return false;
        }
        return typeEquals(af->type_, bf->type_);
    }

    if (auto at = dynamic_cast<TypeTuple*>(a)) {
        auto bt = dynamic_cast<TypeTuple*>(b);
        if (!bt) return false;
        if (!at->listtype_ || !bt->listtype_) return false;
        if (at->listtype_->size() != bt->listtype_->size()) return false;
        for (size_t i = 0; i < at->listtype_->size(); ++i) {
            if (!typeEquals((*at->listtype_)[i], (*bt->listtype_)[i])) return false;
        }
        return true;
    }

    if (auto as = dynamic_cast<TypeSum*>(a)) {
        auto bs = dynamic_cast<TypeSum*>(b);
        if (!bs) return false;
        if (!as->type_1 || !as->type_2 || !bs->type_1 || !bs->type_2) return false;
        return typeEquals(as->type_1, bs->type_1) && typeEquals(as->type_2, bs->type_2);
    }

    if (auto al = dynamic_cast<TypeList*>(a)) {
        auto bl = dynamic_cast<TypeList*>(b);
        if (!bl) return false;
        if (!al->type_ || !bl->type_) return false;
        return typeEquals(al->type_, bl->type_);
    }

    if (auto ar = dynamic_cast<TypeRef*>(a)) {
        auto br = dynamic_cast<TypeRef*>(b);
        if (!br) return false;
        if (!ar->type_ || !br->type_) return false;
        return typeEquals(ar->type_, br->type_);
    }

    if (auto av = dynamic_cast<TypeVariant*>(a)) {
        auto bv = dynamic_cast<TypeVariant*>(b);
        if (!bv) return false;
        if (!av->listvariantfieldtype_ || !bv->listvariantfieldtype_) return false;

        std::unordered_map<std::string, std::shared_ptr<Type>> mapA;
        std::unordered_map<std::string, std::shared_ptr<Type>> mapB;

        auto collect = [&](ListVariantFieldType* list,
                           std::unordered_map<std::string, std::shared_ptr<Type>>& out) -> bool {
            for (auto& raw : *list) {
                if (!raw) return false;
                auto vf = dynamic_cast<AVariantFieldType*>(raw);
                if (!vf) return false;
                if (vf->optionaltyping_) {
                    auto saveCur = currentType_;
                    auto saveExp = expectedType_;
                    expectedType_.reset();
                    vf->optionaltyping_->accept(this);
                    auto ty = currentType_;
                    currentType_ = saveCur;
                    expectedType_ = saveExp;
                    if (!ty) return false;
                    out[vf->stellaident_] = std::shared_ptr<Type>(ty->clone());
                } else {
                    out[vf->stellaident_] = std::make_shared<TypeUnit>();
                }
            }
            return true;
        };

        if (!collect(av->listvariantfieldtype_, mapA)) return false;
        if (!collect(bv->listvariantfieldtype_, mapB)) return false;
        if (mapA.size() != mapB.size()) return false;

        for (auto& kv : mapA) {
            auto it = mapB.find(kv.first);
            if (it == mapB.end()) return false;
            if (!typeEquals(kv.second.get(), it->second.get())) return false;
        }
        return true;
    }

    if (auto ar = dynamic_cast<TypeRecord*>(a)) {
        auto br = dynamic_cast<TypeRecord*>(b);
        if (!br) return false;
        if (!ar->listrecordfieldtype_ || !br->listrecordfieldtype_) return false;
        if (ar->listrecordfieldtype_->size() != br->listrecordfieldtype_->size()) return false;
        for (size_t i = 0; i < ar->listrecordfieldtype_->size(); ++i) {
            auto fa_raw = (*ar->listrecordfieldtype_)[i];
            auto fb_raw = (*br->listrecordfieldtype_)[i];
            if (!fa_raw || !fb_raw) return false;
            auto fa = dynamic_cast<ARecordFieldType*>(fa_raw);
            auto fb = dynamic_cast<ARecordFieldType*>(fb_raw);
            if (!fa || !fb) return false;
            if (fa->stellaident_ != fb->stellaident_) return false;
            if (!fa->type_ || !fb->type_) return false;
            if (!typeEquals(fa->type_, fb->type_)) return false;
        }
        return true;
    }

    return false;
}

std::unordered_map<std::string, std::shared_ptr<Type>>& Context::getTop() { return scopes_.top(); }

void Context::popScope() {
    if (!scopes_.empty()) {
        scopes_.pop();
    }
}
void Context::addScope() {
    if (scopes_.empty()) {
        scopes_.push({});
    } else {
        auto parent = scopes_.top();
        scopes_.push(parent);
    }
}

void Context::bindNew(const std::string& name, std::shared_ptr<Type> stellaType) {
    if (scopes_.empty()) throw "There is no scope";
    scopes_.top()[name] = std::move(stellaType);
}

std::shared_ptr<Type> Context::lookup(const std::string& name) const {
    if (scopes_.empty()) return nullptr;
    const auto& top = scopes_.top();
    auto it = top.find(name);
    return it == top.end() ? nullptr : it->second;
}

void VisitTypeCheck::print(Type* a) {
    std::function<void(Type*)> p = [&](Type* t) {
        if (!t) {
            std::cout << "<null>";
            return;
        }
        if (dynamic_cast<TypeBool*>(t)) {
            std::cout << "Bool";
            return;
        }
        if (dynamic_cast<TypeNat*>(t)) {
            std::cout << "Nat";
            return;
        }
        if (dynamic_cast<TypeUnit*>(t)) {
            std::cout << "Unit";
            return;
        }
        if (dynamic_cast<TypeTop*>(t)) {
            std::cout << "Top";
            return;
        }
        if (dynamic_cast<TypeBottom*>(t)) {
            std::cout << "Bottom";
            return;
        }

        if (auto tf = dynamic_cast<TypeFun*>(t)) {
            std::cout << "Fun(";
            if (tf->listtype_ && !tf->listtype_->empty()) {
                for (size_t i = 0; i < tf->listtype_->size(); ++i) {
                    if (i) std::cout << ", ";
                    p((*tf->listtype_)[i]);
                }
            } else {
                std::cout << "<no-params>";
            }
            std::cout << ") -> ";
            p(tf->type_);
            return;
        }
        if (auto tt = dynamic_cast<TypeTuple*>(t)) {
            std::cout << "Tuple(";
            if (tt->listtype_ && !tt->listtype_->empty()) {
                for (size_t i = 0; i < tt->listtype_->size(); ++i) {
                    if (i) std::cout << ", ";
                    p((*tt->listtype_)[i]);
                }
            } else {
                std::cout << "<empty>";
            }
            std::cout << ")";
            return;
        }
        if (auto ts = dynamic_cast<TypeSum*>(t)) {
            std::cout << "Sum(";
            p(ts->type_1);
            std::cout << " | ";
            p(ts->type_2);
            std::cout << ")";
            return;
        }
        if (auto tl = dynamic_cast<TypeList*>(t)) {
            std::cout << "List[";
            if (tl->type_)
                p(tl->type_);
            else
                std::cout << "<elem?>";
            std::cout << "]";
            return;
        }
        if (auto tr = dynamic_cast<TypeRef*>(t)) {
            std::cout << "Ref<";
            if (tr->type_)
                p(tr->type_);
            else
                std::cout << "<inner?>";
            std::cout << ">";
            return;
        }
        if (auto trec = dynamic_cast<TypeRecord*>(t)) {
            std::cout << "Record{";
            if (trec->listrecordfieldtype_ && !trec->listrecordfieldtype_->empty()) {
                bool first = true;
                for (auto rf : *trec->listrecordfieldtype_) {
                    if (!rf) continue;
                    auto arf = dynamic_cast<ARecordFieldType*>(rf);
                    if (!arf) continue;
                    if (!first) std::cout << ", ";
                    first = false;
                    std::cout << arf->stellaident_ << " : ";
                    if (arf->type_)
                        p(arf->type_);
                    else
                        std::cout << "<type?>";
                }
            } else {
                std::cout << "<no-fields>";
            }
            std::cout << "}";
            return;
        }

        if (auto tv = dynamic_cast<TypeVariant*>(t)) {
            std::cout << "Variant{";
            if (tv->listvariantfieldtype_ && !tv->listvariantfieldtype_->empty()) {
                bool first = true;
                for (auto vf : *tv->listvariantfieldtype_) {
                    if (!vf) continue;
                    auto av = dynamic_cast<AVariantFieldType*>(vf);
                    if (!av) continue;
                    if (!first) std::cout << ", ";
                    first = false;
                    std::cout << av->stellaident_ << " : ";
                    if (av->optionaltyping_) {
                        std::cout << "<payload?>";
                    } else {
                        std::cout << "Unit";
                    }
                }
            } else {
                std::cout << "<no-fields>";
            }
            std::cout << "}";
            return;
        }

        if (auto tv = dynamic_cast<TypeVar*>(t)) {
            if (!tv->stellaident_.empty())
                std::cout << "Var(" << tv->stellaident_ << ")";
            else
                std::cout << "TypeVar";
            return;
        }

        std::cout << "<unknown-type>";
    };

    p(a);
    std::cout << std::endl;
}
}  // namespace Stella
