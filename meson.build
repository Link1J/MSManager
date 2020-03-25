project('MSManager', 'c', 'cpp',
	version: '1.0.0',
	default_options: [
		'cpp_std=c++11'
	]
)

gnome = import('gnome')
cxx = meson.get_compiler('cpp')

program_name = 'msmanager'

cpp_sources = [
	'main.cc',
	'exampleappwindow.cc',
	'exampleapplication.cc',
	'exampleappprefs.cc',
	'serverconnection.cc',
]

deps = [
	dependency('gtkmm-3.0'),
	dependency('boost', modules : ['system', 'asio']),
	dependency('nlohmann_json'),
	dependency('threads'),
]

resources = gnome.compile_resources('resources',
									'msmanager.gresource.xml',
									source_dir: '.')
schemas = gnome.compile_schemas(depend_files: 'msmanager.gschema.xml')

executable(program_name,
	cpp_sources, resources, schemas,
	dependencies: deps
)

python3 = import('python').find_installation('python3')
source_step1_dir = meson.current_source_dir() 
install_cmd = source_step1_dir / 'install-cmd.py'

conf_data = configuration_data()
conf_data.set('bindir', meson.current_build_dir())
desktop_file = configure_file(
	input: 'msmanager.desktop.in',
	output: '@BASENAME@',
	configuration: conf_data,
)

run_target('install-desktop-file',
	command: [
		python3, install_cmd, 'desktop',
		program_name,
		desktop_file,
		source_step1_dir / 'msmanager.png',
	]
)

run_target('install-gschema-file',
	command: [
		python3, install_cmd, 'gschema',
		meson.current_source_dir() / 'msmanager.gschema.xml',
	]
)
