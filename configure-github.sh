#!/bin/bash

# Script to configure GitHub username for publishing
# Usage: ./configure-github.sh <github-username>

if [ -z "$1" ]; then
    echo "Usage: ./configure-github.sh <github-username>"
    echo "Example: ./configure-github.sh nishantapps"
    exit 1
fi

GITHUB_USERNAME="$1"

echo "Configuring GitHub username: $GITHUB_USERNAME"

# Update app/build.gradle.kts
BUILD_GRADLE="app/build.gradle.kts"

if [ ! -f "$BUILD_GRADLE" ]; then
    echo "Error: $BUILD_GRADLE not found!"
    exit 1
fi

# Create backup
cp "$BUILD_GRADLE" "${BUILD_GRADLE}.backup"

# Replace YOUR_USERNAME with actual username
sed -i "s/YOUR_USERNAME/$GITHUB_USERNAME/g" "$BUILD_GRADLE"

echo "✓ Updated $BUILD_GRADLE"

# Update README.md
README="README.md"
if [ -f "$README" ]; then
    cp "$README" "${README}.backup"
    sed -i "s/YOUR_USERNAME/$GITHUB_USERNAME/g" "$README"
    echo "✓ Updated $README"
fi

# Update USAGE_GUIDE.md
USAGE_GUIDE="USAGE_GUIDE.md"
if [ -f "$USAGE_GUIDE" ]; then
    cp "$USAGE_GUIDE" "${USAGE_GUIDE}.backup"
    sed -i "s/YOUR_USERNAME/$GITHUB_USERNAME/g" "$USAGE_GUIDE"
    echo "✓ Updated $USAGE_GUIDE"
fi

# Update SETUP_GUIDE.md
SETUP_GUIDE="SETUP_GUIDE.md"
if [ -f "$SETUP_GUIDE" ]; then
    cp "$SETUP_GUIDE" "${SETUP_GUIDE}.backup"
    sed -i "s/YOUR_USERNAME/$GITHUB_USERNAME/g" "$SETUP_GUIDE"
    echo "✓ Updated $SETUP_GUIDE"
fi

echo ""
echo "Configuration complete!"
echo ""
echo "Next steps:"
echo "1. Update your name and email in app/build.gradle.kts (lines 109-110)"
echo "2. Create a GitHub repository: https://github.com/new"
echo "3. Run: git remote add origin https://github.com/$GITHUB_USERNAME/nexusllmllama.git"
echo "4. Run: git push -u origin main"
echo "5. Follow the SETUP_GUIDE.md for publishing"

