# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [v1.0.1] - 2018-01-30

## Fixed
- Fixed missing version number link in changelog.

## [v1.0.0] - 2018-01-30

### Added
- Added 'Unsubscribe(subscriberId)' and 'UnsubscribeAll()' methods to the 'SerialFiller' class, closes #12.
- Added test to make sure a 'Publish()' with no data works correctly, closes #3.

## [v1.0.0-beta.1] - 2018-01-25

### Added
- Added this changelog file, closes #8.
- Added VS Code project files, closes #9.
- Added a base SerialFillerException class that all serial filler exceptions derive from, closes #10.
- Added header guard to Crc16Ccitt1021.hpp.
- Added 'Definitions.hpp' and moved the definitions for 'ByteArray' and 'ByteQueue' to this file.

### Changed
- Tidied up header includes and improved the method of forward declaration, closes #11.

[Unreleased]: https://github.com/mbedded-ninja/CppTemplate/compare/v1.0.1...HEAD
[v1.0.1]: https://github.com/mbedded-ninja/CppTemplate/compare/v1.0.0...v1.0.1
[v1.0.0]: https://github.com/mbedded-ninja/CppTemplate/compare/v1.0.0-beta.1...v1.0.0
[v1.0.0-beta.1]: https://github.com/mbedded-ninja/CppTemplate/compare/v0.3.7...v1.0.0-beta.1