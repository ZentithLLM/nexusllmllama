plugins {
    alias(libs.plugins.android.library)
    id("maven-publish")
}

group = "in.nishantapps.nexusllmllama"
version = "1.0.0"

android {
    namespace = "in.nishantapps.nexusllmllama"
    compileSdk = 36

    publishing {
        singleVariant("release") {
            withSourcesJar()
        }
    }
    defaultConfig {
        minSdk = 24

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags += listOf("-std=c++20", "-fexceptions", "-frtti")
                abiFilters += listOf("armeabi-v7a", "arm64-v8a", "x86", "x86_64")
                arguments += listOf(
                    "-DANDROID_STL=c++_shared",
                    "-DGGML_VULKAN=OFF",     // Vulkan C++ headers not available in Android NDK
                    "-DGGML_OPENMP=OFF",     // generally safer on Android
                    "-DGGML_NATIVE=OFF",      // avoid CPU-specific host tuning issues
                    "-DGGML_LLAMAFILE=OFF"    // not supported on Android; avoids armv7 fp16 intrinsics
                )
            }
        }
        ndk { abiFilters += listOf("armeabi-v7a", "arm64-v8a", "x86", "x86_64") }
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1+"
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    buildFeatures {
        viewBinding = true
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.constraintlayout)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}

afterEvaluate {
    publishing {
        publications {
            create<MavenPublication>("release") {
                from(components["release"])
                groupId = "in.nishantapps.nexusllmllama"
                artifactId = "nexusllmllama"
                version = "1.0.0"

                pom {
                    name.set("NexusLLMLlama")
                    description.set("Android library for running Llama models")
                    url.set("https://github.com/zentithllm/nexusllmllama")

                    licenses {
                        license {
                            name.set("MIT License")
                            url.set("https://opensource.org/licenses/MIT")
                        }
                    }

                    developers {
                        developer {
                            id.set("nishantapps")
                            name.set("Nishant")
                            email.set("support@nishantapps.in")
                        }
                    }

                    scm {
                        connection.set("scm:git:git://github.com/zentithllm/nexusllmllama.git")
                        developerConnection.set("scm:git:ssh://github.com/zentithllm/nexusllmllama.git")
                        url.set("https://github.com/zentithllm/nexusllmllama")
                    }
                }
            }
        }

        repositories {
            maven {
                name = "GitHubPackages"
                url = uri("https://maven.pkg.github.com/YOUR_USERNAME/nexusllmllama")
                credentials {
                    username = System.getenv("GITHUB_ACTOR") ?: findProperty("gpr.user") as String?
                    password = System.getenv("GITHUB_TOKEN") ?: findProperty("gpr.token") as String?
                }
            }
        }
    }
}

