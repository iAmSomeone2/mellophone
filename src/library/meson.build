library_srcs = ['Library.cpp', 'Library.hpp', 'Track.cpp', 'Track.hpp']

openssl = dependency('openssl', required: true)
vorbis_lib = dependency('vorbis', required: true)
flac_lib = dependency('flac++', required: true)

library_lib = static_library('library', library_srcs,
    include_directories: [proj_include],
    link_with: [dblib],
    dependencies: [flac_lib, vorbis_lib, openssl, thread_lib])