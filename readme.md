<div align="center">
    <img src="https://raw.githubusercontent.com/mosswg/dropout-dl/main/assets/dropout_dl_logo.png" width="50%" />
</div>

* [Fork Features](#fork-features)
* [Installation](#installation)
  * [Docker](#docker)
  * [How to Build](#how-to-build)
  * [Dependencies](#Dependencies)
* [Usage](#how-to-use)
  * [Options](#options)
  * [Login](#login)
  * [Cookies](#cookies)

---

# Fork Features

# ⚠️ READ THIS FIRST ⚠️

**AI-Assisted Development**: This fork was developed with AI assistance. No warranties or guarantees. Use at your own risk.

**Not Affiliated**: This project is not affiliated with, endorsed by, or connected to Dropout, College Humor, or the original dropout-dl maintainers.

**SUPPORT DROPOUT**: We strongly encourage you to [subscribe to Dropout](https://www.dropout.tv). They create amazing content and deserve your support. This tool exists solely for personal convenience (offline viewing, backups, Plex organization) for paying subscribers.

**NO PIRACY**: This tool is intended for personal use by paying subscribers who want offline access or Plex organization. Redistribution of downloaded content is illegal. I'm a README, not a cop, but please don't be that person who ruins it for everyone.

---

This fork adds several performance improvements, stability fixes, and new features on top of the original dropout-dl:

### Performance Enhancements
- **⚡ Concurrent Segment Buffering** - Downloads multiple segments simultaneously using a sliding window algorithm for significantly faster downloads
  - Controlled via `--concurrent-segments` flag (default: 5 concurrent segments)
  - Maintains sequential write order while downloading in parallel
  - Tested with 240p through 1080p content
- **🎯 Improved Audio Quality Matching** - Audio quality now intelligently matches video quality tier instead of defaulting to medium

### New Features
- **📦 MKV Container Format** - Support for outputting to Matroska (MKV) format via `--format mkv` flag
- **🎬 Plex-Compatible Naming** - Follows Plex naming conventions for proper library detection
  - Format: `Series Name - S##E## - Episode Name.ext`
  - Special episode support (S00Exx for specials, including "Last Looks" and "Behind the Scenes" content)
- **🔍 Preview Downloads** - `--list-urls` flag to see what would be downloaded without actually downloading
- **🐛 Docker Debugging Support** - Build with `DEBUG_BUILD=1` for gdb integration and crash analysis
- **💾 Atomic File Operations** - All files are downloaded to `.tmp` extensions and only renamed to final format upon successful completion
  - Ensures interrupted downloads are easily identifiable and resumable
  - Prevents partially downloaded files from appearing as complete

### Stability Improvements
- Multiple memory safety fixes preventing segmentation faults
- Bounds checking for HTML parsing loops
- Null pointer checks throughout codebase
- Fixed episode numbering bugs in series/season downloads

### Documentation
- Added [TESTING.md](TESTING.md) with Docker testing workflows and debugging instructions

For the original project, see: [mosswg/dropout-dl](https://github.com/mosswg/dropout-dl)

---

# Installation
## Docker
A docker image was created that makes it easier to build and use dropout-dl. You can simply build the docker image without worrying about installing any system dependencies:
```shell
docker build -t dropout-dl:latest .
```
After its done building, you can use it by adding your arguments to the end of the `docker run` command:
```shell
docker run --rm -it -v $PWD/login:/app/login -v $PWD/out:/Downloads dropout-dl:latest --output-directory /Downloads --captions -e https://www.dropout.tv/dimension-20/season:10/videos/the-chosen-ones
```
**Note:** The docker image expects the `login` file to be at `/app/login`.\
You must specify an output directory and mount that directory to the host so that you can retrieve the files from the docker container. In the above command I tell dropout-dl to output everything in `/Downloads` inside the container, which is mounted to a folder named `out` inside the current directory (`$PWD` is current directory).

## Submodule
This repository uses [a json library](https://github.com/nlohmann/json/). Either clone the repository with the `--recurse-submodules` flag or after cloning run:
```
git submodule update --init --recursive
```

## How to Build
```
cmake -S <source-dir> -B <build-dir>
cmake --build <build-dir>
```

### Dependencies

#### Required
* [cURL](https://curl.se/libcurl/) - Required for downloading pages and videos.
#### Optional
* [SQLite](https://www.sqlite.org/index.html) - Required for retrieving cookies from browsers.
* [libgcrypt](https://www.gnupg.org/software/libgcrypt/index.html) - Used for decrypting chrome cookies retrieved from the sqlite database.

##### Debian/Ubuntu
```
sudo apt install libcurl4-gnutls-dev
```
To install the optional dependencies, run:
```
sudo apt install libsqlite3-dev libgcrypt-dev
```

##### Void Linux
```
sudo xbps-install -S libcurl
```
To install the optional dependencies, run:
```
sudo xbps-install -S sqlite-devel libgcrypt-devel
```

## How to Use
```
./dropout-dl [options] <url>
```
By default, dropout-dl will download episodes in a season with the format `<series>/<season>/<series> - S<season-num>E<episode-num> - <episode-name>.mp4` and single episodes with the format `<series>/<season>/<series> - <season> - <episode-name>.mp4`.

### Options
```
--help              -h   Display this message
--quality           -q   Set the quality of the downloaded video. Quality can be set to 'all' which
                            will download all qualities and place them into separate folders
--output            -o   Set the output filename. Only works for single episode downloads
--output-directory  -d   Set the directory where files are output
--verbose           -v   Display debug information while running
--browser-cookies   -bc  Use cookies from the browser placed in 'firefox_profile' or 'chrome_profile'
--force-cookies          Interpret the next to argument as the session cookie
--series            -S   Interpret the url as a link to a series and download all episodes from all seasons
--season            -s   Interpret the url as a link to a season and download all episodes from all seasons
--episode           -e   Interpret the url as a link to a single episode
--captions          -c   Download the captions along with the episode. Overridden by --captions-only if set.
--captions-only     -co  Download the captions only, without the episode.
--format              -f   Set the output container format (mp4 or mkv). Defaults to mp4
--list-urls           -l   List the URLs of episodes that would be downloaded without downloading them
--concurrent-segments -cs  Set the number of segments to download concurrently per stream.
                            Higher values = faster downloads but more network connections. Defaults to 5
```

If series, season, or episode is not used, the type will be inferred based on the link format.

### Login
Login in information must be placed in a file called `login` in the same directory as the executable. The file must be email then on a new line password. For example if your email is `email@example.com` and password `password123` the file would be:
```
email@example.com
password123
```

### Cookies
If you would like to avoid logging in for any reason, cookies can be used. The option `browser-cookies` must be provided.
#### Firefox
Create a file named `firefox_profile` in the build directory and paste in your [firefox profile folder path](https://support.mozilla.org/en-US/kb/profiles-where-firefox-stores-user-data)
#### Chrome
Install libgcrypt and create a file named `chrome_profile` in the build directory and paste in your chrome profile folder path (found on [chrome://version](chrome://version))
#### Other/No Sqlite
Use the `--force-cookies` program option to manually input cookies.


## Contributing
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](https://makeapullrequest.com)
### Issues
If you have any issues or would like a feature to be added please don't hesitate to submit an issue after checking to make sure it hasn't already been submitted. Using the templates is a good place to start, but sometimes they're overkill. For example, if the program segfaults for you, you don't need to state that the intended behaviour is to not segfault. \
\
If you'd like to contribute a good place to start is looking at open issues and trying to fix one with a pull request. \

## Contributors
- [mosswg](https://github.com/mosswg)
- [SeanOMik](https://github.com/SeanOMik) - Docker support
- [Hello-User](https://github.com/Hello-User) - Skip downloading if file exists
- [MisterSheeple](https://github.com/MisterSheeple) - Optional dependency installation instructions
- [BradMJustice](https://github.com/BradMJustice) - Caption improvements
