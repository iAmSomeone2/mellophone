library_srcs = ['Library.cpp', 'Library.hpp', 
    'Track.cpp', 'Track.hpp',
    'FLACTrack.cpp', 'FLACTrack.hpp']

openssl = dependency('openssl', required: true)
ogg_lib = dependency('ogg', required: true)
flac_lib = dependency('flac++', required: true)

library_lib = static_library('library', library_srcs,
    include_directories: [proj_include],
    link_with: [dblib],
    dependencies: [flac_lib, openssl, boost_libs, thread_lib])