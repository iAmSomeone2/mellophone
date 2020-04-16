db_test = executable('db-test', 'DatabaseTest.cpp',
    dependencies: [gtest], link_with: dblib,
    include_directories: [proj_include])

test('Database Test', db_test)