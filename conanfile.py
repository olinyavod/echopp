from conans import ConanFile, CMake

class echo(ConanFile):
	name = "echo"
	version = "0.1"
	settings = "os", "compiler", "build_type", "arch"
	requires = "asio/1.18.1"
	generators = "cmake"

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()	