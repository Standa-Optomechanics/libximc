#ifndef FWLIBGENERATOR_HH
#define FWLIBGENERATOR_HH

#include "common.hh"
#include "visitor.hh"
#include "basegenerator.hh"
#include "generatorhelper.hh"

namespace xigen
{

	class FwLibGenerator : protected DefaultVisitor, public Noncopyable, public BaseGenerator
	{
		protected:

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (cookie >= 1)
					return false;
				visitCommandImpl( command );
				return true;
			}

			void printLengthLine (std::string name, const Communicator* writer, const Communicator* reader)
			{
					stream() << "\t{ " << toupper(name) << "_CMD"
						<< ", " << helpers::AlignedInt<4>(writer ? writer->size : 0)
						<< ", " << helpers::AlignedInt<4>(reader ? reader->size : 0)
						<< " },\n";
			}

			virtual void visitCommandImpl (Command& command)
			{
				if (!command.withAnyFields())
					return;
				if (command.unsynced)
					return;

				if (command.communicatorWriter && command.communicatorReader)
				{
					printLengthLine(command.communicatorWriter->name,
							command.communicatorWriter, command.communicatorReader);
				}
				else if (!command.communicatorWriter && command.communicatorReader)
				{
					printLengthLine(command.communicatorReader->name,
							NULL, command.communicatorReader);
				}
				else if (command.communicatorWriter && !command.communicatorReader)
				{
					printLengthLine(command.communicatorWriter->name,
							command.communicatorWriter, NULL);
				}
			}

			void enableComments (bool enable)
			{
				m_enableComments = enable;
			}

		private:

			bool m_enableComments;

			std::ostringstream m_os;

			std::ostream& stream()
			{
				return m_os;
			}

			void doGenerate (Protocol* protocol, std::ostream* os)
			{
				protocol->accept( *this );

				echoBanner( "BEGIN OF GENERATED length declarations", os );

				*os << "// Command lengths\n\n";
				*os << "CmdLengthStr CmdLengths[] = {\n";
				*os << m_os.str();
				*os << "\t{ 0, 0, 0}\n";
				*os << "};";

				*os << std::endl;

				echoBanner( "END OF GENERATED CODE", os );
			}

			void echoBanner (std::string bannerText, std::ostream* os)
			{
				if (m_enableComments)
					*os << "\n/*\n"
						<< " " 		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "   "	<< bannerText << "\n"
						<< " "		<< std::string( bannerText.size()+4, '-' ) << "\n"
						<< "*/\n";
			}

		public:

			explicit FwLibGenerator ()
				: m_enableComments(true)
			{
			}

	};

}

#endif

/* vim: set ts=2 sw=2: */
