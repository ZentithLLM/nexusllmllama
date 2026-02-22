# GitHub Publishing Setup Guide

## Overview
This guide will help you publish your NexusLLMLlama library to GitHub Packages and use it as a dependency in other projects.

## Prerequisites
- A GitHub account
- Your repository pushed to GitHub
- Git installed locally

## Step 1: Prepare Your Repository

### 1.1 Update Repository URLs
In `app/build.gradle.kts`, replace `YOUR_USERNAME` with your actual GitHub username in:
- Line 97: `url.set("https://github.com/YOUR_USERNAME/nexusllmllama")`
- Line 115: `connection.set("scm:git:git://github.com/YOUR_USERNAME/nexusllmllama.git")`
- Line 116: `developerConnection.set("scm:git:ssh://github.com/YOUR_USERNAME/nexusllmllama.git")`
- Line 117: `url.set("https://github.com/YOUR_USERNAME/nexusllmllama")`
- Line 126: `url = uri("https://maven.pkg.github.com/YOUR_USERNAME/nexusllmllama")`

### 1.2 Update Developer Information
In `app/build.gradle.kts`, update:
- Line 109: `name.set("Your Name")`
- Line 110: `email.set("your-email@example.com")`

### 1.3 Initialize Git (if not already done)
```bash
cd /home/nishant/AndroidStudioProjects/nexusllmllama
git init
git add .
git commit -m "Initial commit"
```

### 1.4 Create GitHub Repository
1. Go to https://github.com/new
2. Repository name: `nexusllmllama`
3. Make it public or private (both work with GitHub Packages)
4. Don't initialize with README (you already have one)
5. Click "Create repository"

### 1.5 Push to GitHub
```bash
git remote add origin https://github.com/YOUR_USERNAME/nexusllmllama.git
git branch -M main
git push -u origin main
```

## Step 2: Publish to GitHub Packages

### Option A: Using GitHub Actions (Recommended)

The repository includes a GitHub Actions workflow that will automatically publish when you create a tag.

#### 2.1 Create and Push a Tag
```bash
git tag v1.0.0
git push origin v1.0.0
```

#### 2.2 Monitor the Action
1. Go to your repository on GitHub
2. Click "Actions" tab
3. You should see a workflow running
4. Once complete, your package will be published!

### Option B: Manual Publishing from Local Machine

#### 2.1 Generate GitHub Personal Access Token
1. Go to GitHub Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Click "Generate new token (classic)"
3. Give it a name: "Publish NexusLLMLlama"
4. Select scopes:
   - `write:packages` (includes read:packages)
   - `repo` (if your repository is private)
5. Generate and SAVE the token securely

#### 2.2 Set Environment Variables
```bash
export GITHUB_ACTOR=your-github-username
export GITHUB_TOKEN=ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

#### 2.3 Publish
```bash
cd /home/nishant/AndroidStudioProjects/nexusllmllama
./gradlew :app:publish
```

## Step 3: Using the Library in Another Project

### 3.1 Create Personal Access Token for Reading
1. Go to GitHub Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Generate new token
3. Name: "Read GitHub Packages"
4. Scope: `read:packages`
5. Generate and save the token

### 3.2 Configure Consumer Project

In your app project's `settings.gradle.kts`:
```kotlin
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        
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
```

### 3.3 Add Dependency

In your app's `build.gradle.kts`:
```kotlin
dependencies {
    implementation("in.nishantapps.nexusllmllama:nexusllmllama:1.0.0")
    // ... other dependencies
}
```

### 3.4 Set Credentials

**Option A: Environment Variables**
```bash
export GITHUB_ACTOR=your-github-username
export GITHUB_TOKEN=ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
./gradlew build
```

**Option B: gradle.properties**

Add to `~/.gradle/gradle.properties` (in your home directory):
```properties
gpr.user=your-github-username
gpr.token=ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

⚠️ **NEVER** commit tokens to version control!

## Step 4: Verify Publication

### 4.1 Check GitHub Packages
1. Go to your repository on GitHub
2. Click "Packages" on the right side
3. You should see "nexusllmllama" package listed

### 4.2 Test in Another Project
Create a test Android project and follow Step 3 to verify the library can be downloaded and used.

## Updating the Library

### 1. Update Version
In `app/build.gradle.kts`, change:
```kotlin
version = "1.0.1"  // or your new version
```

And in the publishing block:
```kotlin
version = "1.0.1"
```

### 2. Commit Changes
```bash
git add .
git commit -m "Update to version 1.0.1"
git push
```

### 3. Create New Tag
```bash
git tag v1.0.1
git push origin v1.0.1
```

GitHub Actions will automatically publish the new version.

## Troubleshooting

### "Could not find artifact"
- Verify the package exists on GitHub
- Check your credentials are correct
- Ensure the repository URL is correct
- Verify you have the `read:packages` permission

### "401 Unauthorized"
- Your GitHub token has expired or is invalid
- Generate a new token with proper permissions
- Check environment variables are set correctly

### "Package not found"
- The package hasn't been published yet
- Check GitHub Actions completed successfully
- Verify the groupId, artifactId, and version match

### Build Errors
- Ensure you have Android SDK installed
- Check that CMake is available
- Verify NDK is installed for arm64-v8a

## Security Notes

1. **Never commit tokens** to your repository
2. Use environment variables or `~/.gradle/gradle.properties`
3. Add `gradle.properties` to `.gitignore` if storing tokens there
4. Regularly rotate your GitHub tokens
5. Use fine-grained tokens when possible

## References

- [GitHub Packages Documentation](https://docs.github.com/en/packages)
- [Gradle Publishing Documentation](https://docs.gradle.org/current/userguide/publishing_maven.html)
- [Android Library Publishing](https://developer.android.com/studio/build/maven-publish-plugin)

---

**Need Help?**
- Check the GitHub Actions logs for publishing errors
- Verify all YOUR_USERNAME placeholders are replaced
- Ensure your tokens have the correct permissions

