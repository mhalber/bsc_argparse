// Simple argument parsing
// Author:  Maciej Halber
// Date:    03/24/16
// License: Public Domain

/*
Usage:

*/

#pragma once

#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace bsc
{
  template <typename T>
  struct Argument
  {
    std::string name;
    std::string short_name;
    std::string message;
    T * value = NULL;

    bool required = false;
    int positional = -1;
    int length = -1;

    Argument ( std::string name, std::string message, T * value, int length = 0 );
    Argument ( std::string short_name, std::string name, std::string message, T * value, int length = 1 );
  };

  class ArgParse
  {
  public:
    std::string name;
    std::string description;

    void add( Argument<std::string> arg ) { add( arg, str_args ); }
    void add( Argument<int> arg )         { add( arg, int_args ); }
    void add( Argument<float> arg )       { add( arg, float_args ); }
    void add( Argument<double> arg )      { add( arg, double_args ); }
    void add( Argument<bool> arg )        { add( arg, bool_args ); }

    void parse( int argc, char** argv );

    bool exists_already( std::string name ) const;
    Argument<std::string> * find_string( std::string key ) { return find( key, str_args ); }
    Argument<int>         * find_int( std::string key )    { return find( key, int_args ); }
    Argument<float>       * find_float( std::string key )  { return find( key, float_args ); }
    Argument<double>      * find_double( std::string key ) { return find( key, double_args ); }
    Argument<bool>        * find_bool( std::string key )   { return find( key, bool_args ); }

    void print_help() const;

  private:
    std::unordered_map< std::string, Argument< std::string > > str_args;
    std::unordered_map< std::string, Argument< int > >         int_args;
    std::unordered_map< std::string, Argument< float > >         float_args;
    std::unordered_map< std::string, Argument< double > >         double_args;
    std::unordered_map< std::string, Argument< bool > >        bool_args;

    template <typename T>
    void add( Argument<T> & arg, std::unordered_map< std::string, Argument< T > > & args );

    template <typename T>
    Argument<T> * find( std::string key, std::unordered_map< std::string, Argument< T > > & args );

    template <typename T>
    Argument<T> * find_at_position ( int position, std::unordered_map< std::string, Argument< T > > & args );
  };

  static int argparse_num_required = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

// Argument constructors. little lengthy due to error checking
template <typename T>
bsc::Argument<T>::
Argument ( std::string name, std::string message, T * value, int length )
{
  if ( name.empty() )
  {
    printf( "Argument name must not be an empty string\n" );
    exit(-1);
  }

  this->name        = name;
  this->value       = value;
  this->message     = message;
  this->length      = length;

  if ( name[0] == '-' && name[1] == '-' )
  {
    this->required = false;
    if ( length < 2 ) this->length = 1;
    if ( name.length() < 3 )
    {
      printf( "Argument \"%s\" name must be at least 3 characters long! ( --<name> )\n", name.c_str() );
      exit(-1);
    }
  }
  else if ( name[0] == '-' )
  {
    this->required = false;
    if ( length < 2 ) this->length = 1;
    if ( name.length() != 2 )
    {
      printf( "Argument \"%s\" shorthand \"%s\" must be at least 2 characters long ( -<letter> )\n",
              name.c_str(), name.c_str() );
      exit(-1);
    }
  }
  else
  {
    this->required   = true;
    this->positional = ++argparse_num_required - 1;
  }
}

template <typename T>
bsc::Argument<T>::
Argument ( std::string short_name, std::string name, std::string message, T * value, int length )
{
  if ( name.length() < 3 )
  {
    printf( "Argument \"%s\" name must be at least 3 characters long! ( --<name> )\n", name.c_str() );
    exit(-1);
  }

  if ( short_name.length() != 2 )
  {
    printf( "Argument \"%s\" shorthand \"%s\" must be at least 2 characters long ( -<letter> )\n",
            name.c_str(), short_name.c_str() );
    exit(-1);
  }

  this->name        = name;
  this->short_name  = short_name;
  this->value       = value;
  this->message     = message;
  this->length      = length;
  this->required    = false;
}


bool bsc::ArgParse ::
exists_already ( std::string name ) const
{
  if ( str_args.find( name )    != str_args.end() )   return true;
  if ( int_args.find( name )    != int_args.end() )   return true;
  if ( float_args.find( name )  != float_args.end() )   return true;
  if ( double_args.find( name ) != double_args.end() )   return true;
  if ( bool_args.find( name )   != bool_args.end() ) return true;
  return false;
}


template <typename T>
void bsc::ArgParse ::
add ( Argument<T> & arg, std::unordered_map< std::string, Argument< T > > & args )
{
  bool check_name = !exists_already( arg.name );
  bool check_short_name = arg.short_name.empty() ? 1 : !exists_already( arg.short_name );
  if ( check_name && check_short_name )
  {
    args.insert( std::make_pair( arg.name, arg )  );
    if ( !arg.required ) args.insert( std::make_pair( arg.short_name, arg )  );
  }
  else
  {
    printf("Argument with name \"%s\" is in conflict!", arg.name.c_str() );
    print_help();
  }
}

template <typename T>
bsc::Argument<T> * bsc::ArgParse ::
find ( std::string key, std::unordered_map< std::string, Argument< T > > & args )
{
  auto arg = args.find( key );
  if ( arg != args.end() ) return &( arg->second );
  else return nullptr;
}

template <typename T>
bsc::Argument<T> * bsc::ArgParse ::
find_at_position ( int position, std::unordered_map< std::string, Argument< T > > & args )
{
  Argument<T> * retval = nullptr;
  for ( auto & arg : args )
  {
    if ( arg.second.positional == position )
    {
      retval = &( arg.second );
      break;
    }
  }
  return retval;
}

void bsc::ArgParse ::
parse ( int argc, char** argv )
{
  // skip name of program
  name = std::string( *argv );
  argc--; argv++;

  // parse into helper data structure
  std::vector< std::string > args_to_parse;
  args_to_parse.reserve( argc );
  while ( argc > 0 )
  {
    args_to_parse.push_back( std::string( *argv ) );
    argc--; argv++;
  }

  // helper variables
  int idx = 0;
  int n_args = args_to_parse.size();

  // check if arg passed during parsing is correct, when parsing multiple values
  auto check_validity = [n_args]( int idx, auto to_parse, auto arg_ptr )
  {
    if ( idx >= n_args || to_parse[0] == '-' )
    {
      printf ("No more values to parse for argument %s (%s), correct value is %d argument(s)\n",
              arg_ptr->name.c_str(), arg_ptr->short_name.c_str(), arg_ptr->length );
      exit( -1 );
    }
  };

  // parse value to correct argument
  auto parse_argument = [ &args_to_parse, &idx, n_args, check_validity ]
                        ( auto to_parse, auto str_arg, auto int_arg,
                          auto float_arg, auto double_arg, auto bool_arg )
  {
    // define some conversion function wrappers
    auto identity = [] ( std::string arg ) { return arg; };
    auto to_int   = [] ( std::string arg ) { return std::stoi( arg ); };
    auto to_float   = [] ( std::string arg ) { return std::stof( arg ); };
    auto to_double   = [] ( std::string arg ) { return std::stod( arg ); };
    auto to_bool  = [] ( std::string arg ) { return (bool)std::stoi( arg ); };
    auto accept   = [] ( std::string arg ) { return 1; };

    // actual parsing happens here. We either accept the value ( if length is 0 ),
    // or convert to correct type using conversion fucntion
    auto parse_arg = [ &args_to_parse, &idx, check_validity ]
                     ( auto arg_ptr, auto to_parse, auto acceptance_function, auto conversion_funct )
    {
      if ( arg_ptr->length == 0  ) *(arg_ptr->value) = acceptance_function( to_parse );

      for ( int i = 0 ; i < arg_ptr->length ; ++i )
      {
        to_parse = args_to_parse[ ++idx ];
        check_validity( idx, to_parse, arg_ptr );
        *(arg_ptr->value + i) = conversion_funct( to_parse );
      }
    };

    // based on which type is valid, we will use different functions
    if ( str_arg != nullptr )       parse_arg( str_arg,  to_parse, identity, identity );
    else if ( int_arg != nullptr )  parse_arg( int_arg,  to_parse, accept, to_int );
    else if ( float_arg != nullptr )  parse_arg( float_arg,  to_parse, accept, to_float );
    else if ( double_arg != nullptr )  parse_arg( double_arg,  to_parse, accept, to_double );
    else if ( bool_arg != nullptr ) parse_arg( bool_arg, to_parse, accept, to_bool );
  };

  // check if help is there
  for ( int str_idx = 0 ; str_idx < n_args ; ++str_idx )
  {
    if ( args_to_parse[str_idx] == "--help" ||
         args_to_parse[str_idx] == "-h" )
    {
      print_help();
    }
  }

  // Some corner cases
  if ( n_args < argparse_num_required ) print_help();
  if ( args_to_parse.empty() ) return;

  // Get required positional arguments
  for ( int position = 0 ; position < argparse_num_required ; ++position )
  {
    Argument<std::string> * str_arg = find_at_position( position, str_args );
    Argument<int> * int_arg         = find_at_position( position, int_args );
    Argument<float> * float_arg     = find_at_position( position, float_args );
    Argument<double> * double_arg   = find_at_position( position, double_args );
    Argument<bool> * bool_arg       = find_at_position( position, bool_args );

    std::string cur_arg = args_to_parse[ idx ];
    if ( cur_arg[0] == '-' )
    {
      printf( "Invalid argument \"%s\" at position %d!\n", cur_arg.c_str(), position );
      print_help();
    }

    parse_argument( cur_arg, str_arg, int_arg, float_arg, double_arg, bool_arg );
    idx++;
  }

  // Get the rest of arguments
  while ( idx < n_args )
  {
    // check which one it is
    std::string cur_arg = args_to_parse[ idx ];

    // try to find it
    bool found = true;
    Argument<std::string> * str_arg = find( cur_arg, str_args );
    Argument<int> * int_arg         = find( cur_arg, int_args );
    Argument<float> * float_arg     = find( cur_arg, float_args );
    Argument<double> * double_arg   = find( cur_arg, double_args );
    Argument<bool> * bool_arg       = find( cur_arg, bool_args );

    if ( str_arg == nullptr && int_arg == nullptr &&
         float_arg == nullptr && double_arg == nullptr &&
         bool_arg == nullptr )
    {
      found = false;
    }

    if ( found )
    {
      parse_argument( cur_arg, str_arg, int_arg, float_arg, double_arg, bool_arg );
    }
    else
    {
      printf( "Unrecognized argument \"%s\"\n", cur_arg.c_str() );
      print_help();
    }

    idx++;
  }
}

void bsc::ArgParse ::
print_help() const
{
  // helper storage struct
  struct argument_info
  {
    std::string name;
    std::string short_name;
    std::string message;
    std::string type;
    int count;
  };

  // printing help is here by default
  argument_info help_info = { "--help", "-h", "Show this help message and exit", "", 0 };

  // storage
  std::vector< argument_info > required_args;
  std::vector< argument_info > optional_args;
  required_args.resize( argparse_num_required );
  std::unordered_map< std::string, argument_info > optional_args_map;

  // copy from actual argument storage to local sturctures that do not vary on type
  auto group_arguments = [ &required_args, &optional_args_map ] ( auto args, std::string type )
  {
    for ( const auto & arg : args )
    {
      argument_info info = { arg.second.name, arg.second.short_name,arg.second.message, type, arg.second.length };
      if ( arg.second.required ) {
        required_args[ arg.second.positional ] = info;
      }
      else
      {
        optional_args_map.insert( std::make_pair( info.name, info ) );
      }
    }
  };

  // formatted printing
  auto print_args = []( auto & args )
  {
    for ( const auto & arg : args )
    {
      std::string full_name = arg.name;
      if ( !arg.short_name.empty() ) full_name =  arg.short_name + ", " + full_name;

      if ( arg.count <= 1 )
      {
        printf("  %-24s - %s <%s>\n", full_name.c_str(),
                                      arg.message.c_str(),
                                      arg.type.c_str() );
      }
      else
      {
        printf("  %-24s - %s <%d %ss>\n", full_name.c_str(),
                                          arg.message.c_str(),
                                          arg.count,
                                          arg.type.c_str() );
      }
    }
  };

  // Gather info
  group_arguments( str_args,    "string" );
  group_arguments( int_args,    "int" );
  group_arguments( float_args,  "float" );
  group_arguments( double_args, "doule" );
  group_arguments( bool_args,   "bool" );

  // Copy form a map to vector ( done to avoid duplicates )
  for ( const auto & elem : optional_args_map )
  {
    optional_args.push_back( elem.second );
  }
  optional_args.push_back( help_info );

  // Sort optionals
  std::sort( optional_args.begin(), optional_args.end(),
            []( const argument_info & a, const argument_info & b )
              {
                int idx_a = a.name.find_last_of("-") + 1;
                int idx_b = b.name.find_last_of("-") + 1;
                std::string a_name = a.name.substr( idx_a, -1 );
                std::string b_name = b.name.substr( idx_b, -1 );
                return a_name < b_name;
              } );

  // Actual printing of help message
  printf( "\nUsage : %s ", name.c_str() );
  for ( const auto & arg : required_args ) printf( "%s ", arg.name.c_str() );
  for ( const auto & arg : optional_args ) printf( "%s ", arg.name.c_str() );
  printf("\n\n");

  if ( !description.empty() ) printf("Description: %s\n\n", description.c_str() );

  if ( !required_args.empty() )
  {
    printf("Required Arguments:\n");
    print_args( required_args );
    printf("\n");
  }

  printf("Optional Arguments:\n");
  print_args( optional_args );
  printf("\n");

  exit(-1);
}


