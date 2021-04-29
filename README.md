# echopp

See this project for exploring the asio networking library. Study on the link https://habr.com/ru/post/195386/

## Requrements for build

- Cmake >= 3.10
> conan (https://bincrafters.readthedocs.io/en/latest/introduction_to_conan.html)

## How to build

```bash
$ git clone https://github.com/olinyavod/echopp.git
$ cd echopp
$ conan install . --install-folder build
$ conan build . --build-folder build
```