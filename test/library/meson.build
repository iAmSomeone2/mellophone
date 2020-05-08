library_test = executable('lib-test', 'LibraryTest.cpp',
    dependencies: [gtest], link_with: [library_lib, dblib], 
    include_directories: [proj_include])

test('Library Test', library_test)

flac_track_test = executable('flac-track-test', 'FLACTrackTest.cpp',
    dependencies: [gtest], link_with: [library_lib],
    include_directories: [proj_include])

test('FLAC Track Test', flac_track_test)

vorbis_track_test = executable('vorbis-track-test', 'VorbisTrackTest.cpp',
    dependencies: [gtest], link_with: [library_lib],
    include_directories: [proj_include])

test('Vorbis Track Test', vorbis_track_test)
