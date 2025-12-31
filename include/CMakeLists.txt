cmake_minimum_required(VERSION 3.16)
project(EmergencyRaylib CXX)

set(CMAKE_CXX_STANDARD 17) # 17 كافية ومستقرة جداً
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. البحث عن ملفات السورس (.cpp) ---
# هذا السطر يجمع كل ملفات cpp الموجودة في src
file(GLOB SOURCES "src/*.cpp")

# --- 2. إعداد Raylib ---
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)

if (NOT raylib_FOUND)
    message(STATUS "Raylib not found locally, fetching via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# --- 3. إنشاء البرنامج (Executable) ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- 4. ربط المجلدات (أهم خطوة) ---
# هنا نقول للمشروع: ملفات .h كاينة في include وملفات .cpp كاينة في src
target_include_directories(${PROJECT_NAME} PRIVATE include src)

# --- 5. ربط المكتبات ---
set(PLATFORM_LIBS "")
if (WIN32)
    list(APPEND PLATFORM_LIBS opengl32 gdi32 winmm)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE raylib ${PLATFORM_LIBS})

# --- 6. نسخ مجلد assets إذا كان موجوداً ---
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()