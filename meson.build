project('mellophone', ['cpp'], version: '0.0.1',
    default_options: ['c_std=c11', 'cpp_std=c++17'],
    license: 'Apache-2.0')

thread_lib = dependency('threads', required: true)

sqlite3 = dependency('sqlite3', version: '>=3.6.19', required: true)
boost_libs = dependency('boost', version: '>=1.30.0', required: true)

proj_include = include_directories('include')

subdir('src')

subdir('test')