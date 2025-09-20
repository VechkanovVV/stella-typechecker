#include "TypeCheck.h"

#include <iostream>

#include "Stella/Absyn.H"
#include "VisitTypeCheck.h"

namespace Stella {
void typecheckProgram(Program* program) { program->accept(new VisitTypeCheck()); }
}  // namespace Stella
