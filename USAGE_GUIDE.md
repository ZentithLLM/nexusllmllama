# How to Use NexusLLMLlama Library in Your App

## Step 1: Configure GitHub Packages Access

### In your app project's `settings.gradle.kts`:

```kotlin
pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        
        // Add GitHub Packages repository
        maven {
            name = "GitHubPackages"
            url = uri("https://maven.pkg.github.com/YOUR_USERNAME/nexusllmllama")
            credentials {
                username = System.getenv("GITHUB_ACTOR") ?: project.findProperty("gpr.user") as String?
                password = System.getenv("GITHUB_TOKEN") ?: project.findProperty("gpr.token") as String?
            }
        }
    }
}

rootProject.name = "YourAppName"
include(":app")
```

**Important:** Replace `YOUR_USERNAME` with your actual GitHub username.

## Step 2: Add Dependency

### In your app module's `build.gradle.kts`:

```kotlin
dependencies {
    implementation("in.nishantapps.nexusllmllama:nexusllmllama:1.0.0")
    
    // ... other dependencies
}
```

## Step 3: Set Up Authentication

### Option A: Environment Variables (Recommended)

Set these before building your project:

```bash
export GITHUB_ACTOR=your-github-username
export GITHUB_TOKEN=ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

### Option B: gradle.properties

Add to `~/.gradle/gradle.properties` (user home directory):

```properties
gpr.user=your-github-username
gpr.token=ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

**Note:** Never commit your token to version control!

## Step 4: Generate GitHub Personal Access Token

1. Go to GitHub: Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Click "Generate new token (classic)"
3. Give it a name (e.g., "Gradle GitHub Packages")
4. Select scope: `read:packages`
5. Generate and copy the token
6. Use this token as your `GITHUB_TOKEN` or `gpr.token`

## Step 5: Sync and Build

Sync your Gradle project and build. The library will be downloaded from GitHub Packages.

## Troubleshooting

### Authentication Failed
- Verify your GitHub username and token are correct
- Ensure token has `read:packages` permission
- Check that environment variables or gradle.properties are properly set

### Package Not Found
- Verify the library has been published to GitHub Packages
- Check that the repository URL includes the correct username
- Ensure the version number is correct

### Certificate Issues
If you encounter SSL/certificate issues, ensure your JDK is up to date.

