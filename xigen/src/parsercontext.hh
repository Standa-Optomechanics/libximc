#ifndef PARSERCONTEXT_HH
#define PARSERCONTEXT_HH

#include "common.hh"
#include "model.hh"

namespace xigen
{

	class ParserContext
	{
		public:

			~ParserContext()
			{
			}

			void prepare()
			{
				protocol = Protocol();
				currentFeatures = Features();
			}

			void addCommand(Command* command)
			{
				protocol.commands.push_back( command );
			}

			Command* currentCommand()
			{
				XI_ASSERT(!protocol.commands.empty());
				return protocol.commands.back();
			}

			void addFlagset(Flagset* flagset)
			{
				protocol.flagsets.push_back( flagset );
			}

			Flagset* currentFlagset()
			{
				XI_ASSERT(!protocol.flagsets.empty());
				return protocol.flagsets.back();
			}

			Features currentFeatures;
			bool inAnswer;
			Protocol protocol;
	};

}

#endif
/* vim: set ts=2 sw=2: */
