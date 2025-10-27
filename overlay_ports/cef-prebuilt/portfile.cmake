set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_141.0.11+g7e73ac4+chromium-141.0.7390.123_windows64.tar.bz2")

vcpkg_download_distfile(ARCHIVE
    URLS ${CEF_URL}
    FILENAME "cef141.tar.bz2"
    SHA512 f6e6a658b8dc41b0be3f0c5f9287cab408a9e54bdf141b8497391cf0154f070ff49776b6e11e7aa4a5774930c50380008615c5f30f2a8a5a8bceca94cc367b80
)

vcpkg_extract_source_archive_ex(
    OUT_SOURCE_PATH SOURCE_PATH
    ARCHIVE ${ARCHIVE}
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt DESTINATION ${SOURCE_PATH})

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

file(INSTALL "${SOURCE_PATH}/include" DESTINATION "${CURRENT_PACKAGES_DIR}")
file(INSTALL "${SOURCE_PATH}/Debug" "${SOURCE_PATH}/Release" "${SOURCE_PATH}/Resources" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
