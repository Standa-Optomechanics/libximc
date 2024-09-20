#ifndef SIZECHECK_HH
#define SIZECHECK_HH

#include "common.hh"
#include "visitor.hh"
#include "generatorhelper.hh"

namespace xigen
{


	class SizeCheck : public DefaultVisitor, public Noncopyable
	{
		protected:

			void doVisitBaseCommand (Command& command)
			{
				m_current = &command;
				m_location = locationRequest;
				m_withDynamicArray = false;
				m_sumRequestSize = 0;
			}

			virtual bool visitCommand (Command& command, size_t cookie)
			{
				if (!cookie)
				{
					doVisitBaseCommand( command );
				}
				return !cookie;
			}

			void checkError (Command& command, bool inReq, size_t declaredSize)
			{
				size_t actualSize = inReq ? m_sumRequestSize : m_sumAnswerSize;
				if (actualSize != declaredSize)
				{
					char buf[256];
					sprintf( buf, "Command %s fails static size validation: specified %d, actual %d bytes",
							command.name().c_str(), (int)declaredSize, (int)actualSize
					);
					throw ast_error( buf, &command );
				}
			}

			virtual void visitCommandPost (Command& command)
			{
				// ignore dynamic arrays now
				if (m_withDynamicArray)
					return;

				m_sumRequestSize += 4;
				if (command.is("answer"))
					m_sumAnswerSize += 4;

				if (command.is("crc") && command.withFields())
					m_sumRequestSize += 2;
				// crc flag doesn't affect answer crc. it must be always written
				if (command.withAnswer())
					m_sumAnswerSize += 2;

				if (m_verbose)
					std::cerr << "For command " << command.name() << " sumreq=" << m_sumRequestSize << " sumans=" << m_sumAnswerSize << std::endl;

				if (command.withAnswer() && !command.is("answer"))
					throw ast_error( "Command with answer can not be marked as 'noanswer'", &command );

				if (command.communicatorWriter && command.communicatorReader)
				{
					if (command.withFields())
					{
						if (!command.communicatorWriter)
							throw ast_error( "Wrong communicator detected while checking for size constraints", &command );
					}
					if (command.withAnswer())
					{
						if (!command.communicatorReader)
							throw ast_error( "Wrong communicator detected while checking for size constraints", &command );
					}
					checkError( command, false, command.communicatorReader->size );
					checkError( command, true, command.communicatorWriter->size );
				}
				else if (!command.communicatorWriter && command.communicatorReader)
				{
					checkError( command, false, command.communicatorReader->size );
				}
				else if (command.communicatorWriter && !command.communicatorReader)
				{
					checkError( command, true, command.communicatorWriter->size );
				}
			}

			virtual void visitRequest ()
			{
				m_sumRequestSize = 0;
				m_location = locationRequest;
			}

			virtual void visitAnswer ()
			{
				m_sumAnswerSize = 0;
				m_location = locationAnswer;
			}


			virtual void visitDataField (DataField& field)
			{
				switch (field.calibrationType())
				{
					case CalibrationEnum::normal:
					case CalibrationEnum::none:
						doField( field );
						break;
				}
			}

			virtual void visitConstantField (ConstantField& field)
			{
				doField( field );
			}

			virtual void visitArrayField (ArrayField& field)
			{
				m_withDynamicArray = m_withDynamicArray || field.isDynamic();
				switch (field.calibrationType())
				{
					case CalibrationEnum::normal:
					case CalibrationEnum::none:
						doField( field );
						break;
				}
			}

			virtual void visitFlagField (FlagField& field)
			{
				doField( field );
			}

			virtual void visitReservedField (ReservedField& field)
			{
				doField( field );
			}

			template <class F>
			void doField (const F& field)
			{
				size_t current = field.getSize();
				if (current != (size_t)-1)
					sumByLocation() += current;
			}

			size_t& sumByLocation()
			{
				return m_location == locationRequest ? m_sumRequestSize : m_sumAnswerSize;
			}

		private:
			typedef enum { locationRequest, locationAnswer } Location;

			Location m_location;
			Command* m_current;
			const bool m_verbose;

			size_t m_sumRequestSize;
			size_t m_sumAnswerSize;

			bool m_withDynamicArray;

		public:

			explicit SizeCheck(bool verbose)
				: m_current(NULL), m_verbose(verbose)
			{
			}
	};

}

#endif


/* vim: set ts=2 sw=2: */
