#include <../argparse.h>

using namespace std;
using namespace bsc;

typedef vector<string> str_vec;

struct
Options
{
  int         this_is_an_int;
  bool        this_is_a_bool;
  float       this_is_a_float;
  double      this_is_a_double;
  string      this_is_a_string;

  int         this_is_pod_array[3];
  str_vec     this_is_a_vector_of_strings;

  float       this_is_required_float;
  string      this_is_required_string;
};


int main ( int argc, char** argv )
{
  // Create argparse object and storage
  ArgParse args;
  Options opts;

  // For non-fixed size structure we need to specify initial size.
  opts.this_is_a_vector_of_strings.resize( 3 );

  // Start adding arguments
  args.add( Argument<int>( "-i", "--integer", "This is an integer argument", &opts.this_is_an_int ) );

  // Parse the command line arguments
  args.parse( argc, argv );

  // Print the parsed values
  printf(" Integer Argument : %d\n ", opts.this_is_an_int );
}
