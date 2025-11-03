# Testing dropout-dl

## Architecture Overview

The codebase follows a clean, single-responsibility architecture with a DRY (Don't Repeat Yourself) principle:

### Code Flow
```
--episode → episode URL → download
--season  → season.episode_urls → download each URL
--series  → series.get_seasons() → season.episode_urls → download each URL
```

### Responsibilities
- **series**: Finds all season URLs for a show
- **season**: Extracts episode URLs from a season page
- **episode**: Downloads a single episode
- **main**: Orchestrates the flow and handles all downloading

All three modes (`--episode`, `--season`, `--series`) converge to a single download loop in main.cpp that iterates through episode URLs.

## Quick Testing with --list-urls

For fast testing without waiting for downloads, use the `--list-urls` (or `-l`) flag to see what episodes would be downloaded:

```bash
# Test a series with multiple seasons (Dimension 20: Dungeons and Drag Queens has 2 seasons)
docker run --rm -v "$(pwd)/login:/app/login" dropout-dl:latest \
  --list-urls --series \
  https://watch.dropout.tv/dimension-20-dungeons-and-drag-queens

# Test a single season
docker run --rm -v "$(pwd)/login:/app/login" dropout-dl:latest \
  --list-urls --season \
  https://watch.dropout.tv/game-changer/season:1

# Test a single episode
docker run --rm -v "$(pwd)/login:/app/login" dropout-dl:latest \
  --list-urls --episode \
  https://watch.dropout.tv/game-changer/season:1/videos/lie-detector-1
```

This allows you to verify that:
- Series correctly finds all seasons
- Seasons correctly extract all episode URLs
- The scraping logic is working without waiting for downloads

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
  -v "$(pwd)/login:/app/login" \
  -v "$(pwd)/test-output:/output" \
  dropout-dl:latest \
  --output-directory /output \
  --format mkv \
  --season \
  --quality lowest \
  https://watch.dropout.tv/game-changer/season:1
```

**Note:** This issue only affects Docker testing on Windows with Git Bash. Normal users running the compiled binary directly will never encounter this issue, as they would use native paths for their operating system:
- Linux/Mac: `./dropout-dl -d ~/videos ...`
- Windows: `./dropout-dl -d "C:\Users\...\videos" ...`

## Quick Docker Test Commands

Build the Docker image:
```bash
docker build -t dropout-dl:latest .
```

Test with a single episode:
```bash
env MSYS_NO_PATHCONV=1 docker run --rm \
  -v "$(pwd)/login:/app/login" \
  -v "$(pwd)/test-output:/output" \
  dropout-dl:latest \
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
  dropout-dl:latest \
  --output-directory /output \
  --format mkv \
  --season \
  --quality lowest \
  https://watch.dropout.tv/game-changer/season:1
```

Test with an entire series:
```bash
env MSYS_NO_PATHCONV=1 docker run --rm \
  -v "$(pwd)/login:/app/login" \
  -v "$(pwd)/test-output:/output" \
  dropout-dl:latest \
  --output-directory /output \
  --format mkv \
  --series \
  --quality lowest \
  https://watch.dropout.tv/game-changer
```

## Format Support

The `--format` flag supports both:
- **mp4** (default): H.264 video + AAC audio in MP4 container
- **mkv**: H.264 video + AAC audio in Matroska container

Both formats contain the same video/audio streams, only the container differs.
