# InfoCLI project changelog

The followings list every change ever to happen to the codebase on a behavioral level.
Each version has their own subheader with sub-subheaders defining what's been added, changed, or removed. 
Additionally, every version-level header may have a 'Developer notes' subheader defining notes
by the developer publishing that version addressed to other developers (or their older selves),
which are more implementation detail oriented.
Patch updates may not include the whole set of subheaders at the developer's discretion.

Versioning is according to [semantic versioning](https://semver.org):
Format: *major*.*minor*.*patch*
 - *Major* updates break backwards compatibility with previous *major* versions.
 *Major* version `0` means project is not yet released and any and all API-s
 are unstable. Any project with *major* version `0` does not get codenamed releases.
 *Major* version other than `0` and `1` are represented in the project name.
 - *Minor* updates add new functionality to the project. New *minor* updates
 are respected with new codenames.
 - *Patch* updates do not introduce new functionality, but are either bugfixes
 or contain a change of functionality which is backwards compatible.
 New *patch* updates are represented in the codename by concatenating the patch
 version + 1 to the end with a dash eg. 'Daedalus-2' for InfoUtils `v1.3.1`.

## VERSION 2.0.1 - Helium-2

### Added:
 - First-class MSVC support
 - Support for type modifiers like `cli::repeat<int>` for opts that can 
   be called multiple times
 
### Removed:
 - Hideous compile times
 
### Changed: 
 - DSL syntax completely remade
 - Whole internal architecture remaster

## VERSION 2.0.0 - Helium

We don't talk about it.

## VERSION 1.0.0 - Hydrogen

### Added:
 - Option DSL
 - Reference callbacks
 - Function callbacks
 - Aggregating types
 - User-defined callback types
 - Auto-help
 
### Changed: 
 None
 
### Removed:
 None

## VERSION 0.1.0

### Added:
 - Starter project files
 
### Changed: 
 N/A
 
### Removed:
 N/A
