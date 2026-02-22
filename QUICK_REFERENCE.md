# Quick Reference Card

## Initial Setup (One-time)

```bash
# 1. Configure GitHub username
./configure-github.sh your-github-username

# 2. Initialize and push to GitHub
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/YOUR_USERNAME/nexusllmllama.git
git push -u origin main
```

## Publishing a New Version

```bash
# Option 1: Using GitHub Actions (Recommended)
git tag v1.0.0
git push origin v1.0.0

# Option 2: Manual publish
export GITHUB_ACTOR=your-username
export GITHUB_TOKEN=your-token
./gradlew :app:publish
```

## Using in Another Project

### settings.gradle.kts
```kotlin
repositories {
    google()
    mavenCentral()
    maven {
        url = uri("https://maven.pkg.github.com/YOUR_USERNAME/nexusllmllama")
        credentials {
            username = System.getenv("GITHUB_ACTOR")
            password = System.getenv("GITHUB_TOKEN")
        }
    }
}
```

### build.gradle.kts
```kotlin
dependencies {
    implementation("in.nishantapps.nexusllmllama:nexusllmllama:1.0.0")
}
```

### Build with credentials
```bash
export GITHUB_ACTOR=your-username
export GITHUB_TOKEN=your-token
./gradlew build
```

## Token Permissions

### Publishing: `write:packages` + `repo` (for private repos)
### Consuming: `read:packages`

Generate at: https://github.com/settings/tokens

## Files to Update Before Publishing

1. `app/build.gradle.kts`:
   - Replace `YOUR_USERNAME` with GitHub username
   - Update `name` and `email` in developers section

2. All documentation files will be auto-updated by `configure-github.sh`

## Common Commands

```bash
# Build library locally
./gradlew :app:assembleRelease

# Publish to local Maven (testing)
./gradlew :app:publishToMavenLocal

# Publish to GitHub Packages
./gradlew :app:publish

# Clean build
./gradlew clean

# Check for updates
./gradlew :app:dependencies
```

## Version Update Checklist

- [ ] Update `version = "X.Y.Z"` in app/build.gradle.kts (line 8)
- [ ] Update `version = "X.Y.Z"` in publishing block (line 92)
- [ ] Commit changes
- [ ] Create and push tag: `git tag vX.Y.Z && git push origin vX.Y.Z`
- [ ] Wait for GitHub Actions to complete
- [ ] Update version in consuming projects

## Troubleshooting Quick Fixes

| Error | Fix |
|-------|-----|
| 401 Unauthorized | Regenerate GitHub token |
| Package not found | Check package exists on GitHub, verify URL |
| Build failure | Clean project: `./gradlew clean` |
| Certificate error | Update JDK |
| NDK not found | Install NDK from Android SDK Manager |

## Important URLs

- Repository: `https://github.com/YOUR_USERNAME/nexusllmllama`
- Package: `https://github.com/YOUR_USERNAME/nexusllmllama/packages`
- Tokens: `https://github.com/settings/tokens`
- Actions: `https://github.com/YOUR_USERNAME/nexusllmllama/actions`

---
For detailed instructions, see **SETUP_GUIDE.md**

