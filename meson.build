project('mutils', 'cpp')

inc = include_directories('include')



mutils = static_library(
  'mutils',
  'src/ansi.cc',
  'src/env.cc',
  include_directories : inc,
  install : true,
  cpp_args: ['-std=c++20']
) 



mutils_dep = declare_dependency(
  include_directories : inc,
  link_with: mutils,
  compile_args: ['-std=c++20']
  )
