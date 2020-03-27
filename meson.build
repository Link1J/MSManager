project('MSManager', 'c', 'cpp',
	version: '1.0.0',
	default_options: [
		'cpp_std=c++latest'
	]
)

qt5 = import('qt5')
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
	dependency('qt5', modules: ['Core', 'Gui']),
	dependency('boost', modules: ['system']),
	dependency('nlohmann_json', fallback: ['nlohmann_json', 'nlohmann_json_dep']),
	dependency('threads'),
]

if host_machine.system() == 'windows'
    deps += cxx.find_library('ws2_32')
	add_global_arguments('-D__USE_W32_SOCKETS', language : 'c')
	add_global_arguments('-D__USE_W32_SOCKETS', language : 'cpp')
endif

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
