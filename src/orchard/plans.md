# Orchard

Orchard is the package manager and build system for Cherry and Pit.

## CLI

`orchard new <name>`
Creates a package skeleton

`orchard build`
Build the current package and its dependencies

`orchard run`
Build, then run the package's executable

`orchard test`
Build and run tests

`orchard check`
Parse and type-check without emitting code

`orchard add <dep>`
Adds a dependency to `orchard.toml`

`orchard update`
Re-resolve dependencies and rewrite the lock file

`orchard doc`
Generate documentation from the module graph

`orchard fmt`
Run the formatter

`orchard clean`
Remove build outputs
