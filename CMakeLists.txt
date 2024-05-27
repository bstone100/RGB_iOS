cmake_minimum_required(VERSION 3.5)

set(MY_APP_PROJECT_NAME "RGB")
set(PROJECT_VERSION 0.0)
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
set(MY_APP_REVERSE_DNS_IDENTIFIER "com.bstone.RGB")

project(${MY_APP_PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# to get universal app (works on m1 and intel chip)
#set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

# C++ visibility needed to generate symbols for upload.
# Essential for being able to symbolicate crash reports.
set(CMAKE_CXX_VISIBILITY_PRESET default)

# Determine the platform and set the appropriate icon file
if(APPLE)
    if(IOS)
        # Provide the launch screen and app icon asset catalog to Xcode such that they get included in copied bundles
        set(RESOURCE_ASSETS
            "${CMAKE_CURRENT_SOURCE_DIR}/iOS/LaunchScreen.storyboard"
            "${CMAKE_CURRENT_SOURCE_DIR}/iOS/AppIconSet.xcassets"
        )
    else() # Assuming macOS if not iOS
        set(app_icon "${CMAKE_CURRENT_SOURCE_DIR}/images/rgb.icns")
        set(MACOSX_BUNDLE_ICON_FILE rgb.icns)

        set_source_files_properties(${app_icon} PROPERTIES
                   MACOSX_PACKAGE_LOCATION "Resources")
    endif()
endif()

if (ANDROID)
    include(FetchContent)

    FetchContent_Declare(
      android_openssl
      DOWNLOAD_EXTRACT_TIMESTAMP true
      URL      https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
#      URL_HASH MD5=c97d6ad774fab16be63b0ab40f78d945 #optional
    )
    FetchContent_MakeAvailable(android_openssl)
    include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Multimedia Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Multimedia Svg)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RGB
        MANUAL_FINALIZATION
        main.cpp
        mainwindow.cpp
        mainwindow.h
        resources.qrc
        AI/openai_request.h AI/openai_request.cpp AI/openai_message.h AI/openai_message.cpp
        audio/audiorecorder.cpp audio/audiorecorder.h audio/audiolevel.h audio/audiolevel.cpp
        AI/api.h AI/api.cpp
        ${app_icon}
        widgets/sidepanel.h widgets/sidepanel.cpp
        widgets/svgbutton.h widgets/svgbutton.cpp
        widgets/chattextedit.h widgets/chattextedit.cpp
        widgets/resizingcombobox.h widgets/resizingcombobox.cpp
        audio/audiolevelcalculator.h audio/audiolevelcalculator.cpp
        macOS/notificationhelper.h macOS/notificationhelper.mm

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RGB APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    if (ANDROID)
        add_android_openssl_libraries(RGB)
    endif()
else()
    if(ANDROID)
        add_library(RGB SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RGB
            ${PROJECT_SOURCES}
        )
    endif()
endif()


# Conditionally compile specific sources
if(APPLE)
    if(IOS)
        target_sources(RGB PRIVATE
            iOS/hapticfeedbackhelper.mm iOS/hapticfeedback.h
            iOS/AudioSessionHelper.h iOS/AudioSessionHelper.mm
            iOS/DarkModeDetector.h iOS/DarkModeDetector.mm
            macOS/AudioRecorderBridge.h macOS/AudioRecorderBridge.mm
            ${RESOURCE_ASSETS}
        )
        set_source_files_properties(${RESOURCE_ASSETS}
                TARGET_DIRECTORY RGB
                PROPERTIES
                    MACOSX_PACKAGE_LOCATION Resources
        )
    else() # Assuming macOS if not iOS
        target_sources(RGB PRIVATE
            macOS/MacThemeDetector.h macOS/MacThemeDetector.mm
            macOS/AudioRecorderBridge.h macOS/AudioRecorderBridge.mm
        )
    endif()
endif()



if(APPLE AND IOS)
    # Find the UIKit framework
    find_library(UIKIT_LIBRARY UIKit)
endif()

if(APPLE)
    find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
    find_library(COREAUDIO_FRAMEWORK CoreAudio)
    find_library(USER_NOTIFICATIONS_FRAMEWORK UserNotifications)
endif()

target_link_libraries(RGB PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Svg
    ${UIKIT_LIBRARY}
    ${AVFOUNDATION_FRAMEWORK}
    ${COREAUDIO_FRAMEWORK}
    ${USER_NOTIFICATIONS_FRAMEWORK}
)


set_target_properties(RGB PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE

    # Build Info.plist the old way
    MACOSX_BUNDLE_GUI_IDENTIFIER            ${MY_APP_REVERSE_DNS_IDENTIFIER}
    MACOSX_BUNDLE_BUNDLE_NAME               ${MY_APP_PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION            ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING      ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_COPYRIGHT                 "Copyright © 2024 Benjamin Stone"
    MACOSX_BUNDLE_INFO_PLIST                "${CMAKE_CURRENT_SOURCE_DIR}/iOS/Info.plist.in"
    QT_IOS_LAUNCH_SCREEN                    "${CMAKE_CURRENT_SOURCE_DIR}/iOS/LaunchScreen.storyboard"


    # New Xcode attributes for General tab
    # My convention for version numbering is public marketing version 1.2, Xcode build 1.2.3
    # Xcode now supports other formats (even though Apple documentation doesn't say so yet)
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER   ${MY_APP_REVERSE_DNS_IDENTIFIER}
    XCODE_ATTRIBUTE_PRODUCT_NAME                ${MY_APP_PROJECT_NAME}
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION     ${PROJECT_VERSION}
    XCODE_ATTRIBUTE_MARKETING_VERSION           ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME  "AppIcon"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY      1 # iPhone only

    XCODE_ATTRIBUTE_INFOPLIST_KEY_CFBundleDisplayName       ${MY_APP_PROJECT_NAME}
    XCODE_ATTRIBUTE_INFOPLIST_KEY_LSApplicationCategoryType "public.app-category.productivity"

    XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES"
)

include(GNUInstallDirs)
install(TARGETS RGB
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RGB)
endif()
