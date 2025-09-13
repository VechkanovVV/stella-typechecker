#include "VisitTypeCheck.h"
#include "Context.h"
#include <iostream>
#include <utility>

namespace Stella
{
    void VisitTypeCheck::visitProgram(Program* p) {}                   // abstract class
    void VisitTypeCheck::visitLanguageDecl(LanguageDecl* p) {}         // abstract class
    void VisitTypeCheck::visitExtension(Extension* p) {}               // abstract class
    void VisitTypeCheck::visitDecl(Decl* p) {}                         // abstract class
    void VisitTypeCheck::visitLocalDecl(LocalDecl* p) {}               // abstract class
    void VisitTypeCheck::visitAnnotation(Annotation* p) {}             // abstract class
    void VisitTypeCheck::visitParamDecl(ParamDecl* p) {}               // abstract class
    void VisitTypeCheck::visitReturnType(ReturnType* p) {}             // abstract class
    void VisitTypeCheck::visitThrowType(ThrowType* p) {}               // abstract class
    void VisitTypeCheck::visitType(Type* p) {}                         // abstract class
    void VisitTypeCheck::visitMatchCase(MatchCase* p) {}               // abstract class
    void VisitTypeCheck::visitOptionalTyping(OptionalTyping* p) {}     // abstract class
    void VisitTypeCheck::visitPatternData(PatternData* p) {}           // abstract class
    void VisitTypeCheck::visitExprData(ExprData* p) {}                 // abstract class
    void VisitTypeCheck::visitPattern(Pattern* p) {}                   // abstract class
    void VisitTypeCheck::visitLabelledPattern(LabelledPattern* p) {}   // abstract class
    void VisitTypeCheck::visitBinding(Binding* p) {}                   // abstract class
    void VisitTypeCheck::visitExpr(Expr* p) {}                         // abstract class
    void VisitTypeCheck::visitPatternBinding(PatternBinding* p) {}     // abstract class
    void VisitTypeCheck::visitVariantFieldType(VariantFieldType* p) {} // abstract class
    void VisitTypeCheck::visitRecordFieldType(RecordFieldType* p) {}   // abstract class
    void VisitTypeCheck::visitTyping(Typing* p) {}                     // abstract class
    void VisitTypeCheck::visitPatternCastAs(PatternCastAs* p) {}       // abstract class
    void VisitTypeCheck::visitTryCastAs(TryCastAs* p) {}               // abstract class
    void VisitTypeCheck::visitPatternAsc(PatternAsc* p) {}             // abstract class
    void VisitTypeCheck::visitTypeAuto(TypeAuto* p) {}                 // abstract class

    void VisitTypeCheck::visitAProgram(AProgram* a_program)
    {
        env_ = std::make_shared<Context>();
        env_->addScope();

        if (a_program->languagedecl_) a_program->languagedecl_->accept(this);
        if (a_program->listextension_) a_program->listextension_->accept(this);
        
        if (a_program->listdecl_) {
            for (auto decl : *a_program->listdecl_) {
                if (auto f = dynamic_cast<DeclFun*>(decl)) {
                    const std::string name = f->stellaident_;
                    if (!f->listparamdecl_ || f->listparamdecl_->size() != 1) {
                        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "function must have exactly one parameter");
                    }

                    auto param = dynamic_cast<AParamDecl*>(f->listparamdecl_->front());
                    if (!param || !param->type_) {
                        typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "parameter must have explicit type");
                    }

                    param->accept(this);
                    auto paramType = currentType_;

                    auto* srt = dynamic_cast<SomeReturnType*>(f->returntype_);
                    if (!srt || !srt->type_) {
                         typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "missing return type");
                    }
                    
                    srt->type_->accept(this);
                    auto retTy = currentType_;

                    ListType* listTypePtr = new ListType();
                    listTypePtr->push_back(paramType.get()); 
                    auto funcType = std::make_shared<TypeFun>(listTypePtr, retTy.get());
                    env_->bindNew(name, funcType);
                }
            }
        }

        if (!env_->lookup("main")) {
            typeError("ERROR_MISSING_MAIN", "program has no main");
        }

        if (a_program->listdecl_) {
            a_program->listdecl_->accept(this);
        }
    }

    void VisitTypeCheck::visitLanguageCore(LanguageCore* language_core)
    {
        /* Code For LanguageCore Goes Here */
    }

    void VisitTypeCheck::visitAnExtension(AnExtension* an_extension)
    {
        /* Code For AnExtension Goes Here */

        if (an_extension->listextensionname_)
            an_extension->listextensionname_->accept(this);
    }

    void VisitTypeCheck::visitDeclFun(DeclFun* decl_fun)
    {
        env_->addScope();

        if (decl_fun->listannotation_) {
            decl_fun->listannotation_->accept(this);
        }
        
        visitStellaIdent(decl_fun->stellaident_);
        if (decl_fun->listparamdecl_) {
            decl_fun->listparamdecl_->accept(this);

        }
        if (decl_fun->returntype_)
            decl_fun->returntype_->accept(this);
        if (decl_fun->throwtype_)
            decl_fun->throwtype_->accept(this);
        if (decl_fun->listdecl_)
            decl_fun->listdecl_->accept(this);
        if (decl_fun->expr_)
            decl_fun->expr_->accept(this);

        env_->popScope();
    }

    void VisitTypeCheck::visitDeclTypeAlias(DeclTypeAlias* decl_type_alias)
    {
        /* Code For DeclTypeAlias Goes Here */

        visitStellaIdent(decl_type_alias->stellaident_);
        if (decl_type_alias->type_)
            decl_type_alias->type_->accept(this);
    }

    void VisitTypeCheck::visitDeclExceptionType(DeclExceptionType* decl_exception_type)
    {
        /* Code For DeclExceptionType Goes Here */

        if (decl_exception_type->type_)
            decl_exception_type->type_->accept(this);
    }

    void VisitTypeCheck::visitDeclExceptionVariant(DeclExceptionVariant* decl_exception_variant)
    {
        /* Code For DeclExceptionVariant Goes Here */

        visitStellaIdent(decl_exception_variant->stellaident_);
        if (decl_exception_variant->type_)
            decl_exception_variant->type_->accept(this);
    }

    void VisitTypeCheck::visitAssign(Assign* assign)
    {
        /* Code For Assign Goes Here */

        if (assign->expr_1)
            assign->expr_1->accept(this);
        if (assign->expr_2)
            assign->expr_2->accept(this);
    }

    void VisitTypeCheck::visitRef(Ref* ref)
    {
        /* Code For Ref Goes Here */

        if (ref->expr_)
            ref->expr_->accept(this);
    }

    void VisitTypeCheck::visitDeref(Deref* deref)
    {
        /* Code For Deref Goes Here */

        if (deref->expr_)
            deref->expr_->accept(this);
    }

    void VisitTypeCheck::visitPanic(Panic* panic)
    {
        /* Code For Panic Goes Here */
    }

    void VisitTypeCheck::visitThrow(Throw* throw_)
    {
        /* Code For Throw Goes Here */

        if (throw_->expr_)
            throw_->expr_->accept(this);
    }

    void VisitTypeCheck::visitTryCatch(TryCatch* try_catch)
    {
        /* Code For TryCatch Goes Here */

        if (try_catch->expr_1)
            try_catch->expr_1->accept(this);
        if (try_catch->pattern_)
            try_catch->pattern_->accept(this);
        if (try_catch->expr_2)
            try_catch->expr_2->accept(this);
    }

    void VisitTypeCheck::visitTryWith(TryWith* try_with)
    {
        /* Code For TryWith Goes Here */

        if (try_with->expr_1)
            try_with->expr_1->accept(this);
        if (try_with->expr_2)
            try_with->expr_2->accept(this);
    }

    void VisitTypeCheck::visitALocalDecl(ALocalDecl* a_local_decl)
    {
        /* Code For ALocalDecl Goes Here */

        if (a_local_decl->decl_)
            a_local_decl->decl_->accept(this);
    }

    void VisitTypeCheck::visitInlineAnnotation(InlineAnnotation* inline_annotation)
    {
        /* Code For InlineAnnotation Goes Here */
    }

    void VisitTypeCheck::visitAParamDecl(AParamDecl* a_param_decl)
    {
        /* Code For AParamDecl Goes Here */
        std::cout << "Kek: " << a_param_decl->stellaident_ << std::endl;
        visitStellaIdent(a_param_decl->stellaident_);
        if (a_param_decl->type_)
            a_param_decl->type_->accept(this);
        env_->bindNew(a_param_decl->stellaident_, currentType_);
    }

    void VisitTypeCheck::visitNoReturnType(NoReturnType* no_return_type)
    {
        /* Code For NoReturnType Goes Here */
    }

    void VisitTypeCheck::visitSomeReturnType(SomeReturnType* some_return_type)
    {
        /* Code For SomeReturnType Goes Here */

        if (some_return_type->type_)
            some_return_type->type_->accept(this);
    }

    void VisitTypeCheck::visitNoThrowType(NoThrowType* no_throw_type)
    {
        /* Code For NoThrowType Goes Here */
    }

    void VisitTypeCheck::visitSomeThrowType(SomeThrowType* some_throw_type)
    {
        /* Code For SomeThrowType Goes Here */

        if (some_throw_type->listtype_)
            some_throw_type->listtype_->accept(this);
    }

    void VisitTypeCheck::visitTypeFun(TypeFun* type_fun)
    {
        /* Code For TypeFun Goes Here */

        if (type_fun->listtype_)
            type_fun->listtype_->accept(this);
        if (type_fun->type_)
            type_fun->type_->accept(this);
    }

    void VisitTypeCheck::visitTypeRec(TypeRec* type_rec)
    {
        /* Code For TypeRec Goes Here */

        visitStellaIdent(type_rec->stellaident_);
        if (type_rec->type_)
            type_rec->type_->accept(this);
    }

    void VisitTypeCheck::visitTypeSum(TypeSum* type_sum)
    {
        /* Code For TypeSum Goes Here */

        if (type_sum->type_1)
            type_sum->type_1->accept(this);
        if (type_sum->type_2)
            type_sum->type_2->accept(this);
    }

    void VisitTypeCheck::visitTypeTuple(TypeTuple* type_tuple)
    {
        /* Code For TypeTuple Goes Here */

        if (type_tuple->listtype_)
            type_tuple->listtype_->accept(this);
    }

    void VisitTypeCheck::visitTypeRecord(TypeRecord* type_record)
    {
        /* Code For TypeRecord Goes Here */

        if (type_record->listrecordfieldtype_)
            type_record->listrecordfieldtype_->accept(this);
    }

    void VisitTypeCheck::visitTypeVariant(TypeVariant* type_variant)
    {
        /* Code For TypeVariant Goes Here */

        if (type_variant->listvariantfieldtype_)
            type_variant->listvariantfieldtype_->accept(this);
    }

    void VisitTypeCheck::visitTypeList(TypeList* type_list)
    {
        /* Code For TypeList Goes Here */

        if (type_list->type_)
            type_list->type_->accept(this);
    }

    void VisitTypeCheck::visitTypeBool(TypeBool* type_bool)
    {
       currentType_ = std::make_shared<TypeBool>();
    }

    void VisitTypeCheck::visitTypeNat(TypeNat* type_nat)
    {
        currentType_ = std::make_shared<TypeNat>();
    }

    void VisitTypeCheck::visitTypeUnit(TypeUnit* type_unit)
    {
        currentType_ = std::make_shared<TypeUnit>();
    }

    void VisitTypeCheck::visitTypeTop(TypeTop* type_top)
    {
        /* Code For TypeTop Goes Here */
    }

    void VisitTypeCheck::visitTypeBottom(TypeBottom* type_bottom)
    {
        /* Code For TypeBottom Goes Here */
    }

    void VisitTypeCheck::visitTypeRef(TypeRef* type_ref)
    {
        /* Code For TypeRef Goes Here */

        if (type_ref->type_)
            type_ref->type_->accept(this);
    }

    void VisitTypeCheck::visitTypeVar(TypeVar* type_var)
    {
        /* Code For TypeVar Goes Here */

        visitStellaIdent(type_var->stellaident_);
    }

    void VisitTypeCheck::visitAMatchCase(AMatchCase* a_match_case)
    {
        /* Code For AMatchCase Goes Here */

        if (a_match_case->pattern_)
            a_match_case->pattern_->accept(this);
        if (a_match_case->expr_)
            a_match_case->expr_->accept(this);
    }

    void VisitTypeCheck::visitNoTyping(NoTyping* no_typing)
    {
        /* Code For NoTyping Goes Here */
    }

    void VisitTypeCheck::visitSomeTyping(SomeTyping* some_typing)
    {
        /* Code For SomeTyping Goes Here */

        if (some_typing->type_)
            some_typing->type_->accept(this);
    }

    void VisitTypeCheck::visitNoPatternData(NoPatternData* no_pattern_data)
    {
        /* Code For NoPatternData Goes Here */
    }

    void VisitTypeCheck::visitSomePatternData(SomePatternData* some_pattern_data)
    {
        /* Code For SomePatternData Goes Here */

        if (some_pattern_data->pattern_)
            some_pattern_data->pattern_->accept(this);
    }

    void VisitTypeCheck::visitNoExprData(NoExprData* no_expr_data)
    {
        /* Code For NoExprData Goes Here */
    }

    void VisitTypeCheck::visitSomeExprData(SomeExprData* some_expr_data)
    {
        /* Code For SomeExprData Goes Here */

        if (some_expr_data->expr_)
            some_expr_data->expr_->accept(this);
    }

    void VisitTypeCheck::visitPatternVariant(PatternVariant* pattern_variant)
    {
        /* Code For PatternVariant Goes Here */

        visitStellaIdent(pattern_variant->stellaident_);
        if (pattern_variant->patterndata_)
            pattern_variant->patterndata_->accept(this);
    }

    void VisitTypeCheck::visitPatternInl(PatternInl* pattern_inl)
    {
        /* Code For PatternInl Goes Here */

        if (pattern_inl->pattern_)
            pattern_inl->pattern_->accept(this);
    }

    void VisitTypeCheck::visitPatternInr(PatternInr* pattern_inr)
    {
        /* Code For PatternInr Goes Here */

        if (pattern_inr->pattern_)
            pattern_inr->pattern_->accept(this);
    }

    void VisitTypeCheck::visitPatternTuple(PatternTuple* pattern_tuple)
    {
        /* Code For PatternTuple Goes Here */

        if (pattern_tuple->listpattern_)
            pattern_tuple->listpattern_->accept(this);
    }

    void VisitTypeCheck::visitPatternRecord(PatternRecord* pattern_record)
    {
        /* Code For PatternRecord Goes Here */

        if (pattern_record->listlabelledpattern_)
            pattern_record->listlabelledpattern_->accept(this);
    }

    void VisitTypeCheck::visitPatternList(PatternList* pattern_list)
    {
        /* Code For PatternList Goes Here */

        if (pattern_list->listpattern_)
            pattern_list->listpattern_->accept(this);
    }

    void VisitTypeCheck::visitPatternCons(PatternCons* pattern_cons)
    {
        /* Code For PatternCons Goes Here */

        if (pattern_cons->pattern_1)
            pattern_cons->pattern_1->accept(this);
        if (pattern_cons->pattern_2)
            pattern_cons->pattern_2->accept(this);
    }

    void VisitTypeCheck::visitPatternFalse(PatternFalse* pattern_false)
    {
        /* Code For PatternFalse Goes Here */
    }

    void VisitTypeCheck::visitPatternTrue(PatternTrue* pattern_true)
    {
        /* Code For PatternTrue Goes Here */
    }

    void VisitTypeCheck::visitPatternUnit(PatternUnit* pattern_unit)
    {
        /* Code For PatternUnit Goes Here */
    }

    void VisitTypeCheck::visitPatternInt(PatternInt* pattern_int)
    {
        /* Code For PatternInt Goes Here */

        visitInteger(pattern_int->integer_);
    }

    void VisitTypeCheck::visitPatternSucc(PatternSucc* pattern_succ)
    {
        /* Code For PatternSucc Goes Here */

        if (pattern_succ->pattern_)
            pattern_succ->pattern_->accept(this);
    }

    void VisitTypeCheck::visitPatternVar(PatternVar* pattern_var)
    {
        /* Code For PatternVar Goes Here */

        visitStellaIdent(pattern_var->stellaident_);
    }

    void VisitTypeCheck::visitALabelledPattern(ALabelledPattern* a_labelled_pattern)
    {
        /* Code For ALabelledPattern Goes Here */

        visitStellaIdent(a_labelled_pattern->stellaident_);
        if (a_labelled_pattern->pattern_)
            a_labelled_pattern->pattern_->accept(this);
    }

    void VisitTypeCheck::visitABinding(ABinding* a_binding)
    {
        /* Code For ABinding Goes Here */

        visitStellaIdent(a_binding->stellaident_);
        if (a_binding->expr_)
            a_binding->expr_->accept(this);
    }

    void VisitTypeCheck::visitSequence(Sequence* sequence)
    {
        /* Code For Sequence Goes Here */

        if (sequence->expr_1)
            sequence->expr_1->accept(this);
        if (sequence->expr_2)
            sequence->expr_2->accept(this);
    }

    void VisitTypeCheck::visitIf(If* if_)
    {
        //condition
        if (if_->expr_1) {
            if_->expr_1->accept(this);
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

        if (!typeEquals(thenType, elseType)) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "then/selde branch type mismatch");
        }
        
        currentType_ = thenType;
    }

    void VisitTypeCheck::visitLet(Let* let)
    {
        /* Code For Let Goes Here */

        if (let->listpatternbinding_)
            let->listpatternbinding_->accept(this);
        if (let->expr_)
            let->expr_->accept(this);
    }

    void VisitTypeCheck::visitLetRec(LetRec* let_rec)
    {
        /* Code For LetRec Goes Here */

        if (let_rec->listpatternbinding_)
            let_rec->listpatternbinding_->accept(this);
        if (let_rec->expr_)
            let_rec->expr_->accept(this);
    }

    void VisitTypeCheck::visitLessThan(LessThan* less_than)
    {
        /* Code For LessThan Goes Here */

        if (less_than->expr_1)
            less_than->expr_1->accept(this);
        if (less_than->expr_2)
            less_than->expr_2->accept(this);
    }

    void VisitTypeCheck::visitLessThanOrEqual(LessThanOrEqual* less_than_or_equal)
    {
        /* Code For LessThanOrEqual Goes Here */

        if (less_than_or_equal->expr_1)
            less_than_or_equal->expr_1->accept(this);
        if (less_than_or_equal->expr_2)
            less_than_or_equal->expr_2->accept(this);
    }

    void VisitTypeCheck::visitGreaterThan(GreaterThan* greater_than)
    {
        /* Code For GreaterThan Goes Here */

        if (greater_than->expr_1)
            greater_than->expr_1->accept(this);
        if (greater_than->expr_2)
            greater_than->expr_2->accept(this);
    }

    void VisitTypeCheck::visitGreaterThanOrEqual(GreaterThanOrEqual* greater_than_or_equal)
    {
        /* Code For GreaterThanOrEqual Goes Here */

        if (greater_than_or_equal->expr_1)
            greater_than_or_equal->expr_1->accept(this);
        if (greater_than_or_equal->expr_2)
            greater_than_or_equal->expr_2->accept(this);
    }

    void VisitTypeCheck::visitEqual(Equal* equal)
    {
        /* Code For Equal Goes Here */

        if (equal->expr_1)
            equal->expr_1->accept(this);
        if (equal->expr_2)
            equal->expr_2->accept(this);
    }

    void VisitTypeCheck::visitNotEqual(NotEqual* not_equal)
    {
        /* Code For NotEqual Goes Here */

        if (not_equal->expr_1)
            not_equal->expr_1->accept(this);
        if (not_equal->expr_2)
            not_equal->expr_2->accept(this);
    }

    void VisitTypeCheck::visitTypeAsc(TypeAsc* type_asc)
    {
        /* Code For TypeAsc Goes Here */

        if (type_asc->expr_)
            type_asc->expr_->accept(this);
        if (type_asc->type_)
            type_asc->type_->accept(this);
    }

    void VisitTypeCheck::visitTypeCast(TypeCast* type_cast)
    {
        /* Code For TypeCast Goes Here */

        if (type_cast->expr_)
            type_cast->expr_->accept(this);
        if (type_cast->type_)
            type_cast->type_->accept(this);
    }

    void VisitTypeCheck::visitAbstraction(Abstraction* abstraction)
    {
        /* Code For Abstraction Goes Here */

        if (abstraction->listparamdecl_)
            abstraction->listparamdecl_->accept(this);
        if (abstraction->expr_)
            abstraction->expr_->accept(this);
    }

    void VisitTypeCheck::visitVariant(Variant* variant)
    {
        /* Code For Variant Goes Here */

        visitStellaIdent(variant->stellaident_);
        if (variant->exprdata_)
            variant->exprdata_->accept(this);
    }

    void VisitTypeCheck::visitMatch(Match* match)
    {
        /* Code For Match Goes Here */

        if (match->expr_)
            match->expr_->accept(this);
        if (match->listmatchcase_)
            match->listmatchcase_->accept(this);
    }

    void VisitTypeCheck::visitList(List* list)
    {
        /* Code For List Goes Here */

        if (list->listexpr_)
            list->listexpr_->accept(this);
    }

    void VisitTypeCheck::visitAdd(Add* add)
    {
        /* Code For Add Goes Here */

        if (add->expr_1)
            add->expr_1->accept(this);
        if (add->expr_2)
            add->expr_2->accept(this);
    }

    void VisitTypeCheck::visitSubtract(Subtract* subtract)
    {
        /* Code For Subtract Goes Here */

        if (subtract->expr_1)
            subtract->expr_1->accept(this);
        if (subtract->expr_2)
            subtract->expr_2->accept(this);
    }

    void VisitTypeCheck::visitLogicOr(LogicOr* logic_or)
    {
        /* Code For LogicOr Goes Here */

        if (logic_or->expr_1)
            logic_or->expr_1->accept(this);
        if (logic_or->expr_2)
            logic_or->expr_2->accept(this);
    }

    void VisitTypeCheck::visitMultiply(Multiply* multiply)
    {
        /* Code For Multiply Goes Here */

        if (multiply->expr_1)
            multiply->expr_1->accept(this);
        if (multiply->expr_2)
            multiply->expr_2->accept(this);
    }

    void VisitTypeCheck::visitDivide(Divide* divide)
    {
        /* Code For Divide Goes Here */

        if (divide->expr_1)
            divide->expr_1->accept(this);
        if (divide->expr_2)
            divide->expr_2->accept(this);
    }

    void VisitTypeCheck::visitLogicAnd(LogicAnd* logic_and)
    {
        /* Code For LogicAnd Goes Here */

        if (logic_and->expr_1)
            logic_and->expr_1->accept(this);
        if (logic_and->expr_2)
            logic_and->expr_2->accept(this);
    }

    void VisitTypeCheck::visitApplication(Application* application)
    {   
        std::cout << application->expr_->line_number << std::endl;

        if (application->expr_) {   
            application->expr_->accept(this);
            if (!dynamic_cast<TypeFun*>(currentType_.get())) {
                typeError("ERROR_NOT_A_FUNCTION", "Function was expected");
            }
        }

        auto calleeType = currentType_; 
        if (!application->listexpr_ || application->listexpr_->size() != 1) {
            typeError("Functions with one parameter are supported");
        }

        if (application->listexpr_)
            application->listexpr_->accept(this);
    }

    void VisitTypeCheck::visitDotRecord(DotRecord* dot_record)
    {
        /* Code For DotRecord Goes Here */

        if (dot_record->expr_)
            dot_record->expr_->accept(this);
        visitStellaIdent(dot_record->stellaident_);
    }

    void VisitTypeCheck::visitDotTuple(DotTuple* dot_tuple)
    {
        /* Code For DotTuple Goes Here */

        if (dot_tuple->expr_)
            dot_tuple->expr_->accept(this);
        visitInteger(dot_tuple->integer_);
    }

    void VisitTypeCheck::visitTuple(Tuple* tuple)
    {
        /* Code For Tuple Goes Here */

        if (tuple->listexpr_)
            tuple->listexpr_->accept(this);
    }

    void VisitTypeCheck::visitRecord(Record* record)
    {
        /* Code For Record Goes Here */

        if (record->listbinding_)
            record->listbinding_->accept(this);
    }

    void VisitTypeCheck::visitConsList(ConsList* cons_list)
    {
        /* Code For ConsList Goes Here */

        if (cons_list->expr_1)
            cons_list->expr_1->accept(this);
        if (cons_list->expr_2)
            cons_list->expr_2->accept(this);
    }

    void VisitTypeCheck::visitHead(Head* head)
    {
        /* Code For Head Goes Here */

        if (head->expr_)
            head->expr_->accept(this);
    }

    void VisitTypeCheck::visitIsEmpty(IsEmpty* is_empty)
    {
        /* Code For IsEmpty Goes Here */

        if (is_empty->expr_)
            is_empty->expr_->accept(this);
    }

    void VisitTypeCheck::visitTail(Tail* tail)
    {
        /* Code For Tail Goes Here */

        if (tail->expr_)
            tail->expr_->accept(this);
    }

    void VisitTypeCheck::visitInl(Inl* inl)
    {
        /* Code For Inl Goes Here */

        if (inl->expr_)
            inl->expr_->accept(this);
    }

    void VisitTypeCheck::visitInr(Inr* inr)
    {
        /* Code For Inr Goes Here */

        if (inr->expr_)
            inr->expr_->accept(this);
    }

    void VisitTypeCheck::visitSucc(Succ* succ)
    {
        /* Code For Succ Goes Here */

        if (succ->expr_) {
            succ->expr_->accept(this);
            if (!dynamic_cast<TypeNat*>(currentType_.get())) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "Succ expects Nat");
            currentType_ = std::make_shared<TypeNat>();
        }
    }

    void VisitTypeCheck::visitLogicNot(LogicNot* logic_not)
    {
        /* Code For LogicNot Goes Here */

        if (logic_not->expr_)
            logic_not->expr_->accept(this);
    }

    void VisitTypeCheck::visitPred(Pred* pred)
    {
        /* Code For Pred Goes Here */

        if (pred->expr_)
            pred->expr_->accept(this);
    }

    void VisitTypeCheck::visitIsZero(IsZero* is_zero)
    {
        if (is_zero->expr_) {
            is_zero->expr_->accept(this);
            if (!dynamic_cast<TypeNat*>(currentType_.get())) typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "IsZero expects Bool");
            currentType_ = std::make_shared<TypeBool>();
        }
    }

    void VisitTypeCheck::visitFix(Fix* fix)
    {
        /* Code For Fix Goes Here */

        if (fix->expr_)
            fix->expr_->accept(this);
    }

    void VisitTypeCheck::visitNatRec(NatRec* nat_rec)
    {
        /* Code For NatRec Goes Here */

        if (nat_rec->expr_1)
            nat_rec->expr_1->accept(this);
        if (nat_rec->expr_2)
            nat_rec->expr_2->accept(this);
        if (nat_rec->expr_3)
            nat_rec->expr_3->accept(this);
    }

    void VisitTypeCheck::visitFold(Fold* fold)
    {
        /* Code For Fold Goes Here */

        if (fold->type_)
            fold->type_->accept(this);
        if (fold->expr_)
            fold->expr_->accept(this);
    }

    void VisitTypeCheck::visitUnfold(Unfold* unfold)
    {
        /* Code For Unfold Goes Here */

        if (unfold->type_)
            unfold->type_->accept(this);
        if (unfold->expr_)
            unfold->expr_->accept(this);
    }

    void VisitTypeCheck::visitConstTrue(ConstTrue* const_true)
    {
        currentType_ = std::make_shared<TypeBool>();
    }

    void VisitTypeCheck::visitConstFalse(ConstFalse* const_false)
    {
        currentType_ = std::make_shared<TypeBool>();
    }

    void VisitTypeCheck::visitConstUnit(ConstUnit* const_unit)
    {
        currentType_ = std::make_shared<TypeUnit>();
    }

    void VisitTypeCheck::visitConstInt(ConstInt* const_int)
    {
        /* Code For ConstInt Goes Here */

        visitInteger(const_int->integer_);
        currentType_ = std::make_shared<TypeNat>();
    }

    void VisitTypeCheck::visitConstMemory(ConstMemory* const_memory)
    {
        /* Code For ConstMemory Goes Here */

        visitMemoryAddress(const_memory->memoryaddress_);
    }

    void VisitTypeCheck::visitVar(Var* var)
    {
        auto ty = env_->lookup(var->stellaident_);

        if (!ty) {
            typeError("ERROR_UNDEFINED_VARIABLE", "Variable " + var->stellaident_ + " does not exist");
        }

        currentType_ = ty;
    }

    void VisitTypeCheck::visitAPatternBinding(APatternBinding* a_pattern_binding)
    {
        /* Code For APatternBinding Goes Here */

        if (a_pattern_binding->pattern_)
            a_pattern_binding->pattern_->accept(this);
        if (a_pattern_binding->expr_)
            a_pattern_binding->expr_->accept(this);
    }

    void VisitTypeCheck::visitAVariantFieldType(AVariantFieldType* a_variant_field_type)
    {
        /* Code For AVariantFieldType Goes Here */

        visitStellaIdent(a_variant_field_type->stellaident_);
        if (a_variant_field_type->optionaltyping_)
            a_variant_field_type->optionaltyping_->accept(this);
    }

    void VisitTypeCheck::visitARecordFieldType(ARecordFieldType* a_record_field_type)
    {
        /* Code For ARecordFieldType Goes Here */

        visitStellaIdent(a_record_field_type->stellaident_);
        if (a_record_field_type->type_)
            a_record_field_type->type_->accept(this);
    }

    void VisitTypeCheck::visitATyping(ATyping* a_typing)
    {
        /* Code For ATyping Goes Here */

        if (a_typing->expr_)
            a_typing->expr_->accept(this);
        if (a_typing->type_)
            a_typing->type_->accept(this);
    }

    void VisitTypeCheck::visitListStellaIdent(ListStellaIdent* list_stella_ident)
    {
        for (const auto& i : *list_stella_ident)
        {
            visitStellaIdent(i);
        }
    }

    void VisitTypeCheck::visitListExtensionName(ListExtensionName* list_extension_name)
    {
        for (const auto& i : *list_extension_name)
        {
            visitExtensionName(i);
        }
    }

    void VisitTypeCheck::visitListExtension(ListExtension* list_extension)
    {
        for (const auto& i : *list_extension)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListDecl(ListDecl* list_decl)
    {
        for (const auto& i : *list_decl)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListLocalDecl(ListLocalDecl* list_local_decl)
    {
        for (const auto& i : *list_local_decl)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListAnnotation(ListAnnotation* list_annotation)
    {
        for (const auto& i : *list_annotation)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListParamDecl(ListParamDecl* list_param_decl)
    {
        //тут мы отсавляем как есть
        for (const auto& i : *list_param_decl)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListType(ListType* list_type)
    {
        for (const auto& i : *list_type)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListMatchCase(ListMatchCase* list_match_case)
    {
        for (const auto& i : *list_match_case)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListPattern(ListPattern* list_pattern)
    {
        for (const auto& i : *list_pattern)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListLabelledPattern(ListLabelledPattern* list_labelled_pattern)
    {
        for (const auto& i : *list_labelled_pattern)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListBinding(ListBinding* list_binding)
    {
        for (const auto& i : *list_binding)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListExpr(ListExpr* list_expr)
    {
        for (const auto& i : *list_expr)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListPatternBinding(ListPatternBinding* list_pattern_binding)
    {
        for (const auto& i : *list_pattern_binding)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListVariantFieldType(ListVariantFieldType* list_variant_field_type)
    {
        for (const auto& i : *list_variant_field_type)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitListRecordFieldType(ListRecordFieldType* list_record_field_type)
    {
        for (const auto& i : *list_record_field_type)
        {
            i->accept(this);
        }
    }

    void VisitTypeCheck::visitInteger(Integer x)
    {
        if (x != 0) {
            typeError("ERROR_UNEXPECTED_TYPE_FOR_EXPRESSION", "ConstInt must to be 0");
        }
    }

    void VisitTypeCheck::visitChar(Char x)
    {
        /* Code for Char Goes Here */
    }

    void VisitTypeCheck::visitDouble(Double x)
    {
        /* Code for Double Goes Here */
    }

    void VisitTypeCheck::visitString(String x)
    {
        /* Code for String Goes Here */
    }

    void VisitTypeCheck::visitIdent(Ident x)
    {
        /* Code for Ident Goes Here */
    }

    void VisitTypeCheck::visitStellaIdent(StellaIdent x)
    {
        /* Code for StellaIdent Goes Here */
    }

    void VisitTypeCheck::visitExtensionName(ExtensionName x)
    {
        /* Code for ExtensionName Goes Here */
    }

    void VisitTypeCheck::visitMemoryAddress(MemoryAddress x)
    {
        /* Code for MemoryAddress Goes Here */
    }

    void VisitTypeCheck::visitDeclFunGeneric(DeclFunGeneric* decl_fun_generic)
    {
        /* Code For DeclFunGeneric Goes Here */

        if (decl_fun_generic->listannotation_)
            decl_fun_generic->listannotation_->accept(this);
        visitStellaIdent(decl_fun_generic->stellaident_);
        if (decl_fun_generic->liststellaident_)
            decl_fun_generic->liststellaident_->accept(this);
        if (decl_fun_generic->listparamdecl_)
            decl_fun_generic->listparamdecl_->accept(this);
        if (decl_fun_generic->returntype_)
            decl_fun_generic->returntype_->accept(this);
        if (decl_fun_generic->throwtype_)
            decl_fun_generic->throwtype_->accept(this);
        if (decl_fun_generic->listdecl_)
            decl_fun_generic->listdecl_->accept(this);
        if (decl_fun_generic->expr_)
            decl_fun_generic->expr_->accept(this);
    }

    void VisitTypeCheck::visitTypeAbstraction(TypeAbstraction* type_abstraction)
    {
        /* Code For TypeAbstraction Goes Here */

        if (type_abstraction->liststellaident_)
            type_abstraction->liststellaident_->accept(this);
        if (type_abstraction->expr_)
            type_abstraction->expr_->accept(this);
    }

    void VisitTypeCheck::visitTypeApplication(TypeApplication* type_application)
    {
        /* Code For TypeApplication Goes Here */

        if (type_application->expr_)
            type_application->expr_->accept(this);
        if (type_application->listtype_)
            type_application->listtype_->accept(this);
    }

    void VisitTypeCheck::visitTypeForAll(TypeForAll* type_for_all)
    {
        /* Code For TypeForAll Goes Here */

        if (type_for_all->liststellaident_)
            type_for_all->liststellaident_->accept(this);
        if (type_for_all->type_)
            type_for_all->type_->accept(this);
    }

    [[noreturn]] void VisitTypeCheck::typeError(const char* tag, const std::string& msg)
    {
        if (msg.empty()) {
            std::cerr << tag << '\n';
        } else {
            std::cerr << tag << ": " << msg << '\n';
        }
        std::exit(1);
    }

    bool VisitTypeCheck::typeEquals(std::shared_ptr<Type>& a, std::shared_ptr<Type>& b) {
        if (!a || !b) return false;

        //basic
        if (dynamic_cast<TypeBool*>(a.get())) return dynamic_cast<TypeBool*>(b.get());
        if (dynamic_cast<TypeNat*>(a.get())) return dynamic_cast<TypeNat*>(b.get());
        if (dynamic_cast<TypeUnit*>(a.get())) return dynamic_cast<TypeUnit*>(b.get());

        if (dynamic_cast<TypeFun*>(a.get())) return dynamic_cast<TypeFun*>(b.get());
        if (dynamic_cast<TypeTuple*>(a.get())) return dynamic_cast<TypeTuple*>(b.get()); 

        return false;
    }

    ///Context impl

    std::unordered_map<std::string, std::shared_ptr<Type>>& Context::getTop() {
        return scopes_.top();
    }
    
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

} // namespace Stella
