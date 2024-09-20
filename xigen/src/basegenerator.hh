#ifndef BASEGENERATOR_HH
#define BASEGENERATOR_HH

#include "generator.hh"
#include "locale.hh"

namespace xigen
{

	class BaseGenerator : public IGenerator
	{
		public:

			BaseGenerator()
				: m_verbose(false), m_protocol(NULL), m_locale(NULL)
			{
			}

			virtual void setProtocol (Protocol* protocol)
			{
				m_protocol = protocol;
			}

			virtual void setOutputFile (std::string file)
			{
				m_outputFile = file;
			}

			virtual void setTemplateFile (std::string file)
			{
				m_templateFile = file;
			}

			virtual void setVersionFile (std::string file)
			{
				m_versionFile = file;
			}

			virtual void setVerbose (bool verbose)
			{
				m_verbose = verbose;
			}

			virtual void setLocale (Locale* locale)
			{
				m_locale = locale;
			}

			virtual void enableComments (bool )
			{
			}

			virtual void generate()
			{
				XI_ASSERT(m_protocol);
				XI_ASSERT(m_locale);

				fetchVersion();

				std::ostream* os;
				std::ofstream ofs;
				bool isFile = !m_outputFile.empty();
				if (isFile)
				{
					ofs.open( m_outputFile.c_str() );
					if (!ofs)
						throw std::runtime_error( "Can't open file for writing " + m_outputFile );
					ofs.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
					os = &ofs;
				}
				else
					os = &std::cout;

				if (m_templateFile.empty())
				{
					doGenerate( m_protocol, os );
				}
				else
				{
					std::ifstream ifs( m_templateFile.c_str() );
					if (!ifs.good())
						throw std::runtime_error( "Can't read from template file " + m_templateFile );

					std::string line;
					ifs >> std::noskipws;
					while (ifs)
					{
						if (std::getline( ifs, line ))
						{
							if (line.find( "@@GENERATED_CODE@@" ) != std::string::npos)
							{
								doGenerate( m_protocol, os );
							}
							else
							{
								os->write( line.c_str(), line.size() );
								os->put( '\n' );
							}
						}
					}

					ifs.close();

				}

				if (isFile)
					ofs.close();
			}

		private:

			void fetchVersion()
			{
				m_ximcVersion = "";
				if (!m_versionFile.empty())
				{
					std::ifstream ifs( m_versionFile.c_str() );
					if (!ifs.good())
						throw std::runtime_error( "Can't read from version file " + m_templateFile );
					std::string line;
					if (std::getline( ifs, line ) && !line.empty() && isdigit( line[0] ))
						m_ximcVersion = line;
					ifs.close();
				}
			}

		protected:

			std::string m_ximcVersion;
			bool m_verbose;

			virtual void doGenerate (Protocol* protocol, std::ostream* os) = 0;

			Protocol* m_protocol;
			Locale* m_locale;

		private:
			std::string m_outputFile;
			std::string m_templateFile;
			std::string m_versionFile;
	};


}

#endif


/* vim: set ts=2 sw=2: */
