# VisitorPattern

For a Generic Class that would normally hold the pure virtual methods (The Visitable Base Class), inerit VisitableBase with CRTP, pass all the other classes that will inherit your Visitable Base Class. Passing everything in is a small price to pay, that saves typing in the long term. VisitableBase will then have 3 type definitions:

VisitorReturnType: Inherit this in your visitor that returns a value. Templated return type
VisitorType: Inherit this in you visitor that returns nothing. (I could've combined these two into the same Type, however, I think this way is a bit more explicit)
Visitable: Inherit this in the classes that derive from your generic class (using CRTP)

# Example Usage:
```
#include <Visitors.h>
#include <memory> //works with unique pointers

class Expr
  : public visit::VisitableBase<Expr, struct Identifier, struct Literal> {
public:
  //Additional members declared in Expr can be passed down to children
};

//inherit publically
struct Identifier : Expr::Visitable<Identifier> {
  std::string_view ident;
};
//These are no longer PODs, cant aggregate initialize them. Constructors are now required, or manual member initialization for each member
struct Literal : Expr::Visitable<Literal> {
  Literal(int value) : value(value) {}
  int value;
};

class ExprPrinter
	: public Expr::VisitorReturner<std::string> {
public:
  //each Expr type must be covered in methods, or else compile time error will occur
	virtual void visit(Identifier& expr) {
		returnValue(expr.ident); //we must use inherited method "returnValue" to return anything from Visitor
	}
	virtual void visit(Literal& expr) {
    returnValue(std::to_string(expr.value))
	}
};

int main() {
  std::unique_ptr<Expr> expr = std::make_unique<Literal>(2);
  std::cout << ExprPrinter{}.visitPtr(expr); //prints 2
  return 0;
}
```
