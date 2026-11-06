test_env = environment()
test_env.set('LC_ALL', 'C')

gtest_dep = dependency('gtest', required: get_option('tests'))
gtest_main_dep = dependency('gtest', main: true)

####################

test_rpackage = executable(
  'test_rpackage',
  ['test_rpackage.cc'] + common_sources,
  cpp_args: gtk_cpp_args + ['-g3'],
  dependencies: [glib_dep, common_deps],
  include_directories: [root_inc, common_inc, gui_inc],
)

test('test_rpackage', test_rpackage, env: test_env)

####################

test_rpackagefilter = executable(
  'test_rpackagefilter',
  ['test_rpackagefilter.cc'] + common_sources,
  cpp_args: gtk_cpp_args + ['-g3'],
  dependencies: [glib_dep, common_deps],
  include_directories: [root_inc, common_inc, gui_inc],
)

test('test_rpackagefilter', test_rpackagefilter, env: test_env)

####################

test_rpackageview = executable(
  'test_rpackageview',
  ['test_rpackageview.cc'] + common_sources,
  cpp_args: gtk_cpp_args + ['-g3'],
  dependencies: [glib_dep, common_deps],
  include_directories: [root_inc, common_inc, gui_inc],
)

test('test_rpackageview', test_rpackageview, env: test_env)

####################

if gtest_dep.found()

  ####################

  test_rsources = executable(
    'test_rsources',
    ['test_rsources.cc'] + common_sources,
    cpp_args: gtk_cpp_args + ['-g3'],
    dependencies: [glib_dep, common_deps, gtest_dep],
    include_directories: [root_inc, common_inc, gui_inc],
  )

  test('test_rsources', test_rsources, env: test_env)

  ####################
  
  gtest_rpackage = executable(
    'gtest_rpackage',
    ['gtest_rpackage.cc'] + common_sources,
    cpp_args: gtk_cpp_args + ['-g3'],
    dependencies: [glib_dep, common_deps, gtest_main_dep],
    include_directories: [root_inc, common_inc, gui_inc],
  )

  test('gtest_rpackage', gtest_rpackage, env: test_env)
  
  ####################

endif

####################
