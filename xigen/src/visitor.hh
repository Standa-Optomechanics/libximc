#ifndef VISITOR_HH
#define VISITOR_HH

#include <common.hh>

namespace xigen
{

	class Protocol;
	class Command;
	class Field;
	class DataField;
	class ConstantField;
	class ArrayField;
	class FlagField;
	class ReservedField;
	class Flagset;

	class IVisitor
	{
		public:
			virtual ~IVisitor() { }

			virtual void visitProtocol (Protocol& protocol) = 0;
			virtual void visitCommands () = 0;

			virtual void visitFlagset (Flagset& flagset) = 0;
			virtual void visitFlagsetPost (Flagset& flagset) = 0;

			virtual bool visitCommand (Command& command, size_t cookie) = 0;
			virtual void visitCommandPost (Command& command) = 0;

			virtual void visitRequest () = 0;
			virtual void visitAnswer () = 0;

			virtual void visitDataField (DataField& field) = 0;
			virtual void visitConstantField (ConstantField& field) = 0;
			virtual void visitArrayField (ArrayField& field) = 0;
			virtual void visitFlagField (FlagField& field) = 0;
			virtual void visitReservedField (ReservedField& field) = 0;
	};

	class DefaultVisitor : public IVisitor
	{
		public:
			virtual void visitProtocol (Protocol& ) { }
			virtual void visitCommands () { };

			virtual void visitFlagset (Flagset& ) { }
			virtual void visitFlagsetPost (Flagset& ) { };

			virtual bool visitCommand (Command& , size_t cookie) { return !cookie; };
			virtual void visitCommandPost (Command& ) { };

			virtual void visitRequest () { };
			virtual void visitAnswer () { };

			virtual void visitDataField (DataField& ) { };
			virtual void visitConstantField (ConstantField& ) { };
			virtual void visitArrayField (ArrayField& ) { };
			virtual void visitFlagField (FlagField& ) { };
			virtual void visitReservedField (ReservedField& ) { };
	};

	class IVisitable
	{
		public:
			virtual void accept (IVisitor& visitor) = 0;
	};

}

#endif


/* vim: set ts=2 sw=2: */
