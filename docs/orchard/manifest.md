# Manifest

Contains metadata for the build system.

**File extensions**:

- `.orch`
- `.orchard`

To use comments in the manifest use the `#` character.

---

## Structure

Root manifest:

```toml
project: "<project_name>"
version: "<version>" # optional, default "0.0.1"

author: "<author_name>" # if one
authors: ["<author_name>", "<author_name>"] # if multiple

source_dir: "<source_directory>" # optional, default "/src"
entry: "<entry_file>" # relative to source_dir
```

---

## Grammar

```ebnf
manifest = { entry } ;
entry = key ":" value ;

key = identifier ;
value = string | number | boolean | list ;

list = "[" [ string { "," string } ] "]" ;
```
