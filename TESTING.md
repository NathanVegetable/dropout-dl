# Testing dropout-dl

## Important Notes

### Bugs Fixed in This Session
1. **Segmentation Fault (Issue #45)** - Fixed by correcting multiple utility functions
   - `substr_is()` in util.cpp had incorrect logic
   - `remove_leading_and_following_whitespace()` lacked bounds checking
   - `get_series_name()` could create negative substring lengths
   - `getenv("HOME")` returned nullptr in Docker environments
   - Several other string manipulation safety issues

2. **MKV Format Support (Issue #34)** - Added `--format` flag supporting both mp4 and mkv containers

### Debug Output
The code currently contains extensive DEBUG print statements added during bug fixing. These should be removed before final commit. They are helpful for diagnosing issues but not needed in production.

## Docker Testing on Windows (Git Bash)

### Known Issue: MSYS Path Conversion

When testing the Docker container from Git Bash on Windows, you may encounter a path conversion issue where paths like `/output` are automatically converted to Windows paths like `C:/Program Files/Git/output`.

**Symptom:**
```
Error opening input file C:/Program Files/Git/output/...
```

**Solution:**
Use `env MSYS_NO_PATHCONV=1` before the docker command to disable automatic path conversion:

```bash
env MSYS_NO_PATHCONV=1 docker run --rm \
  -v "C:\Users\Nathan\source\repos\dropout-dl\login:/app/login" \
  -v "C:\Users\Nathan\source\repos\dropout-dl\test-output:/output" \
  dropout-dl:latest \
  --output-directory /output \
  --format mkv \
  --season \
  --quality lowest \
  https://watch.dropout.tv/game-changer
```

**Verified:** This approach successfully creates MKV files in the test-output directory.

**Note:** This issue only affects Docker testing on Windows with Git Bash. Normal users running the compiled binary directly will never encounter this issue, as they would use native paths for their operating system:
- Linux/Mac: `./dropout-dl -d ~/videos ...`
- Windows: `./dropout-dl -d "C:\Users\...\videos" ...`

## Quick Docker Test Commands

Build the Docker image:
```bash
docker build -t dropout-dl:test .
```

Test with a single episode:
```bash
env MSYS_NO_PATHCONV=1 docker run --rm \
  -v "$(pwd)/login:/app/login" \
  -v "$(pwd)/test-output:/output" \
  dropout-dl:test \
  --output-directory /output \
  --format mkv \
  --episode \
  https://watch.dropout.tv/game-changer/season:1/videos/lie-detector-1
```

Test with a full season:
```bash
env MSYS_NO_PATHCONV=1 docker run --rm \
  -v "$(pwd)/login:/app/login" \
  -v "$(pwd)/test-output:/output" \
  dropout-dl:test \
  --output-directory /output \
  --format mkv \
  --season \
  --quality lowest \
  https://watch.dropout.tv/game-changer/season:1
```
