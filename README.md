# NexusLLMLlama

Android library for running Llama models on device.

## Installation

### 1. Add GitHub Packages repository

Add the following to your project's `settings.gradle.kts`:

```kotlin
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        maven {
            url = uri("https://maven.pkg.github.com/YOUR_USERNAME/nexusllmllama")
            credentials {
                username = System.getenv("GITHUB_ACTOR") ?: project.findProperty("gpr.user") as String?
                password = System.getenv("GITHUB_TOKEN") ?: project.findProperty("gpr.token") as String?
            }
        }
    }
}
```

### 2. Add dependency

Add the following to your app's `build.gradle.kts`:

```kotlin
dependencies {
    implementation("in.nishantapps.nexusllmllama:nexusllmllama:1.0.0")
}
```

### 3. Authentication

To access GitHub Packages, you need to authenticate. You have two options:

#### Option A: Using environment variables (recommended for CI/CD)
```bash
export GITHUB_ACTOR=your-github-username
export GITHUB_TOKEN=your-github-personal-access-token
```

#### Option B: Using gradle.properties
Add to your `~/.gradle/gradle.properties`:
```properties
gpr.user=your-github-username
gpr.token=your-github-personal-access-token
```

**Note:** Generate a Personal Access Token (classic) from GitHub with `read:packages` scope:
- Go to GitHub Settings → Developer settings → Personal access tokens → Tokens (classic)
- Generate new token with `read:packages` permission

## Publishing

### From GitHub Actions

1. Create a new tag and push it:
```bash
git tag v1.0.0
git push origin v1.0.0
```

2. The GitHub Actions workflow will automatically build and publish the library.

### Manual Publishing

1. Set environment variables:
```bash
export GITHUB_ACTOR=your-github-username
export GITHUB_TOKEN=your-github-personal-access-token
```

2. Run the publish task:
```bash
./gradlew :app:publish
```

## Usage

[Add usage examples here]

## Requirements

- Android SDK 24+
- Target SDK 36
- ARM64-v8a architecture

## License

MIT License

