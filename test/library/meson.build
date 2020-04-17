library_test = executable('lib-test', 'LibraryTest.cpp',
    dependencies: [gtest], link_with: [library_lib, dblib], 
    include_directories: [proj_include])

test('Library Test', library_test)