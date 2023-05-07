project('mutils', 'c++')

inc = include_directories('include')



mutils = static_library(
  'mutils',
  'src/ansi.cc',
  'src/env.cc',
  include_directories : inc,
  install : true
) 



mutils_dep = declare_dependency(
  include_directories : inc,
  link_with: mutils
  )
