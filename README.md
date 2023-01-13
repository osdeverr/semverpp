# semverpp - Semantic Versioning for C++

This library implements the [Semantic Versioning](https://semver.org/spec/v2.0.0-rc.2.html) specification in C++, allowing you to easily compare software versions.

## Setting up

Use semverpp as a [Re](https://github.com/osdeverr/rebs) dependency by adding the following to your `re.yml`:

```yaml
deps:
    # <...>
    - github:osdeverr/semverpp@v1.0.0 [semverpp]
```

Additionally, you can install it as a global package by running `re pkg install github:osdeverr/semverpp@v1.0.0 as semverpp` and later use it by adding it as a dependency like that:
```yaml
deps:
    # <...>
    - global:semverpp [semverpp]
```

## Usage

```cpp
#include <semverpp/version.hpp>

/* <...> */

semverpp::version version = "1.2.3";
// or semverpp::version version{"1.2.3"}
// or semverpp::version version{1, 2, 3}

assert(version > semverpp::version{"1.2"} && version < semverpp::version{"1.3"});
assert(version == semverpp::version{"1.2.3+some.build.metadata"});

assert(semverpp::version{"1.2.3-beta"} > semverpp::version{"1.2.3-alpha"});
```
