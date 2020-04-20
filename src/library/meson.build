library_srcs = ['Library.cpp', 'Library.hpp', 'Track.cpp', 'Track.hpp']

flac_lib = dependency('flac++', required: true)

library_lib = static_library('library', library_srcs,
    include_directories: [proj_include], link_with: [dblib], dependencies: [flac_lib])