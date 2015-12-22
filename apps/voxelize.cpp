
/* Copyright (c) 2015, EPFL/Blue Brain Project
 *                     Jafet.VillafrancaDiaz@epfl.ch
 *                     Stefan.Eilemann@epfl.ch
 *                     Daniel.Nachbaur@epfl.ch
 */

#include <fivox/fivox.h>
#include <fivox/itk/beerLambertProjectionImageFilter.h>

#include <itkImageFileWriter.h>
#include <itkRescaleIntensityImageFilter.h>

#include <lunchbox/file.h>
#include <lunchbox/log.h>
#include <lunchbox/uri.h>
#include <boost/program_options.hpp>

namespace
{
typedef Volume::Pointer VolumePtr;
typedef fivox::FieldFunctor< Volume > FieldFunctor;
typedef std::shared_ptr< FieldFunctor > FieldFunctorPtr;
typedef fivox::ImageSource< Volume > ImageSource;
typedef ImageSource::Pointer ImageSourcePtr;

typedef float FloatPixelType;
typedef itk::Image< FloatPixelType, 2 > FloatImageType;

const double sigmaVSDProjection =  0.00045; // units per um (0.45 per mm)

template< typename T >
class VolumeWriter
{
    typedef itk::RescaleIntensityImageFilter
                < Volume, itk::Image< T, 3 >> RescaleFilterType;
    typedef itk::ImageFileWriter< itk::Image< T, 3 >> Writer;

public:
    VolumeWriter( VolumePtr input )
        : _rescaleFilter( RescaleFilterType::New( ))
        , _writer( Writer::New( ))
    {
        _rescaleFilter->SetInput( input );
        _writer->SetInput( _rescaleFilter->GetOutput( ));
    }

    typename Writer::Pointer operator->()
    {
        return _writer;
    }

private:
    typename RescaleFilterType::Pointer _rescaleFilter;
    typename Writer::Pointer _writer;
};

template<>
class VolumeWriter< float >
{
    typedef itk::ImageFileWriter< Volume > Writer;

public:
    VolumeWriter( VolumePtr input )
        : _writer( Writer::New( ))
    {
        _writer->SetInput( input );
    }

    typename Writer::Pointer operator->()
    {
        return _writer;
    }

private:
    typename Writer::Pointer _writer;
};

template< typename T >
void _sample( ImageSourcePtr source, const vmml::Vector2ui& frameRange,
              const bool vsdProjection, const float volumeResolution,
              const std::string& outputFile )
{
    VolumePtr input = source->GetOutput();
    VolumeWriter< T > writer( input );

    const size_t numDigits = std::to_string( frameRange.y( )).length( );
    for( uint32_t i = frameRange.x(); i < frameRange.y(); ++i )
    {
        std::string filename;
        if( frameRange.y() - frameRange.x() > 1 )
        {
            std::ostringstream fileStream;
            fileStream << outputFile << std::setfill('0')
                       << std::setw( numDigits ) << i;
            filename = fileStream.str();
        }
        else
            filename = outputFile;

        source->getFunctor()->getSource()->load( i );

        const std::string& volumeName = filename + ".mhd";
        writer->SetFileName( volumeName );
        source->Modified();
        writer->Update(); // Run pipeline to write volume
        LBINFO << "Volume written as " << volumeName << std::endl;

        if( !vsdProjection )
            continue;

        // The projection filter computes the output using the real value of
        // the data, i.e. not limited by the precision of the final image
        typedef fivox::BeerLambertProjectionImageFilter
                < Volume, FloatImageType > FilterType;
        FilterType::Pointer projection = FilterType::New();
        projection->SetInput( input );
        projection->SetProjectionDimension( 1 ); // projection along Y-axis
        projection->SetPixelSize( 1.0 / volumeResolution );
        projection->SetSigma( sigmaVSDProjection );

        // Write output image
        typedef itk::ImageFileWriter< FloatImageType > ImageWriter;
        ImageWriter::Pointer imageWriter = ImageWriter::New();
        imageWriter->SetInput( projection->GetOutput( ));

        const std::string& imageFile = filename + ".vtk";
        imageWriter->SetFileName( imageFile );
        imageWriter->Update();
        LBINFO << "VSD projection written as " << imageFile << std::endl;
    }
}
}

namespace vmml
{
std::istream& operator>>( std::istream& is, Vector2f& vec )
{
    return is >> std::skipws >> vec.x() >> vec.y();
}

std::istream& operator>>( std::istream& is, Vector2ui& vec )
{
    return is >> std::skipws >> vec.x() >> vec.y();
}
}

namespace po = boost::program_options;

int main( int argc, char* argv[] )
{
    // Default values
    size_t size = 256;
    std::string outputFile( "volume" );
    std::string uri( "fivox://" );

    // Argument parsing
    po::variables_map vm;
    po::options_description desc( "Supported options", 140 /*line len*/ );
    desc.add_options()
        ( "help,h", "Show help message" )
        ( "version,v", "Show program name and version" )
        ( "volume", po::value< std::string >(),
//! [Usage]
          "Volume URI with parameters in the form:\n"
          "- Compartment reports:\n"
          "    fivox[compartments]://BlueConfig?report=string,target=string[ or #target]\n"
          "- Soma reports:\n"
          "    fivoxsomas://BlueConfig?report=string,target=string[ or #target]\n"
          "- Spike reports:\n"
          "    fivoxspikes://BlueConfig?duration=float,spikes=path,target=string[ or #target]\n"
          "- Synapse densities:\n"
          "    fivoxsynapses://BlueConfig?target=string[ or #target]\n"
          "- Voltage-sensitive dye reports:\n"
          "    fivoxvsd://BlueConfig?dyecurve=string,target=string[ or #target]\n"
          "\n"
          "Note: If target=string and #target parameters are given at the same time\n"
          "target=string has the precedence over #target parameter. Giving the #target as\n"
          "a parameter is deprecated\n"
          "\n"
          "Parameters for all types :\n"
          "- BlueConfig: BlueConfig file path\n"
          "              (default: 'configs/BlueConfigVSD' for VSD reports,\n"
          "               BBPTestData otherwise)\n"
          "- target: name of the BlueConfig target (default: CircuitTarget)\n"
          "- magnitude: value multiplied to each sampled voxel value\n"
          "             (defaults: 0.1 for Compartments and Somas,\n"
          "                        1.5 / duration for Spikes\n"
          "                        1.0 for Synapses and VSD)\n"
          "- functor: type of functor to sample the data into the voxels\n"
          "             (defaults: \"density\" for Synapses,\n"
          "                        \"frequency\" for Spikes,\n"
          "                        \"field\" for Compartments, Somas and VSD)\n"
          "- resolution: number of voxels per micrometer (default: 1.0)\n"
          "- maxBlockSize: maximum memory usage allowed for one block in bytes\n"
          "                (default: 64MB)\n"
          "- maxError: maximum error allowed (default: 0.001). If the event's\n"
          "            contribution is less than the specified error, the event\n"
          "            is discarded. The units are not defined and depend on\n"
          "            the current data. This parameter is used only with the\n"
          "            field functor to compute the cutoff distance.\n"
          "- showProgress: display progress bar for current voxelization step\n"
          "                (default: 0/off)\n"
          "\n"
          "Parameters for Compartments:\n"
          "- report: name of the compartment report\n"
          "          (default: 'voltage'; 'allvoltage' if BlueConfig is BBPTestData)\n"
          "- dt: timestep between requested frames in milliseconds\n"
          "      (default: report dt)\n"
          "\n"
          "Parameters for Somas:\n"
          "- report: name of the soma report\n"
          "          (default: 'soma'; 'voltage' if BlueConfig is BBPTestData)\n"
          "- dt: timestep between requested frames in milliseconds\n"
          "      (default: report dt)\n"
          "\n"
          "Parameters for Spikes:\n"
          "- duration: time window in milliseconds to load spikes (default: 10)\n"
          "- spikes: path to an alternate out.dat/out.spikes file\n"
          "          (default: SpikesPath specified in the BlueConfig)\n"
          "\n"
          "Parameters for VSD:\n"
          "- report: name of the soma report\n"
          "          (default: 'soma'; 'voltage' if BlueConfig is BBPTestData)\n"
          "- dyecurve: path to the dye curve file to apply, e.g. attenuation\n"
          "            (default: no file; attenuation of 1.0)\n"
//! [Usage]
          )
        ( "datatype,d", po::value< std::string >()->default_value( "float" ),
          "Type of the data in the output volume "
          "[float (default), int, short, char]" )
        ( "size,s", po::value< size_t >()->default_value( size ),
          "Size of the output volume" )
        ( "time,t", po::value< float >(),
          "Timestamp to load in the report" )
        ( "times", po::value< fivox::Vector2f >(),
          "Time range [start end) to load in the report" )
        ( "frame,f", po::value< unsigned >(),
          "Frame to load in the report" )
        ( "frames", po::value< fivox::Vector2ui >(),
          "Frame range [start end) to load in the report" )
        ( "output,o", po::value< std::string >()->default_value( outputFile ),
          "Name of the output volume file (mhd and raw); contains frame number "
          "if --frames or --times" )
        ( "projection,p", "Generate the corresponding projected 2D image "
          "(only for VSD volumes)" );

    po::store( po::parse_command_line( argc, argv, desc ), vm );
    po::notify( vm );

    if( vm.count( "help" ))
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if( vm.count( "version" ))
    {
        std::cout << argv[0] << " version " << fivox::Version::getString()
                  << std::endl;
        return EXIT_SUCCESS;
    }

    if( vm.count( "volume" ))
        uri = vm["volume"].as< std::string >();
    else
        LBINFO << "Using " << uri << " as volume" << std::endl;

    if( vm.count( "size" ))
        size = vm["size"].as< size_t >();
    if( vm.count( "output" ))
        outputFile = vm["output"].as< std::string >();

    ::fivox::URIHandler params( uri );

    ImageSourcePtr source = params.newImageSource< float >();

    ::fivox::EventSourcePtr loader = source->getFunctor()->getSource();
    const fivox::AABBf& bbox = loader->getBoundingBox();
    const fivox::Vector3f& position = bbox.getMin();
    const float extent = bbox.getDimension().find_max();

    Volume::SizeType vSize;
    vSize.Fill( size );

    Volume::RegionType region;
    region.SetSize( vSize );

    VolumePtr output = source->GetOutput();
    output->SetRegions( region );

    typename Volume::SpacingType spacing;
    spacing.Fill( extent / float( size ));
    output->SetSpacing( spacing );

    typename Volume::PointType origin;
    origin[0] = position[0];
    origin[1] = position[1];
    origin[2] = position[2];
    output->SetOrigin( origin );

    fivox::Vector2ui frameRange( 0, 1 ); // just frame 0 by default
    if( vm.count( "time" ))
    {
        const size_t frame = vm["time"].as< float >() / loader->getDt();
        frameRange = fivox::Vector2ui( frame, frame + 1 );
    }
    if( vm.count( "times" ))
    {
        const fivox::Vector2f times = vm["times"].as< fivox::Vector2f >();
        frameRange = fivox::Vector2ui( times.x() / loader->getDt(),
                                       times.y() / loader->getDt( ));
    }
    if( vm.count( "frame" ))
    {
        const size_t frame = vm["frame"].as< unsigned >();
        frameRange = fivox::Vector2ui( frame, frame + 1 );
    }
    if( vm.count( "frames" ))
        frameRange = vm["frames"].as< fivox::Vector2ui >();

    const bool vsdProjection(
            params.getType() == fivox::TYPE_VSD && vm.count( "projection" ));

    const std::string& datatype( vm["datatype"].as< std::string >( ));
    if( datatype == "char" )
    {
        LBINFO << "Sampling volume as char (uint8_t) data" << std::endl;
        _sample< uint8_t >( source, frameRange, vsdProjection,
                            params.getResolution(), outputFile );
    }
    else if( datatype == "short" )
    {
        LBINFO << "Sampling volume as short (uint16_t) data" << std::endl;
        _sample< uint16_t >( source, frameRange, vsdProjection,
                             params.getResolution(), outputFile );
    }
    else if( datatype == "int" )
    {
        LBINFO << "Sampling volume as int (uint32_t) data" << std::endl;
        _sample< uint32_t >( source, frameRange, vsdProjection,
                             params.getResolution(), outputFile );
    }
    else
    {
        LBINFO << "Sampling volume as floating point data" << std::endl;
        _sample< float >( source, frameRange, vsdProjection,
                          params.getResolution(), outputFile );
    }
}
