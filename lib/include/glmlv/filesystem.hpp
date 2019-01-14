#pragma once

#ifdef GLMLV_USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#else
#include <experimental/filesystem>
#endif

namespace glmlv
{

#ifdef GLMLV_USE_BOOST_FILESYSTEM
namespace fs = boost::filesystem;
#else
namespace fs = std::experimental::filesystem; // Shorter namespace for experimental filesystem standard library
#endif

}