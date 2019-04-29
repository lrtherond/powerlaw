#include "powerlawCommon.h"
#include <boost/program_options.hpp>

/**
 * @author: W.M. Otte (wim@invivonmr.uu.nl); Image Sciences Institute, UMC Utrecht, NL.
 * @date: 19-11-2009
 *
 * Estimate powerlaw scaling parameter from input distribution.
 *
 * ***************************************************************************
 * Method: "Power-law distributions in empirical data", Clauset et al, 2009
 * http://www.santafe.edu/~aaronc/powerlaws/
 * ***************************************************************************
 */
class PowerLawFit
{

public:

	typedef double ValueType;
	typedef std::vector< ValueType > VectorType;

	/**
	 * Power law fit.
	 */
	void run( const std::string& inputFileName, bool nosmall, bool finite,
					double startXmin, double incrementXmin, double endXmin,
						bool bootstrap, unsigned int bootstrapIterations, bool verbose )
	{
		// [ 1 ] read input from text file ...
		VectorType values = getInput( inputFileName );

		// [ 2 ] bootstrap or single fit ...
		VectorType results;

		if ( bootstrap )
		{
			graph::Powerlaw< ValueType >::BootstrapFit( values, results, nosmall, finite, startXmin, incrementXmin, endXmin, bootstrapIterations, verbose );

			if ( ! results.empty() )
			{
				std::cout << "Alpha," << results.at( 0 ) <<  std::endl;
				std::cout << "Xmin," << results.at( 1 ) << std::endl;
				std::cout << "Log-likelihood," << results.at( 2 ) << std::endl;
				std::cout << "Alpha_sd," << results.at( 3 ) <<  std::endl;
				std::cout << "Xmin_sd," << results.at( 4 ) << std::endl;
				std::cout << "Log-likelihood_sd," << results.at( 5 ) << std::endl;

			}
			else
			{
				std::cerr << "*** ERROR ***: maximum likelihood "
					"bootstrap estimation failed! -> check input ..." << std::endl;
			}

		}
		else
		{
			graph::Powerlaw< ValueType >::SingleFit( values, results, nosmall, finite,
					startXmin, incrementXmin, endXmin );

			if ( ! results.empty() )
			{
				std::cout << "Alpha," << results.at( 0 ) << std::endl;
				std::cout << "Xmin," << results.at( 1 ) << std::endl;
				std::cout << "Log-likelihood," << results.at( 2 ) << std::endl;
			}
			else
			{
				std::cerr << "*** ERROR ***: maximum likelihood "
					"single estimation failed! -> check input ..." << std::endl;
			}
		}
	}

protected:

	/**
	 * Return input from given text file as vector.
	 */
	VectorType getInput( const std::string& input )
	{
		std::ifstream inFile;

		inFile.open( input.c_str() );
		if ( !inFile )
		{
			std::cout << "*** ERROR ***: Unable to open: " << input << "." << std::endl;
			exit( EXIT_FAILURE );
		}

		double x;
		VectorType output;

		while ( inFile >> x )
		{
			output.push_back( x );
		}
		inFile.close();

		/**
		 * Negative values will be converted to complex numbers in matlab,
		 * but not with the stl ...
		 *
		 * No support is given (yet) for complex number mle.
		 */
		if ( *( std::min_element( output.begin(), output.end() ) ) < 0 )
		{
			std::cerr << "*** ERROR ***: Negative input not supported!" << std::endl;
			exit (EXIT_FAILURE );
		}

		return output;
	}
};

// ************************************************************************************

/**
 * Throw error if required option is not specified.
 */
void required_option( const boost::program_options::variables_map& vm,
		const std::string& required_option )
{
	if ( vm.count( required_option ) == 0 )
		throw std::logic_error( "Option: '" + required_option + "' is required!" );
}

/**
 * Fit powerlaw to list of numbers.
 */
int main(int argc, char* argv[])
{
	namespace po = boost::program_options;

	// application description ...
	std::string description = "Fits a power-law distributional model to data.\n";

	// options ...
	std::string input;

	bool nosmall;
	bool finite;
	bool bootstrap;
	bool verbose;

	double startXmin;
	double incrementXmin;
	double endXmin;

	unsigned int bootstrapIterations;

	try {

        po::options_description desc("Available options");

        desc.add_options()

            ( "input,i", po::value< std::string >( &input )
            		, "string: input file with distribution values in column format." )

            ( "finite,f", po::value< bool >( &finite )
            		->default_value( false )
            		->zero_tokens()
            		, "bool: use an experimental finite-size correction." )

            ( "verbose,v", po::value< bool >( &verbose )
					->default_value( false )
            		->zero_tokens()
            		, "bool: print bootstrap status." )

            ( "nosmall,s", po::value< bool >( &nosmall )
            		->default_value( false )
            		->zero_tokens()
            		, "bool: truncate the search over xmin values before the finite-size bias becomes significant." )

            ( "bootstrap,b", po::value< bool >( &bootstrap )
            		->default_value( false )
            		->zero_tokens()
            		, "bool: run non-parametric bootstrap instead of single estimation." )

            ( "start-xmin,x", po::value< double >( &startXmin )
            		->default_value( 1.5 )
            		, "float: start value for discrete xmin estimation." )

            ( "increment-xmin,y", po::value< double >( &incrementXmin )
            		->default_value( 0.01 )
            		, "float: increment value for discrete xmin estimation." )

            ( "end-xmin,z", po::value< double >( &endXmin )
            		->default_value( 3.5 )
            		, "float: end value for discrete xmin estimation." )

            ( "bootstrap-iterations,n", po::value< unsigned int >( &bootstrapIterations )
            		->default_value( 1000 )
            		, "uint: bootstrap iterations." )

            ( "help,h", "bool: produce help message." )
        ;

        po::variables_map vm;
        po::store( po::parse_command_line( argc, argv, desc ), vm );
        po::notify( vm );

        // help message ...
        if ( vm.count( "help" ) )
        {
            std::cout << argv[0] << ": " << description << std::endl;
        	std::cout << desc << "\n";

        	return EXIT_SUCCESS;
        }

        // required options ...
        required_option( vm, "input" );

        // run application ...
        PowerLawFit powerlawFit;

    	powerlawFit.run( input, nosmall, finite,
							startXmin, incrementXmin, endXmin,
											bootstrap, bootstrapIterations, verbose );

    }
    catch( std::exception& e )
    {
        std::cerr << "*** ERROR ***: " << e.what() << "\n";
        std::cerr << "Use \"" << argv[0]
        		  << " --help\" for information about application usage."
        		  << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
